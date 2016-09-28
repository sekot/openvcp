/*	OpenVCP
 *	Copyright (C) 2006 Gerrit Wyen <gerrit@t4a.net>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "main.h"
#include "setup.h"
#include "fs.h"
#include "strings.h"
#include "misc.h"


int setup_config(char *name, char *ctx)
{
	char path1[BUF_SIZE+1], path2[BUF_SIZE+1];
	char *configdir, *xid;
	int xid_int, ret;

	memset(path1, 0, BUF_SIZE+1);
	memset(path2, 0, BUF_SIZE+1);

	configdir = concat(global_settings.config_dir, "/", name, NULL);

	if(mkdir(configdir, 0755) == -1)
	{
		free(configdir);
		return -1;
	}

	/* Directorys */
	sconcat(path1, BUF_SIZE, configdir, "/interfaces", NULL);
	mkdir(path1, 0755);

	sconcat(path1, BUF_SIZE, configdir, "/uts", NULL);
	mkdir(path1, 0755);

	sconcat(path1, BUF_SIZE, configdir, "/name", NULL);
	file_write(path1, name); file_attach(path1, "\n");


	/* Files */
	if (ctx == NULL)
	{
		xid = file_read(XID_COUNTER);

		if(xid != NULL)
		{
			trim_string(xid);
			xid_int = atoi(xid);

			xid_int += 1;
			free(xid);

			xid = (char *)malloc(BUF_SIZE+1);
			snprintf(xid, BUF_SIZE, "%d", xid_int);
			file_write(XID_COUNTER, xid);
		}
		else
		{
			xid = strdup(global_settings.startxid);
			file_write(XID_COUNTER, xid);
		}
	}
	else
	{
		xid = strdup(ctx);
	}


	sconcat(path1, BUF_SIZE, configdir, "/context", NULL);
	file_write_many(path1, xid, "\n", NULL);
	free(xid);


	sconcat(path1, BUF_SIZE, configdir, "/fstab", NULL);
	file_write(path1, "none /proc proc defaults 0 0\nnone /dev/pts devpts gid=5,mode=620 0 0\n");

	/* Symlinks */
	sconcat(path1, BUF_SIZE, global_settings.run_dir, "/", name, NULL);
	sconcat(path2, BUF_SIZE, configdir, "/run", NULL);
	ret = symlink(path1, path2);

	sconcat(path1, BUF_SIZE, global_settings.root_dir, "/", name, NULL);
	sconcat(path2, BUF_SIZE, configdir, "/vdir", NULL);
	ret = symlink(path1, path2);

	sconcat(path1, BUF_SIZE, global_settings.cache_dir, "/", name, NULL);
	sconcat(path2, BUF_SIZE, configdir, "/cache", NULL);
	ret = symlink(path1, path2);




	free(configdir);


	return 1;
}

int setup_image(char *name, char *dist, char *ip)
{
	char path[BUF_SIZE+1];
	char *context, *homedir, *imagedir, *backupdir;
	char cachepath[BUF_SIZE];
	char *tmpimagedir, *tmphomedir;
	int ret, i = 0, tun = 0;
	struct image_st image;

	memset(path, 0, BUF_SIZE+1);

	if(name == NULL || dist == NULL)
		return -1;

	backupdir = concat(global_settings.backup_dir, "/_", name, NULL);
	imagedir = concat(global_settings.image_dir, "/", dist, NULL);
	homedir = concat(global_settings.root_dir, "/", name, NULL);


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/cache", NULL);
	ret = readlink(path, cachepath, BUF_SIZE);
	if(ret > 0)
	{
		cachepath[ret] = 0;
		if(dir_exist(cachepath))
		{
			dir_remove(cachepath);
		}
	}

	if (strcmp(ip,"0") == 0)
	{
		if(!dir_exist(imagedir))
		{
			free(backupdir);
			free(imagedir);
			free(homedir);

			return -1;
		}
	}


	if(dir_exist(homedir))
	{

		sconcat(path, BUF_SIZE, homedir, "/backup", NULL);

		if(dir_exist(path))
		{

			if(dir_exist(global_settings.backup_dir))
			{
				execb_cmd("mv", "mv", path, backupdir, NULL);
			}
		}

		execb_cmd("rm", "rm", "-rf", homedir, NULL);

	}

	mkdir(homedir, 0755);

	tmpimagedir = concat(imagedir, "/.", NULL);
	tmphomedir = concat(homedir, "/.", NULL);
	if (strcmp(ip,"0") == 0)
	{
		execb_cmd("cp", "cp", "-a", tmpimagedir, tmphomedir, NULL);
	}
	else
	{
		tmpimagedir = concat(ip, ":",imagedir, "/.", NULL);
		execb_cmd("rsync", "rsync", "-a", "-e", "ssh", tmpimagedir, tmphomedir, NULL);
	}
	free(tmpimagedir); free(tmphomedir);

	if(dir_exist(backupdir))
	{
		sconcat(path, BUF_SIZE, homedir, "/backup", NULL);
		execb_cmd("mv", "mv", backupdir, path, NULL);
	}

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/context", NULL);
	context = file_read(path);
	trim_string(context);

	if(fs_tagxid_support(homedir)) execb_cmd("chxid", "chxid", "-c", context, "-R", homedir, NULL);


	free(context);




	if(parse_image(dist, &image))
	{

		if(image.name != NULL)
		{
			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/image_name", NULL);
			file_write_many(path, image.name, "\n", NULL);
		}

		if(image.initstyle != NULL)
		{
			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/apps", NULL);
			if(!dir_exist(path)) mkdir(path, 0755);
			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init", NULL);
			if(!dir_exist(path)) mkdir(path, 0755);

		 	if(strcmp(image.initstyle, "plain") == 0
		 		|| strcmp(image.initstyle, "gentoo") == 0
		 		|| strcmp(image.initstyle, "minit") == 0
		 		|| strcmp(image.initstyle, "sysv") == 0)
			{
				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style", NULL);
				file_write_many(path, image.initstyle, "\n", NULL);
			}
		}
		else
		{
			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style", NULL);
			if(file_exist(path)) unlink(path);
		}

		free_image(&image);

	}
	else
	{
		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/image_name", NULL);
		file_write_many(path, dist, "\n", NULL);
	}

	//create tun device if needed after reinstall
	snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d", global_settings.config_dir, name, i);
	while(dir_exist(path))
	{
		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/tun", global_settings.config_dir, name, i);
		if(file_exist(path))
		{
			tun = 1;
		}
		i++;
		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d", global_settings.config_dir, name, i);
	}
 	if(tun == 1)
 	{
 		sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/dev", NULL);

		if(!dir_exist(path))
		{
			mkdir(path, 0755);
		}

		sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/dev/net", NULL);

		if(!dir_exist(path))
		{
			mkdir(path, 0755);
		}

		sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/dev/net/tun", NULL);
		mknod( path, S_IFCHR|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, makedev(10,200) );
 	}

	free(backupdir);
	free(imagedir);
	free(homedir);


	return 1;
}

