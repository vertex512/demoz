/* @file: stat_diff.c
 * #desc:
 *    The implementations of file status difference conversion.
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
#include <demoz/c/stdint.h>
#include <demoz/c/sys/stat.h>
#include <demoz/c/sys/syscall.h>


/* @func: ___stat_mode
 * #desc:
 *    file mode difference conversion.
 *
 * #1: file mode
 * #r: file mode
 */
uint32_t C_SYMBOL(___stat_mode)(uint32_t mode)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	uint32_t ret = 0;

	if (mode & X_S_IRUSR)
		ret |= S_IRUSR;
	if (mode & X_S_IWUSR)
		ret |= S_IWUSR;
	if (mode & X_S_IXUSR)
		ret |= S_IXUSR;
	if (mode & X_S_IRGRP)
		ret |= S_IRGRP;
	if (mode & X_S_IWGRP)
		ret |= S_IWGRP;
	if (mode & X_S_IXGRP)
		ret |= S_IXGRP;
	if (mode & X_S_IROTH)
		ret |= S_IROTH;
	if (mode & X_S_IWOTH)
		ret |= S_IWOTH;
	if (mode & X_S_IXOTH)
		ret |= S_IXOTH;
	if (mode & X_S_ISUID)
		ret |= S_ISUID;
	if (mode & X_S_ISGID)
		ret |= S_ISGID;
	if (mode & X_S_ISVTX)
		ret |= S_ISVTX;

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___stat_mode_inv
 * #desc:
 *    file mode difference inverse conversion.
 *
 * #1: file mode
 * #r: file mode
 */
uint32_t C_SYMBOL(___stat_mode_inv)(uint32_t mode)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	uint32_t ret = 0;

	if (mode & S_IRUSR)
		ret |= X_S_IRUSR;
	if (mode & S_IWUSR)
		ret |= X_S_IWUSR;
	if (mode & S_IXUSR)
		ret |= X_S_IXUSR;
	if (mode & S_IRGRP)
		ret |= X_S_IRGRP;
	if (mode & S_IWGRP)
		ret |= X_S_IWGRP;
	if (mode & S_IXGRP)
		ret |= X_S_IXGRP;
	if (mode & S_IROTH)
		ret |= X_S_IROTH;
	if (mode & S_IWOTH)
		ret |= X_S_IWOTH;
	if (mode & S_IXOTH)
		ret |= X_S_IXOTH;
	if (mode & S_ISUID)
		ret |= X_S_ISUID;
	if (mode & S_ISGID)
		ret |= X_S_ISGID;
	if (mode & S_ISVTX)
		ret |= X_S_ISVTX;

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
