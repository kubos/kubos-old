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

#include <slash/slash.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <errno.h>

#include <csp/arch/csp_thread.h>

#include <libslash.h>

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef SLASH_ASF
#  include <stdio_serial.h>
#  include <si_stdio.h>
#endif

#ifdef TARGET_LIKE_KUBOS_MSP430_GCC
#  include <kubos-hal-msp430f5529/msp430-stdio.h>
#endif

/* Terminal codes */
#define ESC '\x1b'
#define DEL '\x7f'

#define CONTROL(code) (code - '@')
#define ESCAPE(code) "\x1b[0" code
#define ESCAPE_NUM(code) "\x1b[%u" code

/* Created by GCC before and after slash ELF section */
extern struct slash_command __start_slash;
extern struct slash_command __stop_slash;

/* Calculate command section size */
static const struct slash_command slash_size_dummy[2];
#define SLASH_COMMAND_SIZE ((intptr_t) &slash_size_dummy[1] - (intptr_t) &slash_size_dummy[0])

#define slash_for_each_command(_c) \
	for (_c = &__stop_slash-1; \
	     _c >= &__start_slash; \
	     _c = (struct slash_command *)(intptr_t)((char *)_c - SLASH_COMMAND_SIZE))

/* Terminal handling */
static size_t slash_escaped_strlen(const char *s)
{
	int len = 0;
	bool escaped = false;

	while (*s) {
		if (escaped) {
			if (*s == 'm')
				escaped = false;
		} else if (*s == ESC) {
			escaped = true;
		} else {
			len++;
		}
		s++;
	}

	return len;
}

static int slash_rawmode_enable(struct slash *slash)
{
#ifdef HAVE_TERMIOS_H
	struct termios raw;

	if (tcgetattr(slash->fd_read, &slash->original) < 0)
		return -ENOTTY;

	raw = slash->original;
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;

	if (tcsetattr(slash->fd_read, TCSANOW, &raw) < 0)
		return -ENOTTY;
#endif
	return 0;
}

static int slash_rawmode_disable(struct slash *slash)
{
#ifdef HAVE_TERMIOS_H
	if (tcsetattr(slash->fd_read, TCSANOW, &slash->original) < 0)
		return -ENOTTY;
#endif
	return 0;
}

static void slash_atexit(void)
{
	// FIXME slash_rawmode_disable(slash);
}

static int slash_configure_term(struct slash *slash)
{
	if (slash_rawmode_enable(slash) < 0)
		return -ENOTTY;

	slash->rawmode = true;

	if (!slash->atexit_registered) {
		atexit(slash_atexit);
		slash->atexit_registered = true;
	}

	return 0;
}

static int slash_restore_term(struct slash *slash)
{
	if (slash_rawmode_disable(slash) < 0)
		return -ENOTTY;

	slash->rawmode = false;

	return 0;
}

static int slash_write(struct slash *slash, const char *buf, size_t count)
{
	return write(slash->fd_write, buf, count);
}

static int slash_read(struct slash *slash, void *buf, size_t count)
{
	return read(slash->fd_read, buf, count);
}

static int slash_putchar(struct slash *slash, char c)
{
	return slash_write(slash, &c, 1);
}

static int slash_getchar(struct slash *slash)
{
	unsigned char c;

#if defined(SLASH_ASF)
	while (!stdio_is_byte_pending())
		vTaskDelay(1);
#endif

	// TODO: this delay loop is a workaround for not being able to peak ahead
	// in the STDIO queue in KubOS RT, we need to add that to our UART library
	while (slash_read(slash, &c, 1) < 1) {
		vTaskDelay(50);
	}

	return c;
}

static int slash_putnl(struct slash *slash)
{
    return slash_write(slash, SLASH_NL, SLASH_NL_LEN);
}

int slash_getchar_nonblock(struct slash *slash)
{
	unsigned char c;

#ifdef HAVE_FCNTL_H
	/* Set nonblocking */
	fcntl(slash->fd_read, F_SETFL, fcntl(slash->fd_read, F_GETFL) |  O_NONBLOCK);
#endif

	if (slash_read(slash, &c, 1) < 1)
		return -EIO;

#ifdef HAVE_FCNTL_H
	/* Set back to blocking */
	fcntl(slash->fd_read, F_SETFL, fcntl(slash->fd_read, F_GETFL) & ~O_NONBLOCK);
#endif

	return c;
}

int slash_printf(struct slash *slash, const char *format, ...)
{
	int ret;
	va_list args;

	va_start(args, format);

	ret = vdprintf(slash->fd_write, format, args);

	va_end(args);

	return ret;
}

static bool slash_line_empty(char *line, size_t linelen)
{
	while (*line && linelen--)
		if (!isspace(*line++))
			return false;

	return true;
}

static struct slash_command *
slash_command_find(struct slash *slash, char *line, size_t linelen, char **args)
{
	struct slash_command *cmd;

	/* Maximum length match */
	size_t max_matchlen = 0;
	struct slash_command *max_match_cmd = NULL;

	slash_for_each_command(cmd) {
		/* Skip commands that are longer than linelen */
		if (linelen < strlen(cmd->name))
			continue;

		/* Find an exact match */
		if (strncmp(line, cmd->name, strlen(cmd->name)) == 0) {

			/* Update the max-length match */
			if (strlen(cmd->name) > max_matchlen) {
				max_match_cmd = cmd;

				/* Calculate arguments position */
				*args = line + strlen(cmd->name);

			}

		}

	}

	return max_match_cmd;
}

static int slash_build_args(char *args, char **argv, int *argc)
{
	/* Quote level */
	enum {
		SLASH_QUOTE_NONE,
		SLASH_QUOTE_SINGLE,
		SLASH_QUOTE_DOUBLE,
	} quote = SLASH_QUOTE_NONE;

	*argc = 0;

	while (*args && *argc < SLASH_ARG_MAX) {
		/* Check for quotes */
		if (*args == '\'') {
			quote = SLASH_QUOTE_SINGLE;
			args++;
		} else if (*args == '\"') {
			quote = SLASH_QUOTE_DOUBLE;
			args++;
		}

		/* Argument starts here */
		argv[(*argc)++] = args;

		/* Loop over input argument */
		while (*args) {
			if (quote == SLASH_QUOTE_SINGLE && *args == '\'') {
				quote = SLASH_QUOTE_NONE;
				break;
			} else if (quote == SLASH_QUOTE_DOUBLE &&
				   *args == '\"') {
				quote = SLASH_QUOTE_NONE;
				break;
			} else if (quote == SLASH_QUOTE_NONE && *args == ' ') {
				break;
			}

			args++;
		}

		/* End argument with zero byte */
		if (*args)
			*args++ = '\0';

		/* Skip trailing white space */
		while (*args && *args == ' ')
			args++;
	}

	/* Test for quote mismatch */
	if (quote != SLASH_QUOTE_NONE)
		return -1;

	/* According to C11 section 5.1.2.2.1, argv[argc] must be NULL */
	argv[*argc] = NULL;

	return 0;
}

static void slash_command_usage(struct slash *slash, struct slash_command *command)
{
	const char *args = command->args ? command->args : "";
	const char *type = command->func ? "usage" : "group";
	slash_println(slash, "%s: %s %s", type, command->name, args);
}

static void slash_command_description(struct slash *slash, struct slash_command *command)
{
	slash_println(slash, "%-15s", command->name);
}

int slash_execute(struct slash *slash, char *line)
{
	struct slash_command *command;
	char *args, *argv[SLASH_ARG_MAX];
	int ret, argc = 0;

	command = slash_command_find(slash, line, strlen(line), &args);
	if (!command) {
		slash_println(slash, "No such command: %s", line);
		return -ENOENT;
	}

	if (!command->func) {
		// TODO: Help user here with autocomplete?
#if 0
		slash_printf(slash, "Available subcommands in \'%s\' group:\n",
			     command->name);
		slash_list_for_each(cur, &command->sub, command)
			slash_command_description(slash, cur);
#endif
		return -EINVAL;
	}

	/* Build args */
	if (slash_build_args(args, argv, &argc) < 0) {
		slash_println(slash, "Mismatched quotes");
		return -EINVAL;
	}

#ifdef HAVE_GETOPT
	/* Reset state for getopt(3) */
	optarg = 0;
	optind = 0;
	opterr = 1;
	optopt = '?';
#endif

	slash->argc = argc;
	slash->argv = argv;
	ret = command->func(slash);

	if (ret == SLASH_EUSAGE)
		slash_command_usage(slash, command);

	return ret;
}

/* Completion */
void slash_bell(struct slash *slash)
{
	slash_putchar(slash, '\a');
}

int slash_prefix_length(const char *s1, const char *s2)
{
	int len = 0;

	while (*s1 && *s2 && *s1 == *s2) {
		len++;
		s1++;
		s2++;
	}

	return len;
}

static void slash_complete(struct slash *slash)
{
	int matches = 0;
	size_t prefixlen = -1;
	struct slash_command *prefix = NULL;
	struct slash_command *cmd;
	size_t buffer_len = strlen(slash->buffer);

	slash_for_each_command(cmd) {

		if (strncmp(slash->buffer, cmd->name, slash_min(strlen(cmd->name), buffer_len)) == 0) {

			/* Count matches */
			matches++;

			/* Find common prefix */
			if (prefixlen == (size_t) -1) {
				prefix = cmd;
				prefixlen = strlen(prefix->name);
			} else {
				size_t new_prefixlen = slash_prefix_length(prefix->name, cmd->name);
				if (new_prefixlen < prefixlen)
					prefixlen = new_prefixlen;
			}

			/* Print newline on first match */
			if (matches == 1)
				slash_putnl(slash);

			/* We only print all commands over 1 match here */
			if (matches > 1)
				slash_command_description(slash, cmd);

		}

	}

	if (!matches) {
		slash_bell(slash);
	} else if (matches == 1) {
		if (slash->cursor <= prefixlen) {
			strncpy(slash->buffer, prefix->name, prefixlen);
			slash->buffer[prefixlen] = '\0';
			strcat(slash->buffer, " ");
			slash->cursor = slash->length = strlen(slash->buffer);
		} else {
			if (prefix->completer) {
				prefix->completer(slash, slash->buffer + prefixlen + 1);
			}
		}
	} else if (slash->last_char != '\t') {
		/* Print the first match as well */
		slash_command_description(slash, prefix);
		strncpy(slash->buffer, prefix->name, prefixlen);
		slash->buffer[prefixlen] = '\0';
		slash->cursor = slash->length = strlen(slash->buffer);
		slash_bell(slash);
	}

}

/* History */
static char *slash_history_increment(struct slash *slash, char *ptr)
{
	if (++ptr > &slash->history[slash->history_size-1])
		ptr = slash->history;
	return ptr;
}

static char *slash_history_decrement(struct slash *slash, char *ptr)
{
	if (--ptr < slash->history)
		ptr = &slash->history[slash->history_size-1];
	return ptr;
}

static void slash_history_push_head(struct slash *slash)
{
	*slash->history_head = '\0';
	slash->history_head = slash_history_increment(slash, slash->history_head);
	slash->history_avail++;
}

static void slash_history_push_tail(struct slash *slash, char c)
{
	*slash->history_tail = c;
	slash->history_tail = slash_history_increment(slash, slash->history_tail);
	slash->history_avail--;
}

static void slash_history_pull_tail(struct slash *slash)
{
	*slash->history_tail = '\0';
	slash->history_tail = slash_history_decrement(slash, slash->history_tail);
	slash->history_avail++;
}

static size_t slash_history_strlen(struct slash *slash, char *ptr)
{
	size_t len = 0;

	while (*ptr) {
		ptr = slash_history_increment(slash, ptr);
		len++;
	}

	return len;
}


static void slash_history_copy(struct slash *slash, char *dst, char *src, size_t len)
{
	while (len--) {
		*dst++ = *src;
		src = slash_history_increment(slash, src);
	}
}

static char *slash_history_search_back(struct slash *slash,
				       char *start, size_t *startlen)
{
	if (slash->history_cursor == slash->history_head)
		return NULL;

	/* Skip first two trailing zeros */
	start = slash_history_decrement(slash, start);
	start = slash_history_decrement(slash, start);

	while (*start) {
		if (start == slash->history_head)
			break;
		start = slash_history_decrement(slash, start);
	}

	/* Skip leading zero */
	if (start != slash->history_head)
		start = slash_history_increment(slash, start);

	*startlen = slash_history_strlen(slash, start);

	return start;
}

static char *slash_history_search_forward(struct slash *slash,
					  char *start, size_t *startlen)
{
	if (slash->history_cursor == slash->history_tail)
		return NULL;

	while (*start) {
		start = slash_history_increment(slash, start);
		if (start == slash->history_tail)
			return NULL;
	}

	/* Skip trailing zero */
	start = slash_history_increment(slash, start);

	if (start == slash->history_tail)
		*startlen = 0;
	else
		*startlen = slash_history_strlen(slash, start);

	return start;
}

static void slash_history_pull(struct slash *slash, size_t len)
{
	while (len-- > 0)
		slash_history_push_head(slash);
	while (*slash->history_head != '\0')
		slash_history_push_head(slash);

	/* Push past final zero byte */
	slash_history_push_head(slash);
}

static void slash_history_push(struct slash *slash, char *buf, size_t len)
{
	/* Remove oldest entry until space is available */
	if (len > slash->history_avail)
		slash_history_pull(slash, len - slash->history_avail);

	/* Copy to history */
	while (len--)
		slash_history_push_tail(slash, *buf++);

	slash->history_cursor = slash->history_tail;
}

static void slash_history_rewind(struct slash *slash, size_t len)
{
	while (len-- > 0)
		slash_history_pull_tail(slash);

	*slash->history_tail = '\0';

	slash->history_rewind_length = 0;
}

static void slash_history_add(struct slash *slash, char *line)
{
	/* Check if we are browsing history and clear latest entry */
	if (slash->history_depth != 0 && slash->history_rewind_length != 0)
		slash_history_rewind(slash, slash->history_rewind_length);

	/* Reset history depth */
	slash->history_depth = 0;
	slash->history_rewind_length = 0;
	slash->history_cursor = slash->history_tail;

	/* Check if last command was similar */
	size_t srclen;
	char *src = slash_history_search_back(slash, slash->history_cursor, &srclen);
	if ((src) && (strlen(line) == srclen) && (strncmp(src, line, srclen) == 0))
		return;

	/* Push including trailing zero */
	if (!slash_line_empty(line, strlen(line)))
		slash_history_push(slash, line, strlen(line) + 1);
}

static void slash_history_next(struct slash *slash)
{
	char *src;
	size_t srclen;

	src = slash_history_search_forward(slash, slash->history_cursor, &srclen);
	if (!src)
		return;

	slash->history_depth--;
	slash_history_copy(slash, slash->buffer, src, srclen);
	slash->buffer[srclen] = '\0';
	slash->history_cursor = src;
	slash->cursor = slash->length = srclen;

	/* Rewind if use to store buffer temporarily */
	if (!slash->history_depth && slash->history_cursor != slash->history_tail)
		slash_history_rewind(slash, slash->history_rewind_length);
}

static void slash_history_previous(struct slash *slash)
{
	char *src;
	size_t srclen, buflen;

	src = slash_history_search_back(slash, slash->history_cursor, &srclen);
	if (!src)
		return;

	/* Store current buffer temporarily */
	buflen = strlen(slash->buffer);
	if (!slash->history_depth && buflen) {
		slash_history_add(slash, slash->buffer);
		slash->history_rewind_length = buflen + 1;
	}

	slash->history_depth++;
	slash_history_copy(slash, slash->buffer, src, srclen);
	slash->buffer[srclen] = '\0';
	slash->history_cursor = src;
	slash->cursor = slash->length = srclen;
}

/* Line editing */
static void slash_insert(struct slash *slash, int c)
{
	if (slash->length + 1 < slash->line_size) {
		memmove(&slash->buffer[slash->cursor + 1],
			&slash->buffer[slash->cursor],
			slash->length - slash->cursor);
		slash->buffer[slash->cursor] = c;
		slash->cursor++;
		slash->length++;
		slash->buffer[slash->length] = '\0';
	}
}

static int slash_refresh(struct slash *slash)
{
	char esc[16];

	/* Ensure line is zero terminated */
	slash->buffer[slash->length] = '\0';

	/* Move cursor to left edge */
	snprintf(esc, sizeof(esc), "\r");
	if (slash_write(slash, esc, strlen(esc)) < 0)
		return -1;

	/* Write the prompt and the current buffer content */
	if (slash_write(slash, slash->prompt, slash->prompt_length) < 0)
		return -1;
	if (slash_write(slash, slash->buffer, slash->length) < 0)
		return -1;

	/* Erase to right */
	snprintf(esc, sizeof(esc), ESCAPE("K"));
	if (slash_write(slash, esc, strlen(esc)) < 0)
		return -1;

	/* Move cursor to original position. */
	snprintf(esc, sizeof(esc), "\r" ESCAPE_NUM("C"),
		(unsigned int)(slash->cursor + slash->prompt_print_length));
	if (slash_write(slash, esc, strlen(esc)) < 0)
		return -1;

	return 0;
}

static void slash_reset(struct slash *slash)
{
	slash->buffer[0] = '\0';
	slash->length = 0;
	slash->cursor = 0;
}

static void slash_arrow_up(struct slash *slash)
{
	slash_history_previous(slash);
}

static void slash_arrow_down(struct slash *slash)
{
	slash_history_next(slash);
}

static void slash_arrow_right(struct slash *slash)
{
	if (slash->cursor < slash->length)
		slash->cursor++;
}

static void slash_arrow_left(struct slash *slash)
{
	if (slash->cursor > 0)
		slash->cursor--;
}

static void slash_delete(struct slash *slash)
{
	if (slash->cursor < slash->length) {
		slash->length--;
		memmove(&slash->buffer[slash->cursor],
			&slash->buffer[slash->cursor + 1], slash->length - slash->cursor);
		slash->buffer[slash->length] = '\0';
	}
}

static void slash_clear_screen(struct slash *slash)
{
	char *esc = (char *) ESCAPE("H") ESCAPE("2J");
	slash_write(slash, esc, strlen(esc));
}

static void slash_backspace(struct slash *slash)
{
	if (slash->cursor > 0) {
		slash->cursor--;
		slash->length--;
		memmove(&slash->buffer[slash->cursor],
			&slash->buffer[slash->cursor + 1], slash->length - slash->cursor);
		slash->buffer[slash->length] = '\0';
	}
}

static void slash_delete_word(struct slash *slash)
{
	int old_cursor = slash->cursor, erased;

	while (slash->cursor > 0 && slash->buffer[slash->cursor-1] == ' ')
		slash->cursor--;
	while (slash->cursor > 0 && slash->buffer[slash->cursor-1] != ' ')
		slash->cursor--;

	erased = old_cursor - slash->cursor;

	memmove(slash->buffer + slash->cursor, slash->buffer + old_cursor, erased);
	slash->length -= erased;
}

static void slash_swap(struct slash *slash)
{
	char tmp;

	if (slash->cursor > 0 && slash->cursor < slash->length) {
		tmp = slash->buffer[slash->cursor-1];
		slash->buffer[slash->cursor-1] = slash->buffer[slash->cursor];
                slash->buffer[slash->cursor] = tmp;
                if (slash->cursor != slash->length-1)
			slash->cursor++;
	}
}

char *slash_readline(struct slash *slash, const char *prompt)
{
	char *ret = slash->buffer;
	int c, esc[3];
	bool done = false, escaped = false;

	if (slash_configure_term(slash) < 0)
		return NULL;

	slash->prompt = prompt;
	slash->prompt_length = strlen(prompt);
	slash->prompt_print_length = slash_escaped_strlen(prompt);

	/* Reset buffer */
	slash_reset(slash);
	slash_refresh(slash);

	while (!done && ((c = slash_getchar(slash)) >= 0)) {
		if (escaped) {
			esc[0] = c;
			esc[1] = slash_getchar(slash);

			if (esc[0] == '[' && esc[1] == 'A') {
				slash_arrow_up(slash);
			} else if (esc[0] == '[' && esc[1] == 'B') {
				slash_arrow_down(slash);
			} else if (esc[0] == '[' && esc[1] == 'C') {
				slash_arrow_right(slash);
			} else if (esc[0] == '[' && esc[1] == 'D') {
				slash_arrow_left(slash);
			} else if (esc[0] == '[' && (esc[1] > '0' &&
						     esc[1] < '7')) {
				esc[2] = slash_getchar(slash);
				if (esc[1] == '3' && esc[2] == '~')
					slash_delete(slash);
			} else if (esc[0] == 'O' && esc[1] == 'H') {
				slash->cursor = 0;
			} else if (esc[0] == 'O' && esc[1] == 'F') {
				slash->cursor = slash->length;
			} else if (esc[0] == '1' && esc[1] == '~') {
				slash->cursor = 0;
			} else if (esc[0] == '4' && esc[1] == '[') {
				esc[2] = slash_getchar(slash);
				if (esc[2] == '~')
					slash->cursor = slash->length;
			}
			escaped = false;
		} else if (iscntrl(c)) {
			switch (c) {
			case CONTROL('A'):
				slash->cursor = 0;
				break;
			case CONTROL('B'):
				slash_arrow_left(slash);
				break;
			case CONTROL('C'):
				slash_reset(slash);
				done = true;
				break;
			case CONTROL('D'):
				if (slash->length > 0) {
					slash_delete(slash);
				} else {
					ret = NULL;
					done = true;
				}
				break;
			case CONTROL('E'):
				slash->cursor = slash->length;
				break;
			case CONTROL('F'):
				slash_arrow_right(slash);
				break;
			case CONTROL('K'):
				slash->length = slash->cursor;
				break;
			case CONTROL('L'):
				slash_clear_screen(slash);
				break;
			case CONTROL('N'):
				slash_arrow_down(slash);
				break;
			case CONTROL('P'):
				slash_arrow_up(slash);
				break;
			case CONTROL('T'):
				slash_swap(slash);
				break;
			case CONTROL('U'):
				slash->cursor = 0;
				slash->length = 0;
				break;
			case CONTROL('W'):
				slash_delete_word(slash);
				break;
			case '\t':
				slash_complete(slash);
				break;
			case '\r':
			case '\n':
				done = true;
				break;
			case '\b':
			case DEL:
				slash_backspace(slash);
				break;
			case ESC:
				escaped = true;
				break;
			default:
				/* Unknown control */
				break;
			}
		} else if (isprint(c)) {
			/* Add to buffer */
			slash_insert(slash, c);
		}

		slash->last_char = c;

		if (!done)
			slash_refresh(slash);
	}

	slash_restore_term(slash);
	slash_putnl(slash);
	slash_history_add(slash, slash->buffer);

	return ret;
}

/* Builtin commands */
static int slash_builtin_help(struct slash *slash)
{
	char *args;
	char find[slash->line_size];
	int i;
	size_t available = sizeof(find);
	struct slash_command *command;

	/* If no arguments given, just list all top-level commands */
	if (slash->argc < 2) {
		struct slash_command *cmd;
		slash_for_each_command(cmd) {
			slash_command_description(slash, cmd);
		}
		return SLASH_SUCCESS;
	}

	find[0] = '\0';

	for (i = 1; i < slash->argc; i++) {
		if (strlen(slash->argv[i]) >= (size_t) available)
			return SLASH_ENOSPC;
		strcat(find, slash->argv[i]);
		strcat(find, " ");
	}
	command = slash_command_find(slash, find, strlen(find), &args);
	if (!command) {
		slash_println(slash, "No such command: %s", find);
		return SLASH_EINVAL;
	}

	slash_command_usage(slash, command);

	return SLASH_SUCCESS;
}
slash_command(help, slash_builtin_help, "[command]",
	      "Show available commands");

static int slash_builtin_history(struct slash *slash)
{
	char *p = slash->history_head;

	while (p != slash->history_tail) {
		if (*p) {
			slash_putchar(slash, *p);
		} else {
			slash_putnl(slash);
		}
		p = slash_history_increment(slash, p);
	}

	return SLASH_SUCCESS;
}
slash_command(history, slash_builtin_history, NULL,
	      "Show previous commands");

#if !SLASH_NOEXIT
static int slash_builtin_exit(struct slash *slash)
{
	return SLASH_EXIT;
}
slash_command(exit, slash_builtin_exit, NULL,
	      "Exit slash");
#endif

static int slash_builtin_watch(struct slash *slash)
{
	if (slash->argc < 3)
		return SLASH_EUSAGE;

	char * endptr = NULL;
	unsigned int interval = strtoul(slash->argv[1], &endptr, 10);
	if (*endptr != '\0')
		return SLASH_EUSAGE;

	slash_println(slash, "Executing \"%s\" each %u ms - press <enter> to stop",
			slash->argv[2], interval);

	/* Take a compy of the cmd */
	char cmd[slash->line_size];
	strncpy(cmd, slash->argv[2], slash->line_size);

	while(1) {

		/* Make another copy, since slash_exec will modify this */
		char cmd_exec[slash->line_size];
		strncpy(cmd_exec, cmd, slash->line_size);

		/* Execute command */
		slash_execute(slash, cmd_exec);

		/* Delay (press enter to exit) */
#if defined(SLASH_ASF)
		if (stdio_is_byte_pending())
#else
		if (slash_getchar_nonblock(slash) != -EIO)
#endif
			break;
		csp_sleep_ms(interval);
#if defined(SLASH_ASF)
		if (stdio_is_byte_pending())
#else
		if (slash_getchar_nonblock(slash) != -EIO)
#endif
			break;

	}

	return SLASH_SUCCESS;
}
slash_command(watch, slash_builtin_watch, "<interval> <cmd>", "Loop command");

/* Core */
int slash_loop(struct slash *slash, const char *prompt_good, const char *prompt_bad)
{
	int ret;
	char *line;
	const char *prompt = prompt_good;

	while ((line = slash_readline(slash, prompt))) {
		if (!slash_line_empty(line, strlen(line))) {
			/* Run command */
			ret = slash_execute(slash, line);
			if (ret == SLASH_EXIT)
				break;

			/* Update prompt if enabled */
			if (prompt_bad && ret != SLASH_SUCCESS)
				prompt = prompt_bad;
			else
				prompt = prompt_good;
		} else {
			prompt = prompt_good;
		}
	}

	return 0;
}

struct slash *slash_create(size_t line_size, size_t history_size)
{
	struct slash *slash;

	/* Allocate slash context */
	slash = calloc(sizeof(*slash), 1);
	if (!slash)
		return NULL;

	/* Setup default values */
	slash->fd_read = STDIN_FILENO;
	slash->fd_write = STDOUT_FILENO;

	/* Allocate zero-initialized line and history buffers */
	slash->line_size = line_size;
	slash->buffer = calloc(slash->line_size, 1);
	if (!slash->buffer) {
		free(slash);
		return NULL;
	}

	slash->history_size = history_size;
	slash->history = calloc(slash->history_size, 1);
	if (!slash->history) {
		free(slash->buffer);
		free(slash);
		return NULL;
	}

	/* Initialize history */
	slash->history_head = slash->history;
	slash->history_tail = slash->history;
	slash->history_cursor = slash->history;
	slash->history_avail = slash->history_size - 1;

	/* Register commands */
	/*
	slash_list_init(&slash->commands);
	slash_for_each_command(cmd)
		slash_command_register(slash, cmd, cmd->group);
	*/

	return slash;
}

void slash_destroy(struct slash *slash)
{
	if (slash->buffer) {
		free(slash->buffer);
		slash->buffer = NULL;
	}
	if (slash->history) {
		free(slash->history);
		slash->history = NULL;
	}
}
