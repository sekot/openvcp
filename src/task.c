/*  OpenVCP
 *  Copyright (C) 2010 Gerrit Wyen <gerrit@ionscale.com>
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
#include <unistd.h>
#include <time.h>

#include "main.h"
#include "strings.h"
#include "misc.h"
#include "task.h"


static sqlite3 *openvcp_db;

int ovcp_taskcontrol_init(void)
{
	int ret;
	
	ret = sqlite3_open(OPENVCP_DATABASE, &openvcp_db);
	if(ret != 0)
	{
		ovcp_log(OVCP_ERROR, "Can't open database: (%s)", sqlite3_errmsg(openvcp_db));
		sqlite3_close(openvcp_db);
		exit(-1);
	}      

	ret = sqlite3_exec(openvcp_db,
		"CREATE TABLE tasks ('id', 'state', 'percent', 'msg', 'alterationdate', PRIMARY KEY(id) )", NULL, 0, NULL);

	return ret;
}

int ovcp_task_new(void)
{
	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int  ret;
	
	int id;
	
	ret = sqlite3_get_table(openvcp_db, "SELECT MAX(id)+1 FROM tasks", &sql_result, &nrow, &ncolumn, &sql_error);

	
	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return -1;
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return -1;
	}

	id = strtoll(sql_result[(ncolumn)+0], NULL, 0);

	sqlite3_free_table(sql_result);


	
	snprintf(sql_query, BUF_SIZE, "INSERT INTO tasks('id', 'state', 'alterationdate') VALUES('%d', '%d', '%d')", id, OVCP_TASK_WAITING, (int)time(NULL));
	ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	if(ret != SQLITE_OK)
		return -1;
	
	return id;
}

int ovcp_task_setMsg(int id, char *msg)
{
	char sql_query[BUF_SIZE];
	int ret;
	
	snprintf(sql_query, BUF_SIZE, "UPDATE tasks SET msg='%s' AND alterationdate='%d' WHERE id='%d'", msg, (int)time(NULL), id);
	
	ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	if(ret != SQLITE_OK)
		return -1;
	
	
	return 1;
}

int ovcp_task_setState(int id, int state)
{
	char sql_query[BUF_SIZE];
	int ret;
	
	snprintf(sql_query, BUF_SIZE, "UPDATE tasks SET state='%d' AND alterationdate='%d' WHERE id='%d'", state, (int)time(NULL), id);
	
	ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	if(ret != SQLITE_OK)
		return -1;
	
	return 1;	
}

int ovcp_task_setPercent(int id, float percent)
{
	char sql_query[BUF_SIZE];
	int ret;
	
	snprintf(sql_query, BUF_SIZE, "UPDATE tasks SET percent='%f' AND alterationdate='%d' WHERE id='%d'", percent, (int)time(NULL), id);
	
	ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	if(ret != SQLITE_OK)
		return -1;
		
	return 1;
}

char *ovcp_task_getMsg(int id)
{
	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int  ret;
	char *msg;	
	
	snprintf(sql_query, BUF_SIZE, "SELECT msg FROM tasks WHERE id='%d'", id);
	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	
	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return NULL;
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return NULL;
	}

	msg = strdup(sql_result[(ncolumn)+0]);

	sqlite3_free_table(sql_result);

	return msg;
}

int ovcp_task_getState(int id)
{
	int state;
	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int  ret;

	
	snprintf(sql_query, BUF_SIZE, "SELECT state FROM tasks WHERE id='%d'", id);
	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	
	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return -1;
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return -1;
	}

	state = atoi(sql_result[(ncolumn)+0]);

	sqlite3_free_table(sql_result);

	return state;
}

float ovcp_task_getPercent(int id)
{
	float percent;
	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int  ret;

	
	snprintf(sql_query, BUF_SIZE, "SELECT state FROM tasks WHERE id='%d'", id);
	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	
	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return -1;
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return -1;
	}

	percent = strtoll(sql_result[(ncolumn)+0], NULL, 0);


	return percent;
}

