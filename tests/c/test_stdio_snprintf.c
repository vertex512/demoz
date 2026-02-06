/* @file: test_stdio_snprintf.c
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
#include <demoz/c/float.h>
#include <demoz/c/stdio.h>


#define OUT(s, n) printf("%3d:%s", n, s)

char g_buf[4096];

int main(void)
{
	int32_t len = sizeof(g_buf), r;

	/* signed */
	r = snprintf(g_buf, len,
		" libc: hhd: %hhd %hhd\n", INT8_MIN, INT8_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhd: %hhd %hhd\n", INT8_MIN, INT8_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hhd: %hhd %hhd\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhd: %hhd %hhd\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hd: %hd %hd\n", INT16_MIN, INT16_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hd: %hd %hd\n", INT16_MIN, INT16_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hd: %hd %hd\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hd: %hd %hd\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: d: %d %d\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: d: %d %d\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: d: %d %d\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: d: %d %d\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: ld: %ld %ld\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: ld: %ld %ld\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: lld: %lld %lld\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: lld: %lld %lld\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	/* unsigned */
	r = snprintf(g_buf, len,
		" libc: hhu: %hhu\n", UINT8_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhu: %hhu\n", UINT8_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hhu: %hhu\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhu: %hhu\n", UINT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hu: %hu\n", UINT16_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hu: %hu\n", UINT16_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hu: %hu\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hu: %hu\n", UINT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: u: %u\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: u: %u\n", UINT32_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: u: %u\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: u: %u\n", UINT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: lu: %lu\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: lu: %lu\n", UINT64_MAX);
	OUT(g_buf, r);

	/* signed */
	r = snprintf(g_buf, len,
		" libc: hhx: %hhx %hhx\n", INT8_MIN, INT8_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhx: %hhx %hhx\n", INT8_MIN, INT8_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hhx: %hhx %hhx\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhx: %hhx %hhx\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hx: %hx %hx\n", INT16_MIN, INT16_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hx: %hx %hx\n", INT16_MIN, INT16_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hx: %hx %hx\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hx: %hx %hx\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: x: %x %x\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: x: %x %x\n", INT32_MIN, INT32_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: x: %x %x\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: x: %x %x\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: lx: %lx %lx\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: lx: %lx %lx\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: llx: %llx %llx\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: llx: %llx %llx\n", INT64_MIN, INT64_MAX);
	OUT(g_buf, r);

	/* unsigned hex */
	r = snprintf(g_buf, len,
		" libc: llu: %llu\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: llu: %llu\n", UINT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hhx: %hhx\n", UINT8_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhx: %hhx\n", UINT8_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hhx: %hhx\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hhx: %hhx\n", UINT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: hx: %hx\n", UINT16_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hx: %hx\n", UINT16_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: hx: %hx\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: hx: %hx\n", UINT32_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: x: %x\n", UINT32_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: x: %x\n", UINT32_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: x: %x\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: x: %x\n", UINT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: lx: %lx\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: lx: %lx\n", UINT64_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: llx: %llx\n", UINT64_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: llx: %llx\n", UINT64_MAX);
	OUT(g_buf, r);

	/* floating */
	r = snprintf(g_buf, len,
		" libc: f: %.380f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: f: %.380f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: f: %.380f\n", -X_FP_DBL_MAX);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: f: %.380f\n", -X_FP_DBL_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: f: %.380f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: f: %.380f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: f: %.380f\n", -X_FP_DBL_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: f: %.380f\n", -X_FP_DBL_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: f: %.380f\n", -X_FP_DBL_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: f: %.380f\n", -X_FP_DBL_MIN);
	OUT(g_buf, r);

	/* character and string */
	r = snprintf(g_buf, len,
		" libc: c: %c\n", 'w');
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: c: %c\n", 'w');
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: c: %9c\n", 'w');
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: c: %9c\n", 'w');
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %s\n", "Hello, World");
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %s\n", "Hello, World");
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %19s\n", "Hello, World");
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %19s\n", "Hello, World");
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %-19s-\n", "Hello, World");
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %-19s-\n", "Hello, World");
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %33s-\n", "Hello, World");
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %33s-\n", "Hello, World");
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %33s ω %s-\n", "Hello, World ω", NULL);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %33s ω %s-\n", "Hello, World ω", NULL);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: s: %*s ω %s-\n", -33, "Hello, World ω", NULL);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: s: %*s ω %s-\n", -33, "Hello, World ω", NULL);
	OUT(g_buf, r);

	/* align */
	r = snprintf(g_buf, len,
		" libc: %20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %020lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %020lx$\n", INT64_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %-20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %-20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %-020lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %-020lx$\n", INT64_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %-.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %-.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %-0.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %-0.20lx$\n", INT64_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %#.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %#.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %40.20lx$\n", INT64_MIN);
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %40.20lx$\n", INT64_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %040c$\n", 'A');
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %040c$\n", 'A');
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %40c$\n", 'A');
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %40c$\n", 'A');
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		" libc: %-40c$\n", 'A');
	OUT(g_buf, r);
	r = C_SYMBOL(snprintf)(g_buf, len,
		"demo1: %-40c$\n", 'A');
	OUT(g_buf, r);

	/* floating */
	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 0.0);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 0.0);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 17.9729);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 17.9729);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 1.79729);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 1.79729);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 0.179729);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 0.179729);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.0f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.0f\n", X_FP_DBL_MIN);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.0f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.0f\n", X_FP_DBL_MAX);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 0.001897);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 0.001897);
	OUT(g_buf, r);

	r = snprintf(g_buf, len,
		"demo1: %.324f\n", 527.1481286241434);
	OUT(g_buf, r);
	r = snprintf(g_buf, len,
		" libc: %.324f\n", 527.1481286241434);
	OUT(g_buf, r);

	return 0;
}

