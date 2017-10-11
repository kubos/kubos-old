/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "testutil.hpp"

int main (void)
{
    fprintf (stderr, "test_router_mandatory_tipc running...\n");

    void *ctx = zmq_init (1);
    assert (ctx);

    // Creating the first socket.
    void *sa = zmq_socket (ctx, ZMQ_ROUTER);
    assert (sa);

    int rc = zmq_bind (sa, "tipc://{15560,0,0}");
    assert (rc == 0);

    // Sending a message to an unknown peer with the default setting
    rc = zmq_send (sa, "UNKNOWN", 7, ZMQ_SNDMORE);
    assert (rc == 7);
    rc = zmq_send (sa, "DATA", 4, 0);
    assert (rc == 4);

    int mandatory = 1;

    // Set mandatory routing on socket
    rc = zmq_setsockopt (sa, ZMQ_ROUTER_MANDATORY, &mandatory, sizeof (mandatory));
    assert (rc == 0);

    // Send a message and check that it fails
    rc = zmq_send (sa, "UNKNOWN", 7, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    assert (rc == -1 && errno == EHOSTUNREACH);

    rc = zmq_close (sa);
    assert (rc == 0);

    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
