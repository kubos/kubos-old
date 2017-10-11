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

#include "precompiled.hpp"
#include "epoll.hpp"
#if defined ZMQ_USE_EPOLL

#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <new>

#include "macros.hpp"
#include "epoll.hpp"
#include "err.hpp"
#include "config.hpp"
#include "i_poll_events.hpp"

zmq::epoll_t::epoll_t (const zmq::ctx_t &ctx_) :
    ctx(ctx_),
    stopping (false)
{
#ifdef ZMQ_USE_EPOLL_CLOEXEC
    //  Setting this option result in sane behaviour when exec() functions
    //  are used. Old sockets are closed and don't block TCP ports, avoid
    //  leaks, etc.
    epoll_fd = epoll_create1 (EPOLL_CLOEXEC);
#else
    epoll_fd = epoll_create (1);
#endif
    errno_assert (epoll_fd != -1);
}

zmq::epoll_t::~epoll_t ()
{
    //  Wait till the worker thread exits.
    worker.stop ();

    close (epoll_fd);
    for (retired_t::iterator it = retired.begin (); it != retired.end (); ++it) {
        LIBZMQ_DELETE(*it);
    }
}

zmq::epoll_t::handle_t zmq::epoll_t::add_fd (fd_t fd_, i_poll_events *events_)
{
    poll_entry_t *pe = new (std::nothrow) poll_entry_t;
    alloc_assert (pe);

    //  The memset is not actually needed. It's here to prevent debugging
    //  tools to complain about using uninitialised memory.
    memset (pe, 0, sizeof (poll_entry_t));

    pe->fd = fd_;
    pe->ev.events = 0;
    pe->ev.data.ptr = pe;
    pe->events = events_;

    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_, &pe->ev);
    errno_assert (rc != -1);

    //  Increase the load metric of the thread.
    adjust_load (1);

    return pe;
}

void zmq::epoll_t::rm_fd (handle_t handle_)
{
    poll_entry_t *pe = (poll_entry_t*) handle_;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_DEL, pe->fd, &pe->ev);
    errno_assert (rc != -1);
    pe->fd = retired_fd;
    retired_sync.lock ();
    retired.push_back (pe);
    retired_sync.unlock ();

    //  Decrease the load metric of the thread.
    adjust_load (-1);
}

void zmq::epoll_t::set_pollin (handle_t handle_)
{
    poll_entry_t *pe = (poll_entry_t*) handle_;
    pe->ev.events |= EPOLLIN;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::reset_pollin (handle_t handle_)
{
    poll_entry_t *pe = (poll_entry_t*) handle_;
    pe->ev.events &= ~((short) EPOLLIN);
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::set_pollout (handle_t handle_)
{
    poll_entry_t *pe = (poll_entry_t*) handle_;
    pe->ev.events |= EPOLLOUT;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::reset_pollout (handle_t handle_)
{
    poll_entry_t *pe = (poll_entry_t*) handle_;
    pe->ev.events &= ~((short) EPOLLOUT);
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::start ()
{
    ctx.start_thread (worker, worker_routine, this);
}

void zmq::epoll_t::stop ()
{
    stopping = true;
}

int zmq::epoll_t::max_fds ()
{
    return -1;
}

void zmq::epoll_t::loop ()
{
    epoll_event ev_buf [max_io_events];

    while (!stopping) {

        //  Execute any due timers.
        int timeout = (int) execute_timers ();

        //  Wait for events.
        int n = epoll_wait (epoll_fd, &ev_buf [0], max_io_events,
            timeout ? timeout : -1);
        if (n == -1) {
            errno_assert (errno == EINTR);
            continue;
        }

        for (int i = 0; i < n; i ++) {
            poll_entry_t *pe = ((poll_entry_t*) ev_buf [i].data.ptr);

            if (pe->fd == retired_fd)
                continue;
            if (ev_buf [i].events & (EPOLLERR | EPOLLHUP))
                pe->events->in_event ();
            if (pe->fd == retired_fd)
               continue;
            if (ev_buf [i].events & EPOLLOUT)
                pe->events->out_event ();
            if (pe->fd == retired_fd)
                continue;
            if (ev_buf [i].events & EPOLLIN)
                pe->events->in_event ();
        }

        //  Destroy retired event sources.
        retired_sync.lock ();
        for (retired_t::iterator it = retired.begin (); it != retired.end (); ++it) {
            LIBZMQ_DELETE(*it);
        }
        retired.clear ();
        retired_sync.unlock ();
    }
}

void zmq::epoll_t::worker_routine (void *arg_)
{
    ((epoll_t*) arg_)->loop ();
}

#endif
