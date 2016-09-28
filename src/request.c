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
#include <unistd.h>

#include "strings.h"
#include "misc.h"
#include "main.h"
#include "request.h"
#include "error.h"
#include "logtraffic.h"
#include "fs.h"
#include "trafficlimit.h"
#include "modules.h"


int init_modules(void)
{


	/* Node Module */
	ovcp_add_module("node");
	ovcp_add_method("node", "version", node_version);
	ovcp_add_method("node", "halt", node_halt);
	ovcp_add_method("node", "reboot", node_reboot);
	ovcp_add_method("node", "stat", node_stat);
	ovcp_add_method("node", "get_interfaces", node_get_interfaces);
	ovcp_add_method("node", "get_images", node_get_images);
	ovcp_add_method("node", "traffic", node_get_traffic);
	ovcp_add_method("node", "traffic_total", node_get_traffic_total);
	ovcp_add_method("node", "traffic_hour", node_get_traffic_hour);
	ovcp_add_method("node", "get_uptime", node_get_uptime);
	ovcp_add_method("node", "get_load", node_get_load);
	ovcp_add_method("node", "get_mem", node_get_memory);
	ovcp_add_method("node", "get_space", node_get_space);
	ovcp_add_method("node", "guest_list", vserver_list);



	/* Vserver Module */
	ovcp_add_module("vserver");
	ovcp_add_method("vserver", "get_xid",  get_xid);
	ovcp_method_sethelp("vserver", "get_xid", "", "int", "string", NULL);
	ovcp_add_method("vserver", "get_state", get_state);
	ovcp_method_sethelp("vserver", "get_state", "", "string", "string", NULL);
	ovcp_add_method("vserver", "get_ips", get_ips);
	ovcp_add_method("vserver", "get_flags", get_flags);
	ovcp_add_method("vserver", "get_schedule", get_schedule);
	ovcp_add_method("vserver", "get_hostname", get_hostname);
	ovcp_add_method("vserver", "set_filter", set_filter);
	ovcp_add_method("vserver", "get_filter", get_filter);
	ovcp_add_method("vserver", "del_filter", del_filter);
	ovcp_add_method("vserver", "set_trafficlimit", set_trafficlimit);
	ovcp_add_method("vserver", "get_trafficlimit", get_trafficlimit);
	ovcp_add_method("vserver", "del_trafficlimit", del_trafficlimit);
	ovcp_add_method("vserver", "get_space", get_space);
	ovcp_add_method("vserver", "get_rlimit", get_rlimit);
	ovcp_add_method("vserver", "get_image", get_image);
	ovcp_add_method("vserver", "get_uptime", get_uptime);
	ovcp_add_method("vserver", "get_mark", get_mark);
	ovcp_add_method("vserver", "get_load", get_load);
	ovcp_add_method("vserver", "set_rlimit", set_rlimit);
	ovcp_add_method("vserver", "add_flag", set_flag_add);
	ovcp_add_method("vserver", "remove_flag", set_flag_remove);
	ovcp_add_method("vserver", "set_schedule", set_schedule);
	ovcp_add_method("vserver", "add_iface", add_ip);
	ovcp_add_method("vserver", "remove_iface", remove_ip);
	ovcp_add_method("vserver", "set_space", set_space);
	ovcp_add_method("vserver", "set_rootpw", set_rootpw);
	ovcp_add_method("vserver", "set_hostname", set_hostname);
	ovcp_add_method("vserver", "set_mark", set_mark);
	ovcp_add_method("vserver", "set_bandwidth", set_bandwidth);
	ovcp_add_method("vserver", "get_bandwidth", get_bandwidth);
	ovcp_add_method("vserver", "del_bandwidth", del_bandwidth);
	ovcp_add_method("vserver", "remove", vserver_remove);
	ovcp_add_method("vserver", "traffic", vserver_traffic);
	ovcp_add_method("vserver", "traffic_total", vserver_traffic_total);
	ovcp_add_method("vserver", "traffic_hour", vserver_traffic_hour);
	ovcp_add_method("vserver", "start", vserver_control_start);
	ovcp_add_method("vserver", "stop", vserver_control_stop);
	ovcp_add_method("vserver", "get_resources", get_resources);
	ovcp_add_method("vserver", "resources_hour", vserver_resources_hour);
	ovcp_add_method("vserver", "rescue", vserver_rescue);
	ovcp_add_method("vserver", "get_userbackups", get_userbackups);
	ovcp_add_method("vserver", "create_userbackups", create_userbackups);
	ovcp_add_method("vserver", "restore_userbackups", restore_userbackups);
	ovcp_add_method("vserver", "delete_userbackups", delete_userbackups);
	ovcp_add_method("vserver", "refresh_userbackups", refresh_userbackups);
	ovcp_add_method("vserver", "get_cgroup", get_cgroup);
	ovcp_add_method("vserver", "set_cgroup", set_cgroup);
	ovcp_add_method("vserver", "move", vserver_move);


	/* Setup Module */
	ovcp_add_module("setup");
	ovcp_add_method("setup", "vserver_config", vserver_setup_config);
	ovcp_add_method("setup", "vserver_image", vserver_setup_image);


	/* System Module */
	ovcp_add_module("system");

	ovcp_add_method("system", "listMethods", system_list_methods);
	ovcp_method_sethelp("system", "listMethods", "Enumerate the methods implemented by the server", "array", NULL);

	ovcp_add_method("system", "methodSignature",  system_method_signature);
	ovcp_method_sethelp("system", "methodSignature", "This method takes one parameter, the name of a method implemented by the server.\n\n"
			"It returns the signature for the method. A signature is an array of types. The first of these types is the return type of the method, the rest are parameters.\n\n"
			"A signature is restricted to the top level parameters expected by a method. For instance if a method expects one array of structs as a parameter, and it "
			"returns a string, its signature is simply \"string, array\". If it expects three integers, its signature is \"string, int, int, int\".\n\nIf no signature"
			" is defined for the method, a none-array value is returned.", "array", "string", NULL);

	ovcp_add_method("system", "methodHelp", system_method_help);
	ovcp_method_sethelp("system", "methodHelp", "This method takes one parameter, the name of a method implemented by the server.\n\nIt returns a documentation string describing the"
					"use of that method. If no such string is available, an empty string is returned.\n\nThe documentation string may contain HTML markup.",
					"string", "string", NULL);

	ovcp_add_method("system", "getAPIVersion", system_get_api_version);
	ovcp_method_sethelp("system", "getAPIVersion", "This method returns the version of the XML-RPC API used by the server.\n\nIt might be a good idea to check that the"
							" version is what the client expects.", "string", NULL);



	/* Python Extensions */

	ovcp_extensions_load();


	return 0;
}



struct ovcp_response_st *system_list_methods(struct ovcp_request_st *request)
{
	char buf[BUF_SIZE];
	struct ovcp_response_st *response;
	int i, j;

	response = ovcp_response_new();
       	ovcp_response_array_new(response);

	i = 0;

	while(modules[i].name != NULL)
	{

		j=0;
		while(modules[i].methods[j].name != NULL)
		{
			sconcat(buf, BUF_SIZE,modules[i].name, ".", modules[i].methods[j].name, NULL);
			ovcp_response_array_add_string(response, buf);

			j++;
		}
		i++;
	}

	ovcp_response_array_end(response);

	return response;
}

struct ovcp_response_st *system_method_signature(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;
	char *method_string, *module, *method;
	int i, j, k;

	arg0 = ovcp_request_get_arg(request, 0);
	method_string = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || method_string == NULL)
	  return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	response = NULL;

	module = strtok(method_string, ".");
	method = strtok(NULL, ".");

	i = 0;

	while(modules[i].name != NULL)
	{
	  if(strcmp(modules[i].name, module) == 0)
	  {
	    j = 0;

	    while(modules[i].methods[j].name != NULL)
	    {
	      if(strcmp(modules[i].methods[j].name, method) == 0)
	      {
		if(modules[i].methods[j].signature[0] != NULL)
		{
		  response = ovcp_response_new();
		  ovcp_response_array_new(response);

		  k = 0;
		  while(modules[i].methods[j].signature[k] != NULL)
		  {
		    ovcp_response_array_add_string(response, modules[i].methods[j].signature[k]);
		    k++;
		  }

		  ovcp_response_array_end(response);
		}
		break;
	      }
	      j++;
	    }
	    break;
	  }
	  i++;
	}

	if(response == NULL)
	{
	  response = ovcp_response_new();
	  ovcp_response_add_string(response, "");
	}

	return response;
}

struct ovcp_response_st *system_method_help(struct ovcp_request_st *request)
{
	struct ovcp_data_st *arg0;
	struct ovcp_response_st *response;
	char *method_string, *module, *method;
	int i, j;

	arg0 = ovcp_request_get_arg(request, 0);
	method_string = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || method_string == NULL)
	  return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	response = NULL;

	module = strtok(method_string, ".");
	method = strtok(NULL, ".");

	i = 0;

	while(modules[i].name != NULL)
	{
	  if(strcmp(modules[i].name, module) == 0)
	  {
	    j = 0;

	    while(modules[i].methods[j].name != NULL)
	    {
	      if(strcmp(modules[i].methods[j].name, method) == 0)
	      {
		response = ovcp_response_new();
		ovcp_response_add_string(response, modules[i].methods[j].help);
		break;
	      }
	      j++;
	    }
	    break;
	  }
	  i++;
	}

	if(response == NULL)
	{
	  response = ovcp_response_new();
	  ovcp_response_add_string(response, "");
	}

	return response;
}

struct ovcp_response_st *system_get_api_version(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	response = ovcp_response_new();
	ovcp_response_add_string(response, OVCP_XML_RPC_API_VERSION);

	return response;
}

struct ovcp_response_st *vserver_rescue(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char srcpath[BUF_SIZE];
	char destpath[BUF_SIZE];
	char *name; int ret;
	char *runfile;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	runfile = concat(global_settings.run_dir, "/", name, NULL);
	if(file_exist(runfile))
		return ovcp_response_error(OVCP_ERROR_NOTSTOPPED, OVCP_ERROR_NOTSTOPPED_DESC);
	free(runfile);


	if(!vserver_lock(name))
		return ovcp_response_error(OVCP_ERROR_VSLOCKED, OVCP_ERROR_VSLOCKED_DESC);




	snprintf(srcpath, BUF_SIZE, "%s/%s/rescue", global_settings.config_dir, name);


	if(!file_exist(srcpath))
	{

		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.image_dir, "rescue");
		ret = readlink(srcpath, destpath, BUF_SIZE);

		if(ret <= 0) return ovcp_response_error(OVCP_ERROR_NORESCUEIMG, OVCP_ERROR_NORESCUEIMG_DESC);


		//backup initstyle
		sconcat(srcpath, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style", NULL);
		if(file_exist(srcpath))
		{
			sconcat(destpath, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style.backup", NULL);
			mv(srcpath, destpath);
		}

		// backup vserver
		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.root_dir, name);
		snprintf(destpath, BUF_SIZE, "%s/%s", global_settings.backup_dir, name);

		if(execb_cmd("mv", "mv", srcpath, destpath, NULL) != 0)
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		if(!dir_exist(destpath))
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		// readout rescue image
		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.image_dir, "rescue");
		ret = readlink(srcpath, destpath, BUF_SIZE);

		destpath[ret] = 0;
		trim_slashes(destpath);




		// install rescue image
		char *imagename;

		if(strncmp(destpath, global_settings.image_dir, strlen(global_settings.image_dir)) == 0)
			imagename = destpath+strlen(global_settings.image_dir)+1;
		else imagename = destpath;

		ret = setup_image(name, imagename, "0");

		if(!ret) return ovcp_response_error(OVCP_ERROR_UABLCPYIMG, OVCP_ERROR_UABLCPYIMG_DESC);



		sconcat(srcpath, BUF_SIZE, global_settings.image_dir, "/", imagename, "-pre.sh", NULL);

		if(file_exist(srcpath))
			execb_cmd("sh", 900, "sh", srcpath, name, NULL);



		// move vserver backup to subfolder
		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.backup_dir, name);
		snprintf(destpath, BUF_SIZE, "%s/%s/vserver", global_settings.root_dir, name);

		if(execb_cmd("mv", "mv", srcpath, destpath, NULL) != 0)
			return ovcp_response_error(OVCP_ERROR_UABLRESTORE, OVCP_ERROR_UABLRESTORE_DESC);



		snprintf(srcpath, BUF_SIZE, "%s/%s/rescue", global_settings.config_dir, name);
		file_touch(srcpath);

		response = ovcp_response_new();
		ovcp_response_add_string(response, "On");
	}
	else
	{
		snprintf(srcpath, BUF_SIZE, "%s/%s/vserver", global_settings.root_dir, name);
		snprintf(destpath, BUF_SIZE, "%s/%s", global_settings.backup_dir, name);

		if(execb_cmd("mv", "mv", srcpath, destpath, NULL) != 0)
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		if(!dir_exist(destpath))
			return ovcp_response_error(OVCP_ERROR_BACKUPFAIL, OVCP_ERROR_BACKUPFAIL_DESC);

		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.root_dir, name);
		if(execb_cmd("rm", "rm", "-rf", srcpath, NULL) != 0)
			return ovcp_response_error(OVCP_ERROR_UABLDELETE, OVCP_ERROR_UABLDELETE_DESC);


		snprintf(srcpath, BUF_SIZE, "%s/%s", global_settings.backup_dir, name);
		snprintf(destpath, BUF_SIZE, "%s/%s", global_settings.root_dir, name);

		if(execb_cmd("mv", "mv", srcpath, destpath, NULL) != 0)
			return ovcp_response_error(OVCP_ERROR_UABLRESTORE, OVCP_ERROR_UABLRESTORE_DESC);


		snprintf(srcpath, BUF_SIZE, "%s/%s/rescue", global_settings.config_dir, name);
		unlink(srcpath);

		//restore initstyle
		sconcat(srcpath, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style.backup", NULL);
		if(file_exist(srcpath))
		{
			sconcat(destpath, BUF_SIZE, global_settings.config_dir, "/", name, "/apps/init/style", NULL);
			if(file_exist(destpath)) unlink(destpath);
			mv(srcpath, destpath);
		}

		response = ovcp_response_new();
		ovcp_response_add_string(response, "Off");
	}


	vserver_unlock(name);

	return response;
}



struct ovcp_response_st *vserver_control_start(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *output, *name;
	char path[BUF_SIZE];

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	if(!vserver_lock(name))
		return ovcp_response_error(OVCP_ERROR_VSLOCKED, OVCP_ERROR_VSLOCKED_DESC);


	response = ovcp_response_new();

	output = exec_cmd("vserver", 900, "vserver", name, "start", NULL);

	trim_string(output);
	ovcp_response_add_string(response, output);

	vserver_unlock(name);

	free(output);


	sconcat(path, BUF_SIZE, global_settings.root_dir, "/", name, "/image-post.sh", NULL);


        if(file_exist(path))
	{
		output = exec_cmd("vserver", 900, "vserver", name, "exec", "/image-post.sh", NULL);

		if(output != NULL)
		{
			trim_string(output);
			ovcp_response_add_string(response, output);
			free(output);
		}

		remove(path);
	}



	return response;
}

struct ovcp_response_st *vserver_control_stop(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *output, *name;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	if(!vserver_lock(name))
		return ovcp_response_error(OVCP_ERROR_VSLOCKED, OVCP_ERROR_VSLOCKED_DESC);


	response = ovcp_response_new();

	output = exec_cmd("vserver", 900, "vserver", name, "stop", NULL);

	trim_string(output);
	ovcp_response_add_string(response, output);

	vserver_unlock(name);

	free(output);

	return response;
}





struct ovcp_response_st *vserver_list(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	DIR *dir_ptr;
	struct dirent *dir_entry;
	char path[BUF_SIZE];

	response = ovcp_response_new();


	ovcp_response_array_new(response);

	if((dir_ptr = opendir(global_settings.config_dir)) != NULL)
	{

		while((dir_entry = readdir(dir_ptr)) != NULL)
		{


			if(dir_entry->d_name[0] != '.')
			{
				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", dir_entry->d_name, NULL);

				if(dir_exist(path))
					ovcp_response_array_add_string(response, dir_entry->d_name);
			}

		}


		closedir(dir_ptr);
	}

	ovcp_response_array_end(response);

	return response;
}

struct ovcp_response_st *vserver_resources_hour(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2, *arg3;
	int i, ret, *month, *year, *day, nrow, ncolumn;
	char *name;

	char *sql_error;
	char sql_query[BUF_SIZE];
	char **sql_result;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	month = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	year = (int *)ovcp_data_get(arg2);

	if(!ovcp_data_is_int(arg2) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg3 = ovcp_request_get_arg(request, 3);
	day = (int *)ovcp_data_get(arg3);

	if(!ovcp_data_is_int(arg3) || day == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);




	snprintf(sql_query, BUF_SIZE,
		"SELECT * FROM vserver_hour_resources WHERE (name='%s' and month=%d and day=%d and year=%d)", name, *month, *day, *year);

	ovcp_log(OVCP_DEBUG, "SQL Traffic Query: [%s]", sql_query);



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


		ovcp_response_struct_add_string(response, "month", sql_result[i*(ncolumn)+0]);
		ovcp_response_struct_add_string(response, "hour", sql_result[i*(ncolumn)+1]);
		ovcp_response_struct_add_string(response, "day", sql_result[i*(ncolumn)+2]);
		ovcp_response_struct_add_string(response, "year", sql_result[i*(ncolumn)+3]);
		ovcp_response_struct_add_string(response, "proc", sql_result[i*(ncolumn)+5]);
		ovcp_response_struct_add_string(response, "vm", sql_result[i*(ncolumn)+6]);
		ovcp_response_struct_add_string(response, "vml", sql_result[i*(ncolumn)+7]);
		ovcp_response_struct_add_string(response, "rss", sql_result[i*(ncolumn)+8]);
		ovcp_response_struct_add_string(response, "anon", sql_result[i*(ncolumn)+9]);
		ovcp_response_struct_add_string(response, "files", sql_result[i*(ncolumn)+10]);
		ovcp_response_struct_add_string(response, "ofd", sql_result[i*(ncolumn)+11]);
		ovcp_response_struct_add_string(response, "locks", sql_result[i*(ncolumn)+12]);
		ovcp_response_struct_add_string(response, "sock", sql_result[i*(ncolumn)+13]);
		ovcp_response_struct_add_string(response, "msgq", sql_result[i*(ncolumn)+14]);
		ovcp_response_struct_add_string(response, "shm", sql_result[i*(ncolumn)+15]);


		ovcp_response_struct_end(response);

		i++;
	}


	sqlite3_free_table(sql_result);




	return response;

}


struct ovcp_response_st *vserver_traffic_hour(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2, *arg3;

	char *sql_error;
	char sql_query[BUF_SIZE+1];

	char *name;
	int ret;
	int i = 0;

	int nrow, ncolumn;
	char **sql_result;
	int *month, *year, *day;

	char *ddn_ip;
	ovcp_ip ipdec;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	month = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	year = (int *)ovcp_data_get(arg2);

	if(!ovcp_data_is_int(arg2) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg3 = ovcp_request_get_arg(request, 3);
	day = (int *)ovcp_data_get(arg3);

	if(!ovcp_data_is_int(arg3) || day == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);




	snprintf(sql_query, BUF_SIZE,
		"SELECT * FROM vserver_hour_traffic WHERE (month=%d and day=%d and year=%d and name='%s')", *month, *day, *year, name);

	ovcp_log(OVCP_DEBUG, "SQL Traffic Query: [%s]", sql_query);



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


		#ifdef IPV6_SUPPORT
		if(strstr(sql_result[i*(ncolumn)+4], ":") != NULL)
		{
			ddn_ip = strdup(sql_result[i*(ncolumn)+4]);
		}
		else
		{
			sscanf(sql_result[i*(ncolumn)+4], "%u", (unsigned int *)&ipdec);
			ddn_ip = ip2str(*((uint32_t *)&ipdec));
		}
		#else
		sscanf(sql_result[i*(ncolumn)+4], "%u", &ipdec);
		ddn_ip = ip2str(ipdec);
		#endif

		ovcp_response_struct_add_string(response, "month", sql_result[i*(ncolumn)+0]);
		ovcp_response_struct_add_string(response, "hour", sql_result[i*(ncolumn)+1]);
		ovcp_response_struct_add_string(response, "day", sql_result[i*(ncolumn)+2]);
		ovcp_response_struct_add_string(response, "year", sql_result[i*(ncolumn)+3]);
		ovcp_response_struct_add_string(response, "ip", ddn_ip);
		ovcp_response_struct_add_string(response, "in", sql_result[i*(ncolumn)+5]);
		ovcp_response_struct_add_string(response, "out", sql_result[i*(ncolumn)+6]);
		ovcp_response_struct_add_string(response, "total", sql_result[i*(ncolumn)+7]);

		free(ddn_ip);

		ovcp_response_struct_end(response);

		i++;
	}




	sqlite3_free_table(sql_result);




	return response;
}



struct ovcp_response_st *vserver_traffic(struct ovcp_request_st *request)
{

	char *ddn_ip;
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;

	char *sql_error;
	char sql_query[BUF_SIZE+1];

	char *name;
	int ret;
	int i = 0;

	int nrow, ncolumn;
	char **sql_result;
	int *month, *year;

	ovcp_ip ipdec;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	month = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	year = (int *)ovcp_data_get(arg2);

	if(!ovcp_data_is_int(arg2) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);





	snprintf(sql_query, BUF_SIZE,
		"SELECT * FROM vserver_traffic WHERE (month=%d and year=%d and name='%s')", *month, *year, name);

	ovcp_log(OVCP_DEBUG, "SQL Traffic Query: [%s]", sql_query);



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


		#ifdef IPV6_SUPPORT
		if(strstr(sql_result[i*(ncolumn)+3], ":") != NULL)
		{
			ddn_ip = strdup(sql_result[i*(ncolumn)+3]);
		}
		else
		{
			sscanf(sql_result[i*(ncolumn)+3], "%u", (unsigned int *)&ipdec);
			ddn_ip = ip2str(*((uint32_t *)&ipdec));
		}
		#else
		sscanf(sql_result[i*(ncolumn)+3], "%u", &ipdec);
		ddn_ip = ip2str(ipdec);
		#endif

		ovcp_response_struct_add_string(response, "month", sql_result[i*(ncolumn)+0]);
		ovcp_response_struct_add_string(response, "day", sql_result[i*(ncolumn)+1]);
		ovcp_response_struct_add_string(response, "year", sql_result[i*(ncolumn)+2]);
		ovcp_response_struct_add_string(response, "ip", ddn_ip);
		ovcp_response_struct_add_string(response, "in", sql_result[i*(ncolumn)+4]);
		ovcp_response_struct_add_string(response, "out", sql_result[i*(ncolumn)+5]);
		ovcp_response_struct_add_string(response, "total", sql_result[i*(ncolumn)+6]);


		free(ddn_ip);

		ovcp_response_struct_end(response);

		i++;
	}




	sqlite3_free_table(sql_result);



	return response;
}




struct ovcp_response_st *vserver_traffic_total(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;

	char *sql_error;
	char sql_query[BUF_SIZE+1];

	char *name;
	int ret;
	int i = 0;

	int nrow, ncolumn;
	char **sql_result;

	double day_traffic_in, day_traffic_out, day_traffic_total;
	double month_traffic_in = 0, month_traffic_out = 0, month_traffic_total = 0;
	int *month, *year;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	month = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	year = (int *)ovcp_data_get(arg2);

	if(!ovcp_data_is_int(arg2) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);



	snprintf(sql_query, BUF_SIZE,
		"SELECT \"in\",\"out\",\"total\" FROM vserver_traffic WHERE (month=%d and year=%d and name='%s')", *month, *year, name);

	ovcp_log(OVCP_DEBUG, "SQL Traffic Query: [%s]", sql_query);



	ret = sqlite3_get_table(ovcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}

	i = 1;

	while(i != nrow+1)
	{


		day_traffic_in = strtod(sql_result[i*(ncolumn)], NULL);
		day_traffic_out = strtod(sql_result[i*(ncolumn)+1], NULL);
		day_traffic_total = strtod(sql_result[i*(ncolumn)+2], NULL);


		month_traffic_in += day_traffic_in;
		month_traffic_out += day_traffic_out;
		month_traffic_total += day_traffic_total;

		i++;
	}

	sqlite3_free_table(sql_result);



	response = ovcp_response_new();

	ovcp_response_struct_new(response);

	ovcp_response_struct_add_double(response, "in", month_traffic_in);
	ovcp_response_struct_add_double(response, "out", month_traffic_out);
	ovcp_response_struct_add_double(response, "total", month_traffic_total);

	ovcp_response_struct_end(response);


	return response;
}

struct ovcp_response_st *vserver_setup_config(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *name, *ctx;


	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	ctx = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || ctx == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	if(setup_config(name, ctx) != 1)
		return ovcp_response_error(OVCP_ERROR_UABLCRTCONF, OVCP_ERROR_UABLCRTCONF_DESC);

	response = ovcp_response_new();

	ovcp_response_add_boolean(response, TRUE);

	return response;
}

struct ovcp_response_st *vserver_setup_image(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;
	char *name, *image, *ip, *output;
	char path1[BUF_SIZE], path2[BUF_SIZE];

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	if(!vserver_lock(name))
		return ovcp_response_error(OVCP_ERROR_VSLOCKED, OVCP_ERROR_VSLOCKED_DESC);


	arg1 = ovcp_request_get_arg(request, 1);
	image = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || image == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

        arg2 = ovcp_request_get_arg(request, 2);
        ip = (char *)ovcp_data_get(arg2);

        if(!ovcp_data_is_string(arg2) || ip == NULL)
                return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	if(setup_image(name, image, ip) != 1)
		return  ovcp_response_error(OVCP_ERROR_UABLCPYIMG, OVCP_ERROR_UABLCPYIMG_DESC);


	sconcat(path1, BUF_SIZE, global_settings.image_dir, "/", image, "-post.sh", NULL);

	if(file_exist(path1))
	{
		sconcat(path2, BUF_SIZE, global_settings.root_dir, "/", name, "/image-post.sh", NULL);
		copy(path1, path2);
	}



	response = ovcp_response_new();

	sconcat(path1, BUF_SIZE, global_settings.image_dir, "/", image, "-pre.sh", NULL);


        if(file_exist(path1))
	{
		output = exec_cmd("sh", 900, "sh", path1, name, NULL);

		if(output != NULL)
		{
			ovcp_response_add_string(response, output);
			free(output);
		}
		else
		{
			ovcp_response_add_boolean(response, TRUE);
		}
	}
	else
	{
		ovcp_response_add_boolean(response, TRUE);
	}




	vserver_unlock(name);



	return response;
}

struct ovcp_response_st *vserver_remove(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *name,  *path, *output;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);


	path = concat(global_settings.config_dir, "/", name, NULL);

	if(dir_exist(path))
	{
		output = exec_cmd("rm", 10, "rm", "-rf", path, NULL);
		free(output);
	}

	free(path);
	path = concat(global_settings.root_dir, "/", name, NULL);

	if(dir_exist(path))
	{
		output = exec_cmd("rm", 180, "rm", "-rf", path, NULL);
		free(output);
	}

	free(path);

	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);


	return response;
}

struct ovcp_response_st *vserver_move(struct ovcp_request_st *request)
{
  struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;
	char *name, *ip;
	int step;

	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);

	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

  arg1 = ovcp_request_get_arg(request, 1);
	ip = (char *)ovcp_data_get(arg1);

	if(!ovcp_data_is_string(arg1) || ip == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	if(!ovcp_data_is_int(arg2))
		return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
	step = *((int *)ovcp_data_get(arg2));

	if(step == 1 && vserver_exist(name))
	  return ovcp_response_error(OVCP_ERROR_VSEXIST, OVCP_ERROR_VSEXIST_DESC);
	else if (2 == step && !vserver_exist(name))
	  return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	else if (step != 1 && step != 2)
	  return ovcp_response_error(OVCP_ERROR_WRONGARG, OVCP_ERROR_WRONGARG_DESC);

	if(move_vserver(name, ip) != 1)
		return ovcp_response_error(OVCP_ERROR_UABLCPYIMG, OVCP_ERROR_UABLCPYIMG_DESC);

  if(step == 1 && !vserver_exist(name))
	  return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);

	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);

	return response;
}

