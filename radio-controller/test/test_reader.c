#include <stdio.h>
#include <uv.h>
#include "radio-controller/packet.h"

void on_read(uv_fs_t * req);

char * filepath = "packets.bin";

uv_loop_t * loop;

uv_fs_t open_req, read_req, write_req, close_req;

char buffer[PACKET_SIZE];

uv_buf_t buffer_handle;

void on_close(uv_fs_t * req)
{
    printf("In close callback\n");
    if (0 > req->result)
    {
        printf("Error writing to STDOUT: %ld %s\n", req->result,
               uv_strerror(req->result));
    }
}

void on_write(uv_fs_t * req)
{
    printf("In write callback\n");
    if (0 > req->result)
    {
        // error
        printf("Error writing to STDOUT: %ld %s\n", req->result,
               uv_strerror(req->result));
    }
    else
    {
        uv_fs_read(loop, &read_req, open_req.result, &buffer_handle, 1, -1,
                   on_read);
    }
}

void on_read(uv_fs_t * req)
{
    printf("In read callback\n");
    if (0 > req->result)
    {
        // error
        printf("Error reading %s: %ld %s\n", filepath, req->result,
               uv_strerror(req->result));
    }
    else if (0 == req->result)
    {
        // done reading
        printf("Closing file\n");
        uv_fs_close(loop, &close_req, open_req.result, on_close);
    }
    else
    {
        // write to STDOUT
        printf("Read %ld bytes\n", req->result);
        if (PACKET_SIZE == req->result)
        {
            // Write to packet processor
            telecommand_process(buffer_handle.base, req->result);
            uv_fs_read(loop, &read_req, open_req.result, &buffer_handle, 1,
                       -1, on_read);
        }
        else
        {
            printf("Short Packet!\n");
            uv_fs_close(loop, &close_req, open_req.result, on_close);
        }
    }
}

void on_open(uv_fs_t * req)
{
    printf("In open callback\n");
    if (0 > req->result)
    {
        printf("Error opening %s: %ld %s\n", filepath, req->result,
               uv_strerror(req->result));
    }
    else
    {
        printf("Successfully opened %s %ld\n", filepath, req->result);
        printf("Initializing buffer\n");
        buffer_handle = uv_buf_init(buffer, PACKET_SIZE);
        printf("Reading Packet from %s\n", filepath);

        uv_fs_read(loop, &read_req, req->result, &buffer_handle, 1, -1,
                   on_read);
    }
}

int main(int argc, char * argv[])
{
    int rv  = 0;
    int err = 0;

    if (argc > 1)
    {
        filepath = argv[1];
    }

    do
    {
        printf("Getting default loop\n");
        if (NULL == (loop = uv_default_loop()))
        {
            printf("uv_default_loop() returned NULL\n");
            rv = 2;
            break;
        }

        printf("Opening file: %s\n", filepath);
        if (0 != (err = uv_fs_open(loop, &open_req, filepath, O_RDONLY, 0,
                                   on_open)))
        {
            printf("uv_fs_open() returned error: %d %s\n", err,
                   uv_strerror(err));
            rv = 3;
            break;
        }

        printf("Running Loop\n");
        uv_run(loop, UV_RUN_DEFAULT);

        printf("Out of loop. Cleaning up.\n");
        uv_fs_req_cleanup(&open_req);
        uv_fs_req_cleanup(&read_req);
        uv_fs_req_cleanup(&write_req);
        uv_fs_req_cleanup(&close_req);
    } while (0);
}
