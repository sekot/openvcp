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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "misc.h"
#include "parseconfig.h"
#include "strings.h"
#include "fs.h"


int parse_config(char *filename, struct config_st *config)
{
	int fd;
	unsigned int directives_count;
	char *line, **directives;

	memset(config, 0, sizeof(struct config_st));


	if((fd = open(filename, O_RDONLY)) != -1)
	{

		while((line = file_readline(fd)) != NULL)
		{
			if(line[0] == '#')
			{
				free(line);
				continue;
			}

			directives = split_string(line, '=', &directives_count);

			if(directives_count >= 2)
			{


				if(strcasecmp(directives[0], "IP") == 0)
				{
					config->ip = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->ip, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "Port") == 0)
				{
					config->port = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->port, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "Password") == 0)
				{
					config->password = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->password, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "Logfile") == 0)
				{
					config->logfile = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->logfile, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "Loglevel") == 0)
				{
					config->loglevel = atoi(directives[1]);
				}
				else
				if(strcasecmp(directives[0], "StartXID") == 0)
				{
					config->startxid = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->startxid, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "RootDir") == 0)
				{
					config->root_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->root_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "ConfigDir") == 0)
				{
					config->config_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->config_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "BackupDir") == 0)
				{
					config->backup_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->backup_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "UserBackupDir") == 0)
				{
					config->userbackup_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->userbackup_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "ImageDir") == 0)
				{
					config->image_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->image_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "RunDir") == 0)
				{
					config->run_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->run_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "CacheDir") == 0)
				{
					config->cache_dir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->cache_dir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "PidFile") == 0)
				{
					config->pid_file = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->pid_file, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "Ifaces") == 0)
				{
					config->ifaces = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->ifaces, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "IfbDev") == 0)
				{
					config->ifbdev = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->ifbdev, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "AdminEmail") == 0)
				{
					config->adminemail = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->adminemail, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "CGroupMountPoint") == 0)
				{
					config->cgroupdir = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->cgroupdir, directives[1]);
				}
				else
				if(strcasecmp(directives[0], "CGroupMountOptions") == 0)
				{
					config->cgroupmountoptions = (char *) malloc(strlen(directives[1])+1);
					strcpy(config->cgroupmountoptions, directives[1]);
				}

			}

			free_strings(directives, directives_count);
			free(line);
		}


	}
	else
	{
		printf("\nERROR: Could not open %s\n\n", filename);
		exit(-1);
	}

	if(config->ip == NULL)
		die("ERROR: IP Directive not Found\n");
	if(config->port == NULL)
		die("ERROR: Port Directive not Found\n");
	if(config->password == NULL)
		die("ERROR: Password Directive not Found\n");
	if(config->logfile == NULL)
		die("ERROR: Logfile Directive not Found\n");
	if(config->startxid == NULL)
		die("ERROR: StartXID Directive not Found\n");
	if(config->root_dir == NULL || strlen(config->root_dir) <= 1)
		die("ERROR: RootDir Directive not Found\n");
	if(config->config_dir == NULL)
		die("ERROR: ConfigDir Directive not Found\n");
	if(config->backup_dir == NULL)
		die("ERROR: BackupDir Directive not Found\n");
	if(config->image_dir == NULL)
		die("ERROR: ImageDir Directive not Found\n");
	if(config->run_dir == NULL)
		die("ERROR: RunDir Directive not Found\n");
	if(config->cache_dir == NULL)
		die("ERROR: CacheDir Directive not Found\n");
	if(config->pid_file == NULL)
		die("ERROR: PidFile Directive not Found\n");
	if(config->ifbdev == NULL)
		die("ERROR: IfbDev Directive not Found\n");
	if(config->adminemail == NULL)
		die("ERROR: AdminEmail Directive not Found\n");
	if(config->userbackup_dir == NULL)
		die("ERROR: UserBackupDir Directive not Found\n");
	if(config->cgroupdir == NULL)
		die("ERROR: CGroupMountPoint Directive not Found\n");
	if(config->cgroupmountoptions == NULL)
		die("ERROR: CGroupMountOptions Directive not Found\n");

	if(!dir_exist(config->root_dir))
		die("ERROR: Root Directory does not exist\n");
	if(!dir_exist(config->config_dir))
		die("ERROR: Config Directory does not exist\n");
	if(!dir_exist(config->backup_dir))
		die("ERROR: Backup Directory does not exist\n");
	if(!dir_exist(config->image_dir))
		die("ERROR: Image Directory does not exist\n");
	if(!dir_exist(config->run_dir))
		die("ERROR: Run Directory does not exist\n");
	if(!dir_exist(config->userbackup_dir))
		die("ERROR: User Backup Directory does not exist\n");


	close(fd);

	return 1;
}



int parse_image(char *id, struct image_st *image)
{
	char filename[BUF_SIZE];
	int fd;
	unsigned int directives_count;
	char *line, **directives;

	memset(image, 0, sizeof(struct image_st));


	sconcat(filename, BUF_SIZE, global_settings.image_dir, "/", id, ".conf", NULL);
	if(!file_exist(filename))
		return -1;

	if((fd = open(filename, O_RDONLY)) != -1)
	{

		while((line = file_readline(fd)) != NULL)
		{
			if(line[0] == '#')
			{
				free(line);
				continue;
			}

			directives = split_string(line, '=', &directives_count);

			if(directives_count >= 2)
			{


				if(strcmp(directives[0], "name") == 0)
				{
					image->name = (char *) malloc(strlen(directives[1])+1);
					strcpy(image->name, directives[1]);
				}
				else
				if(strcmp(directives[0], "initstyle") == 0)
				{
					image->initstyle = (char *) malloc(strlen(directives[1])+1);
					strcpy(image->initstyle, directives[1]);
				}


			}

			free_strings(directives, directives_count);
			free(line);
		}


	}
	else
	{
		return -1;
	}


	close(fd);

	return 1;
}


int free_image(struct image_st *image)
{
	if(image->name != NULL)
		free(image->name);

	if(image->initstyle != NULL)
		free(image->initstyle);

	return 0;
}


