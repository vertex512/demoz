/* @file: test_getopt.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/getopt.h>


int main(int argc, char **argv)
{
	int32_t r, longind, ind = 1;
	char *arg = NULL;

	static int32_t flag;
	static const struct option_r longopts[] = {
		{ "along", NO_ARGUMENT,       NULL,  'a' },
		{ "blong", REQUIRED_ARGUMENT, NULL,  'b' },
		{ "clong", NO_ARGUMENT,       NULL,  'c' },
		{ "olong", OPTIONAL_ARGUMENT, NULL,  'o' },
		{ "flag",  NO_ARGUMENT,       &flag, 'f' },
		{ NULL, 0, NULL, 0 },
		};

	while ((r = C_SYMBOL(getopt_long_r)(argc, argv, "-ab:co::+",
			longopts, &longind, &arg, &ind)) != -1) {
		switch (r) {
			case 0:
				printf("flag option '%c'\n", flag);
				break;
			case 1:
				printf("non option argument '%s'\n", arg);
				arg = NULL;
				break;
			case 'a':
				if (longind != -1) {
					printf("option long 'a'\n");
				} else {
					printf("option 'a'\n");
				}
				break;
			case 'b':
				if (longind != -1) {
					printf("option long 'b' with argument '%s'\n", arg);
				} else {
					printf("option 'b' with argument '%s'\n", arg);
				}
				arg = NULL;
				break;
			case 'c':
				if (longind != -1) {
					printf("option long 'c'\n");
				} else {
					printf("option 'c'\n");
				}
				break;
			case 'o':
				if (longind != -1) {
					if (arg) {
						printf("option long 'o' with argument '%s'\n", arg);
						arg = NULL;
					} else {
						printf("option long 'o' optional argument\n");
					}
				} else {
					if (arg) {
						printf("option 'o' with argument '%s'\n", arg);
						arg = NULL;
					} else {
						printf("option 'o' optional argument\n");
					}
				}
				break;
			case '+':
				printf("option '+'\n");
				break;
			case '-':
				printf("option '-'\n");
				break;
			case ':':
				if (longind != -1) {
					printf("option long %s needs a value!\n", arg);
				} else {
					printf("option -%c needs a value!\n", *arg);
				}
				return 1;
			default:
				if (longind != -1) {
					printf("unknown long '%s' option!\n", arg);
				} else {
					printf("unknown '%c' option!\n", *arg);
				}
				return 1;
		}
	}

	return 0;
}
