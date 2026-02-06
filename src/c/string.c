/* @file: string.c
 * #desc:
 *    The implementations of string operations.
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
#include <demoz/c/string.h>


/* @func: memcpy
 * #desc:
 *    copy memory area.
 *
 * #1: target pointer
 * #2: source pointer
 * #3: length
 * #r: target pointer
 */
void *C_SYMBOL(memcpy)(void *t, const void *s, size_t len)
{
	if (!len) /* len is zero */
		return t;

#if 1 /* little endian */
#	define LS >>
#	define RS <<
#else
#	define LS <<
#	define RS >>
#endif

	volatile uint8_t *_t = t, *_s = (uint8_t *)s;
	uint32_t w, x;

	for (; ((uintptr_t)_s & 3) && len; len--)
		*_t++ = *_s++;

	switch ((uintptr_t)_t & 3) {
		case 0:
			for (; len > 15; len -= 16) {
				((uint32_t *)_t)[0] = ((uint32_t *)_s)[0];
				((uint32_t *)_t)[1] = ((uint32_t *)_s)[1];
				((uint32_t *)_t)[2] = ((uint32_t *)_s)[2];
				((uint32_t *)_t)[3] = ((uint32_t *)_s)[3];
				_t += 16;
				_s += 16;
			}
			break;
		case 1:
			if (len < 32)
				break;

			w = *((uint32_t *)_s);
			*_t++ = *_s++;
			*_t++ = *_s++;
			*_t++ = *_s++;
			len -= 3;

			for (; len > 16; len -= 16) {
				x = *((uint32_t *)(_s + 1));
				((uint32_t *)_t)[0] = (w LS 24) | (x RS 8);
				w = *((uint32_t *)(_s + 5));
				((uint32_t *)_t)[1] = (x LS 24) | (w RS 8);
				x = *((uint32_t *)(_s + 9));
				((uint32_t *)_t)[2] = (w LS 24) | (x RS 8);
				w = *((uint32_t *)(_s + 13));
				((uint32_t *)_t)[3] = (x LS 24) | (w RS 8);
				_t += 16;
				_s += 16;
			}
			break;
		case 2:
			if (len < 32)
				break;

			w = *((uint32_t *)_s);
			*_t++ = *_s++;
			*_t++ = *_s++;
			len -= 2;

			for (; len > 17; len -= 16) {
				x = *((uint32_t *)(_s + 2));
				((uint32_t *)_t)[0] = (w LS 16) | (x RS 16);
				w = *((uint32_t *)(_s + 6));
				((uint32_t *)_t)[1] = (x LS 16) | (w RS 16);
				x = *((uint32_t *)(_s + 10));
				((uint32_t *)_t)[2] = (w LS 16) | (x RS 16);
				w = *((uint32_t *)(_s + 14));
				((uint32_t *)_t)[3] = (x LS 16) | (w RS 16);
				_t += 16;
				_s += 16;
			}
			break;
		case 3:
			if (len < 32)
				break;

			w = *((uint32_t *)_s);
			*_t++ = *_s++;
			len -= 1;

			for (; len > 18; len -= 16) {
				x = *((uint32_t *)(_s + 3));
				((uint32_t *)_t)[0] = (w LS 8) | (x RS 24);
				w = *((uint32_t *)(_s + 7));
				((uint32_t *)_t)[1] = (x LS 8) | (w RS 24);
				x = *((uint32_t *)(_s + 11));
				((uint32_t *)_t)[2] = (w LS 8) | (x RS 24);
				w = *((uint32_t *)(_s + 15));
				((uint32_t *)_t)[3] = (x LS 8) | (w RS 24);
				_t += 16;
				_s += 16;
			}
			break;
		default:
			break;
	}

	if (len & 16) {
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
	}
	if (len & 8) {
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
	}
	if (len & 4) {
		*_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++; *_t++ = *_s++;
	}
	if (len & 2) {
		*_t++ = *_s++; *_t++ = *_s++;
	}
	if (len & 1)
		*_t++ = *_s++;

	return t;
}

/* @func: memmove
 * #desc:
 *    copy memory area (memory overlap and then copy).
 *
 * #1: target pointer
 * #2: source pointer
 * #3: length
 * #r: target pointer
 */
void *C_SYMBOL(memmove)(void *t, const void *s, size_t len)
{
	if (t <= s)
		return C_SYMBOL(memcpy)(t, s, len);

	volatile uint8_t *_t = (uint8_t *)t + len, *_s = (uint8_t *)s + len;
	while (len--)
		*--_t = *--_s;

	return t;
}

/* @func: memset
 * #desc:
 *    fill memory with a constant byte.
 *
 * #1: target pointer
 * #2: character
 * #3: length
 * #r: target pointer
 */
void *C_SYMBOL(memset)(void *t, int8_t c, size_t len)
{
	int8_t cc[4] = { c, c, c, c };
	volatile int8_t *_t = t;

	for (; ((uintptr_t)_t & 3) && len; len--)
		*_t++ = c;

	for (; len > 15; len -= 16) {
		((uint32_t *)_t)[0] = *((uint32_t *)cc);
		((uint32_t *)_t)[1] = *((uint32_t *)cc);
		((uint32_t *)_t)[2] = *((uint32_t *)cc);
		((uint32_t *)_t)[3] = *((uint32_t *)cc);
		_t += 16;
	}
	for (; len > 3; len -= 4) {
		*((uint32_t *)_t) = *((uint32_t *)cc);
		_t += 4;
	}

	while (len--)
		*_t++ = c;

	return t;
}

/* @func: memchr
 * #desc:
 *    scan memory for a character.
 *
 * #1: source pointer
 * #2: character
 * #3: length
 * #r: target location / NULL pointer
 */
void *C_SYMBOL(memchr)(const void *s, uint8_t c, size_t len)
{
	for (uint8_t *t = (uint8_t *)s; len--; t++) {
		if (*t == c)
			return (void *)t;
	}

	return NULL;
}

/* @func: memrchr
 * #desc:
 *    scan memory for a character from the back.
 *
 * #1: source pointer
 * #2: character
 * #3: length
 * #r: target location / NULL pointer
 */
void *C_SYMBOL(memrchr)(const void *s, uint8_t c, size_t len)
{
	for (uint8_t *t = (uint8_t *)s + len; len--; ) {
		if (*--t == c)
			return (void *)t;
	}

	return NULL;
}

/* @func: memcmp
 * #desc:
 *    compare memory areas.
 *
 * #1: source1 pointer
 * #2: source2 pointer
 * #3: length
 * #r: 0: equal, 0<N>0: *s2 - *s1
 */
int32_t C_SYMBOL(memcmp)(const void *s1, const void *s2, size_t len)
{
	if (!len)
		return 0;

	uint8_t *t1 = (uint8_t *)s1, *t2 = (uint8_t *)s2;
	for (; --len && *t1 == *t2; t1++, t2++);

	return *t1 - *t2;
}

/* @func: strlen
 * #desc:
 *    calculate the length of a string.
 *
 * #1: string pointer
 * #r: string length
 */
size_t C_SYMBOL(strlen)(const char *s)
{
	const char *a = s;
	for (; *s != '\0'; s++);

	return (size_t)(s - a);
}

/* @func: strnlen
 * #desc:
 *    calculate the length of a string.
 *
 * #1: string pointer
 * #2: string length-max
 * #r: string length
 */
size_t C_SYMBOL(strnlen)(const char *s, size_t len)
{
	const char *a = s;
	for (; len && *s != '\0'; s++, len--);

	return (size_t)(s - a);
}

/* @func: strcpy
 * #desc:
 *    copy string.
 *
 * #1: target pointer
 * #2: source pointer
 * #r: target pointer
 */
char *C_SYMBOL(strcpy)(char *t, const char *s)
{
	char *a = t;
	while (*s != '\0')
		*t++ = *s++;
	*t = '\0';

	return a;
}

/* @func: strncpy
 * #desc:
 *    copy the string into the buffer and zero the rest.
 *
 * #1: target pointer
 * #2: source pointer
 * #3: target length-max
 * #r: target pointer
 */
char *C_SYMBOL(strncpy)(char *t, const char *s, size_t len)
{
	char *a = t;
	for (; len && *s != '\0'; len--)
		*t++ = *s++;
	C_SYMBOL(memset)(t, 0, len);

	return a;
}

/* @func: strcat
 * #desc:
 *    string append or catenate.
 *
 * #1: target pointer
 * #2: source pointer
 * #r: target pointer
 */
char *C_SYMBOL(strcat)(char *t, const char *s)
{
	t += C_SYMBOL(strlen)(t);
	C_SYMBOL(strcpy)(t, s);

	return t;
}

/* @func: strncat
 * #desc:
 *    string append or catenate.
 *
 * #1: target pointer
 * #2: source pointer
 * #3: source length-max
 * #r: target pointer
 */
char *C_SYMBOL(strncat)(char *t, const char *s, size_t len)
{
	char *a = t;

	t += C_SYMBOL(strlen)(t);
	for (; len && *s != '\0'; len--)
		*t++ = *s++;
	*t = '\0';

	return a;
}

/* @func: strchr
 * #desc:
 *    locate character in string.
 *
 * #1: string pointer
 * #2: character
 * #r: string pointer
 */
char *C_SYMBOL(strchr)(const char *s, uint8_t c)
{
	for (; *s != '\0'; s++) {
		if (*((uint8_t *)s) == c)
			return (char *)s;
	}

	return (*((uint8_t *)s) == c) ? (char *)s : NULL;
}

/* @func: strrchr
 * #desc:
 *    locate the character in the string from the end.
 *
 * #1: string pointer
 * #2: character
 * #r: string pointer
 */
char *C_SYMBOL(strrchr)(const char *s, uint8_t c)
{
	size_t n = C_SYMBOL(strlen)(s) + 1;

	return C_SYMBOL(memrchr)(s, c, n);
}

/* @func: strcmp
 * #desc:
 *    compare two string.
 *
 * #1: string1 pointer
 * #2: string2 pointer
 * #r: 0: equal, 0<N>0: *s1 - *s2
 */
int32_t C_SYMBOL(strcmp)(const char *s1, const char *s2)
{
	for (; *s1 == *s2 && *s1 != '\0'; s1++, s2++);

	return *((uint8_t *)s1) - *((uint8_t *)s2);
}

/* @func: strncmp
 * #desc:
 *    compare two string.
 *
 * #1: string1 pointer
 * #2: string2 pointer
 * #3: length
 * #r: 0: equal, 0<N>0: *s1 - *s2
 */
int32_t C_SYMBOL(strncmp)(const char *s1, const char *s2, size_t len)
{
	if (!len)
		return 0;

	for (; --len && *s1 == *s2 && *s1 != '\0'; s1++, s2++);

	return *((uint8_t *)s1) - *((uint8_t *)s2);
}

/* @func: strstr
 * #desc:
 *    locate a substring.
 *
 * #1: string1 pointer
 * #2: string2 pointer
 * #r: string1 location / NULL pointer
 */
char *C_SYMBOL(strstr)(const char *s1, const char *s2)
{
	size_t n1 = C_SYMBOL(strlen)(s1);
	size_t n2 = C_SYMBOL(strlen)(s2);

	for (size_t n = 0; (n1 - n) >= n2; s1++, n++) {
		if (!C_SYMBOL(memcmp)(s2, s1, n2))
			return (char *)s1;
	}

	return NULL;
}

/* @func: strpbrk
 * #desc:
 *    locate character set in string.
 *
 * #1: string pointer
 * #2: character set
 * #r: string location / NULL pointer
 */
char *C_SYMBOL(strpbrk)(const char *s, const char *cs)
{
	for (; *s != '\0'; s++) {
		if (C_SYMBOL(strchr)(cs, *s))
			return (char *)s;
	}

	return NULL;
}

/* @func: strspn
 * #desc:
 *    get the prefix length of a substring consisting of a
 *    character set.
 *
 * #1: string pointer
 * #2: character set
 * #r: string location length
 */
size_t C_SYMBOL(strspn)(const char *s, const char *cs)
{
	size_t n = 0;
	for (; *s != '\0'; s++, n++) {
		if (!C_SYMBOL(strchr)(cs, *s))
			break;
	}

	return n;
}

/* @func: strcspn
 * #desc:
 *    get the prefix length of the substring that does not belong
 *    to the character set.
 *
 * #1: string pointer
 * #2: character set
 * #r: string location length
 */
size_t C_SYMBOL(strcspn)(const char *s, const char *cs)
{
	size_t n = 0;
	for (; *s != '\0'; s++, n++) {
		if (C_SYMBOL(strchr)(cs, *s))
			return n;
	}

	return n;
}

/* @func: strtok_r
 * #desc:
 *    extract tokens from strings.
 *
 * #1: string pointer
 * #2: separation character set
 * #3: separation location save pointer
 * #r: separation location / NULL pointer
 */
char *C_SYMBOL(strtok_r)(char *s, const char *sp, char **sl)
{
	if (!s)
		s = *sl;

	char *tmp = s;
	if (*tmp == '\0')
		return NULL;

	for (; *tmp != '\0'; tmp++) {
		if (C_SYMBOL(strchr)(sp, *tmp)) {
			*sl = tmp + 1;
			*tmp = '\0';
			return s;
		}
	}
	*sl = tmp;

	return s;
}
