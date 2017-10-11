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

#ifndef __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__
#define __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__

#include "stdint.hpp"

#if defined ZMQ_FORCE_MUTEXES
#define ZMQ_ATOMIC_COUNTER_MUTEX
#elif defined ZMQ_HAVE_ATOMIC_INTRINSICS
#define ZMQ_ATOMIC_COUNTER_INTRINSIC
#elif (defined ZMQ_CXX11 && defined __cplusplus && __cplusplus >= 201103L)
#define ZMQ_ATOMIC_COUNTER_CXX11
#elif (defined __i386__ || defined __x86_64__) && defined __GNUC__
#define ZMQ_ATOMIC_COUNTER_X86
#elif defined __ARM_ARCH_7A__ && defined __GNUC__
#define ZMQ_ATOMIC_COUNTER_ARM
#elif defined ZMQ_HAVE_WINDOWS
#define ZMQ_ATOMIC_COUNTER_WINDOWS
#elif (defined ZMQ_HAVE_SOLARIS || defined ZMQ_HAVE_NETBSD || defined ZMQ_HAVE_GNU)
#define ZMQ_ATOMIC_COUNTER_ATOMIC_H
#elif defined __tile__
#define ZMQ_ATOMIC_COUNTER_TILE
#else
#define ZMQ_ATOMIC_COUNTER_MUTEX
#endif

#if defined ZMQ_ATOMIC_COUNTER_MUTEX
#include "mutex.hpp"
#elif defined ZMQ_ATOMIC_COUNTER_CXX11
#include <atomic>
#elif defined ZMQ_ATOMIC_COUNTER_WINDOWS
#include "windows.hpp"
#elif defined ZMQ_ATOMIC_COUNTER_ATOMIC_H
#include <atomic.h>
#elif defined ZMQ_ATOMIC_COUNTER_TILE
#include <arch/atomic.h>
#endif

namespace zmq
{

    //  This class represents an integer that can be incremented/decremented
    //  in atomic fashion.

    class atomic_counter_t
    {
    public:

        typedef uint32_t integer_t;

        inline atomic_counter_t (integer_t value_ = 0) :
            value (value_)
        {
        }

        inline ~atomic_counter_t ()
        {
        }

        //  Set counter value (not thread-safe).
        inline void set (integer_t value_)
        {
            value = value_;
        }

        //  Atomic addition. Returns the old value.
        inline integer_t add (integer_t increment_)
        {
            integer_t old_value;

#if defined ZMQ_ATOMIC_COUNTER_WINDOWS
            old_value = InterlockedExchangeAdd ((LONG*) &value, increment_);
#elif defined ZMQ_ATOMIC_COUNTER_INTRINSIC
            old_value = __atomic_fetch_add(&value, increment_, __ATOMIC_ACQ_REL);
#elif defined ZMQ_ATOMIC_COUNTER_CXX11
            old_value = value.fetch_add(increment_, std::memory_order_acq_rel);
#elif defined ZMQ_ATOMIC_COUNTER_ATOMIC_H
            integer_t new_value = atomic_add_32_nv (&value, increment_);
            old_value = new_value - increment_;
#elif defined ZMQ_ATOMIC_COUNTER_TILE
            old_value = arch_atomic_add (&value, increment_);
#elif defined ZMQ_ATOMIC_COUNTER_X86
            __asm__ volatile (
                "lock; xadd %0, %1 \n\t"
                : "=r" (old_value), "=m" (value)
                : "0" (increment_), "m" (value)
                : "cc", "memory");
#elif defined ZMQ_ATOMIC_COUNTER_ARM
            integer_t flag, tmp;
            __asm__ volatile (
                "       dmb     sy\n\t"
                "1:     ldrex   %0, [%5]\n\t"
                "       add     %2, %0, %4\n\t"
                "       strex   %1, %2, [%5]\n\t"
                "       teq     %1, #0\n\t"
                "       bne     1b\n\t"
                "       dmb     sy\n\t"
                : "=&r"(old_value), "=&r"(flag), "=&r"(tmp), "+Qo"(value)
                : "Ir"(increment_), "r"(&value)
                : "cc");
#elif defined ZMQ_ATOMIC_COUNTER_MUTEX
            sync.lock ();
            old_value = value;
            value += increment_;
            sync.unlock ();
#else
#error atomic_counter is not implemented for this platform
#endif
            return old_value;
        }

        //  Atomic subtraction. Returns false if the counter drops to zero.
        inline bool sub (integer_t decrement)
        {
#if defined ZMQ_ATOMIC_COUNTER_WINDOWS
            LONG delta = - ((LONG) decrement);
            integer_t old = InterlockedExchangeAdd ((LONG*) &value, delta);
            return old - decrement != 0;
#elif defined ZMQ_ATOMIC_COUNTER_INTRINSIC
            integer_t nv = __atomic_sub_fetch(&value, decrement, __ATOMIC_ACQ_REL);
            return nv != 0;
#elif defined ZMQ_ATOMIC_COUNTER_CXX11
            integer_t old = value.fetch_sub(decrement, std::memory_order_acq_rel);
            return old - decrement != 0;
#elif defined ZMQ_ATOMIC_COUNTER_ATOMIC_H
            int32_t delta = - ((int32_t) decrement);
            integer_t nv = atomic_add_32_nv (&value, delta);
            return nv != 0;
#elif defined ZMQ_ATOMIC_COUNTER_TILE
            int32_t delta = - ((int32_t) decrement);
            integer_t nv = arch_atomic_add (&value, delta);
            return nv != 0;
#elif defined ZMQ_ATOMIC_COUNTER_X86
            integer_t oldval = -decrement;
            volatile integer_t *val = &value;
            __asm__ volatile ("lock; xaddl %0,%1"
                : "=r" (oldval), "=m" (*val)
                : "0" (oldval), "m" (*val)
                : "cc", "memory");
            return oldval != decrement;
#elif defined ZMQ_ATOMIC_COUNTER_ARM
            integer_t old_value, flag, tmp;
            __asm__ volatile (
                "       dmb     sy\n\t"
                "1:     ldrex   %0, [%5]\n\t"
                "       sub     %2, %0, %4\n\t"
                "       strex   %1, %2, [%5]\n\t"
                "       teq     %1, #0\n\t"
                "       bne     1b\n\t"
                "       dmb     sy\n\t"
                : "=&r"(old_value), "=&r"(flag), "=&r"(tmp), "+Qo"(value)
                : "Ir"(decrement), "r"(&value)
                : "cc");
            return old_value - decrement != 0;
#elif defined ZMQ_ATOMIC_COUNTER_MUTEX
            sync.lock ();
            value -= decrement;
            bool result = value ? true : false;
            sync.unlock ();
            return result;
#else
#error atomic_counter is not implemented for this platform
#endif
        }

        inline integer_t get () const
        {
            return value;
        }

    private:

#if defined ZMQ_ATOMIC_COUNTER_CXX11
        std::atomic<integer_t> value;
#else
        volatile integer_t value;
#endif

#if defined ZMQ_ATOMIC_COUNTER_MUTEX
        mutex_t sync;
#endif

#if ! defined ZMQ_ATOMIC_COUNTER_CXX11
        atomic_counter_t (const atomic_counter_t&);
        const atomic_counter_t& operator = (const atomic_counter_t&);
#endif
    };

}

//  Remove macros local to this file.
#undef ZMQ_ATOMIC_COUNTER_MUTEX
#undef ZMQ_ATOMIC_COUNTER_INTRINSIC
#undef ZMQ_ATOMIC_COUNTER_CXX11
#undef ZMQ_ATOMIC_COUNTER_X86
#undef ZMQ_ATOMIC_COUNTER_ARM
#undef ZMQ_ATOMIC_COUNTER_WINDOWS
#undef ZMQ_ATOMIC_COUNTER_ATOMIC_H
#undef ZMQ_ATOMIC_COUNTER_TILE

#endif
