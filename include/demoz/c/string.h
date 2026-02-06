/* @file: string.h
 * #desc:
 *    The definitions of string operations.
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

#ifndef _DEMOZ_C_STRING_H
#define _DEMOZ_C_STRING_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* c/string.c */

extern
void *C_SYMBOL(memcpy)(void *t, const void *s, size_t len)
;

extern
void *C_SYMBOL(memmove)(void *t, const void *s, size_t len)
;

extern
void *C_SYMBOL(memset)(void *t, int8_t c, size_t len)
;

extern
void *C_SYMBOL(memchr)(const void *s, uint8_t c, size_t len)
;

extern
void *C_SYMBOL(memrchr)(const void *s, uint8_t c, size_t len)
;

extern
int32_t C_SYMBOL(memcmp)(const void *s1, const void *s2, size_t len)
;

extern
size_t C_SYMBOL(strlen)(const char *s)
;

extern
size_t C_SYMBOL(strnlen)(const char *s, size_t len)
;

extern
char *C_SYMBOL(strcpy)(char *t, const char *s)
;

extern
char *C_SYMBOL(strncpy)(char *t, const char *s, size_t len)
;

extern
char *C_SYMBOL(strcat)(char *t, const char *s)
;

extern
char *C_SYMBOL(strncat)(char *t, const char *s, size_t len)
;

extern
char *C_SYMBOL(strchr)(const char *s, uint8_t c)
;

extern
char *C_SYMBOL(strrchr)(const char *s, uint8_t c)
;

extern
int32_t C_SYMBOL(strcmp)(const char *s1, const char *s2)
;

extern
int32_t C_SYMBOL(strncmp)(const char *s1, const char *s2, size_t len)
;

extern
char *C_SYMBOL(strstr)(const char *s1, const char *s2)
;

extern
char *C_SYMBOL(strpbrk)(const char *s, const char *cs)
;

extern
size_t C_SYMBOL(strspn)(const char *s, const char *cs)
;

extern
size_t C_SYMBOL(strcspn)(const char *s, const char *cs)
;

extern
char *C_SYMBOL(strtok_r)(char *s, const char *sp, char **sl)
;

#ifdef __cplusplus
}
#endif


#endif
