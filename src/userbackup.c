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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>


#include "strings.h"
#include "misc.h"
#include "main.h"
#include "request.h"
#include "error.h"
#include "logtraffic.h"
#include "fs.h"
#include "userbackup.h"

struct ovcp_response_st *get_userbackups(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;

	char *name, *size;
	char filename[BUF_SIZE+1];

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	DIR *dir_ptr;
	struct dirent *dir_entry;

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	if(!dir_exist(global_settings.userbackup_dir))
		return ovcp_response_error(OVCP_ERROR_BCKDIRNFUOD, OVCP_ERROR_BCKDIRNFUOD_DESC);

	response = ovcp_response_new();

	if((dir_ptr = opendir(global_settings.userbackup_dir)) != NULL)
	{

		while((dir_entry = readdir(dir_ptr)) != NULL)
		{

			if(strstr(dir_entry->d_name, name) != NULL)
			{
				if(dir_entry->d_name[(strlen(dir_entry->d_name)-5)] != '.')
				{
					ovcp_response_struct_new(response);

					ovcp_response_struct_add_string(response, "name", dir_entry->d_name);

					//check whether file size file exists
					sconcat(filename, BUF_SIZE, global_settings.userbackup_dir, "/", dir_entry->d_name, ".conf", NULL);
					if(file_exist(filename))
					{
						size = file_read(filename);
						if (size != NULL)
						{
							trim_string(size);
							ovcp_response_struct_add_string(response, "size", size);
							free(size);
						}
					}

					ovcp_response_struct_end(response);
				}
			}

		}

		closedir(dir_ptr);
	}

	return response;

}
struct ovcp_response_st *create_userbackups(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;

	char *name, *homedir, *userbackupdir, *backupname;
	char *min, *hour, *day, *month, *year, *size = NULL;
	char *rsyncoutput, **tmpargv, **tmpargv2;

	int i = 0;
	unsigned int tmpargc, tmpargc2;
	double tmp;

	char filename[BUF_SIZE+1];

	struct tm *currentdate;
	time_t timestamp = time(0);
	currentdate = localtime(&timestamp);

	min = (char *)malloc(4);
	if(currentdate->tm_min < 10) {
		snprintf(min, 3, "0%i", currentdate->tm_min);
	}
	else {
		snprintf(min, 3, "%i", currentdate->tm_min);
	}
	hour = (char *)malloc(4);
	if(currentdate->tm_hour < 10) {
		snprintf(hour, 3, "0%i", currentdate->tm_hour);
	}
	else {
		snprintf(hour, 3, "%i", currentdate->tm_hour);
	}
	day = (char *)malloc(4);
	if(currentdate->tm_mday < 10) {
		snprintf(day, 3, "0%i", currentdate->tm_mday);
	}
	else {
		snprintf(day, 3, "%i", currentdate->tm_mday);
	}
	month = (char *)malloc(4);
	if(currentdate->tm_mon < 9) {
		snprintf(month, 3, "0%i", currentdate->tm_mon+1);
	}
	else {
		snprintf(month, 3, "%i", currentdate->tm_mon+1);
	}
	year = (char *)malloc(6);
	snprintf(year, 5, "%i", currentdate->tm_year+1900);

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	backupname = concat(name, "_", year, ".", month, ".", day, "_", hour, ":", min, NULL);
	userbackupdir = concat(global_settings.userbackup_dir, "/", backupname, NULL);
	homedir = concat(global_settings.root_dir, "/", name, "/", NULL);

	if((rsyncoutput = exec_cmd("rsync", 65535, "rsync", "-a", "-H", "-l", "--stats", homedir, userbackupdir, NULL)) == NULL)
	{
		execb_cmd("rm", "rm", "-rf", userbackupdir, NULL);
		return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);
	}
	
	
	// create backup of init style
	char *srcpath, *destpath;
	srcpath = concat(global_settings.config_dir, "/", name, "/apps/init/style", NULL);
	destpath = concat(userbackupdir, "/initstyle.backup", NULL);

	if(file_exist(srcpath))
		copy(srcpath, destpath);

	free(srcpath);
	free(destpath);
	
	

	response = ovcp_response_new();

	if(dir_exist(userbackupdir))
	{
		ovcp_response_add_string(response, backupname);
	}
	else
	{
		return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);
	}

	sconcat(filename, BUF_SIZE, userbackupdir, ".conf", NULL);

	tmpargv = split_string(rsyncoutput, '\n', &tmpargc);
	for(i = 0; i < tmpargc; i++)
	{
		tmpargv2 = split_string(tmpargv[i], ':', &tmpargc2);
		if(tmpargc2 >= 2)
		{
		  if(strcasecmp(tmpargv2[0], "Total file size") == 0)
		  {
			size = malloc(strlen(tmpargv2[1])+1);
			strcpy(size, tmpargv2[1]);
		  }
		}
		free_strings(tmpargv2, tmpargc2);
	}

	size = strtok(size, "b");
	tmp = atof(size);
	tmp = tmp / 1024;
	sprintf(size, "%lf", tmp);
	file_write_many(filename, size, "\n", NULL);

	free_strings(tmpargv, tmpargc);
	free(min);
	free(hour);
	free(day);
	free(month);
	free(year);
	free(backupname);
	free(userbackupdir);
	free(homedir);


	return response;
}

struct ovcp_response_st *restore_userbackups(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0, *arg1;
	struct ovcp_response_st *response;

	char path[BUF_SIZE+1];
	char cachepath[BUF_SIZE];

	char *name, *homedir, *backupdir, *userbackupdir, *backupname, *context=NULL;

	int ret = 0;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	arg1 = ovcp_request_get_arg(request, 1);
	backupname = (char *)ovcp_data_get(arg1);

	userbackupdir = concat(global_settings.userbackup_dir, "/", backupname, NULL);
	homedir = concat(global_settings.root_dir, "/", name, NULL);
	backupdir = concat(global_settings.backup_dir, "/_", name, NULL);

	//clear cache
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

	response = ovcp_response_new();

	if(dir_exist(homedir) && dir_exist(userbackupdir))
	{

		if(dir_exist(global_settings.backup_dir))
		{
			execb_cmd("mv", "mv", homedir, backupdir, NULL);

			if(!dir_exist(backupdir))
				return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);
		}

		execb_cmd("rm", "rm", "-rf", homedir, NULL);

		if(dir_exist(homedir))
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		execb_cmd("cp", "cp", "-a", userbackupdir, homedir, NULL);

		if(!dir_exist(homedir))
		{
			execb_cmd("mv", "mv", backupdir, homedir, NULL);
			execb_cmd("rm", "rm", "-rf", backupdir, NULL);
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);
		}
		else
		{
		
			// restore backup of init style
			char *srcpath, *destpath;
			srcpath = concat(userbackupdir, "/initstyle.backup", NULL);
			destpath = concat(global_settings.config_dir, "/", name, "/apps/init/style", NULL);

			if(file_exist(srcpath))
			{
				unlink(destpath);
				copy(srcpath, destpath);
			}
			
			free(srcpath);
			free(destpath);
			
		
			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/context", NULL);
			context = file_read(path);
			trim_string(context);

			if(fs_tagxid_support(homedir)) execb_cmd("chxid", "chxid", "-c",  context, "-R", "--", homedir, NULL);


			ovcp_response_add_string(response, "Success");
		}
	}
	execb_cmd("rm", "rm", "-rf", backupdir, NULL);
	free(backupdir);
	if(context != NULL) free(context);
	free(userbackupdir);
	free(homedir);
	return response;

}

struct ovcp_response_st *refresh_userbackups(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0, *arg1;
	struct ovcp_response_st *response;

	char *name, *homedir, *userbackupdir, *backupname;
	char *backupnamenew, *userbackupdirnew, *filename, *size=NULL;
	char *rsyncoutput = NULL, **tmpargv, **tmpargv2;

	int ret=0, i = 0;
	unsigned  int tmpargc, tmpargc2;
	double tmp = 0;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	arg1 = ovcp_request_get_arg(request, 1);
	backupname = (char *)ovcp_data_get(arg1);

	userbackupdir = concat(global_settings.userbackup_dir, "/", backupname, NULL);
	homedir = concat(global_settings.root_dir, "/", name, "/", NULL);

	response = ovcp_response_new();

	if(dir_exist(homedir) && dir_exist(userbackupdir))
	{

		if(!dir_exist(userbackupdir))
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		if((rsyncoutput = exec_cmd("rsync", 65535, "rsync", "-a", "-H", "-l", "--stats", "--delete", homedir, userbackupdir, NULL)) == NULL)
		{
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);
		}

		ovcp_response_add_string(response, "Success");
	}

	char *min, *hour, *day, *month, *year;

	struct tm *currentdate;
	time_t timestamp = time(0);
	currentdate = localtime(&timestamp);

	min = (char *)malloc(4);
	if(currentdate->tm_min < 10) {
		snprintf(min, 3, "0%i", currentdate->tm_min);
	}
	else {
		snprintf(min, 3, "%i", currentdate->tm_min);
	}
	hour = (char *)malloc(4);
	if(currentdate->tm_hour < 10) {
		snprintf(hour, 3, "0%i", currentdate->tm_hour);
	}
	else {
		snprintf(hour, 3, "%i", currentdate->tm_hour);
	}
	day = (char *)malloc(4);
	if(currentdate->tm_mday < 10) {
		snprintf(day, 3, "0%i", currentdate->tm_mday);
	}
	else {
		snprintf(day, 3, "%i", currentdate->tm_mday);
	}
	month = (char *)malloc(4);
	if(currentdate->tm_mon < 9) {
		snprintf(month, 3, "0%i", currentdate->tm_mon+1);
	}
	else {
		snprintf(month, 3, "%i", currentdate->tm_mon+1);
	}
	year = (char *)malloc(6);
	snprintf(year, 5, "%i", currentdate->tm_year+1900);

	backupnamenew = concat(name, "_", year, ".", month, ".", day, "_", hour, ":", min, NULL);
	userbackupdirnew = concat(global_settings.userbackup_dir, "/", backupnamenew, NULL);

	ret = execb_cmd("mv", "mv", userbackupdir, userbackupdirnew, NULL);

	filename = concat(userbackupdirnew, ".conf", NULL);
	tmpargv = split_string(rsyncoutput, '\n', &tmpargc);
	for(i = 0; i < tmpargc; i++)
	{
		tmpargv2 = split_string(tmpargv[i], ':', &tmpargc2);
		if(tmpargc2 >= 2)
		{
		  if(strcasecmp(tmpargv2[0], "Total file size") == 0)
		  {
			size = malloc(strlen(tmpargv2[1])+1);
			strcpy(size, tmpargv2[1]);
		  }
		}
		free_strings(tmpargv2, tmpargc2);
	}

	size = strtok(size, "b");
	tmp = atof(size);
	tmp = tmp / 1024;
	sprintf(size, "%lf", tmp);
	file_write_many(filename, size, "\n", NULL);

	free_strings(tmpargv, tmpargc);
	free(min);
	free(hour);
	free(day);
	free(month);
	free(year);
	free(userbackupdir);
	free(userbackupdirnew);
	free(backupnamenew);
	free(homedir);
	free(filename);
	if(rsyncoutput != NULL) free(rsyncoutput);
	
	return response;

}

struct ovcp_response_st *delete_userbackups(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;

	char *userbackupdir, *backupname;

	arg0 = ovcp_request_get_arg(request, 0);
	backupname = (char *)ovcp_data_get(arg0);

	if(strlen(backupname) > 0)
		userbackupdir = concat(global_settings.userbackup_dir, "/", backupname, NULL);
	else
		return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

	response = ovcp_response_new();

	if(dir_exist(userbackupdir))
	{

		execb_cmd("rm", "rm", "-rf", userbackupdir, NULL);

		if(dir_exist(userbackupdir))
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		ovcp_response_add_string(response, "Success");
	}
	free(userbackupdir);
	return response;
}
