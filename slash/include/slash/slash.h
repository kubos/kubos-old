/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Satlab ApS <satlab@satlab.com>
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

#ifndef _SLASH_H_
#define _SLASH_H_

#include <stddef.h>
#include <stdbool.h>

#include <libslash.h>

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#ifdef SLASH_LF // LF ONLY
#define SLASH_NL "\n"
#define SLASH_NL_LEN 1
#else // CRLF (default)
#define SLASH_NL "\r\n"
#define SLASH_NL_LEN 2
#endif

/* Helper macros */
#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)(intptr_t)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifndef stringify
#define stringify(_var) #_var
#endif

#define slash_max(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

#define slash_min(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

#define __slash_command(_ident, _name, _func, _completer, _args, _help) 	\
	__attribute__((section("slash")))				\
	__attribute__((used))						\
	const struct slash_command _ident = {					\
		.name  = _name,				\
		.func  = _func,						\
		.completer  = _completer,						\
		.args  = _args,						\
	};

#define slash_command(_name, _func, _args, _help)			\
	__slash_command(slash_cmd_ ## _name,				\
			#_name, _func, NULL, _args, _help)

#define slash_command_sub(_group, _name, _func, _args, _help)		\
	__slash_command(slash_cmd_##_group ## _ ## _name ,		\
			#_group" "#_name, _func, NULL, _args, _help)

#define slash_command_subsub(_group, _subgroup, _name, _func, _args, _help) \
	__slash_command(slash_cmd_ ## _group ## _ ## _subgroup ## _name, \
			#_group" "#_subgroup" "#_name, _func, NULL, _args, _help)

#define slash_command_completer(_name, _func, _completer, _args, _help)			\
	__slash_command(slash_cmd_ ## _name,				\
			#_name, _func, _completer, _args, _help)

#define slash_command_sub_completer(_group, _name, _func, _completer, _args, _help)		\
	__slash_command(slash_cmd_##_group ## _ ## _name ,		\
			#_group" "#_name, _func, _completer, _args, _help)

#define slash_command_subsub_completer(_group, _subgroup, _name, _func, _completer, _args, _help) \
	__slash_command(slash_cmd_ ## _group ## _ ## _subgroup ## _name, \
			#_group" "#_subgroup" "#_name, _func, _completer, _args, _help)

#define slash_command_group(_name, _help)

#define slash_command_subgroup(_group, _name, _help)

/* Command prototype */
struct slash;
typedef int (*slash_func_t)(struct slash *slash);
typedef void (*slash_completer_func_t)(struct slash *slash, char * token);

/* Command return values */
#define SLASH_EXIT	( 1)
#define SLASH_SUCCESS	( 0)
#define SLASH_EUSAGE	(-1)
#define SLASH_EINVAL	(-2)
#define SLASH_ENOSPC	(-3)

/* Command struct */
struct slash_command {
	/* Static data */
	const char *name;
	const slash_func_t func;
	const char *args;
	const slash_completer_func_t completer;
};

/* Slash context */
struct slash {

	/* Terminal handling */
#ifdef HAVE_TERMIOS_H
	struct termios original;
#endif
	bool rawmode;
	bool atexit_registered;
	int fd_write;
	int fd_read;

	/* Line editing */
	size_t line_size;
	const char *prompt;
	size_t prompt_length;
	size_t prompt_print_length;
	char *buffer;
	size_t cursor;
	size_t length;
	bool escaped;
	char last_char;

	/* History */
	size_t history_size;
	int history_depth;
	size_t history_avail;
	int history_rewind_length;
	char *history;
	char *history_head;
	char *history_tail;
	char *history_cursor;

	/* Command interface */
	char **argv;
	int argc;
};

struct slash *slash_create(size_t line_size, size_t history_size);

void slash_destroy(struct slash *slash);

char *slash_readline(struct slash *slash, const char *prompt);

int slash_execute(struct slash *slash, char *line);

int slash_loop(struct slash *slash, const char *prompt_good, const char *prompt_bad);

int slash_getchar_nonblock(struct slash *slash);

int slash_printf(struct slash *slash, const char *format, ...);

#define slash_println(_slash, _fmt, ...) slash_printf(_slash, _fmt SLASH_NL, ## __VA_ARGS__)

int slash_prefix_length(const char *s1, const char *s2);

void slash_bell(struct slash *slash);

#endif /* _SLASH_H_ */
