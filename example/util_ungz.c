/* @file: util_ungz.c
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
#include <demoz/c/string.h>
#include <demoz/c/getopt.h>
#include <demoz/lib/crc.h>
#include <demoz/lib/inflate.h>


static void _usage(void)
{
	printf(
		"Usage: ungz [OPTION...] [<stdin>]\n"
		" gzip (INFLATE) uncompression utility.\n"
		"\n"
		" -v    show compress radio\n"
		" -h    display help\n"
		);
}

static INFLATE_NEW(ctx);

static int32_t _ungz(FILE *rfp, FILE *wfp, int32_t is_v)
{
	uint8_t buf[8192];
	const uint32_t *crc_t;
	uint32_t crc = 0xffffffff, in_crc;
	size_t total_len = 0, send_len = 0, len = 0;
	int32_t r;

	F_SYMBOL(inflate_init)(&ctx);
	crc_t = F_SYMBOL(crc32_table)(CRC32_DEFAULT_LSB_TYPE);

	total_len = 10;
	fread(buf, 1, 10, rfp);

	if (buf[3] & 0x04) {
		fread(buf + 4, 1, 2, rfp);
		fread(buf + 4, 1, (uint16_t)buf[4]
			| (uint16_t)buf[4] << 8, rfp);
	}
	if (buf[3] & 0x08) {
		do {
			fread(buf + 4, 1, 1, rfp);
		} while (buf[4]);
	}
	if (buf[3] & 0x10) {
		do {
			fread(buf + 4, 1, 1, rfp);
		} while (buf[4]);
	}
	if (buf[3] & 0x02)
		fread(buf + 4, 1, 2, rfp);

	while ((len = fread(buf, 1, sizeof(buf), rfp))) {
		total_len += len;
		do {
			r = F_SYMBOL(inflate)(&ctx, buf, len, 0);
			if (r < 0) {
				fprintf(stderr, "inflate() error!\n");
				return -1;
			}
			if (r) {
				send_len += INFLATE_LEN(&ctx);
				fwrite(INFLATE_BUF(&ctx),
					1, INFLATE_LEN(&ctx), wfp);
				crc = F_SYMBOL(crc32_lsb)(crc_t, crc,
					INFLATE_BUF(&ctx), INFLATE_LEN(&ctx));
				if (r == INFLATE_IS_END)
					goto e;
			}
		} while (r);
	}

	do {
		r = F_SYMBOL(inflate)(&ctx, NULL, 0, 1);
		if (r < 0) {
			fprintf(stderr, "inflate() error!\n");
			return -1;
		}
		if (r) {
			send_len += INFLATE_LEN(&ctx);
			fwrite(INFLATE_BUF(&ctx),
				1, INFLATE_LEN(&ctx), wfp);
			crc = F_SYMBOL(crc32_lsb)(crc_t, crc,
				INFLATE_BUF(&ctx), INFLATE_LEN(&ctx));
			if (r == INFLATE_IS_END)
				goto e;
		}
	} while (r);
e:

	if (INFLATE_OFFSET(&ctx, len) == len) {
		fread(buf, 1, 8, rfp);
	} else {
		C_SYMBOL(memcpy)(buf, buf + INFLATE_OFFSET(&ctx, len),
			len - INFLATE_OFFSET(&ctx, len));
		len = len - INFLATE_OFFSET(&ctx, len);
		if (len < 8) {
			total_len += 8 - len;
			fread(buf + len, 1, 8 - len, rfp);
		}
	}

	crc ^= 0xffffffff;
	in_crc = (uint32_t)buf[0]
		| (uint32_t)buf[1] << 8
		| (uint32_t)buf[2] << 16
		| (uint32_t)buf[3] << 24;

	if (is_v) {
		fprintf(stderr, "%zu (%zuK) / %zu (%zuK) = %.2f%% (%08x %08x)\n",
			total_len, (total_len / 1024),
			send_len, (send_len / 1024),
			(((double)send_len - total_len) / send_len) * 100,
			crc, in_crc);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int32_t r, ind = 1;
	char *arg = NULL;
	int32_t is_v = 0;

	while ((r = C_SYMBOL(getopt_r)(argc, argv, "hv", &arg, &ind)) != -1) {
		switch (r) {
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

	if (_ungz(stdin, stdout, is_v))
		return 1;

	return 0;
}
