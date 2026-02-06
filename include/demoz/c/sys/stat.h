/* @file: stat.h
 * #desc:
 *    The definitions of file status difference conversion.
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

#ifndef _DEMOZ_C_SYS_STAT_H
#define _DEMOZ_C_SYS_STAT_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/types.h>
#include <demoz/c/sys/time.h>


/* @def: _ */
#define X_S_IRWXU  0700 /* read, write, execute/search by owner */
#define X_S_IRUSR  0400 /* read permission, owner */
#define X_S_IWUSR  0200 /* write permission, owner */
#define X_S_IXUSR  0100 /* execute/search permission, owner */
#define X_S_IRWXG   070 /* read, write, execute/search by group */
#define X_S_IRGRP   040 /* read permission, group */
#define X_S_IWGRP   020 /* write permission, group */
#define X_S_IXGRP   010 /* execute/search permission, group */
#define X_S_IRWXO    07 /* read, write, execute/search by others */
#define X_S_IROTH    04 /* read permission, others */
#define X_S_IWOTH    02 /* write permission, others */
#define X_S_IXOTH    01 /* execute/search permission, others */
#define X_S_ISUID 04000 /* set-user-id on execution */
#define X_S_ISGID 02000 /* set-group-id on execution */
#define X_S_ISVTX 01000 /* on directories, restricted deletion flag */

#define X_S_IFMT  0170000 /* type mask */
#define X_S_IFREG  010000 /* regular */
#define X_S_IFDIR  020000 /* directory */
#define X_S_IFLNK  030000 /* symbolic link */
#define X_S_IFIFO  040000 /* fifo special */
#define X_S_IFCHR  050000 /* character special */
#define X_S_IFBLK  060000 /* block special */
#define X_S_IFSOCK 070000 /* socket */

#define X_S_ISREG(x) (((x) & X_S_IFMT) == X_S_IFREG)
#define X_S_ISDIR(x) (((x) & X_S_IFMT) == X_S_IFDIR)
#define X_S_ISLNK(x) (((x) & X_S_IFMT) == X_S_IFLNK)
#define X_S_ISCHR(x) (((x) & X_S_IFMT) == X_S_IFCHR)
#define X_S_ISBLK(x) (((x) & X_S_IFMT) == X_S_IFBLK)
#define X_S_ISFIFO(x) (((x) & X_S_IFMT) == X_S_IFIFO)
#define X_S_ISSOCK(x) (((x) & X_S_IFMT) == X_S_IFSOCK)

struct x_stat {
	x_dev_t st_dev;
	x_ino_t st_ino;
	x_mode_t st_mode;
	x_nlink_t st_nlink;
	x_uid_t st_uid;
	x_gid_t st_gid;
	x_dev_t st_rdev;
	x_off_t st_size;
	struct x_timespec st_atim;
	struct x_timespec st_mtim;
	struct x_timespec st_ctim;
	x_blksize_t st_blksize;
	x_blkcnt_t st_blocks;
};
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/stat_diff.c */

extern
uint32_t C_SYMBOL(___stat_mode)(uint32_t mode)
;

extern
uint32_t C_SYMBOL(___stat_mode_inv)(uint32_t mode)
;

#ifdef __cplusplus
}
#endif


#endif
