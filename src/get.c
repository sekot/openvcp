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
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <sys/wait.h>

#include "main.h"
#include "config.h"
#include "fs.h"
#include "misc.h"
#include "strings.h"
#include "error.h"
#include "get.h"

typedef unsigned int xid_t;
typedef unsigned int nid_t;
typedef unsigned int tag_t;

#include <vserver.h>

struct ovcp_response_st *get_xid(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *name, *xidfile, *xid;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();

	xidfile = concat(global_settings.config_dir, "/", name, "/context", NULL);

	if(file_exist(xidfile))
	{
		xid = file_read(xidfile);
		trim_string(xid);
		ovcp_response_add_int(response, atoi(xid));
		free(xid);
	}
	free(xidfile);

	return response;
}


struct ovcp_response_st *get_rlimit(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char rlimitfile[BUF_SIZE], *name, *rlimit;
	int i = 0;

	char *rlimit_params[] = {"cpu", "fsize", "data", "stack",
				 "core", "rss", "nproc", "nofile",
				 "memlock", "as", "locks", NULL };

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	response = ovcp_response_new();

	while(rlimit_params[i] != NULL)
	{
		ovcp_response_struct_new(response);

		ovcp_response_struct_add_string(response, "name", rlimit_params[i]);

		sconcat(rlimitfile, BUF_SIZE, global_settings.config_dir, "/", name, "/rlimits/", rlimit_params[i], ".hard", NULL);
		rlimit = file_read(rlimitfile);

		if(rlimit != NULL)
		{
			trim_string(rlimit);
			ovcp_response_struct_add_string(response, "hard", rlimit);
			free(rlimit);
		}

		sconcat(rlimitfile, BUF_SIZE, global_settings.config_dir, "/", name, "/rlimits/", rlimit_params[i], ".soft", NULL);
		rlimit = file_read(rlimitfile);

		if(rlimit != NULL)
		{
			trim_string(rlimit);
			ovcp_response_struct_add_string(response, "soft", rlimit);
			free(rlimit);
		}

		sconcat(rlimitfile, BUF_SIZE, global_settings.config_dir, "/", name, "/rlimits/", rlimit_params[i], ".min", NULL);
		rlimit = file_read(rlimitfile);

		if(rlimit != NULL)
		{
			trim_string(rlimit);
			ovcp_response_struct_add_string(response, "min", rlimit);
			free(rlimit);
		}

		ovcp_response_struct_end(response);

		i++;
	}



	return response;
}

struct ovcp_response_st *get_state(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *name, *rescuefile;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	response = ovcp_response_new();



	if(vserver_isrunning(name))
		ovcp_response_add_string(response, "Online");
	else
		ovcp_response_add_string(response, "Offline");


	rescuefile = concat(global_settings.config_dir, "/", name, "/rescue", NULL);
	if(file_exist(rescuefile))
		ovcp_response_add_string(response, "rescue");
	free(rescuefile);

	return response;
}


struct ovcp_response_st *get_ips(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	struct dirent *dir_info;
	char path[BUF_SIZE+1];
	char *name, *value;
	DIR *dirst;
	int i;

	char *iface_params[] = { "bcast", "dev", "disabled", "ip",
				 "mask", "name", "nodev", "novlandev",
				 "prefix", "scope", "vlandev", "tun",
				 "tap", "linktype", "nocsum", "shared", "peer", NULL };

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/interfaces", NULL);
	dirst = opendir(path);


	if(dirst != NULL)
	{

		while ((dir_info = readdir(dirst)) != NULL)
		{

			if(dir_info->d_name[0] != '.')
			{

				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/interfaces/", dir_info->d_name, NULL);

				if(dir_exist(path))
				{
					i = 0;

					ovcp_response_struct_new(response);
					ovcp_response_struct_add_string(response, "id", dir_info->d_name);

					while(iface_params[i] != NULL)
					{
						sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/interfaces/", dir_info->d_name, "/", iface_params[i], NULL);

						if(file_exist(path))
						{
							value = file_read(path);

							if(value != NULL)
							{
								trim_string(value);
								ovcp_response_struct_add_string(response, iface_params[i], value);
								free(value);
							}
							else
							{
								ovcp_response_struct_add_boolean(response, iface_params[i], TRUE);
							}
						}

						i++;
					}


					ovcp_response_struct_end(response);

				}

			}


		}

		closedir(dirst);

	}


	return response;
}

struct ovcp_response_st *get_flags(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *name, *flagfile;
	char *flags, *flag;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();

	if(kernel_vci_version <= 0x20200)
	{
		flagfile = concat(global_settings.config_dir, "/", name, "/flags", NULL);
	}
	else
	{
		flagfile = concat(global_settings.config_dir, "/", name, "/cflags", NULL);
	}

	ovcp_response_array_new(response);

	if(file_exist(flagfile))
	{

		flags = file_read(flagfile);
		flag = strtok(flags, "\n");

		if(flag != NULL)
		{
			ovcp_response_array_add_string(response, flag);

			while((flag = strtok(NULL, "\n")) != NULL)
				ovcp_response_array_add_string(response, flag);

		}

		free(flags);
	}

	ovcp_response_array_end(response);

	free(flagfile);

	return response;
}


struct ovcp_response_st *get_filter(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	char *name, *sql_error;
	struct ovcp_data_st *arg0;
	char sql_query[BUF_SIZE];
	int i, ret, nrow, ncolumn;
	char **sql_result;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);



	snprintf(sql_query, BUF_SIZE, "SELECT * FROM iptable_rules where name='%s'", name);


	ret = sqlite3_get_table(ovcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}

	i = 1;

	response = ovcp_response_new();


	while(i != nrow+1)
	{
		ovcp_response_struct_new(response);

		ovcp_response_struct_add_string(response, "id", sql_result[i*(ncolumn)+10]);
		ovcp_response_struct_add_string(response, "direction", sql_result[i*(ncolumn)+0]);
		ovcp_response_struct_add_string(response, "proto", sql_result[i*(ncolumn)+1]);

		if(strcmp(sql_result[i*(ncolumn)+2], "(null)") != 0)
			ovcp_response_struct_add_string(response, "srcip", sql_result[i*(ncolumn)+2]);
		if(atoi(sql_result[i*(ncolumn)+3]) != -1)
			ovcp_response_struct_add_string(response, "srcport", sql_result[i*(ncolumn)+3]);

		if(strcmp(sql_result[i*(ncolumn)+4], "(null)") != 0)
			ovcp_response_struct_add_string(response, "destip", sql_result[i*(ncolumn)+4]);
		if(atoi(sql_result[i*(ncolumn)+5]) != -1)
			ovcp_response_struct_add_string(response, "destport", sql_result[i*(ncolumn)+5]);

		ovcp_response_struct_add_string(response, "target", sql_result[i*(ncolumn)+6]);

		if(strcmp(sql_result[i*(ncolumn)+7], "") != 0) {
			ovcp_response_struct_add_string(response, "match", sql_result[i*(ncolumn)+7]);
			ovcp_response_struct_add_string(response, "match-value", sql_result[i*(ncolumn)+8]);
		}

		ovcp_response_struct_end(response);

		i++;
	}


	sqlite3_free_table(sql_result);

	return response;
}

struct ovcp_response_st *get_schedule(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *value, *name;
	char path[BUF_SIZE];
	int i = 0;

	char *sched_params[] = {"fill-rate", "fill-rate2",
				"interval", "interval2",
				"priority-bias", "tokens",
				"tokens-max", "tokens-min",
				"idle-time", NULL};

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();

	ovcp_response_struct_new(response);

	while(sched_params[i] != NULL)
	{
		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/sched/", sched_params[i],  NULL);

		if(file_exist(path))
		{
			value = file_read(path);

			if(value != NULL)
			{
				trim_string(value);
				ovcp_response_struct_add_string(response, sched_params[i], value);
				free(value);
			}
		}

		i++;
	}

	ovcp_response_struct_end(response);

	return response;
}

struct ovcp_response_st *get_cgroup(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *value, *name;
	char path[BUF_SIZE];
	int i = 0;

	char *cgroup_params[] = {"cpu.cfs_hard_limit",
			"cpu.cfs_runtime_us", "cpu.cfs_period_us",
			"cpu.shares", "cpuset.cpus", NULL};

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	if(!fs_cgroup_support())
		return ovcp_response_error(OVCP_ERROR_CGROUP, OVCP_ERROR_CGROUP_DESC);

	response = ovcp_response_new();

	while(cgroup_params[i] != NULL)
	{
		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/cgroup/", cgroup_params[i],  NULL);

		if(file_exist(path))
		{
                        ovcp_response_struct_new(response);

                        ovcp_response_struct_add_string(response, "name", cgroup_params[i]);

                        value = file_read(path);
			if(value != NULL)
			{
				trim_string(value);
				ovcp_response_struct_add_string(response, "value", value);
				free(value);
			}
                        else
                        {
                            ovcp_response_struct_add_string(response, "value", "");
                        }

                	ovcp_response_struct_end(response);
		}

		i++;
	}

	return response;
}

struct ovcp_response_st *get_resources(struct ovcp_request_st *request)
{
	char path[BUF_SIZE];
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;

	char *limitfile;
	char **lines, *word, *name;
	unsigned int lcount;
	int i, xid;



	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	xid = vserver_xid(name);


	snprintf(path, BUF_SIZE, "/proc/virtual/%d/limit", xid);
	limitfile = file_read(path);

	if(limitfile == NULL)
		return ovcp_response_error(OVCP_ERROR_VSNRUNING, OVCP_ERROR_VSNRUNING_DESC);


	response = ovcp_response_new();
	ovcp_response_struct_new(response);

	lines = split_string(limitfile, '\n',  &lcount);
	i = 0;

	while(i < lcount)
	{
		word = strtok(lines[i], " ");
		trim_string(word);


		if(strcmp(word, "PROC:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "proc", word);
		}
		else
		if(strcmp(word, "VM:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "vm", word);
		}
		if(strcmp(word, "VML:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "vml", word);
		}
		else
		if(strcmp(word, "RSS:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "rss", word);
		}
		else
		if(strcmp(word, "ANON:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "anon", word);
		}
		else
		if(strcmp(word, "FILES:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "files", word);
		}
		else
		if(strcmp(word, "OFD:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "ofd", word);
		}
		else
		if(strcmp(word, "LOCKS:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "locks", word);
		}
		else
		if(strcmp(word, "SOCK:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "sock", word);
		}
		else
		if(strcmp(word, "MSGQ:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "msgq", word);
		}
		else
		if(strcmp(word, "SHM:") == 0)
		{
			word = strtok(NULL, " ");
			trim_string(word);
			ovcp_response_struct_add_string(response, "shm", word);
		}


		i++;
	}

	ovcp_response_struct_end(response);

	free_strings(lines, lcount);
	free(limitfile);


	return response;
}

struct ovcp_response_st *get_mark(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char path[BUF_SIZE], *name;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name,"/apps/init/mark", NULL);

	response = ovcp_response_new();

	if(file_exist(path))
		ovcp_response_add_boolean(response, TRUE);
	else
		ovcp_response_add_boolean(response, FALSE);


 	return response;
}

struct ovcp_response_st *get_hostname(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
 	char *hostfile, *host, *name;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


   	hostfile = concat(global_settings.config_dir, "/", name, "/uts/nodename", NULL);

	if(file_exist(hostfile))
	{
		host = file_read(hostfile);
		trim_string(host);
		ovcp_response_add_string(response, host);
		free(host);
	}
	else
	{
		ovcp_response_add_string(response, "");
	}

	free(hostfile);

 	return response;
}


struct ovcp_response_st *get_image(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char path[BUF_SIZE+1], *image, *name;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/image_name", NULL);

	if(file_exist(path))
	{
		image = file_read(path);
		trim_string(image);
		ovcp_response_add_string(response, image);
		free(image);
	}
	else
	{
		ovcp_response_add_string(response, "");
	}

	return response;
}


struct ovcp_response_st *get_space(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;

	char path[BUF_SIZE];
	char *dlimit_file, *pathoflimit, *name;

	int space_total, space_used, space_reserved, space_inodes_total, space_inodes_used;
    	struct vc_ctx_dlimit limits;
	xid_t xid;

	DIR *dlimitdir;
	struct dirent *dlimitentry;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


	xid = vserver_xid(name);

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/", "dlimits/", NULL);
	dlimitdir = opendir(path);


	if(dlimitdir != NULL)
	{

		while((dlimitentry = readdir(dlimitdir)) != NULL)
		{
			space_total = 0;
			space_used = 0;
			space_reserved = 0;
			space_inodes_total = 0;
			space_inodes_used = 0;


			if(dlimitentry->d_name[0] == '.')
				continue;

			sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/", "dlimits/", dlimitentry->d_name, "/directory", NULL);
			pathoflimit = file_read(path);
			trim_string(pathoflimit);

			if(vc_get_dlimit(pathoflimit, xid, 1, &limits) != -1)
			{
				space_total = limits.space_total;
				space_used = limits.space_used;
				space_reserved = limits.reserved;
				space_inodes_total = limits.inodes_total;
				space_inodes_used = limits.inodes_used;

			}
			else
			{

				space_total = -1;
				space_inodes_total = -1;
				space_reserved = -1;

				space_inodes_used = -1;
				space_used = -1;

				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/", "dlimits/", dlimitentry->d_name, "/space_total", NULL);
				dlimit_file = file_read(path);

				if(dlimit_file != NULL)
				{
					trim_string(dlimit_file);
					space_total = atoi(dlimit_file);
					free(dlimit_file);
				}

				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/", "dlimits/", dlimitentry->d_name, "/inodes_total", NULL);
				dlimit_file = file_read(path);

				if(dlimit_file != NULL)
				{
					trim_string(dlimit_file);
					space_inodes_total = atoi(dlimit_file);
					free(dlimit_file);
				}


				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/", "dlimits/", dlimitentry->d_name, "/reserved", NULL);
				dlimit_file = file_read(path);

				if(dlimit_file != NULL)
				{
					trim_string(dlimit_file);
					space_reserved = atoi(dlimit_file);
					free(dlimit_file);
				}

			}

			ovcp_response_struct_new(response);

			if( (strncmp(pathoflimit, global_settings.root_dir, strlen(global_settings.root_dir)) == 0)
			   && ( strncmp(pathoflimit+strlen(global_settings.root_dir)+1, name, strlen(name)) == 0))
			{
				ovcp_response_struct_add_string(response, "path", pathoflimit+strlen(global_settings.root_dir)+strlen(name)+1);
			}
			else
			{
				ovcp_response_struct_add_string(response, "path", pathoflimit);
				ovcp_response_struct_add_boolean(response, "extern", 1);
			}

			ovcp_response_struct_add_int(response, "total", space_total);
			ovcp_response_struct_add_int(response, "used", space_used);
			ovcp_response_struct_add_int(response, "inodes_total", space_inodes_total);
			ovcp_response_struct_add_int(response, "inodes_used", space_inodes_used);
			ovcp_response_struct_add_int(response, "reserved", space_reserved);

			ovcp_response_struct_end(response);


			free(pathoflimit);

		}

		closedir(dlimitdir);
	}
	else
	{

			space_total = -1;
			space_used = -1;
			space_reserved = -1;
			space_inodes_total = -1;
			space_inodes_used = -1;

			sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/", NULL);

			if(vc_get_dlimit(path, xid, 1, &limits) != -1)
			{
				space_total = limits.space_total;
				space_used = limits.space_used;
				space_reserved = limits.reserved;
				space_inodes_total = limits.inodes_total;
				space_inodes_used = limits.inodes_used;

			}

			ovcp_response_struct_new(response);

			ovcp_response_struct_add_string(response, "path", "/");

			ovcp_response_struct_add_int(response, "total", space_total);
			ovcp_response_struct_add_int(response, "used", space_used);
			ovcp_response_struct_add_int(response, "inodes_total", space_inodes_total);
			ovcp_response_struct_add_int(response, "inodes_used", space_inodes_used);
			ovcp_response_struct_add_int(response, "reserved", space_reserved);


	}

	return response;
}



struct ovcp_response_st *get_uptime(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;

	char *cvirt_file, *context, *name;
	char path[BUF_SIZE+1];
	int i = 0;
	char *p;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/context", NULL);
	context = file_read(path);

	if(context != NULL)
	{
		trim_string(context);

		sconcat(path, BUF_SIZE, "/proc/virtual/", context, "/cvirt", NULL);

		cvirt_file = file_read(path);

		if(cvirt_file != NULL)
		{
			p = cvirt_file;

			while(*p != 0 && *p != ':') p++;
			p++;
			while(*p != 0 && (*p == ' ' || *p == '\t')) p++;

			while(p[i] != 0 && p[i] != '\n')
				i++;

			p[i] = 0;

			ovcp_response_add_string(response, p);

			free(cvirt_file);
		}
		else
		{
			ovcp_response_add_string(response, "");
		}

		free(context);
	}
	else
	{
		ovcp_response_add_string(response, "");
	}

	return response;
}



struct ovcp_response_st *get_load(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;

	char *cvirt_file, *context, *name;
	char path[BUF_SIZE+1];
	int i = 0, line_num = 0;
	char *p;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/context", NULL);
	context = file_read(path);

	if(context != NULL)
	{
		trim_string(context);

		sconcat(path, BUF_SIZE, "/proc/virtual/", context, "/cvirt", NULL);

		cvirt_file = file_read(path);

		if(cvirt_file != NULL)
		{
			p = cvirt_file;

			while(*p != 0)
			{
				if(kernel_vci_version >= 0x20200 && line_num == 6)
					break;
				if(kernel_vci_version < 0x20200 && line_num == 12)
					break;

				while(*p != 0 && *p != '\n')
					p++;

				if(*p != 0)
				{
					line_num++;
					p++;
				}
			}


			while(*p != 0 && *p != ':') p++;

			p++;

			while(*p != 0 && (*p == ' ' || *p == '\t')) p++;

			while(p[i] != 0 && p[i] != '\n')
				i++;

			p[i] = 0;

			ovcp_response_add_string(response, p);

			free(cvirt_file);
		}
		else
		{
			ovcp_response_add_string(response, "");
		}

		free(context);
	}
	else
	{
		ovcp_response_add_string(response, "");
	}

	return response;
}
