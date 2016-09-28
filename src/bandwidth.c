/*  OpenVCP
 *  Copyright (C) 2008 Gerrit Wyen <gerrit@t4a.net>
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

#include "main.h"
#include "misc.h"
#include "config.h"
#include "request.h"
#include "error.h"
#include "strings.h"
#include "fs.h"

int ovcp_create_ratethrottle(char *name, int id, int direction, int rate, int ceil, int burst, int cburst);


struct ovcp_response_st *set_bandwidth(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *struct_element;
	char *name, *direction = NULL;
	long int id, rate = -1, ceil = -1, burst = -1, cburst = -1;
	char sql_query[BUF_SIZE];
	char *sql_error; 
	int ret;
		
	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);
		
	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	
	
	arg1 = ovcp_request_get_arg(request, 1);
	
	if(!ovcp_data_is_struct(arg1))
		return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
		
		
	struct_element = ovcp_struct_get_element(arg1, "direction");
	if(ovcp_data_is_string(struct_element))
	{
		direction = ovcp_data_get(struct_element);
		if(strcmp(direction, "IN") != 0 && strcmp(direction, "OUT") != 0)
			return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
	}	
	struct_element = ovcp_struct_get_element(arg1, "rate");
	if(ovcp_data_is_int(struct_element))
		rate = *((int  *)ovcp_data_get(struct_element));
		
	struct_element = ovcp_struct_get_element(arg1, "ceil");
	if(ovcp_data_is_int(struct_element))
		ceil = *((int  *)ovcp_data_get(struct_element));
		
	struct_element = ovcp_struct_get_element(arg1, "burst");
	if(ovcp_data_is_int(struct_element))
		burst = *((int  *)ovcp_data_get(struct_element));
		
	struct_element = ovcp_struct_get_element(arg1, "cburst");
	if(ovcp_data_is_int(struct_element))
		cburst = *((int  *)ovcp_data_get(struct_element));
		
	if(direction == NULL || rate == -1 || ceil == -1 || burst == -1 || cburst == -1)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
		
		

	
	snprintf(sql_query, BUF_SIZE, "INSERT OR REPLACE INTO vserver_bandwidth VALUES(NULL, '%s', '%s', '%ld', '%ld', '%ld', '%ld')", name, direction, rate, ceil, burst, cburst);
	
	
	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);	
	
	ret = sqlite3_exec(ovcp_db, sql_query, NULL, 0, &sql_error);
	
	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}
	
	id = sqlite3_last_insert_rowid(ovcp_db);
	if(id != 0)
		ovcp_create_ratethrottle(name, id, (strcmp(direction, "IN") == 0) ? INGRESS : EGRESS, rate, ceil, burst, cburst);
		
	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);


	return response;
}
	
struct ovcp_response_st *get_bandwidth(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0;
	char *name;
	char sql_query[BUF_SIZE];
	char *sql_error, **sql_result; 
	int i, ret, nrow, ncolumn;

		
	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);
		
	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	

	snprintf(sql_query, BUF_SIZE, "SELECT * FROM vserver_bandwidth where name='%s'", name);


	ret = sqlite3_get_table(ovcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}
	response = ovcp_response_new();
	
	i=1;
	while(i <= nrow)
	{
		ovcp_response_struct_new(response);
		ovcp_response_struct_add_string(response, "direction", sql_result[i*ncolumn+2]);
		ovcp_response_struct_add_string(response, "rate", sql_result[i*ncolumn+3]);
		ovcp_response_struct_add_string(response, "ceil", sql_result[i*ncolumn+4]);
		ovcp_response_struct_add_string(response, "burst", sql_result[i*ncolumn+5]);
		ovcp_response_struct_add_string(response, "cburst", sql_result[i*ncolumn+6]);
		ovcp_response_struct_end(response);
		
		i++;
	}

	
	sqlite3_free_table(sql_result);

	return response;

}

struct ovcp_response_st *del_bandwidth(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *name, *direction;
	char sql_query[BUF_SIZE];
	char *sql_error; 
	int ret;

		
	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);
		
	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	
	arg1 = ovcp_request_get_arg(request, 1);
	direction = (char *)ovcp_data_get(arg1);
		
	if(!ovcp_data_is_string(arg1) || direction == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(strcmp("IN", direction) != 0 && strcmp("OUT", direction) != 0)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
		
	snprintf(sql_query, BUF_SIZE, "DELETE FROM vserver_bandwidth where name='%s' and direction='%s'", name, direction);
	
	ret = sqlite3_exec(ovcp_db, sql_query, NULL, 0, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}

	response = ovcp_response_new();
	ovcp_response_add_boolean(response, TRUE);


	return response;
}


int ovcp_setup_trafficthrottling(sqlite3 *openvcp_db)
{
	char *sql_error, **sql_result; 
	int i, ret, nrow, ncolumn, res=0;
	char **devs; int count;
	
	char *name; int id, direction, rate, ceil, burst, cburst;
	

	ret = sqlite3_get_table(openvcp_db, "SELECT * FROM vserver_bandwidth", &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}
	
	devs = node_interfaces(&count);
	i=0;
	while(i<count)
	{
		execb_cmd("tc", "tc", "qdisc", "del", "dev", devs[i], "root", NULL);
		execb_cmd("tc", "tc", "qdisc", "del", "dev", devs[i], "ingress", NULL);
		i++;
	}
	free_strings(devs, count);
	
	i=1;
	
	while(i <= nrow)
	{
		id = atoi(sql_result[i*ncolumn+0]);
		name = sql_result[i*ncolumn+1];
		
		if(strcmp(sql_result[i*ncolumn+2], "IN") == 0)
			direction = INGRESS;
		else
			direction = EGRESS;
			
		rate = atoi(sql_result[i*ncolumn+3]);
		ceil = atoi(sql_result[i*ncolumn+4]);
		burst = atoi(sql_result[i*ncolumn+5]);
		cburst = atoi(sql_result[i*ncolumn+6]);

		res += ovcp_create_ratethrottle(name, id, direction, rate, ceil, burst, cburst);
		
		
		
		i++;
	}
	
	sqlite3_free_table(sql_result);
	
	return res;
}


int ovcp_create_ratethrottle(char *name, int id, int direction, int rate, int ceil, int burst, int cburst)
{
	struct iface_entry *ips;
	int i;
	char idstr[50], ratestr[50], ceilstr[50];
	char burststr[50], cburststr[50];
	
	ips = parse_ifaces(name);
	if(ips == NULL) return -1;
	
	execb_cmd("ifconfig", "ifconfig", global_settings.ifbdev, "up", NULL);
	execb_cmd("tc", "tc", "qdisc", "add", "dev", global_settings.ifbdev, "root", "handle", "1:0", "htb", NULL);
	
	snprintf(idstr, 50, "1:%i", id);
	snprintf(ratestr, 50, "%ikbps", rate);
	snprintf(ceilstr, 50, "%ikbps", ceil);
	snprintf(burststr, 50, "%i", burst);
	snprintf(cburststr, 50, "%i", cburst);
	
	i = 0;
	
	while(ips[i].ip != NULL)
	{
		if(direction == INGRESS)
		{
			execb_cmd("tc", "tc", "qdisc", "add", "dev", ips[i].dev, "ingress", "handle", "1:0", NULL);
		
			execb_cmd("tc", "tc", "class", "add", "dev", global_settings.ifbdev, "parent", "1:0", "classid", idstr, "htb", "rate", ratestr, "ceil", ceilstr, "burst", burststr, "cburst", cburststr, NULL);


			execb_cmd("tc", "tc", "filter", "add", "dev", global_settings.ifbdev, "parent", "1:", "protocol", "ip", "prio", "1", "u32", "match", "ip", "dst", ips[i].ip, "flowid", idstr, NULL); 

		
			execb_cmd("tc", "tc", "filter", "add", "dev", ips[i].dev, "parent", "ffff:", "protocol", "ip", "prio", "100", "u32", "match", "ip", "dst", ips[i].ip, "flowid", idstr,
							 "action", "mirred", "egress", "redirect", "dev", global_settings.ifbdev, NULL);	
		}
		else
		{
			execb_cmd("tc", "tc", "qdisc", "add", "dev", ips[i].dev, "root", "handle", "1:0", "htb", NULL);
				
			execb_cmd("tc", "tc", "class", "add", "dev", ips[i].dev, "parent", "1:0", "classid", idstr, "htb", "rate", ratestr, "ceil", ceilstr, "burst", burststr, "cburst", cburststr, NULL);
			
			execb_cmd("tc", "tc",  "filter", "add", "dev", ips[i].dev, "parent", "1:0", "protocol", "ip", "u32", "match", "ip", "src", ips[i].ip, "classid", idstr, NULL);
		}
		
		i++;
	}

	free_ifacelist(ips);
	

	return i;
}




int ifb_kernel_support()
{
	char *devsfile = file_read("/proc/net/dev");
	int ret = 0;
	
	if(devsfile != NULL)
	{
		if(strstr(devsfile, "ifb") != NULL)
			ret = 1;
	}
	
	free(devsfile);
	
	return ret;
}
