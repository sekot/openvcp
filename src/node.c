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

#include "node.h"

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/statvfs.h>

#include "strings.h"
#include "fs.h"
#include "config.h"
#include "ovcpxml.h"
#include "error.h"
#include "parseconfig.h"
#include "misc.h"

struct ovcp_response_st *node_version(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	char *kernel_version;
	char util_version[10];
	char vserver_version[10];
	char *utiloutput;
	int i;

	memset(util_version, 0, 10);

	response = ovcp_response_new();

	kernel_version = file_read("/proc/version");

	for(i=0; i<strlen(kernel_version); i++)
	     if(kernel_version[i] == '(')  kernel_version[i-1] = 0;

	snprintf(vserver_version, 10, "0x%x", kernel_vci_version);

	utiloutput = exec_cmd("vserver-info", 10, "vserver-info", NULL);
	if(utiloutput != NULL)
		sscanf(utiloutput, "%*s %*s %*s %*s %*s util-vserver: %10s", util_version);
	if(strlen(util_version) != 0)
		util_version[strlen(util_version)-1] = 0;

	ovcp_response_add_string(response, PACKAGE_VERSION);

	if(strlen(REVISION) > 1)
		ovcp_response_add_string(response, "r"REVISION);
	else ovcp_response_add_string(response, "");

	if(strlen(kernel_version) > 14)
		ovcp_response_add_string(response, kernel_version+14);
	ovcp_response_add_string(response, vserver_version);
	ovcp_response_add_string(response, util_version);

	if(utiloutput != NULL)
		free(utiloutput);
	if(kernel_version != NULL)
		free(kernel_version);

	return response;
}

struct ovcp_response_st *node_halt(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	int ret;

	response = ovcp_response_new();

	if((ret = fork()) == 0)
	{
		execlp ("shutdown", "shutdown", "-h", "+1", NULL);
		exit(0);
	}
	else
	if(ret == -1)
	{
		ovcp_response_add_boolean(response, FALSE);
	}
	else
	{
		ovcp_response_add_boolean(response, TRUE);
	}

	return response;
}

struct ovcp_response_st *node_reboot(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	int ret;

	response = ovcp_response_new();

	if((ret = fork()) == 0)
	{
		execlp ("shutdown", "shutdown", "-r", "+1", NULL);
		exit(0);
	}
	else
	if(ret == -1)
	{
		ovcp_response_add_boolean(response, FALSE);
	}
	else
	{
		ovcp_response_add_boolean(response, TRUE);
	}

	return response;
}


struct ovcp_response_st *node_stat(struct ovcp_request_st *request)
{
	DIR *dir_ptr;
	struct dirent *dir_entry;
	char path[BUF_SIZE];
	struct ovcp_response_st *response;
	int total=0, online=0, offline=0;

	if((dir_ptr = opendir(global_settings.config_dir)) != NULL)
	{

		while((dir_entry = readdir(dir_ptr)) != NULL)
		{


			if(dir_entry->d_name[0] != '.')
			{
				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", dir_entry->d_name, NULL);

				if(dir_exist(path))
				{
					sconcat(path, BUF_SIZE, global_settings.run_dir, "/", dir_entry->d_name, NULL);

					if(file_exist(path))
						online++;
					else offline++;

					total++;
				}
			}

		}


		closedir(dir_ptr);
	}


	response = ovcp_response_new();



	ovcp_response_struct_new(response);

	ovcp_response_struct_add_int(response, "total", total);
	ovcp_response_struct_add_int(response, "online", online);
	ovcp_response_struct_add_int(response, "offline", offline);


	ovcp_response_struct_end(response);


	return response;
}


struct ovcp_response_st *node_get_traffic_hour(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2;

	char *sql_error;
	char sql_query[BUF_SIZE+1];

	int i = 0, ret;

	int nrow, ncolumn;
	char **sql_result;

	int *month, *year, *day;

	arg0 = ovcp_request_get_arg(request, 0);
	month = (int *)ovcp_data_get(arg0);

	if(!ovcp_data_is_int(arg0) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	year = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg2 = ovcp_request_get_arg(request, 2);
	day = (int *)ovcp_data_get(arg2);

	if(!ovcp_data_is_int(arg2) || day == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);


	snprintf(sql_query, BUF_SIZE, "SELECT * FROM node_hour_traffic WHERE (month=%d and day=%d and year=%d)",  *month, *day, *year);



	ret = sqlite3_get_table(ovcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	i = 1;

	response = ovcp_response_new();

	while(i != nrow+1)
	{

		ovcp_response_struct_new(response);

		ovcp_response_struct_add_string(response, "month", sql_result[i*(ncolumn)+0]);
		ovcp_response_struct_add_string(response, "hour", sql_result[i*(ncolumn)+1]);
		ovcp_response_struct_add_string(response, "day", sql_result[i*(ncolumn)+2]);
		ovcp_response_struct_add_string(response, "year", sql_result[i*(ncolumn)+3]);
		ovcp_response_struct_add_string(response, "in", sql_result[i*(ncolumn)+4]);
		ovcp_response_struct_add_string(response, "out", sql_result[i*(ncolumn)+5]);
		ovcp_response_struct_add_string(response, "total", sql_result[i*(ncolumn)+6]);


		ovcp_response_struct_end(response);

		i++;
	}

	sqlite3_free_table(sql_result);



	return response;
}


struct ovcp_response_st *node_get_traffic(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;

	char *sql_error;
	char sql_query[BUF_SIZE+1];

	int i = 0, ret;

	int nrow, ncolumn;
	char **sql_result;

	int *month, *year;

	arg0 = ovcp_request_get_arg(request, 0);
	month = (int *)ovcp_data_get(arg0);

	if(!ovcp_data_is_int(arg0) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	year = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);



	snprintf(sql_query, BUF_SIZE, "SELECT * FROM node_traffic WHERE (month=%d and year=%d)",  *month, *year);



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
		ovcp_response_struct_add_string(response, "day", sql_result[i*(ncolumn)+1]);
		ovcp_response_struct_add_string(response, "year", sql_result[i*(ncolumn)+2]);
		ovcp_response_struct_add_string(response, "in", sql_result[i*(ncolumn)+3]);
		ovcp_response_struct_add_string(response, "out", sql_result[i*(ncolumn)+4]);
		ovcp_response_struct_add_string(response, "total", sql_result[i*(ncolumn)+5]);


		ovcp_response_struct_end(response);

		i++;
	}

	sqlite3_free_table(sql_result);



	return response;
}



struct ovcp_response_st *node_get_traffic_total(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;

	char *sql_error;
	char sql_query[BUF_SIZE];
	int i = 0, ret;

	int nrow, ncolumn;
	char **sql_result;
	int *month, *year;


	double day_traffic_in, day_traffic_out, day_traffic_total;
	double month_traffic_in = 0, month_traffic_out = 0, month_traffic_total = 0;


	arg0 = ovcp_request_get_arg(request, 0);
	month = (int *)ovcp_data_get(arg0);

	if(!ovcp_data_is_int(arg0) || month == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);

	arg1 = ovcp_request_get_arg(request, 1);
	year = (int *)ovcp_data_get(arg1);

	if(!ovcp_data_is_int(arg1) || year == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);




	snprintf(sql_query, BUF_SIZE, "SELECT \"in\",\"out\",\"total\" FROM node_traffic WHERE (month=%d and year=%d)",  *month, *year);



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


struct ovcp_response_st *node_get_interfaces(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	FILE *dev_file;
	char word[500+1];
	int i = 0;

	dev_file = fopen("/proc/net/dev", "r");

	if(dev_file != NULL)
	{
		response = ovcp_response_new();

		ovcp_response_array_new(response);

		while(!feof(dev_file))
		{
			word[0] = 0;
			if(fscanf(dev_file, "%500s", word) != 1) continue;

			if(strlen(word)>0)
			{
				i = 0;

				while(i != strlen(word))
				{
					if(word[i] == ':')
					{


						word[i] = 0;
						ovcp_response_array_add_string(response, word);

						break;
					}

					i++;
				}

			}
		}


		ovcp_response_array_end(response);

		fclose(dev_file);
	}
	else
	{
		response = ovcp_response_error(OVCP_ERROR_FILEMISSING, "node.get_interfaces: "OVCP_ERROR_FILEMISSING_DESC);
	}

	return response;
}

struct ovcp_response_st *node_get_images(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	DIR *image_dir;
 	struct dirent *entry;
	char *path;
	struct image_st image;

	response = ovcp_response_new();


	if((image_dir = opendir(global_settings.image_dir)) != NULL)
	{

		while((entry = readdir(image_dir)) != NULL)
		{

			if(entry->d_name[0] != '.' && strcmp(entry->d_name, "rescue") != 0)
			{

				path = concat(global_settings.image_dir, "/", entry->d_name, NULL);

				if(dir_exist(path))
				{
					ovcp_response_struct_new(response);

					if(parse_image(entry->d_name, &image))
					{
						ovcp_response_struct_add_string(response, "id", entry->d_name);

						if(image.name != NULL)
							ovcp_response_struct_add_string(response, "name", image.name);
						if(image.initstyle != NULL)
							ovcp_response_struct_add_string(response, "initstyle", image.initstyle);

						free_image(&image);
					}
					else
					{
						ovcp_response_struct_add_string(response, "id", entry->d_name);
					}

					ovcp_response_struct_end(response);
				}

				free(path);
			}
		}

		closedir(image_dir);
	}


	return response;
}


struct ovcp_response_st *node_get_load(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	char path[] = "/proc/loadavg";
	char *load;

	load = file_read(path);

	if(load == NULL)
	{
		response = ovcp_response_error(OVCP_ERROR_FILEMISSING, "node.get_load: "OVCP_ERROR_FILEMISSING_DESC);
	}
	else
	{
		trim_string(load);
		response = ovcp_response_new();
		ovcp_response_add_string(response, load);
		free(load);
	}

	return response;
}


struct ovcp_response_st *node_get_uptime(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	char path[] = "/proc/uptime";
	char *uptime;

	uptime = file_read(path);


	if(uptime == NULL)
	{
		response = ovcp_response_error(OVCP_ERROR_FILEMISSING, "node.get_uptime: "OVCP_ERROR_FILEMISSING_DESC);
	}
	else
	{
		trim_string(uptime);
		response = ovcp_response_new();
		ovcp_response_add_string(response, uptime);
		free(uptime);
	}


	return response;
}


struct ovcp_response_st *node_get_space(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct statvfs vfsinfo;

	if(statvfs(global_settings.root_dir, &vfsinfo) == -1)
		return ovcp_response_error(OVCP_ERROR_UNABLSPACE, "node.get_space: "OVCP_ERROR_UNABLSPACE_DESC);


	response = ovcp_response_new();

	ovcp_response_struct_new(response);

	ovcp_response_struct_add_double(response, "space_total", vfsinfo.f_blocks/1024*vfsinfo.f_bsize);
	ovcp_response_struct_add_double(response, "space_free", vfsinfo.f_bfree/1024*vfsinfo.f_bsize);
	ovcp_response_struct_add_double(response, "inodes_total", vfsinfo.f_files);

	ovcp_response_struct_end(response);


	return response;
}

struct ovcp_response_st *node_get_memory(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	char path[] = "/proc/meminfo";
	char *meminfo, **lines, *word;
	unsigned int lcount;
	int i = 0;

	int memtotal = 0, memfree = 0, memcached = 0, membuffered = 0;
	int swaptotal = 0, swapfree = 0;

	meminfo = file_read(path);

	if(meminfo == NULL)
	{
		response = ovcp_response_error(OVCP_ERROR_FILEMISSING, "node.get_mem: "OVCP_ERROR_FILEMISSING_DESC);
	}
	else
	{
		lines = split_string(meminfo, '\n',  &lcount);

		while(i < lcount)
		{
			word = strtok(lines[i], " ");
			trim_string(word);


			if(strcmp(word, "MemTotal:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				memtotal = atoi(word);
			}
			else
			if(strcmp(word, "MemFree:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				memfree = atoi(word);
			}
			else
			if(strcmp(word, "Buffers:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				membuffered = atoi(word);
			}
			else
			if(strcmp(word, "Cached:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				memcached = atoi(word);
			}
			else
			if(strcmp(word, "SwapTotal:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				swaptotal = atoi(word);
			}
			else
			if(strcmp(word, "SwapFree:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				swapfree = atoi(word);
			}



			i++;
		}


		response = ovcp_response_new();

		ovcp_response_struct_new(response);

		ovcp_response_struct_add_int(response, "memtotal", memtotal);
		ovcp_response_struct_add_int(response, "memfree", memfree);
		ovcp_response_struct_add_int(response, "buffers", membuffered);
		ovcp_response_struct_add_int(response, "cached", memcached);
		ovcp_response_struct_add_int(response, "swaptotal", swaptotal);
		ovcp_response_struct_add_int(response, "swapfree", swapfree);

		ovcp_response_struct_end(response);

		free_strings(lines, lcount);
		free(meminfo);
	}



	return response;
}



