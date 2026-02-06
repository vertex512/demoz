/* @file: getopt.c
 * #desc:
 *    The implementations of command-line options parse.
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/getopt.h>


/* @func: getopt_r
 * #desc:
 *    command-line short options parse.
 *
 * #1: argc
 * #2: argv
 * #3: options string
 * #4: argument pointer
 * #5: current argv index
 * #r:
 *    -1: parseing end, ':': missing argument, '?': unknown option,
 *    '<opt-character>': option character
 */
int32_t C_SYMBOL(getopt_r)(int32_t argc, char *const *argv,
		const char *optstr, char **arg, int32_t *ind)
{
	if (*ind >= argc)
		return -1;

	char *opt = *arg;
	if (!opt) {
		opt = *arg = argv[*ind];
		if (opt[0] != '-') { /* non option argument */
			if (opt[0] != '\0' && *optstr == '-') {
				(*ind)++;
				return 1;
			}
			return '?';
		}
		if (opt[1] == '-') /* option '--' */
			return -1;
		if (opt[1] == '\0') { /* option '-' */
			*arg = NULL;
			(*ind)++;
			return '-';
		}
		*arg = ++opt;
	}
	if (*opt == '\0')
		return '?';

	if (*optstr == '-' || *optstr == '+')
		optstr++;

	while (*optstr != '\0') {
		if (*opt != *optstr) {
			if (optstr[1] == ':') {
				if (optstr[2] == ':')
					optstr++;
				optstr++;
			}
			optstr++;
			continue;
		}

		if (optstr[1] == ':') {
			/* optional argument */
			if (optstr[2] == ':') { /* option '-o'[...] */
				(*arg)++;
				if (opt[1] == '\0') /* optional */
					*arg = NULL;
				(*ind)++;
				return *opt;
			}

			/* with option argument */
			if (opt[1] == '\0') { /* next a argv */
				if (++(*ind) >= argc) {
					*arg = opt;
					return ':';
				}
				/* option '-o' '[...]' */
				*arg = argv[*ind];
				(*ind)++;
				return *opt;
			}

			/* option '-o[...]' */
			(*arg)++;
			(*ind)++;
			return *opt;
		}

		/* no option argument */
		(*arg)++;
		if (opt[1] == '\0') { /* next a argv */
			*arg = NULL;
			(*ind)++;
		}
		return *opt;
	}

	return '?';
}

/* @func: getopt_long_r
 * #desc:
 *    command-line long options parse.
 *
 * #1: argc
 * #2: argv
 * #3: short options string
 * #4: long options string
 * #5: long option index
 * #6: argument pointer
 * #7: current argv index
 * #r:
 *    -1: parseing end, ':': missing argument, '?': unknown option,
 *    '<opt-character>': option character/number, 0: use long option flag
 */
int32_t C_SYMBOL(getopt_long_r)(int32_t argc, char *const *argv,
		const char *optstr, const struct option_r *longopts,
		int32_t *longind, char **arg, int32_t *ind)
{
	if (*ind >= argc)
		return -1;

	char *opt = argv[*ind];
	*longind = -1;

	if (!*arg && opt[0] == '-' && opt[1] == '-' && opt[2] != '\0') {
		*arg = opt;
		opt += 2;
		for (int32_t i = 0; longopts->name; longopts++, i++) {
			const char *p = longopts->name;
			for (; *p != '\0'; p++, opt++) {
				if (*p != *opt)
					break;
			}

			*longind = i;
			if (!(*opt == '=' || *opt == '\0') || *p != '\0')
				continue;

			switch (longopts->has_arg) {
				case REQUIRED_ARGUMENT:
					if (*opt == '=') {
						*arg = ++opt;
						(*ind)++;
						break;
					}
					if (*opt == '\0') {
						if (++(*ind) >= argc)
							return ':';
						*arg = argv[*ind];
						(*ind)++;
						break;
					}
					break;
				case OPTIONAL_ARGUMENT:
					if (*opt == '=') {
						*arg = ++opt;
						(*ind)++;
						break;
					}
				case NO_ARGUMENT:
					*arg = NULL;
					(*ind)++;
					break;
				default:
					return -1;
			}

			if (longopts->flag) {
				*longopts->flag = longopts->val;
				return 0;
			}
			return longopts->val;
		}

		return '?';
	}

	return C_SYMBOL(getopt_r)(argc, argv, optstr, arg, ind);
}
