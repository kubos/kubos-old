CSP Interfaces
==============

This is an example of how to implement a new layer-2 interface in CSP. The example is going to show how to create a csp\_if\_fifo, using a set of \[named pipes\](<http://en.wikipedia.org/wiki/Named_pipe>). The complete interface example code can be found in examples/fifo.c. For an example of a fragmenting interface, see the CAN interface in src/interfaces/csp\_if\_can.c.

CSP interfaces are declared in a csp\_iface\_t structure, which sets the interface nexthop function and name. A maximum transmission unit can also be set, which forces CSP to drop outgoing packets above a certain size. The fifo interface is defined as:

``` sourceCode
#include <csp/csp.h>
#include <csp/csp_interface.h>

csp_iface_t csp_if_fifo = {
    .name = "fifo",
    .nexthop = csp_fifo_tx,
    .mtu = BUF_SIZE,
};
```

Outgoing traffic
----------------

The nexthop function takes a pointer to a CSP packet and a timeout as parameters. All outgoing packets that are routed to the interface are passed to this function:

``` sourceCode
int csp_fifo_tx(csp_packet_t *packet, uint32_t timeout) {
    write(tx_channel, &packet->length, packet->length + sizeof(uint32_t) + sizeof(uint16_t));
    csp_buffer_free(packet);
    return 1;
}
```

In the fifo interface, we simply transmit the header, length field and data using a write to the fifo. CSP does not dictate the wire format, so other interfaces may decide to e.g. ignore the length field if the physical layer provides start/stop flags.

\_Important notice: If the transmission succeeds, the interface must free the packet and return 1. If transmission fails, the nexthop function should return 0 and not free the packet, to allow retransmissions by the caller.\_

Incoming traffic
----------------

The interface also needs to receive incoming packets and pass it to the CSP protocol stack. In the fifo interface, this is handled by a thread that blocks on the incoming fifo and waits for packets:

``` sourceCode
void * fifo_rx(void * parameters) {
    csp_packet_t *buf = csp_buffer_get(BUF_SIZE);
    /* Wait for packet on fifo */
    while (read(rx_channel, &buf->length, BUF_SIZE) > 0) {
        csp_new_packet(buf, &csp_if_fifo, NULL);
        buf = csp_buffer_get(BUF_SIZE);
    }
}
```

A new CSP buffer is preallocated with csp\_buffer\_get(). When data is received, the packet is passed to CSP using csp\_new\_packet() and a new buffer is allocated for the next packet. In addition to the received packet, csp\_new\_packet() takes two additional arguments:

``` sourceCode
void csp_new_packet(csp_packet_t *packet, csp_iface_t *interface, CSP_BASE_TYPE *pxTaskWoken);
```

The calling interface must be passed in interface to avoid routing loops. Furthermore, pxTaskWoken must be set to a non-NULL value if the packet is received in an interrupt service routine. If the packet is received in task context, NULL must be passed. ‘pxTaskWoken’ only applies to
