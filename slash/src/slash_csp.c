/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Space Inventor ApS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <csp/csp.h>
#include <csp/csp_cmp.h>
#include <csp/csp_endian.h>
#include <slash/slash.h>
#include "base16.h"

slash_command_group(csp, "Cubesat Space Protocol");

static void csp_hex_dump (const char *desc, void *addr, int len)
{
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Output description if given.
	if (desc != NULL)
		printf ("%s:\r\n", desc);

	if (len == 0) {
		printf("  ZERO LENGTH\r\n");
		return;
	}
	if (len < 0) {
		printf("  NEGATIVE LENGTH: %i\r\n",len);
		return;
	}

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf ("  %s\r\n", buff);

			// Output the offset.
			printf ("  %p ", addr + i);
		}

		// Now the hex code for the specific character.
		printf (" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf ("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf ("  %s\r\n", buff);
}

static int slash_csp_info(struct slash *slash)
{
#ifdef CSP_DEBUG
	csp_rtable_print();
	csp_conn_print_table();
	csp_iflist_print();
#endif
	return SLASH_SUCCESS;
}

slash_command_sub(csp, info, slash_csp_info, NULL, "Show CSP info");

static int slash_csp_ping(struct slash *slash)
{
	if ((slash->argc <= 1) || (slash->argc > 4))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	unsigned int size = 1;
	if (slash->argc >= 4)
		size = atoi(slash->argv[3]);

	slash_printf(slash, "Ping node %u size %u timeout %u: ", node, size, timeout);

	int result = csp_ping(node, timeout, size, CSP_SO_NONE);

	if (result >= 0) {
		slash_println(slash, "Reply in %d [ms]", result);
	} else {
		slash_println(slash, "No reply");
	}

	return SLASH_SUCCESS;
}

slash_command(ping, slash_csp_ping, "<node> [timeout] [size]", "Ping a system");

static int slash_csp_reboot(struct slash *slash)
{
	if (slash->argc != 2)
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);
	csp_reboot(node);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, reboot, slash_csp_reboot, "<node>", "Reboot a node");

static int slash_csp_ps(struct slash *slash)
{
	if ((slash->argc < 2) || (slash->argc > 3))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	csp_ps(node, timeout);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, ps, slash_csp_ps, "<node> [timeout]", "Process list");

static int slash_csp_memfree(struct slash *slash)
{
	if ((slash->argc < 2) || (slash->argc > 3))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	csp_memfree(node, timeout);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, memfree, slash_csp_memfree, "<node> [timeout]", "Memfree");

static int slash_csp_buffree(struct slash *slash)
{
	if ((slash->argc < 2) || (slash->argc > 3))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	csp_buf_free(node, timeout);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, buffree, slash_csp_buffree, "<node> [timeout]", "Memfree");

static int slash_csp_uptime(struct slash *slash)
{
	if ((slash->argc < 2) || (slash->argc > 3))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	csp_uptime(node, timeout);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, uptime, slash_csp_uptime, "<node> [timeout]", "Memfree");

static int slash_csp_cmp_ident(struct slash *slash)
{
	if ((slash->argc < 2) || (slash->argc > 3))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 3)
		timeout = atoi(slash->argv[2]);

	struct csp_cmp_message message;

	if (csp_cmp_ident(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	printf("%s\n%s\n%s\n%s %s\n", message.ident.hostname, message.ident.model, message.ident.revision, message.ident.date, message.ident.time);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, ident, slash_csp_cmp_ident, "<node> [timeout]", "Ident");


static int slash_csp_cmp_route_set(struct slash *slash)
{
	if ((slash->argc < 5) || (slash->argc > 6))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);
	unsigned int dest_node = atoi(slash->argv[2]);
	char * interface = slash->argv[3];
	unsigned int next_hop_mac = atoi(slash->argv[4]);
	unsigned int timeout = 1000;
	if (slash->argc >= 6)
		timeout = atoi(slash->argv[5]);

	struct csp_cmp_message message;

	message.route_set.dest_node = dest_node;
	message.route_set.next_hop_mac = next_hop_mac;
	strncpy(message.route_set.interface, interface, CSP_CMP_ROUTE_IFACE_LEN);

	if (csp_cmp_route_set(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	printf("Set route ok\n");

	return SLASH_SUCCESS;
}

slash_command_sub(csp, route_set, slash_csp_cmp_route_set, "<node> <dest_node> <interface> <mac> [timeout]", "Route set");


static int slash_csp_cmp_ifstat(struct slash *slash)
{
	if ((slash->argc < 3) || (slash->argc > 5))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);

	unsigned int timeout = 1000;
	if (slash->argc >= 4)
		timeout = atoi(slash->argv[3]);

	struct csp_cmp_message message;

	strncpy(message.if_stats.interface, slash->argv[2], CSP_CMP_ROUTE_IFACE_LEN);

	if (csp_cmp_if_stats(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	message.if_stats.tx =       csp_ntoh32(message.if_stats.tx);
	message.if_stats.rx =       csp_ntoh32(message.if_stats.rx);
	message.if_stats.tx_error = csp_ntoh32(message.if_stats.tx_error);
	message.if_stats.rx_error = csp_ntoh32(message.if_stats.rx_error);
	message.if_stats.drop =     csp_ntoh32(message.if_stats.drop);
	message.if_stats.autherr =  csp_ntoh32(message.if_stats.autherr);
	message.if_stats.frame =    csp_ntoh32(message.if_stats.frame);
	message.if_stats.txbytes =  csp_ntoh32(message.if_stats.txbytes);
	message.if_stats.rxbytes =  csp_ntoh32(message.if_stats.rxbytes);
	message.if_stats.irq = 	 csp_ntoh32(message.if_stats.irq);


	printf("%-5s   tx: %05"PRIu32" rx: %05"PRIu32" txe: %05"PRIu32" rxe: %05"PRIu32"\n"
	       "        drop: %05"PRIu32" autherr: %05"PRIu32" frame: %05"PRIu32"\n"
	       "        txb: %"PRIu32" rxb: %"PRIu32"\n\n",
		message.if_stats.interface,
		message.if_stats.tx,
		message.if_stats.rx,
		message.if_stats.tx_error,
		message.if_stats.rx_error,
		message.if_stats.drop,
		message.if_stats.autherr,
		message.if_stats.frame,
		message.if_stats.txbytes,
		message.if_stats.rxbytes);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, ifstat, slash_csp_cmp_ifstat, "<node> <interface> [timeout]", "Ident");

static int slash_csp_cmp_peek(struct slash *slash)
{
	if ((slash->argc < 4) || (slash->argc > 5))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);
	unsigned int address = (unsigned int) strtoul(slash->argv[2], NULL, 16);
	unsigned int len = atoi(slash->argv[3]);

	unsigned int timeout = 1000;
	if (slash->argc > 4)
		timeout = atoi(slash->argv[4]);

	struct csp_cmp_message message;

	message.peek.addr = csp_hton32(address);
	message.peek.len = len;

	if (csp_cmp_peek(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	printf("Peek at address %p\n", (void *) (intptr_t) address);
	csp_hex_dump(NULL, message.peek.data, message.peek.len);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, peek, slash_csp_cmp_peek, "<node> <address> <len> [timeout]", "Peek");

static int slash_csp_cmp_poke(struct slash *slash)
{
	if ((slash->argc < 4) || (slash->argc > 5))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);
	unsigned int address = (unsigned int) strtoul(slash->argv[2], NULL, 16);
	unsigned int timeout = 1000;
	if (slash->argc > 4)
		timeout = atoi(slash->argv[4]);

	struct csp_cmp_message message;

	message.poke.addr = csp_hton32(address);

	int outlen = base16_decode(slash->argv[3], (uint8_t *) message.poke.data);

	message.poke.len = outlen;

	printf("Poke at address %p\n", (void *) (intptr_t) address);
	csp_hex_dump(NULL, message.poke.data, outlen);

	if (csp_cmp_poke(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	printf("Poke ok\n");

	return SLASH_SUCCESS;
}

slash_command_sub(csp, poke, slash_csp_cmp_poke, "<node> <address> <data> [timeout]", "Poke");

static int slash_csp_cmp_time(struct slash *slash)
{
	if ((slash->argc < 3) || (slash->argc > 4))
		return SLASH_EUSAGE;

	unsigned int node = atoi(slash->argv[1]);
	int timestamp = atoi(slash->argv[2]);
	unsigned int timeout = 1000;
	if (slash->argc > 3)
		timeout = atoi(slash->argv[3]);

	struct csp_cmp_message message;

	csp_timestamp_t localtime;
	clock_get_time(&localtime);

	if (timestamp == -1) {
		message.clock.tv_sec = csp_hton32(localtime.tv_sec);
		message.clock.tv_nsec = csp_hton32(localtime.tv_nsec);
	} else {
		message.clock.tv_sec = csp_hton32(timestamp);
		message.clock.tv_nsec = csp_hton32(0);
	}

	if (csp_cmp_clock(node, timeout, &message) != CSP_ERR_NONE) {
		printf("No response\n");
		return SLASH_EINVAL;
	}

	message.clock.tv_sec = csp_ntoh32(message.clock.tv_sec);
	message.clock.tv_nsec = csp_ntoh32(message.clock.tv_nsec);

	int64_t remote_time_ns = message.clock.tv_sec * 1E9 + message.clock.tv_nsec;
	int64_t local_time_ns = localtime.tv_sec * 1E9 + localtime.tv_nsec;

	printf("Remote time is %"PRIu32".%09"PRIu32" (diff %"PRIi32" us)\n", message.clock.tv_sec, message.clock.tv_nsec, (int32_t) (remote_time_ns - local_time_ns) / 1000);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, time, slash_csp_cmp_time, "<node> <timestamp (0 GET, -1 SETLOCAL)> [timeout]", "Time");

#ifdef CSP_USE_RDP
static int slash_csp_rdpopt(struct slash *slash)
{
	if (slash->argc < 6)
		return SLASH_EUSAGE;

	unsigned int window = atoi(slash->argv[1]);
	unsigned int conn_timeout = atoi(slash->argv[2]);
	unsigned int packet_timeout = atoi(slash->argv[3]);
	unsigned int ack_timeout = atoi(slash->argv[4]);
	unsigned int ack_count = atoi(slash->argv[5]);

	printf("Setting rdp options: %u %u %u %u %u\n", window, conn_timeout, packet_timeout, ack_timeout, ack_count);
	csp_rdp_set_opt(window, conn_timeout, packet_timeout, 1, ack_timeout, ack_count);

	return SLASH_SUCCESS;
}

slash_command_sub(csp, rdpopt, slash_csp_rdpopt, "<window> <conn_to> <packet_to> <ack_to> <ack_count>", NULL);
#endif
