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

#define THREAD_COUNT 100

extern "C"
{
    static void *worker (void *s)
    {
        int rc;

        rc = zmq_connect (s, "tipc://{5560,0}@0.0.0");
        assert (rc == 0);

        //  Start closing the socket while the connecting process is underway.
        rc = zmq_close (s);
        assert (rc == 0);

        return NULL;
    }
}

int main (void)
{
    void *ctx;
    void *s1;
    void *s2;
    int i;
    int j;
    int rc;
    pthread_t threads [THREAD_COUNT];

    fprintf (stderr, "test_shutdown_stress_tipc running...\n");

    for (j = 0; j != 10; j++) {

        //  Check the shutdown with many parallel I/O threads.
        ctx = zmq_init (7);
        assert (ctx);

        s1 = zmq_socket (ctx, ZMQ_PUB);
        assert (s1);

        rc = zmq_bind (s1, "tipc://{5560,0,0}");
        assert (rc == 0);

        for (i = 0; i != THREAD_COUNT; i++) {
            s2 = zmq_socket (ctx, ZMQ_SUB);
            assert (s2);
            rc = pthread_create (&threads [i], NULL, worker, s2);
            assert (rc == 0);
        }

        for (i = 0; i != THREAD_COUNT; i++) {
            rc = pthread_join (threads [i], NULL);
            assert (rc == 0);
        }

        rc = zmq_close (s1);
        assert (rc == 0);

        rc = zmq_ctx_term (ctx);
        assert (rc == 0);
    }

    return 0;
}
