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
    setup_test_environment();
    void *ctx = zmq_ctx_new ();
    assert (ctx);

    //  Create a publisher
    void *pub = zmq_socket (ctx, ZMQ_PUB);
    assert (pub);

    int hwm = 2000;
    int rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, 4);
    assert(rc == 0);

    rc = zmq_bind (pub, "inproc://soname");
    assert (rc == 0);

    //  set pub socket options
    int wait = 1;
    rc = zmq_setsockopt (pub, ZMQ_XPUB_NODROP, &wait, 4);
    assert (rc == 0);


    //  Create a subscriber
    void *sub = zmq_socket (ctx, ZMQ_SUB);
    assert (sub);
    rc = zmq_connect (sub, "inproc://soname");
    assert (rc == 0);

    //  Subscribe for all messages.
    rc = zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    int hwmlimit = hwm - 1;
    int send_count = 0;
    
    //  Send an empty message
    for (int i = 0; i < hwmlimit; i++) {
        rc = zmq_send (pub, NULL, 0, 0);
        assert (rc == 0);
        send_count++;
    }

    int recv_count = 0;
    do {
        //  Receive the message in the subscriber
        rc = zmq_recv (sub, NULL, 0, ZMQ_DONTWAIT);
        if (rc == -1)
            assert (errno == EAGAIN);
        else {
            assert (rc == 0);
            recv_count++;
        }
    }
    while (rc == 0);

    assert (send_count == recv_count);
    
    //  Now test real blocking behavior
    //  Set a timeout, default is infinite
    int timeout = 0;
    rc = zmq_setsockopt (pub, ZMQ_SNDTIMEO, &timeout, 4);
    assert (rc == 0);

    send_count = 0;
    recv_count = 0;
    hwmlimit = hwm;
    
    //  Send an empty message until we get an error, which must be EAGAIN
    while (zmq_send (pub, "", 0, 0) == 0)
        send_count++;
    assert (errno == EAGAIN);

    while (zmq_recv (sub, NULL, 0, ZMQ_DONTWAIT) == 0)
        recv_count++;
    assert (send_count == recv_count);

    //  Clean up.
    rc = zmq_close (pub);
    assert (rc == 0);
    rc = zmq_close (sub);
    assert (rc == 0);
    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
