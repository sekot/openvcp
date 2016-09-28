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
 
 
#include <time.h>
#include <string.h>
#include <strings.h>

#include "main.h"
#include "misc.h"
#include "config.h"
#include "request.h"
#include "error.h"
#include "trafficlimit.h"


struct ovcp_response_st *set_trafficlimit(struct ovcp_request_st *request)
{
	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1, *arg2, *arg3;
	char *name, *notify = NULL, *type;
	char sql_query[BUF_SIZE];
	char *sql_error; 
	int ret, total;
		
	arg0 = ovcp_request_get_arg(request, 0);
	name = (char *)ovcp_data_get(arg0);
		
	if(!ovcp_data_is_string(arg0) || name == NULL)
		return ovcp_response_error(OVCP_ERROR_ARGMISS, OVCP_ERROR_ARGMISS_DESC);
	if(!vserver_exist(name))
		return ovcp_response_error(OVCP_ERROR_VSNOTEXIST, OVCP_ERROR_VSNOTEXIST_DESC);
	
	
	arg1 = ovcp_request_get_arg(request, 1);
	if(!ovcp_data_is_int(arg1))
		return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
	total = *((int  *)ovcp_data_get(arg1));
			
	arg2 = ovcp_request_get_arg(request, 2);
	if(ovcp_data_is_string(arg2))
		notify = (char  *)ovcp_data_get(arg2);
	
	if(notify == NULL) notify = "";
	
	arg3 = ovcp_request_get_arg(request, 3);
	type = (char  *)ovcp_data_get(arg3);
	if(!ovcp_data_is_string(arg3) || type == NULL)
		return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
	
	if(strcmp(type, "hard") != 0)
		type = "soft";
	
	snprintf(sql_query, BUF_SIZE, "INSERT OR REPLACE INTO vserver_trafficlimit VALUES(NULL, '%s', '%d', '%s', '%s', '0')", name, total, notify, type);
	
	
	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);
	
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



struct ovcp_response_st *get_trafficlimit(struct ovcp_request_st *request)
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
	

	snprintf(sql_query, BUF_SIZE, "SELECT * FROM vserver_trafficlimit where name='%s'", name);


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
		ovcp_response_struct_add_string(response, "total", sql_result[i*ncolumn+2]);
		ovcp_response_struct_add_string(response, "notify", sql_result[i*ncolumn+3]);
		ovcp_response_struct_add_string(response, "type", sql_result[i*ncolumn+4]);
		ovcp_response_struct_end(response);
		
		i++;
	}

	
	sqlite3_free_table(sql_result);

	return response;

}


struct ovcp_response_st *del_trafficlimit(struct ovcp_request_st *request)
{

	struct ovcp_response_st *response;
	struct ovcp_data_st *arg0, *arg1;
	char *name, *type;
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
	type = (char  *)ovcp_data_get(arg1);
	if(!ovcp_data_is_string(arg1) || type == NULL)
		return ovcp_response_error(OVCP_ERROR_WRONGTYPE, OVCP_ERROR_WRONGTYPE_DESC);
	
	if(strcmp(type, "hard") != 0)
		type = "soft";
	
	snprintf(sql_query, BUF_SIZE, "DELETE FROM vserver_trafficlimit where name='%s' and type='%s'", name, type);

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

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


int ovcp_check_trafficlimits(sqlite3 *openvcp_db)
{
	char sql_query[BUF_SIZE];
	char *sql_error, **sql_result, *notified; 
	int i, total, curtotal, ret, nrow, ncolumn;
	char msg[BUF_SIZE];
	msg[0] = 0;
	
	char *name, *notify, *type;
	time_t current_time;
	struct tm *timeinfo;
	
	time ( &current_time );
	timeinfo = localtime ( &current_time );

	//reset notifications on each first month
	if(timeinfo->tm_mday == 1 && timeinfo->tm_hour == 1) {
		snprintf(sql_query, BUF_SIZE, "UPDATE vserver_trafficlimit SET notified = '0'");
		
		ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);
		
		ret = sqlite3_exec(openvcp_db, sql_query, NULL, 0, &sql_error);
		
		if(ret != SQLITE_OK)
		{
			ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
			sqlite3_free(sql_error);
			exit(-1);
		}
	}
	
	ret = sqlite3_get_table(openvcp_db, "SELECT * FROM vserver_trafficlimit", &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}
	
	
	i=1;
	
	while(i <= nrow)
	{
		name = sql_result[i*ncolumn+1];
		total = atoi(sql_result[i*ncolumn+2]);
		notify = sql_result[i*ncolumn+3];
		type = sql_result[i*ncolumn+4];
		notified = sql_result[i*ncolumn+5];
		
		curtotal = vserver_get_monthtraffic(openvcp_db, name, timeinfo->tm_mon+1, timeinfo->tm_year+1900);
		
		ovcp_log(OVCP_DEBUG, "COMPARING TRAFFIC: %d and %d", total, curtotal/1048576);
	
		if(total <= curtotal/1048576)
		{
			
			if(vserver_isrunning(name))
			{
				if(strcmp(type, "hard") == 0) execb_cmd("vserver", "vserver", name, "stop", NULL);
			
				if(strlen(notify) > 0)
				{
					if(strcmp(type, "hard") == 0)
					{
						snprintf(msg, BUF_SIZE, "To: %s\r\nFrom: %s\r\n"
								"Subject: vServer reached hard traffic limit\r\n\r\ntraffic limit: %d MB traffic used: %d MB\n\n"
								"vServer %s was shutdown.", notify, global_settings.adminemail, total, curtotal/1048576, name);
					
						send_email(msg);
					}
					else if (strcmp(type, "soft") == 0 && strcmp(notified, "1") != 0)
					{
						snprintf(msg, BUF_SIZE, "To: %s\r\nFrom: %s\r\n"
								"Subject: vServer reached soft traffic limit\r\n\r\ntraffic limit: %d MB traffic used: %d MB.",
								notify, global_settings.adminemail, total, curtotal/1048576);
						
						snprintf(sql_query, BUF_SIZE, "UPDATE vserver_trafficlimit SET notified = '1' WHERE name = '%s' AND type = '%s'", name, type);
						
						
						ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);
						
						ret = sqlite3_exec(openvcp_db, sql_query, NULL, 0, &sql_error);
						
						if(ret != SQLITE_OK)
						{
							ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
							sqlite3_free(sql_error);
							exit(-1);
						}
						send_email(msg);
					}

						
				}
			}

		}
		
		
		i++;
	}
	
	sqlite3_free_table(sql_result);
	
	return 0;
}


long long vserver_get_monthtraffic(sqlite3 *openvcp_db, char *name, int month, int year)
{
	char sql_query[BUF_SIZE];
	char *sql_error; 
	int i, ret, nrow, ncolumn;
	char **sql_result;
	long long total = 0;

	
	snprintf(sql_query, BUF_SIZE, 
		"SELECT * FROM vserver_traffic WHERE (month=%d and year=%d and name='%s')", month, year, name);

	ovcp_log(OVCP_DEBUG, "SQL Traffic Query: [%s]", sql_query);


	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		exit(-1);
	}


	i = 1;

	while(i != nrow+1)
	{
		total += strtoll(sql_result[i*(ncolumn)+6], NULL, 0);
		i++;
	}

	sqlite3_free_table(sql_result);
	
	return total;
}
