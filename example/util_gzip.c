/* @file: util_gzip.c
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
#include <demoz/lib/crc.h>
#include <demoz/lib/deflate.h>


static void _usage(void)
{
	printf(
		"Usage: gzip [OPTION...] [<stdin>]\n"
		" gzip (DEFLATE) compression utility.\n"
		"\n"
		" -#    compress level 0..9 (default 6)\n"
		" -v    show compress radio\n"
		" -h    display help\n"
		);
}

static DEFLATE_NEW(ctx);

static int32_t _gzip(FILE *rfp, FILE *wfp, int32_t lev, int32_t is_v)
{
	uint8_t buf[8192];
	const uint32_t *crc_t;
	uint32_t crc = 0xffffffff;
	size_t total_len = 0, send_len = 0, len = 0;
	int32_t r;

	if (F_SYMBOL(deflate_init)(&ctx, lev)) {
		fprintf(stderr, "deflate_init() initialize error!\n");
		return -1;
	}
	crc_t = F_SYMBOL(crc32_table)(CRC32_DEFAULT_LSB_TYPE);

	buf[0] = 0x1f;
	buf[1] = 0x8b;
	buf[2] = 0x08;
	buf[3] = 0x00;
	buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;
	buf[8] = 0x00;
	buf[9] = 0x03;

	send_len = 10;
	fwrite(buf, 1, 10, wfp);

	while ((len = fread(buf, 1, sizeof(buf), rfp))) {
		do {
			r = F_SYMBOL(deflate)(&ctx, buf, len, 0);
			if (r) {
				send_len += DEFLATE_LEN(&ctx);
				fwrite(DEFLATE_BUF(&ctx),
					1, DEFLATE_LEN(&ctx), wfp);
			}
		} while (r);

		total_len += len;
		crc = F_SYMBOL(crc32_lsb)(crc_t, crc, buf, len);
	}

	do {
		r = F_SYMBOL(deflate)(&ctx, NULL, 0, 1);
		if (r) {
			send_len += DEFLATE_LEN(&ctx);
			fwrite(DEFLATE_BUF(&ctx),
				1, DEFLATE_LEN(&ctx), wfp);
			if (r == DEFLATE_IS_END)
				break;
		}
	} while (r);

	crc ^= 0xffffffff;
	buf[0] = (uint8_t)crc;
	buf[1] = (uint8_t)(crc >> 8);
	buf[2] = (uint8_t)(crc >> 16);
	buf[3] = (uint8_t)(crc >> 24);
	buf[4] = (uint8_t)total_len;
	buf[5] = (uint8_t)(total_len >> 8);
	buf[6] = (uint8_t)(total_len >> 16);
	buf[7] = (uint8_t)(total_len >> 24);

	send_len += 8;
	fwrite(buf, 1, 8, wfp);

	if (is_v) {
		fprintf(stderr, "%zu (%zuK) / %zu (%zuK) = %.2f%% (%08x)\n",
			send_len, (send_len / 1024),
			total_len, (total_len / 1024),
			(((double)total_len - send_len) / total_len) * 100,
			crc);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int32_t r, ind = 1;
	char *arg = NULL;
	int32_t is_v = 0, lev = 6;

	while ((r = C_SYMBOL(getopt_r)(argc, argv, "hv0123456789", &arg, &ind)) != -1) {
		switch (r) {
			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				lev = r - '0';
				break;
			case 'v':
				is_v = 1;
				break;
			case 'h':
				_usage();
				return 0;
			default:
				printf("unknown '%c' option!\n", *arg);
				return 1;
		}
	}

	if (_gzip(stdin, stdout, lev, is_v))
		return 1;

	return 0;
}
