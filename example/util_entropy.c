/* @file: util_entropy.c
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
#include <demoz/c/math.h>


int main(void)
{
	uint8_t buf[4096];
	size_t count[256];
	size_t total = 0;

	for (int i = 0; i < 256; i++)
		count[i] = 0;

	size_t n = 0;
	while ((n = fread(buf, 1, sizeof(buf), stdin))) {
		for (size_t i = 0; i < n; i++)
			count[buf[i]]++;
		total += n;
	}

	double h = 0;
	for (int i = 0; i < 256; i++) {
		if (count[i]) {
			double p = (double)count[i] / total;
			h -= p * C_SYMBOL(log2)(p);
		}
	}

	printf("bit/entropy: %.3f (%zu byte)\n", h, total);

	return 0;
}
