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

#include "testutil.hpp"

int main (void)
{
    fprintf (stderr, "test_sub_forward running...\n");

    void *ctx = zmq_init (1);
    assert (ctx);

    //  First, create an intermediate device.
    void *xpub = zmq_socket (ctx, ZMQ_XPUB);
    assert (xpub);
    int rc = zmq_bind (xpub, "tipc://{5560,0,0}");
    assert (rc == 0);
    void *xsub = zmq_socket (ctx, ZMQ_XSUB);
    assert (xsub);
    rc = zmq_bind (xsub, "tipc://{5561,0,0}");
    assert (rc == 0);

    //  Create a publisher.
    void *pub = zmq_socket (ctx, ZMQ_PUB);
    assert (pub);
    rc = zmq_connect (pub, "tipc://{5561,0}@0.0.0");
    assert (rc == 0);

    //  Create a subscriber.
    void *sub = zmq_socket (ctx, ZMQ_SUB);
    assert (sub);
    rc = zmq_connect (sub, "tipc://{5560,0}@0.0.0");
    assert (rc == 0);

    //  Subscribe for all messages.
    rc = zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    //  Pass the subscription upstream through the device.
    char buff [32];
    rc = zmq_recv (xpub, buff, sizeof (buff), 0);
    assert (rc >= 0);
    rc = zmq_send (xsub, buff, rc, 0);
    assert (rc >= 0);

    //  Wait a bit till the subscription gets to the publisher.
    msleep (SETTLE_TIME);

    //  Send an empty message.
    rc = zmq_send (pub, NULL, 0, 0);
    assert (rc == 0);

    //  Pass the message downstream through the device.
    rc = zmq_recv (xsub, buff, sizeof (buff), 0);
    assert (rc >= 0);
    rc = zmq_send (xpub, buff, rc, 0);
    assert (rc >= 0);

    //  Receive the message in the subscriber.
    rc = zmq_recv (sub, buff, sizeof (buff), 0);
    assert (rc == 0);

    //  Clean up.
    rc = zmq_close (xpub);
    assert (rc == 0);
    rc = zmq_close (xsub);
    assert (rc == 0);
    rc = zmq_close (pub);
    assert (rc == 0);
    rc = zmq_close (sub);
    assert (rc == 0);
    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
