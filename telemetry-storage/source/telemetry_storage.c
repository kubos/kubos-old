/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdio.h>

#include <telemetry/telemetry.h>
#include "telemetry-storage/telemetry_storage.h"
#include "telemetry-storage/config.h"


CSP_DEFINE_TASK(telemetry_store_rx)
{
    telemetry_packet packet;
    pubsub_conn * connection;

    while((connection = telemetry_connect()) == NULL)
    {
        csp_sleep_ms(STORAGE_SUBSCRIBE_RETRY_INTERVAL);
    }

    /* Subscribe to telemetry publishers as specified in the configuration */
    while (!telemetry_subscribe(connection, STORAGE_SUBSCRIPTIONS))
    {
        /* Retry subscribing at the interval specified in the configuration*/
        csp_sleep_ms(STORAGE_SUBSCRIBE_RETRY_INTERVAL);
    }

    while (1)
    {
        if (telemetry_read(connection, &packet))
        {
            /* Store telemetry packets from the telemetry system */
            telemetry_store(packet);
        }
    }
}


void telemetry_storage_init(void)
{
    csp_thread_handle_t telem_store_rx_handle;
    csp_thread_create(telemetry_store_rx, "TELEM_STORE_RX", STORAGE_TASK_STACK_DEPTH, NULL, STORAGE_TASK_PRIORITY, &telem_store_rx_handle);
}


/**
 * @brief creates a filename that corresponds to the telemetry packet topic_id and 
 *        the csp packet address.
 * @param filename_buf_ptr a pointer to the char[] to write to.
 * @param topic_id the telemetry packet topic_id from packet.source.topic_id.
 * @param address the csp packet address from packet->id.src. 
 * @param file_extension. 
 * @retval The length of the filename written.
 */
static uint16_t create_filename(char *filename_buf_ptr, uint8_t topic_id, unsigned int address, const char *file_extension)
{
    if (filename_buf_ptr == NULL || file_extension == NULL) 
    {
        return 0;
    }

    int len;

    len = snprintf(filename_buf_ptr, FILE_NAME_BUFFER_SIZE, "%u%u%s", topic_id, address, file_extension);

    if(len < 0 || len >= FILE_NAME_BUFFER_SIZE) 
    {
        printf("Filename char limit exceeded. Have %d, need %d + \\0\n", FILE_NAME_BUFFER_SIZE, len);
        return 0;
    }
    return len;
}


/**
 * @brief creates a formatted log entry from the telemetry packet.
 * @param data_buf_ptr a pointer to the char[] to write to.
 * @param packet a telemetry packet to create a log entry from.
 * @retval The length of the log entry written.
 */
static uint16_t format_log_entry_csv(char *data_buf_ptr, telemetry_packet packet) 
{
    if (data_buf_ptr == NULL) 
    {
        return 0;
    }

    int len = 0;

    if(packet.source.data_type == TELEMETRY_TYPE_INT) 
    {
        len = snprintf(data_buf_ptr, DATA_BUFFER_SIZE, "%u,%d", packet.timestamp, packet.data.i);
        if(len < 0 || len >= DATA_BUFFER_SIZE) 
        {
            printf("Data char limit exceeded for int packet. Have %d, need %d + \\0\n", DATA_BUFFER_SIZE, len);
            return 0;
        }
    }

    if(packet.source.data_type == TELEMETRY_TYPE_FLOAT) 
    {
        len = snprintf(data_buf_ptr, DATA_BUFFER_SIZE, "%u,%f", packet.timestamp, packet.data.f);
        if(len < 0 || len >= DATA_BUFFER_SIZE) 
        {
            printf("Data char limit exceeded for float packet. Have %d, need %d + \\0\n", DATA_BUFFER_SIZE, len);
            return 0;
        }
    }
    return len;
}


/**
 * @brief Filter out any negative error return values of snprintf.
 */
static int length_added(int result_of_snprintf)
{
    return (result_of_snprintf > 0) ? result_of_snprintf : 0;
}


/**
 * @brief creates a formatted hex dump of a telemetry packet.
 * @param data_buf_ptr a pointer to the char[] to write to.
 * @param packet a telemetry packet to create a hex dump from.
 * @retval The length of the hex dump written.
 */
static uint16_t format_log_entry_hex(char *data_buf_ptr, telemetry_packet packet) 
{
    if (data_buf_ptr == NULL) 
    {
        return 0;
    }

    int len = 0;
    const unsigned char *byte_array = (unsigned char*)&packet;

    
    for(int i = 0; i < sizeof(telemetry_packet); i++)
    {
        len += length_added(snprintf(data_buf_ptr + len, DATA_BUFFER_SIZE - len, "%02X", byte_array[i]));
    }
    
    if(len < 0 || len >= DATA_BUFFER_SIZE) 
    {
        printf("Data char limit exceeded for packet. Have %d, need %d + \\0\n", DATA_BUFFER_SIZE, len);
        return 0;
    }
    return len;
}


/**
 * @brief print telemetry packet data.
 * @param packet a telemetry packet with data to print.
 */
static void print_to_console(telemetry_packet packet)
{
    if(packet.source.data_type == TELEMETRY_TYPE_INT) 
    {
        printf("%d\r\n", packet.data.i);
    }

    if(packet.source.data_type == TELEMETRY_TYPE_FLOAT) 
    {
        printf("%f\r\n", packet.data.f);
    }
}


/**
* @brief log formatted data with klog.
* @param filename_buf_ptr a pointer to the filename
* @param data_buf_ptr a pointer to the data to be logged
* @param filename_len the filename length 
* @retval bool true if successful, otherwise false
*/
static bool send_to_klog(char *filename_buf_ptr, char *data_buf_ptr, uint16_t filename_len)
{
    bool ret = false;
    klog_handle telemetry_log_handle = { .config.file_path = filename_buf_ptr, \
                                         .config.file_path_len = filename_len, \
                                         .config.part_size = DATA_PART_SIZE, \
                                         .config.max_parts = DATA_MAX_PARTS, \
                                         .config.klog_console_level = LOG_NONE, \
                                         .config.klog_file_level = LOG_TELEMETRY, \
                                         .config.klog_file_logging = true };

    /* klog_init returns 0 for success */
    if (!klog_init_file(&telemetry_log_handle))
    {
        KLOG_TELEMETRY(&telemetry_log_handle, "", data_buf_ptr);
        klog_cleanup(&telemetry_log_handle);
        ret = true;
    }
    return ret;
}


bool telemetry_store(telemetry_packet packet)
{
    static char filename_buffer[FILE_NAME_BUFFER_SIZE];
    static char *filename_buf_ptr;
    static char data_buffer[DATA_BUFFER_SIZE];
    static char *data_buf_ptr;
    bool ret = false;

    uint16_t data_len;
    uint16_t filename_len;

    filename_buf_ptr = filename_buffer;
    data_buf_ptr = data_buffer;

    switch (DATA_OUTPUT_FORMAT)
    {
    case FORMAT_TYPE_CSV_DATA_ONLY:

        filename_len = create_filename(filename_buf_ptr, packet.source.topic_id, packet.source.subsystem_id, FILE_EXTENSION_NONE);
        data_len = format_log_entry_csv(data_buf_ptr, packet);

        /* Save CSV data only format log entry */
        if (filename_len > 0 && data_len > 0)
        {
            ret = send_to_klog(filename_buf_ptr, data_buf_ptr, filename_len);
        }
        else
        {
            printf("Error decoding telemetry packet. Log entry or filename is blank \r\n");
        }
        break;
    case FORMAT_TYPE_CSV:
        /* Placeholder for the entire telemetry packet in CSV format */
        break;
    case FORMAT_TYPE_HEX_DATA_ONLY:
        /* Placeholder for the telemetry packet data only in hexidecimal format */
        break;
    case FORMAT_TYPE_HEX:

        filename_len = create_filename(filename_buf_ptr, packet.source.topic_id, packet.source.subsystem_id, FILE_EXTENSION_NONE);
        data_len = format_log_entry_hex(data_buf_ptr, packet);

        /* Save HEX format log entry */
        if (filename_len > 0 && data_len > 0)
        {
            ret = send_to_klog(filename_buf_ptr, data_buf_ptr, filename_len);
        }
        else
        {
            printf("Error decoding telemetry packet. Log entry or filename is blank \r\n");
        }
        break;
    default:
        printf("Telemetry storage format type not found\r\n");
        break;
    }
    return ret;
}
