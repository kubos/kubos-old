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

#ifndef __ZMQ_MTRIE_HPP_INCLUDED__
#define __ZMQ_MTRIE_HPP_INCLUDED__

#include <stddef.h>
#include <set>

#include "stdint.hpp"

namespace zmq
{

    class pipe_t;

    //  Multi-trie. Each node in the trie is a set of pointers to pipes.

    class mtrie_t
    {
    public:

        mtrie_t ();
        ~mtrie_t ();

        //  Add key to the trie. Returns true if it's a new subscription
        //  rather than a duplicate.
        bool add (unsigned char *prefix_, size_t size_, zmq::pipe_t *pipe_);

        //  Remove all subscriptions for a specific peer from the trie.
        //  The call_on_uniq_ flag controls if the callback is invoked
        //  when there are no subscriptions left on some topics or on
        //  every removal.
        void rm (zmq::pipe_t *pipe_,
            void (*func_) (unsigned char *data_, size_t size_, void *arg_),
            void *arg_, bool call_on_uniq_);

        //  Remove specific subscription from the trie. Return true is it was
        //  actually removed rather than de-duplicated.
        bool rm (unsigned char *prefix_, size_t size_, zmq::pipe_t *pipe_);

        //  Signal all the matching pipes.
        void match (unsigned char *data_, size_t size_,
            void (*func_) (zmq::pipe_t *pipe_, void *arg_), void *arg_);

    private:

        bool add_helper (unsigned char *prefix_, size_t size_,
            zmq::pipe_t *pipe_);
        void rm_helper (zmq::pipe_t *pipe_, unsigned char **buff_,
            size_t buffsize_, size_t maxbuffsize_,
            void (*func_) (unsigned char *data_, size_t size_, void *arg_),
            void *arg_, bool call_on_uniq_);
        bool rm_helper (unsigned char *prefix_, size_t size_,
            zmq::pipe_t *pipe_);
        bool is_redundant () const;

        typedef std::set <zmq::pipe_t*> pipes_t;
        pipes_t *pipes;

        unsigned char min;
        unsigned short count;
        unsigned short live_nodes;
        union {
            class mtrie_t *node;
            class mtrie_t **table;
        } next;

        mtrie_t (const mtrie_t&);
        const mtrie_t &operator = (const mtrie_t&);
    };

}

#endif

