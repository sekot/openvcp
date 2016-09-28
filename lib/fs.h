/*  OpenVCP
 *  Copyright (C) 2006 Gerrit Wyen <gerrit@t4a.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */



#ifndef FS_H
	#define FS_H

	char *file_readline(int fd);

	int file_exist(char *filename);
	char *file_read(char *filename);
	int file_write(char *filename, char *data);
	int dev_write(char *filename, char *data);
	int file_write_many(char *filename, ...);
	int file_attach(char *filename, char *data);
	int file_touch(char *filename);
	int file_replace(char *filename, char *searched, char *replacement);

	int mv(char *oldpath, char *newpath);
	int copy(char *oldpath, char *newpath);

	int dir_exist(char *dirname);
	int dir_remove(char *dirname);
	int dir_getlevel(char *path);
	int fs_cgroup_support(void);
	int fs_tagxid_support(char *targetpath);

#endif
