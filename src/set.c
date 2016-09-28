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
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>

#include "fs.h"
#include "set.h"
#include "main.h"
#include "config.h"
#include "strings.h"
#include "error.h"
#include "filter.h"
#include "misc.h"

typedef unsigned int xid_t;
typedef unsigned int nid_t;
typedef unsigned int tag_t;

#include <vserver.h>

struct ovcp_response_st *set_rlimit(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;
	char *filepath, *limitname, *limit, *name, *dir;
	int i = 0, ret;
	char rlimit_name[BUF_SIZE];

	struct vc_rlimit current_limit;

	current_limit.hard = 0;
	current_limit.soft = 0;
	current_limit.min = 0;

	xid_t xid;

	struct vlimit_st
	{
		unsigned int id;
		char *name;
	};

	struct vlimit_st rlimit_params[] =
	{
		{0, "cpu"},{1, "fsize"},{2, "data"}, {3, "stack"},
		{4, "core"},{5, "rss"},{6, "nproc"},{7, "nofile"},
		{8, "memlock"}, {9, "as"},{10, "locks"}, {-1, NULL}
	};

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	limitname = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || limitname == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	limit = (char *)ovcp_data_get(arg2);

	if(!ovcp_data_is_string(arg2) || limit == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	while(rlimit_params[i].name != NULL)
	{
		sconcat(rlimit_name, BUF_SIZE, rlimit_params[i].name, ".hard", NULL);

		if(strcmp(rlimit_name, limitname) == 0)
			break;

		sconcat(rlimit_name, BUF_SIZE, rlimit_params[i].name, ".soft", NULL);

		if(strcmp(rlimit_name, limitname) == 0)
			break;

		sconcat(rlimit_name, BUF_SIZE, rlimit_params[i].name, ".min", NULL);

		if(strcmp(rlimit_name, limitname) == 0)
			break;

		i++;
	}

	if(rlimit_params[i].name == NULL)
		return ovcp_response_error(OVCP_ERROR_WRONGRLIMIT, "set_rlimit: "OVCP_ERROR_WRONGRLIMIT_DESC);



	xid = vserver_xid(name);


	if((ret = vc_get_rlimit (xid, rlimit_params[i].id, &current_limit)) != -1)
	{

		if(strcmp(rlimit_name+strlen(rlimit_name)-4, "hard") == 0)
		{
			ovcp_log(OVCP_DEBUG, "Current HARD: %X", (unsigned int)current_limit.hard);
			current_limit.hard = atoi(limit);
		}
		else
		if(strcmp(rlimit_name+strlen(rlimit_name)-4, "soft") == 0)
		{
			ovcp_log(OVCP_DEBUG, "Current SOFT: %X", (unsigned int)current_limit.soft);
			current_limit.soft = atoi(limit);
		}
		else
		if(strcmp(rlimit_name+strlen(rlimit_name)-3, "min") == 0)
		{
			ovcp_log(OVCP_DEBUG, "Current MIN: %X", (unsigned int)current_limit.min);
			current_limit.min = atoi(limit);
		}

		vc_set_rlimit (xid, rlimit_params[i].id, &current_limit);
	}


	response = ovcp_response_new();


	dir = concat(global_settings.config_dir, "/", name, "/rlimits", NULL);
	filepath = concat(dir, "/", limitname, NULL);

	if(!dir_exist(dir))
		mkdir(dir, 0755);


	if(strcmp(limit, "0") == 0)
	{
		if (file_exist(filepath))
			unlink(filepath);
	}
	else
	{
		file_write_many(filepath, limit, "\n", NULL);
	}

	ovcp_response_add_boolean(response, TRUE);

	free(filepath);
	free(dir);

	return response;
}


struct ovcp_response_st *set_filter(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	int i=0, ips_num;
	struct ovcp_ipt_rule rule;
	struct ovcp_data_st *arg0, *arg1, *struct_element;
	char *name, *value, **ips;


	char *filter_params[] = {"direction", "proto", "srcip",
				"srcport", "destip", "destport",
				"target", "match", "match-value",
				NULL};


	memset(&rule, 0, sizeof(struct ovcp_ipt_rule));
	rule.srcport = -1;
	rule.destport = -1;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	arg1 = ovcp_request_get_arg(request, 1);


	if(ovcp_data_is_struct(arg1))
	{
		while(filter_params[i] != NULL)
		{
			struct_element = ovcp_struct_get_element(arg1, filter_params[i]);

			if(struct_element != NULL)
			{
				if(ovcp_data_is_string(struct_element))
				{
					value = ovcp_data_get(struct_element);

					if(strcmp("direction", filter_params[i]) == 0)
					{
						if(strcmp("INPUT", value) == 0)
						{
							rule.direction = IPT_DIRECTION_IN;
						}
						else
						{
							rule.direction = IPT_DIRECTION_OUT;
						}

					}
					else
					if(strcmp("proto", filter_params[i]) == 0)
					{
						if(strcmp("tcp", value) == 0)
						{
							rule.proto = IPT_PROTO_TCP;
						}
						else if(strcmp("udp", value) == 0)
						{
							rule.proto = IPT_PROTO_UDP;
						}
						else if(strcmp("icmp", value) == 0)
						{
							rule.proto = IPT_PROTO_ICMP;
						}
                                                else
                                                {
                                                    rule.proto = IPT_PROTO_ALL;
                                                }
					}
					else
					if(strcmp("srcip", filter_params[i]) == 0)
					{
						rule.srcip = value;
					}
					else
					if(strcmp("srcport", filter_params[i]) == 0)
					{
						rule.srcport = atoi(value);
					}
					else
					if(strcmp("destip", filter_params[i]) == 0)
					{
						rule.destip = value;
					}
					else
					if(strcmp("destport", filter_params[i]) == 0)
					{
						rule.destport = atoi(value);
					}
					else
					if(strcmp("target", filter_params[i]) == 0)
					{
						if(strcmp("DROP", value) == 0)
						{
							rule.target = IPT_TARGET_DROP;
						}
						else if(strcmp("REJECT", value) == 0)
						{
							rule.target = IPT_TARGET_REJECT;
						}
						else
						{
							rule.target = IPT_TARGET_ACCEPT;
						}
					}
					else
					if(strcmp("match", filter_params[i]) == 0)
					{

						if(strcmp("LIMIT", value) == 0)
						{
							rule.match = IPT_MATCH_LIMIT;
						}
						else if(strcmp("ICMP", value) == 0)
						{
							rule.match = IPT_MATCH_ICMP;
						}
						else
						{
							rule.match = IPT_MATCH_STATE;
						}
					}
					else
					if(strcmp("match-value", filter_params[i]) == 0)
					{

						rule.match_value = 0;

						if(rule.match == IPT_MATCH_STATE)
						{
							if(strstr(value, "ESTABLISHED"))
							{
								rule.match_value = rule.match_value ^ IPT_STATE_ESTABLISHED;
							}
							if(strstr(value, "NEW"))
							{
								rule.match_value = rule.match_value ^ IPT_STATE_NEW;
							}
							if(strstr(value, "RELATED"))
							{
								rule.match_value = rule.match_value ^ IPT_STATE_RELATED;
							}
						}
						else
						{
							rule.match_value = atoi(value);
						}
						printf("Match Value: %d", rule.match_value);
					}

				}
				else
				{
					response = ovcp_response_error(OVCP_ERROR_WRONGTYPE,
							"vserver.set_rule: "OVCP_ERROR_WRONGTYPE_DESC);
					break;
				}
			}

			i++;
		}

	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_WRONGTYPE,
						"vserver.set_rule: "OVCP_ERROR_WRONGTYPE_DESC);
	}


	ips = parse_ips(name, &ips_num);



	i=0;
	while(i < ips_num)
	{
		if(rule.srcip != NULL && strcmp(ips[i], rule.srcip) == 0)
			break;
		if(rule.destip != NULL && strcmp(ips[i], rule.destip) == 0)
			break;
		i++;
	}

	free_strings(ips, ips_num);

	if((rule.srcip == NULL && rule.destip == NULL) || i == ips_num)
	{
		response = ovcp_response_error(OVCP_ERROR_RULENPERM,
						"vserver.set_rule: "OVCP_ERROR_RULENPERM_DESC);
	}
	else
	{
		ovcp_iptable_rule(&rule);
		ovcp_store_rule(name, rule);

		response = ovcp_response_new();
		ovcp_response_add_boolean(response, TRUE);
	}

	return response;
}

struct ovcp_response_st *del_filter(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *name, *id, *sql_error;
	char sql_query[BUF_SIZE];
	int ret;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	id = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || id == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);



	snprintf(sql_query, BUF_SIZE, "DELETE FROM iptable_rules where name='%s' and id=%d", name, atoi(id));

	ret = sqlite3_exec(ovcp_db, sql_query, NULL, 0, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return NULL;
	}

	ovcp_create_tables(ovcp_db);


	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);


	return response;
}


struct ovcp_response_st *set_flag_add(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *filepath, *flag, *name;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	flag = (char *)ovcp_data_get(arg1);
	string_toupper(flag);

	if(!ovcp_data_is_string(arg1) || flag == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	response = ovcp_response_new();

	if(kernel_vci_version <= 0x20200)
	{
		filepath = concat(global_settings.config_dir, "/", name, "/flags", NULL);
	}
	else
	{
		filepath = concat(global_settings.config_dir, "/", name, "/cflags", NULL);
	}

	file_attach(filepath, flag);
	file_attach(filepath, "\n");

	ovcp_response_add_boolean(response, TRUE);

	free(filepath);


	return response;
}

struct ovcp_response_st *set_flag_remove(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *filepath, *flagfile, *flag, *p, *name;
	int flagfile_fd, ret;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	flag = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || flag == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	if(kernel_vci_version <= 0x20200)
	{
		filepath = concat(global_settings.config_dir, "/", name, "/flags", NULL);
	}
	else
	{
		filepath = concat(global_settings.config_dir, "/", name, "/cflags", NULL);
	}

	flagfile = file_read(filepath);

	flagfile_fd = open(filepath, O_CREAT | O_TRUNC | O_WRONLY, 0644);

	if(flagfile_fd != -1 && flagfile != NULL)
	{
		p = strstr(flagfile, flag);
		if(p != NULL)
		{
			ret = write(flagfile_fd, flagfile, p-flagfile);
			ret = write(flagfile_fd, p+strlen(flag)+1, strlen(p+strlen(flag)+1));

			response = ovcp_response_new();
			ovcp_response_add_boolean(response, TRUE);

		}
		else
		{
			response = ovcp_response_error(OVCP_ERROR_FLAGNFOUND,
							"vserver.remove_flag: "OVCP_ERROR_FLAGNFOUND_DESC);
			ret = write(flagfile_fd, flagfile, strlen(flagfile));
		}

	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_FILEMISSING,
						"vserver.remove_flag: "OVCP_ERROR_FILEMISSING_DESC);
	}


	if(flagfile != NULL) free(flagfile);
	free(filepath);



	return response;
}


struct ovcp_response_st *set_mark(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1;
	char path[BUF_SIZE], *name;
	int *value;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	value = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_boolean(arg1))
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name,"/apps/", NULL);

	if(!dir_exist(path))
		mkdir(path, 0755);

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name,"/apps/init/", NULL);

	if(!dir_exist(path))
		mkdir(path, 0755);



	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name,"/apps/init/mark", NULL);


	response = ovcp_response_new();

	if(*value == 1)
	{

		if(file_write(path, "default\n") != -1)
			ovcp_response_add_boolean(response, TRUE);
		else
			ovcp_response_add_boolean(response, FALSE);
	}
	else
	{

		if(unlink(path) != -1)
			ovcp_response_add_boolean(response, TRUE);
		else
			ovcp_response_add_boolean(response, FALSE);
	}



	return response;
}


struct ovcp_response_st *set_schedule(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1, *struct_element;
	char filepath[BUF_SIZE], *value, *name;
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

	sconcat(filepath, BUF_SIZE, global_settings.config_dir, "/", name, "/sched/", NULL);
	if(!dir_exist(filepath))
		mkdir(filepath, 0755);


	arg1 = ovcp_request_get_arg(request, 1);


	if(ovcp_data_is_struct(arg1))
	{
		while(sched_params[i] != NULL)
		{
			struct_element = ovcp_struct_get_element(arg1, sched_params[i]);

			if(struct_element != NULL)
			{
				if(ovcp_data_is_string(struct_element))
				{
					value = ovcp_data_get(struct_element);
					sconcat(filepath, BUF_SIZE, global_settings.config_dir, "/", name, "/sched/", sched_params[i], NULL);

					file_write_many(filepath, value, "\n", NULL);
				}
				else
				{
					response = ovcp_response_error(OVCP_ERROR_WRONGTYPE,
							"vserver.set_schedule: "OVCP_ERROR_WRONGTYPE_DESC);
					break;
				}
			}

			i++;
		}

	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_WRONGTYPE,
						"vserver.set_schedule: "OVCP_ERROR_WRONGTYPE_DESC);
	}


	if(response == NULL)
	{
		response = ovcp_response_new();
		ovcp_response_add_boolean(response, TRUE);
	}



	return response;
}

struct ovcp_response_st *set_cgroup(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1, *arg2;
	char filepath[BUF_SIZE], *name, *cgname, *cgvalue;

        /* reserved
	char *cgroup_params[] = {"cpu.cfs_hard_limit",
			"cpu.cfs_runtime_us", "cpu.cfs_period_us",
			"cpu.shares", "cpuset.cpus", NULL}; */


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	if(!fs_cgroup_support())
		return ovcp_response_error(OVCP_ERROR_CGROUP, OVCP_ERROR_CGROUP_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	cgname = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || cgname == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	cgvalue = (char *)ovcp_data_get(arg2);

	if(!ovcp_data_is_string(arg2) || cgvalue == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	sconcat(filepath, BUF_SIZE, global_settings.config_dir, "/", name, "/cgroup", NULL);
	if(!dir_exist(filepath))
		mkdir(filepath, 0755);


	response = ovcp_response_new();

	sconcat(filepath, BUF_SIZE, global_settings.config_dir, "/", name, "/cgroup/", cgname, NULL);

	if(strcmp(cgvalue, "0") == 0)
	{
		if (file_exist(filepath))
                {
                    unlink(filepath);
                }
                
                //try to apply the new limits directly
                sconcat(filepath, BUF_SIZE, global_settings.cgroupdir, "/", name, "/", cgname, NULL);
                if(file_exist(filepath))
                {
                    dev_write(filepath, cgvalue);
                }
	}
	else
	{
		file_write_many(filepath, cgvalue, "\n", NULL);
                //try to apply the new limits directly
                sconcat(filepath, BUF_SIZE, global_settings.cgroupdir, "/", name, NULL);
                if(dir_exist(filepath))
                {
                    sconcat(filepath, BUF_SIZE, global_settings.cgroupdir, "/", name, "/", cgname, NULL);
                    dev_write(filepath, cgvalue);
                }
	}

	ovcp_response_add_boolean(response, TRUE);

	return response;
}


struct ovcp_response_st *add_ip(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1, *struct_element, *struct_member;
	char path[BUF_SIZE], *name, *value;
	int i = 0, iface_id = 0;

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

	arg1 = ovcp_request_get_arg(request, 1);

	snprintf(path, BUF_SIZE, "%s/%s/interfaces", global_settings.config_dir, name);
	if(!dir_exist(path))
	{
		if(mkdir(path, 0755) != 0)
			return ovcp_response_error(OVCP_ERROR_DIRNCREATED,
					"vserver.add_ip: "OVCP_ERROR_DIRNCREATED_DESC);
	}

	if(ovcp_data_is_struct(arg1))
	{

		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d", global_settings.config_dir, name, i);

	   	while(dir_exist(path))
		{
	      		i++;
			snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d", global_settings.config_dir, name, i);
		}

		iface_id = i;

		if(mkdir(path, 0755) != 0)
			return ovcp_response_error(OVCP_ERROR_DIRNCREATED,
						"vserver.add_ip: "OVCP_ERROR_DIRNCREATED_DESC);

		i = 0;

		while(iface_params[i] != NULL)
		{
			struct_member = ovcp_struct_get_member(arg1, iface_params[i]);

			if(struct_member != NULL)
			{

				struct_element = ovcp_member_get(struct_member);

				snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/%s", global_settings.config_dir, name, iface_id, iface_params[i]);

				if(ovcp_data_is_string(struct_element))
				{
					value = ovcp_data_get(struct_element);
					file_write_many(path, value, "\n", NULL);
				}
				else
				{
					file_touch(path);
				}

			}

			i++;
		}
	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_WRONGTYPE,
						"vserver.add_ip: "OVCP_ERROR_WRONGTYPE_DESC);
	}

	//check for extra operations after tun file creation
	snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/tun", global_settings.config_dir, name, iface_id);
	if(file_exist(path))
	{
		struct in_addr ip, netmask, network;
		char *tmp, *mask, *peer;

		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/ip", global_settings.config_dir, name, iface_id);
		tmp = file_read(path);
		trim_string(tmp);
		inet_aton(tmp, &ip);

		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/mask", global_settings.config_dir, name, iface_id);
		mask = file_read(path);
		trim_string(mask);
		inet_aton(mask, &netmask);

		snprintf(path, BUF_SIZE, "%s/%s/interfaces/%d/peer", global_settings.config_dir, name, iface_id);
		peer = file_read(path);
		trim_string(peer);

		network.s_addr = ip.s_addr & netmask.s_addr;

		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/scripts", NULL);
		if(!dir_exist(path)) mkdir(path, 0755);

		snprintf(path, BUF_SIZE, "%s/%s/scripts/pre-start", global_settings.config_dir, name);
		file_write_many(path, "route add -net ", inet_ntoa(network), " netmask ", mask, " gw ", peer, "\nexit 0\n", NULL);

		snprintf(path, BUF_SIZE, "%s/%s/scripts/post-start", global_settings.config_dir, name);
		file_write_many(path, "route del -net ", inet_ntoa(network), " netmask ", mask, " gw ", peer, "\nexit 0\n", NULL);

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

		free(tmp);
		free(mask);
		free(peer);
	}

	if(response == NULL)
	{
		response = ovcp_response_new();
		ovcp_response_add_int(response, iface_id);
	}


	return response;
}



struct ovcp_response_st *remove_ip(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1;
	char path[BUF_SIZE], *name, *iface_id;



	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	iface_id = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || iface_id == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/interfaces/", iface_id, NULL);

	if(dir_exist(path))
	{
		dir_remove(path);
		response = ovcp_response_new();
		ovcp_response_add_boolean(response, TRUE);
	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_IFACENEXIST,
						"vserver.remove_ip: "OVCP_ERROR_IFACENEXIST_DESC);
	}

	return response;


}


struct ovcp_response_st *set_space(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1, *struct_element;
	char path[BUF_SIZE+1], *name;
	char *directory, *space_total, *inodes_total, *reserved;
	char *pathoflimit, *limit_id = NULL;
	int i;

	DIR *dlimitdir;
	struct dirent *dlimitentry;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	arg1 = ovcp_request_get_arg(request, 1);

	if(!ovcp_data_is_struct(arg1))
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	struct_element = ovcp_struct_get_element(arg1, "directory");

	if(struct_element != NULL && ovcp_data_is_string(struct_element))
		directory = (char *)ovcp_data_get(struct_element);
	else
		return ovcp_response_error(OVCP_ERROR_ARGMISS,
				"vserver.set_space: "OVCP_ERROR_ARGMISS_DESC);

	if(directory[0] != '/')
		return ovcp_response_error(OVCP_ERROR_DIRNOSLASH,
				"vserver.set_space: "OVCP_ERROR_DIRNOSLASH_DESC);

	struct_element = ovcp_struct_get_element(arg1, "space_total");

	if(struct_element != NULL && ovcp_data_is_string(struct_element))
		space_total = (char *)ovcp_data_get(struct_element);
	else
		return ovcp_response_error(OVCP_ERROR_ARGMISS,
				"vserver.set_space: "OVCP_ERROR_ARGMISS_DESC);


	struct_element = ovcp_struct_get_element(arg1, "inodes_total");

	if(struct_element != NULL && ovcp_data_is_string(struct_element))
		inodes_total = (char *)ovcp_data_get(struct_element);
	else
		return ovcp_response_error(OVCP_ERROR_ARGMISS,
				"vserver.set_space: "OVCP_ERROR_ARGMISS_DESC);



	struct_element = ovcp_struct_get_element(arg1, "reserved");

	if(struct_element != NULL && ovcp_data_is_string(struct_element))
		reserved = (char *)ovcp_data_get(struct_element);
	else
	return ovcp_response_error(OVCP_ERROR_ARGMISS,
					"vserver.set_space: "OVCP_ERROR_ARGMISS_DESC);



	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits", NULL);

	if(!dir_exist(path))
		mkdir(path, 0755);


	directory = concat(global_settings.root_dir, "/", name, directory, NULL);
	trim_slashes(directory);

	dlimitdir = opendir(path);

	while((dlimitentry = readdir(dlimitdir)) != NULL)
	{
		if(dlimitentry->d_name[0] == '.')
			continue;

		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits/", dlimitentry->d_name, "/directory", NULL);
		pathoflimit = file_read(path);

		if(pathoflimit == NULL)
			continue;

		trim_string(pathoflimit);
		trim_slashes(pathoflimit);


		if(strcmp(pathoflimit, directory) == 0)
		{
			free(pathoflimit);
			limit_id = strdup(dlimitentry->d_name);
			break;
		}

		free(pathoflimit);
	}

	if(limit_id == NULL)
	{
		i = 0;

		do
		{
			sprintf(path, "%s/%s/dlimits/%d", global_settings.config_dir, name, i);
			i++;
		}
		while(dir_exist(path));

		limit_id = dec2str(i-1);
	}
	closedir(dlimitdir);


	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits/", limit_id, NULL);

	if(!dir_exist(path))
	{
		mkdir(path, 0755);
	}
	else
	{

		if(atoi(space_total) == 0)
		{
			dir_remove(path);

			response = ovcp_response_new();
			ovcp_response_add_boolean(response, TRUE);

			free(directory);
			free(limit_id);

			return response;
		}
	}

        sconcat(path, BUF_SIZE,global_settings.config_dir, "/", name, "/dlimits/", limit_id, "/directory", NULL);
	file_write_many(path, directory, "\n", NULL);

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits/", limit_id, "/space_total", NULL);
	file_write_many(path, space_total, "\n", NULL);

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits/", limit_id, "/inodes_total", NULL);
	file_write_many(path, inodes_total, "\n", NULL);

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/dlimits/", limit_id, "/reserved", NULL);
	file_write_many(path, reserved, "\n", NULL);



	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);

	free(directory);
	free(limit_id);

	return response;
}

struct ovcp_response_st *set_rootpw(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1;
	char *name, *pw;
	char cmd[BUF_SIZE];
	FILE *cmdp;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	pw = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || pw == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	snprintf(cmd, BUF_SIZE, "vserver %s exec chpasswd", name);

	cmdp = popen(cmd, "w");

	if(cmdp != NULL)
	{
		fprintf(cmdp, "root:%s\n", pw);
		pclose(cmdp);

		response = ovcp_response_new();
		ovcp_response_add_boolean(response, TRUE);
	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_CODNSETPW,
						"vserver.set_rootpw: "OVCP_ERROR_CODNSETPW_DESC);
	}



	return response;
}



struct ovcp_response_st *set_hostname(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response = NULL;
	struct ovcp_data_st *arg0, *arg1, *arg2;
	char path[BUF_SIZE+1], subhostname[BUF_SIZE+1];
	char *ip = NULL, *hosts, *hostname, *name;
	int i = 0, len;



	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	hostname = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || hostname == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);

	if(ovcp_data_is_string(arg2))
	{
		ip = (char *)ovcp_data_get(arg2);
	}
	else
	{
		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name, "/interfaces/0/ip", NULL);
		ip = file_read(path);
	}

	len = strlen(hostname);

	while(hostname[i] != '.' && i < len && i < BUF_SIZE)
	{
		subhostname[i] = hostname[i];
		i++;
	}

	subhostname[i] = 0;


	if(ip != NULL)
	{
		sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/etc/hosts", NULL);
		trim_string(ip);

		if(kernel_vci_version <= 0x20200)
		{
			if(i != len)
			{
				hosts = concat("# Warning this file is changed when set hostname is called from openvcp webinterface #\n", ip, "\t", hostname, "\t", subhostname,"\tlocalhost\n", NULL);
			}
			else
			{
				hosts = concat("# Warning this file is changed when set hostname is called from openvcp webinterface #\n", ip, "\t", hostname,"\tlocalhost\n", NULL);
			}
		}
		else
		{
			if(i != len)
			{
				hosts = concat("# Warning this file is changed when set hostname is called from openvcp webinterface #\n", ip, "\t", hostname, "\t", subhostname,"\n127.0.0.1\tlocalhost.localdomain\tlocalhost\n", NULL);
			}
			else
			{
				hosts = concat("# Warning this file is changed when set hostname is called from openvcp webinterface #\n", ip, "\t", hostname,"\n127.0.0.1\tlocalhost.localdomain\tlocalhost\n", NULL);
			}
		}

		file_write(path, hosts);

		free(ip);
		free(hosts);
	}

	sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name,"/etc/hostname", NULL);
	file_write(path, subhostname); file_attach(path, "\n");

	sconcat(path, BUF_SIZE, global_settings.config_dir, "/", name,"/uts/nodename", NULL);
	file_write(path, hostname); file_attach(path, "\n");


	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);


	return response;
}

