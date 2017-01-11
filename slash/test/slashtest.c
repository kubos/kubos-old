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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <slash/slash.h>

#define LINE_SIZE		128
#define HISTORY_SIZE		4096

static int cmd_long(struct slash *slash)
{
	printf("Running long command\n");

	return SLASH_SUCCESS;
}
slash_command(long_command, cmd_long, NULL,
	      "A long command\n");

static int cmd_longer(struct slash *slash)
{
	printf("Running longer command\n");

	return SLASH_SUCCESS;
}
slash_command(longer_command, cmd_longer, NULL,
	      "An even longer command\n");

static int cmd_test(struct slash *slash)
{
	if (slash->argc < 2)
		return SLASH_EUSAGE;

	printf("Running test command\n");

	return SLASH_SUCCESS;
}
slash_command(test, cmd_test, "<arg>",
	      "A simple test command\n\n"
	      "Requires a single command which must be present\n");

static int cmd_subcommand(struct slash *slash)
{
	printf("Running the test subcommand\n");

	return SLASH_SUCCESS;
}
slash_command_sub(test, sub_command, cmd_subcommand, NULL,
		  "A subcommand for test");

static int cmd_args(struct slash *slash)
{
	int i;

	printf("Got %d args:\n", slash->argc);

	for (i = 0; i < slash->argc; i++)
		printf("arg %d: \'%s\'\n", i, slash->argv[i]);

	return SLASH_SUCCESS;
}
slash_command_group(group, "Empty command group");
slash_command_sub(group, args, cmd_args, "[arg ...]",
		  "Print argument list");

int main(int argc, char **argv)
{
	struct slash *slash;

	char *prompt_good = "\033[96mslash \033[90m%\033[0m ";
	char *prompt_bad = "\033[96mslash \033[31m!\033[0m ";

	slash = slash_create(LINE_SIZE, HISTORY_SIZE);
	if (!slash) {
		fprintf(stderr, "Failed to init slash\n");
		exit(EXIT_FAILURE);
	}

	slash_loop(slash, prompt_good, prompt_bad);
	slash_destroy(slash);

	return 0;
}
