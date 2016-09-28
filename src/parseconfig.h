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


#ifndef PARSECONFIG_H
	#define PARSECONFIG_H



	struct config_st
	{
		char *ip;
		char *port;
		char *password;
		char *logfile;
		int  loglevel;
		char *startxid;

		char *root_dir;
		char *config_dir;
		char *backup_dir;
		char *userbackup_dir;
		char *image_dir;
		char *run_dir;
		char *cache_dir;
		char *pid_file;

		char *ifaces;
		char *ifbdev;

		char *adminemail;
                char *cgroupdir;
                char *cgroupmountoptions;
	};


	struct image_st
	{
		char *name;
		char *initstyle;
	};


	int parse_config(char *filename, struct config_st *config);


	int parse_image(char *id, struct image_st *image);
	int free_image(struct image_st *image);

#endif
