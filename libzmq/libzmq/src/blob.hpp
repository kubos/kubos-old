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

#ifndef __ZMQ_BLOB_HPP_INCLUDED__
#define __ZMQ_BLOB_HPP_INCLUDED__

#include <string>
#include <string.h>

// Borrowed from id3lib_strings.h:
// They seem to be doing something for MSC, but since I only have gcc, I'll just do that
// Assuming this is uneccessary on GCC 4
// #if (defined(__GNUC__) && (__GNUC__ >= 3) || (defined(_MSC_VER) && _MSC_VER > 1000))
#if (defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC__ <= 4))
namespace std
{
  template<>
    struct char_traits<unsigned char>
    {
      typedef unsigned char char_type;
      // Unsigned as wint_t in unsigned.
      typedef unsigned long int_type;
      typedef streampos pos_type;
      typedef streamoff off_type;
      typedef mbstate_t state_type;

      static void
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static bool
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static bool
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
        for (size_t __i = 0; __i < __n; ++__i)
          if (!eq(__s1[__i], __s2[__i]))
            return lt(__s1[__i], __s2[__i]) ? -1 : 1;
        return 0;
      }

      static size_t
      length(const char_type* __s)
      {
        const char_type* __p = __s;
          while (__p)
            ++__p;
        return (__p - __s);
      }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
      {
        for (const char_type* __p = __s; size_t(__p - __s) < __n; ++__p)
          if (*__p == __a) return __p;
        return 0;
      }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
      { return (char_type*) memmove(__s1, __s2, __n * sizeof(char_type)); }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      { return (char_type*) memcpy(__s1, __s2, __n * sizeof(char_type)); }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
      {
        for (char_type* __p = __s; __p < __s + __n; ++__p)
          assign(*__p, __a);
        return __s;
      }

      static char_type
      to_char_type(const int_type& __c)
      { return char_type(__c); }

      static int_type
      to_int_type(const char_type& __c) { return int_type(__c); }

      static bool
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static int_type
      eof() { return static_cast<int_type>(-1); }

      static int_type
      not_eof(const int_type& __c)
      { return eq_int_type(__c, eof()) ? int_type(0) : __c; }
    };

} // namespace std
#endif // GCC version 3


namespace zmq
{

    //  Object to hold dynamically allocated opaque binary data.
    typedef std::basic_string <unsigned char> blob_t;

}

#endif

