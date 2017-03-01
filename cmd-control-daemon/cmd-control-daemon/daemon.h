#ifndef DAEMON_H
#define DAEMON_H
#include <csp/csp.h>
#include <command-and-control/types.h>
#include <tinycbor/cbor.h>

typedef int (*lib_function)(int, char**);

bool encode_processing_error(uint8_t * data, cnc_command_wrapper * result, CborEncoder * encoder, CborEncoder * container);

bool encode_response(uint8_t * data, cnc_command_wrapper * wrapper, CborEncoder * encoder, CborEncoder * container);

bool finish_encode_response_and_send(uint8_t * data, CborEncoder *encoder, CborEncoder * container);

void get_command(csp_socket_t* sock, char * command);

bool parse (char * args, cnc_command_wrapper * my_arguments);

bool parse_command_cbor(csp_packet_t * packet, char * command);

bool process_and_run_command(cnc_command_wrapper * wrapper);

bool send_buffer(uint8_t * data, size_t data_len);

bool start_encode_response(int message_type, cnc_command_wrapper * wrapper);

bool send_result(cnc_command_wrapper * wrapper);

#endif