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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

#include "fs.h"
#include "strings.h"


int file_replace(char *filename, char *searched, char *replacement)
{

	ssize_t len = 0, ret;
	int filefd, tmpfd;
	char filename_tmp[] = "/tmp/openvcpd.tmp-XXXXXX";
	char data[5000+1];

	char *loc, *next_loc;



	filefd = open(filename, O_RDONLY);
	tmpfd = mkstemp(filename_tmp);


	if(filefd != -1 && tmpfd != -1)
   	{

		while((len = read(filefd, data, 5000)) > 0)
		{

		  	data[len] = 0;

			loc = data;

			while((next_loc = strstr(loc, searched)) != NULL)
			{
				ret = write(tmpfd, loc, next_loc-loc);
				ret = write(tmpfd, replacement, strlen(replacement));

				loc = next_loc+strlen(searched);
			}

			if(loc == data)
				ret = write(tmpfd, data, len);
			else
				ret = write(tmpfd, loc, len-(loc-data));



			if(len == 5000)
			{
				//FIXME
			}
		}


	}


	close(filefd);
	close(tmpfd);


	mv(filename_tmp, filename);


	return 1;
}


int mv(char *oldpath, char *newpath)
{
	int ret;

	ret = rename(oldpath, newpath);

	if(ret == -1)
	{
		if(errno == EXDEV)
		{

			ret = copy(oldpath, newpath);
			unlink(oldpath);
		}
	}

	return ret;
}


int copy(char *oldpath, char *newpath)
{
	char buf[1024+1];
	int fd_old, fd_new;
	int len, total = 0, ret;
	struct stat statbuf;

	fd_old = open(oldpath, O_RDONLY);

	if(fd_old != -1)
	{

		stat(oldpath, &statbuf);


		fd_new = open(newpath, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);

		if(fd_new != -1)
		{

			while((len = read(fd_old, buf, 1024)) > 0)
			{
				ret = write(fd_new, buf, len);

				total += len;
			}

			close(fd_new);
		}

		close(fd_old);
	}


	return total;
}

int file_attach(char *filename, char *data)
{
	int filefd;
	int written_chars = -1;

	filefd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0755 );

	if(filefd != -1)
	{
		written_chars = write(filefd, data, strlen(data));
		close(filefd);
	}

	return written_chars;
}

int file_write(char *filename, char *data)
{
	int filefd;
	int written_chars = -1;


	filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0755);

	if(filefd != -1)
	{
		written_chars = write(filefd, data, strlen(data));
		close(filefd);
	}

	return written_chars;

}

int dev_write(char *filename, char *data)
{
	int filefd;
	int written_chars = -1;


	filefd = open(filename, O_WRONLY|O_CREAT|O_NONBLOCK, 0755);

	if(filefd != -1)
	{
		written_chars = write(filefd, data, strlen(data));
		close(filefd);
	}

	return written_chars;

}

int file_write_many(char *filename, ...)
{
	int filefd, written_chars = 0;
	char *data;
	va_list list;


	filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0755);

	if(filefd != -1)
	{
		va_start(list, filename);

		while((data = va_arg(list, char *)) != NULL)
		{
			written_chars += write(filefd, data, strlen(data));
		}

		va_end(list);

		close(filefd);
	}

	return written_chars;

}


int file_touch(char *filename)
{
	int filefd;

	filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0755);

	if(filefd != -1)
	{
		close(filefd);
		return -1;
	}

	return 1;
}

char *file_read(char *filename)
{
	int filefd;
	int read_chars=0,  total_chars=0;

	char *file_buffer = NULL;
  	char readbuffer[5000];

	filefd = open(filename, O_RDONLY, 0755);

	if(filefd != -1)
	{

		while((read_chars = read(filefd, readbuffer, 5000)) > 0)
		{

			readbuffer[read_chars] = 0;

			file_buffer = (char *)realloc(file_buffer, total_chars+read_chars+1);
			strcpy(file_buffer+total_chars,  readbuffer);

			total_chars += read_chars;

		}

		close(filefd);

	}


	return file_buffer;

}

char *file_readline(int fd)
{
	int i = 0;
	int ret;
	char c = 0;
	char *buffer = NULL;

	while(i < 15000)
	{
		ret = read(fd, &c, 1);

		if(ret != 1)
			break;

		buffer = realloc(buffer, i+2);
		buffer[i] = c;

		if(c == '\n' || c == '\r')
			break;

		i++;
	}

	if(buffer != NULL)
		buffer[i] = 0;

	return buffer;
}

int file_exist(char *filename)
{
	struct stat stat_buf;

	memset(&stat_buf, 0, sizeof(struct stat));

	stat(filename, &stat_buf);

	if(S_ISREG(stat_buf.st_mode))
		return 1;

	return 0;

}



/* Directories */



int dir_exist(char *dirname)
{
	struct stat stat_buf;

	memset(&stat_buf, 0, sizeof(struct stat));

	stat(dirname, &stat_buf);

	if(S_ISDIR(stat_buf.st_mode))
		return 1;

	return 0;
}


int dir_remove(char *dirname)
{
	DIR *dir_ptr;
	struct dirent *entry;
	char path[1000+1];

	dir_ptr = opendir(dirname);

	if(dir_ptr != NULL)
	{
		while((entry = readdir(dir_ptr)) != NULL)
		{
			if(strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0)
			{
				snprintf(path, 1000, "%s/%s", dirname, entry->d_name);

				if(dir_exist(path))
				{
					dir_remove(path);
				}
				else
				{
					unlink(path);
				}

			}
		}

		closedir(dir_ptr);

		rmdir(dirname);
	}

	return 1;
}




int dir_getlevel(char *path)
{
	int slashcount = 0;
	int i=0;

	while(i<strlen(path))
	{
		if(path[i] == '/')
			slashcount++;
		i++;
	}

	return slashcount;
}

int fs_cgroup_support()
{

	char *mountsstr, **linestrs;
	unsigned int line_num;
	int i = 0;
	int highstlevel=-1, support_cpu=0, support_cpuset=0;

	char mpstr[250], optstr[250];

	mountsstr = file_read("/proc/mounts");

	if(mountsstr != NULL)
	{
		linestrs = split_string(mountsstr, '\n', &line_num);

		while(i < line_num)
		{
			sscanf(linestrs[i], "%*s %249s %*s %249s", mpstr, optstr);


			if(strncmp(mpstr, "/dev/cgroup", strlen(mpstr)) == 0)
			{
				if(dir_getlevel(mpstr) > highstlevel)
				{
					if(strstr(optstr, "cpu") != NULL)
						support_cpu=1;
					if(strstr(optstr, "cpuset") != NULL)
						support_cpuset=1;
				}
			}

			i++;
		}

		free_strings(linestrs, line_num);
		free(mountsstr);
	}
	if(support_cpuset && support_cpu)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int fs_tagxid_support(char *targetpath)
{

	char *mountsstr, **linestrs;
	unsigned int line_num;
	int i = 0;
	int highstlevel=-1, support=0;

	char mpstr[250], optstr[250];

	mountsstr = file_read("/proc/mounts");

	if(mountsstr != NULL)
	{
		linestrs = split_string(mountsstr, '\n', &line_num);

		while(i < line_num)
		{
			sscanf(linestrs[i], "%*s %249s %*s %249s", mpstr, optstr);


			if(strncmp(mpstr, targetpath, strlen(mpstr)) == 0)
			{
				if(dir_getlevel(mpstr) > highstlevel)
				{
					if(strstr(optstr, "tagxid") != NULL)
						support=1;
					else if(strstr(optstr, "tag") != NULL)
						support=1;
					else support=0;
				}
			}

			i++;
		}

		free_strings(linestrs, line_num);
		free(mountsstr);
	}

	return support;
}

