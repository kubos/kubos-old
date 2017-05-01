/*
 * KubOS RT
 * Copyright (C) 2016 Kubos Corporation
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
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "telemetry.h"

#include "kubos-hal/gpio.h"
#include "kubos-hal/uart.h"
#include "kubos-hal/i2c.h"
#include "kubos-core/modules/klog.h"

#ifdef YOTTA_CFG_SENSORS_HTU21D
#include "kubos-core/modules/sensors/htu21d.h"
#endif

#ifdef YOTTA_CFG_SENSORS_BNO055
#include "kubos-core/modules/sensors/bno055.h"
#endif

#ifdef YOTTA_CFG_SENSORS_HTU21D
CREATE_TELEMETRY_READING(telemetry_temp, TELEMETRY_READING_HTU21D_TEMP,
            TELEMETRY_UNIT_TEMP);
CREATE_TELEMETRY_READING(telemetry_humidity, TELEMETRY_READING_HTU21D_HUMIDITY,
            TELEMETRY_UNIT_HUMIDITY);
#endif

#ifdef YOTTA_CFG_SENSORS_BNO055
CREATE_TELEMETRY_READING(telemetry_position_x, TELEMETRY_READING_BNO055_POSITION_X,
            TELEMETRY_UNIT_POSITION);
CREATE_TELEMETRY_READING(telemetry_position_y, TELEMETRY_READING_BNO055_POSITION_Y,
            TELEMETRY_UNIT_POSITION);
CREATE_TELEMETRY_READING(telemetry_position_z, TELEMETRY_READING_BNO055_POSITION_Z,
            TELEMETRY_UNIT_POSITION);
CREATE_TELEMETRY_READING(telemetry_position_w, TELEMETRY_READING_BNO055_POSITION_W,
            TELEMETRY_UNIT_POSITION);
#endif

const static telemetry_reading_t *reading;

void test_print_beacon(void) {
 
 for(reading = telemetry_reading_first();
      reading != NULL; reading = reading->next) {
    if(reading->beacon && reading->raw != TELEMETRY_READING_ERROR) {
      printf("Value: %d (%s) Type: %d\r\n", reading->raw, reading->units,
                     reading->type);
        }
    }
}

void task_i2c(void *p) {
    static int x = 0;
    int ret;

/**
 * Example of directly using the Kubos-HAL i2c interface
 */
#if !defined(YOTTA_CFG_SENSORS_HTU21D) && !defined(YOTTA_CFG_SENSORS_BNO055)
    #define I2C_DEV K_I2C1
    #define I2C_SLAVE_ADDR 0x40

    uint8_t cmd = 0xE3;
    uint8_t buffer[3];

    KI2CConf conf = {
        .addressing_mode = K_ADDRESSINGMODE_7BIT,
        .role = K_MASTER,
        .clock_speed = 10000
    };
    // Initialize first i2c bus with configuration
    k_i2c_init(I2C_DEV, &conf);
    // Send single byte command to slave
    k_i2c_write(I2C_DEV, I2C_SLAVE_ADDR, &cmd, 1);
    // Processing delay
    vTaskDelay(50);
    // Read back 3 byte response from slave
    k_i2c_read(I2C_DEV, I2C_SLAVE_ADDR, &buffer, 3);

/**
 * If any sensors are detected then we will use those instead
 */
#else

#ifdef YOTTA_CFG_SENSORS_HTU21D
    float temp, hum;
    htu21d_setup();
    htu21d_reset();
#endif

#ifdef YOTTA_CFG_SENSORS_BNO055
    bno055_setup(OPERATION_MODE_NDOF);
    bno055_quat_data_t pos = { 0, 0, 0, 0};
#endif

    while (1) {
#ifdef YOTTA_CFG_SENSORS_HTU21D
        htu21d_read_temperature(&temp);
        htu21d_read_humidity(&hum);
        
        telemetry_temp_reading.raw = (int) temp;
        telemetry_humidity_reading.raw = (int) hum;
        
        //printf("temp - %f\r\n", temp);
        //printf("humidity - %f\r\n", hum);
#endif

#ifdef YOTTA_CFG_SENSORS_BNO055
        bno055_get_position(&pos);
        
        telemetry_position_x_reading.raw = pos.x;
        telemetry_position_y_reading.raw = pos.y;
        telemetry_position_z_reading.raw = pos.z;
        telemetry_position_w_reading.raw = pos.w;
        
        //printf("imu - %d %d %d %d\r\n", pos.x, pos.y, pos.z, pos.w);
#endif

#if defined(YOTTA_CFG_SENSORS_HTU21D) || defined(YOTTA_CFG_SENSORS_BNO055)
        test_print_beacon();
#endif

        vTaskDelay(100 / portTICK_RATE_MS);
    }
#endif
}

int main(void)
{
    k_uart_console_init();

    #ifdef TARGET_LIKE_STM32
    k_gpio_init(K_LED_GREEN, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_LED_ORANGE, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_LED_RED, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_LED_BLUE, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_BUTTON_0, K_GPIO_INPUT, K_GPIO_PULL_NONE);
    #endif

    #ifdef TARGET_LIKE_MSP430
    k_gpio_init(K_LED_GREEN, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_LED_RED, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
    k_gpio_init(K_BUTTON_0, K_GPIO_INPUT, K_GPIO_PULL_UP);
    /* Stop the watchdog. */
    WDTCTL = WDTPW + WDTHOLD;

    __enable_interrupt();

    P2OUT = BIT1;
    #endif

    #ifdef YOTTA_CFG_SENSORS_HTU21D
    init_telemetry_reading(&telemetry_temp_reading);
    init_telemetry_reading(&telemetry_humidity_reading);
    #endif
    
    #ifdef YOTTA_CFG_SENSORS_BNO055
    init_telemetry_reading(&telemetry_position_x_reading);
    init_telemetry_reading(&telemetry_position_y_reading);
    init_telemetry_reading(&telemetry_position_z_reading);
    init_telemetry_reading(&telemetry_position_w_reading);
    #endif

    xTaskCreate(task_i2c, "I2C", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1);

    return 0;
}
