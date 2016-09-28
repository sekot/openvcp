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


#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include "traffic.h"
#include "logtraffic.h"
#include "main.h"
#include "fs.h"
#include "misc.h"
#include "config.h"
#include "filter.h"
#include "bandwidth.h"
#include "trafficlimit.h"
#include "task.h"


struct traffic_st *parse_day_traffic(int month, int day, double *day_in, double *day_out, double *day_total, int *entry_num);
static int get_vserver_traffic(sqlite3 *openvcp_db, int day, int month, int year, char *ip, char *name, long long int *in, long long int *out, long long int *total);
static int get_node_traffic(sqlite3 *openvcp_db, int day, int month, int year, long long int *in, long long int *out, long long int *total);
static void segv_handle(int sig);




static int get_node_traffic(sqlite3 *openvcp_db, int day, int month, int year, long long int *in, long long int *out, long long int *total)
{


	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int  ret;


	*in = 0;
	*out = 0;
	*total = 0;

	snprintf(sql_query, BUF_SIZE,
		"SELECT \"in\",\"out\",\"total\" FROM node_traffic WHERE (day=%d and month=%d and year=%d)",  day, month, year);

	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		sqlite3_close(openvcp_db);
		exit(-1);
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);

	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return -1;
	}

	*in = strtoll(sql_result[(ncolumn)+0], NULL, 0);
	*out = strtoll(sql_result[(ncolumn)+1], NULL, 0);
	*total = strtoll(sql_result[(ncolumn)+2], NULL, 0);



	ovcp_log(OVCP_DEBUG, "Node Traffic Result: %d, %d, %d, @ %lld, %lld, %lld",
			day, month, year, *in, *out, *total);

	sqlite3_free_table(sql_result);

	return 1;
}

static int get_vserver_traffic(sqlite3 *openvcp_db, int day, int month, int year, char *ip, char *name, long long int *in, long long int *out, long long int *total)
{
	char sql_query[BUF_SIZE];
	char *sql_error;
	int nrow, ncolumn;
	char **sql_result;
	int ret;



	*in = 0;
	*out = 0;
	*total = 0;



	snprintf(sql_query, BUF_SIZE,
		"SELECT \"in\",\"out\",\"total\" FROM vserver_traffic WHERE (day=%d and month=%d and year=%d) and (ip = '%s' and name = '%s')", day, month, year, ip, name);


	ret = sqlite3_get_table(openvcp_db, sql_query, &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		sqlite3_close(openvcp_db);
		exit(-1);
	}

	ovcp_log(OVCP_DEBUG, "SQL QUERY: %s", sql_query);


	if(nrow < 1)
	{
		sqlite3_free_table(sql_result);
		return -1;
	}

	*in = strtoll(sql_result[(ncolumn)+0], NULL, 0);
	*out = strtoll(sql_result[(ncolumn)+1], NULL, 0);
	*total = strtoll(sql_result[(ncolumn)+2], NULL, 0);



	ovcp_log(OVCP_DEBUG, "VServer Traffic Result: %s, %d, %d, %d, @ %lld, %lld, %lld", ip, day, month, year, *in, *out, *total);

	sqlite3_free_table(sql_result);

	return 1;
}

void parse_traffic_tree(node *root_node, sqlite3 *openvcp_db, struct tm *timeinfo,
						long long int *node_in, long long int *node_out, long long int *node_total, struct vsip_entry *iplist)
{
	char sql_query[BUF_SIZE];
	int ret, i = 0, c = 0;
	long long int day_in = 0, day_out = 0, day_total = 0;
	char ipstr[40];

	if(root_node == NULL)
		return;





	#ifdef IPV6_SUPPORT

	if(root_node->ip.in6_u.u6_addr16[0] == 0 && root_node->ip.in6_u.u6_addr16[1] == 0 &&
		root_node->ip.in6_u.u6_addr16[2] == 0 && root_node->ip.in6_u.u6_addr16[3] == 0 &&
		root_node->ip.in6_u.u6_addr16[4] == 0 && root_node->ip.in6_u.u6_addr16[5] == 0xFFFF)
	{
		inet_ntop(AF_INET, &root_node->ip.in6_u.u6_addr16[6], ipstr, 40);
		ovcp_log(OVCP_DEBUG, "IPV4: %s", ipstr);

		while(iplist != NULL && iplist[i].ip != NULL)
		{

			if(strcmp(iplist[i].ip, ipstr) == 0)
				break;
			i++;
		}

		snprintf(ipstr, 40, "%u", str2ip(ipstr));

	}
	else
	{
		inet_ntop(AF_INET6, &root_node->ip, ipstr, 40);

		while(iplist != NULL && iplist[i].ip != NULL)
		{

			if(strcmp(iplist[i].ip, ipstr) == 0)
				break;
			i++;
		}
	}


	#else
	inet_ntop(AF_INET, &root_node->ip, ipstr, 40);

	ovcp_log(OVCP_DEBUG, "IPV4: %s", ipstr);

	while(iplist != NULL && iplist[i].ip != NULL)
	{

		if(strcmp(iplist[i].ip, ipstr) == 0)
			break;
		i++;
	}

	snprintf(ipstr, 40, "%u", str2ip(ipstr));
	#endif



	if(iplist != NULL && iplist[i].name != NULL)
	{

		/* Hour Traffic */

		snprintf(sql_query, BUF_SIZE,
			"INSERT OR REPLACE INTO vserver_hour_traffic VALUES(%d, %d, %d, %d, '%s', %lld, %lld, %lld, '%s')",
			timeinfo->tm_mon+1, timeinfo->tm_hour, timeinfo->tm_mday, timeinfo->tm_year+1900,
				ipstr, root_node->in, root_node->out, root_node->total, iplist[i].name);


		ovcp_log(OVCP_DEBUG, "IP: %s IN: %lld OUT: %lld TOTAL: %lld", ipstr, root_node->in, root_node->out, root_node->total);

		ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	
		if(ret != SQLITE_OK)
		{
			sqlite3_close(openvcp_db);
			exit(-1);
		}

		/* Day Traffic */

		c = 0;

		if(timeinfo->tm_hour != 0)
			get_vserver_traffic(openvcp_db, timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, ipstr, iplist[i].name, &day_in, &day_out, &day_total);

		day_in += root_node->in;
		day_out += root_node->out;
		day_total += root_node->total;


		snprintf(sql_query, BUF_SIZE,
				"INSERT OR REPLACE INTO vserver_traffic VALUES(%d, %d, %d, '%s', %lld, %lld, %.lld, '%s')",
				timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900, ipstr, day_in, day_out, day_total, iplist[i].name);

		ret = ovcp_sqlite_exec(openvcp_db, sql_query);
		

		if(ret != SQLITE_OK)
		{
			sqlite3_close(openvcp_db);
			exit(-1);
		}

	}




	/* Total Traffic */


	*node_in += root_node->in;
	*node_out += root_node->out;
	*node_total += root_node->total;


	if(root_node->right != NULL)
	{
		ovcp_log(OVCP_DEBUG, "Right:");
		parse_traffic_tree(root_node->right, openvcp_db, timeinfo, node_in, node_out, node_total, iplist);
	}
	if(root_node->left != NULL)
	{
		ovcp_log(OVCP_DEBUG, "Left:");
		parse_traffic_tree(root_node->left, openvcp_db, timeinfo, node_in, node_out, node_total, iplist);
	}


}

int parse_resources(sqlite3 *openvcp_db, int month, int hour, int day, int year)
{
	char path[BUF_SIZE];
	char sql_query[BUF_SIZE];

	DIR *procdir;
	struct dirent *vsentry;
	char *limitfile;
	char **lines, *word, *name;
	unsigned int lcount;
	int i, ret;




	int proc, vm, vml, rss, anon, files;
	int ofd, locks, sock, msgq, shm;



	if((procdir = opendir("/proc/virtual/")) == NULL)
		return -1;

	while((vsentry = readdir(procdir)) != NULL)
	{
		proc = 0; vm = 0; vml = 0; rss = 0; anon = 0; files = 0;
		ofd = 0; locks = 0; sock = 0; msgq = 0; shm = 0;

		if(vsentry->d_name[0] == '.' || strcmp(vsentry->d_name, "info") == 0)
			continue;

		sconcat(path, BUF_SIZE, "/proc/virtual/", vsentry->d_name, "/limit", NULL);
		limitfile = file_read(path);

		if(limitfile == NULL)
			continue;


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
				proc = atoi(word);
			}
			else
			if(strcmp(word, "VM:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				vm = atoi(word);
			}
			if(strcmp(word, "VML:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				vml = atoi(word);
			}
			else
			if(strcmp(word, "RSS:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				rss = atoi(word);
			}
			else
			if(strcmp(word, "ANON:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				anon = atoi(word);
			}
			else
			if(strcmp(word, "FILES:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				files = atoi(word);
			}
			else
			if(strcmp(word, "OFD:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				ofd = atoi(word);
			}
			else
			if(strcmp(word, "LOCKS:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				locks = atoi(word);
			}
			else
			if(strcmp(word, "SOCK:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				sock = atoi(word);
			}
			else
			if(strcmp(word, "MSGQ:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				msgq = atoi(word);
			}
			else
			if(strcmp(word, "SHM:") == 0)
			{
				word = strtok(NULL, " ");
				trim_string(word);
				shm = atoi(word);
			}


			i++;
		}

		free_strings(lines, lcount);
		free(limitfile);



		if(proc == 0) continue;


		name = vserver_name(atoi(vsentry->d_name));
		if(name == NULL) continue;


		snprintf(sql_query, BUF_SIZE,
			"INSERT OR REPLACE INTO vserver_hour_resources VALUES(%d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
			month, hour, day, year, name, proc, vm, vml, rss, anon, files, ofd, locks, sock, msgq, shm);

		free(name);

		ret = ovcp_sqlite_exec(openvcp_db, sql_query);

		if(ret != SQLITE_OK)
		{
			sqlite3_close(openvcp_db);
			exit(-1);
		}

	}

	closedir(procdir);


	return 0;
}



int traffic_daemon_processe(void)
{
	sqlite3 *openvcp_db;
	char sql_query[BUF_SIZE];
	struct vsip_entry *iplist;

	time_t rawtime, current_time;
	struct tm * timeinfo;


	int i=1, ret;

	int refresh_delay;




	char **ifaces;
	unsigned int iface_count;
	struct _packetlogger **loggerdata;

	long long int node_in, node_out, node_total;
	long long int day_total, day_in, day_out;


	signal(SIGSEGV, segv_handle);

	ret = sqlite3_open(OPENVCP_DATABASE, &openvcp_db);

	if(ret != 0)
	{
		ovcp_log(OVCP_ERROR, "Can't open database: (%s)", sqlite3_errmsg(openvcp_db));
		sqlite3_close(openvcp_db);
		exit(-1);
	}

	sqlite3_exec(openvcp_db,
		"CREATE TABLE vserver_hour_resources ('month', 'hour', 'day', 'year', 'name', 'proc', 'vm', 'vml', 'rss', 'anon',"
					"'files', 'ofd', 'locks', 'sock', 'msgq', 'shm', UNIQUE(month, day, year, hour, name))", NULL, 0, NULL);

	sqlite3_exec(openvcp_db,
		"CREATE TABLE vserver_traffic ('month', 'day', 'year', 'ip', 'in', 'out', 'total', 'name', UNIQUE(month, day, year, ip))"
																, NULL, 0, NULL);
	sqlite3_exec(openvcp_db, "ALTER TABLE vserver_traffic ADD COLUMN 'name'", NULL, 0, NULL);

	sqlite3_exec(openvcp_db,
		"CREATE TABLE node_traffic ('month', 'day', 'year', 'in', 'out', 'total', UNIQUE(month, day, year))"
													, NULL, 0, NULL);
	sqlite3_exec(openvcp_db,
		"CREATE TABLE vserver_hour_traffic ('month', 'hour', 'day', 'year', 'ip', 'in', 'out', 'total', 'name', UNIQUE(month, hour, day, year, ip))"
																		, NULL, 0, NULL);
	sqlite3_exec(openvcp_db, "ALTER TABLE vserver_hour_traffic ADD COLUMN 'name'", NULL, 0, NULL);

	sqlite3_exec(openvcp_db,
		"CREATE TABLE node_hour_traffic ('month', 'hour', 'day', 'year', 'in', 'out', 'total', UNIQUE(month, hour, day, year))"
								, NULL, 0, NULL);
	sqlite3_exec(openvcp_db,
		"CREATE TABLE iptable_rules ('direction', 'proto', 'srcip', 'srcport', 'destip', 'destport', 'target', 'match',	 'match-value',"
		"'name', 'id' INTEGER PRIMARY KEY AUTOINCREMENT, UNIQUE('direction', 'proto', 'srcip', 'srcport', 'destip', 'destport', 'match', 'match-value'))" , NULL, 0, NULL);

	sqlite3_exec(openvcp_db, "CREATE TABLE vserver_bandwidth ('id' INTEGER PRIMARY KEY AUTOINCREMENT, 'name', 'direction', 'rate', 'ceil', 'burst', 'cburst', UNIQUE(name, direction))", NULL, 0, NULL);

	sqlite3_exec(openvcp_db, "CREATE TABLE vserver_trafficlimit ('id' INTEGER PRIMARY KEY AUTOINCREMENT, 'name', 'total', 'notify', 'type', 'notified', UNIQUE(name, type))", NULL, 0, NULL);

	ovcp_taskcontrol_init();


	execb_cmd("modprobe", "modprobe", "ifb", NULL);

	execb_cmd("modprobe", "modprobe", "ip_tables", NULL);

	ovcp_create_chains();
	ovcp_create_tables(openvcp_db);

	execb_cmd("modprobe", "modprobe", "nf_conntrack_ftp", NULL);


	ifaces = split_string(global_settings.ifaces, ' ', &iface_count);

	loggerdata = (struct _packetlogger **) malloc(sizeof(struct _packetlogger **) * iface_count);

	i = 0;

	while(i != iface_count)
	{
		loggerdata[i] = logtraffic_attach_iface(ifaces[i]);

		ovcp_log(OVCP_DEBUG, "Attaching to IFace: %s", ifaces[i]);
		i++;
	}

	free_strings(ifaces, iface_count);

	while(1)
	{

		time ( &current_time );
		timeinfo = localtime ( &current_time );


		timeinfo->tm_sec = 0;
		timeinfo->tm_min = 0;

		rawtime = current_time;
		rawtime = mktime(timeinfo);

		refresh_delay = rawtime + 3600 - current_time;

		day_in = 0;
		day_out = 0;
		day_total = 0;

		node_in = 0;
		node_out = 0;
		node_total = 0;

		i = 0;

		parse_resources(openvcp_db, timeinfo->tm_mon+1, timeinfo->tm_hour, timeinfo->tm_mday, timeinfo->tm_year+1900);


		while(i != iface_count)
		{
			iplist = vserver_iplist();

			pthread_mutex_lock (&loggerdata[i]->mutex);

			parse_traffic_tree(loggerdata[i]->root_node, openvcp_db, timeinfo, &node_in, &node_out, &node_total, iplist);

			free_tree(loggerdata[i]->root_node);
			loggerdata[i]->root_node = NULL;

			pthread_mutex_unlock (&loggerdata[i]->mutex);

			free_iplist(iplist);

			i++;
		}

		if(ifb_kernel_support())
			ovcp_setup_trafficthrottling(openvcp_db);
		else  ovcp_log(OVCP_WARNING, "ifb Kernel Module not found");

		/* VServer Hour Traffic */



		snprintf(sql_query, BUF_SIZE,
				"INSERT OR REPLACE INTO node_hour_traffic VALUES(%d, %d, %d, %d, %lld, %lld, %lld)",
				timeinfo->tm_mon+1, timeinfo->tm_hour, timeinfo->tm_mday, timeinfo->tm_year+1900,
				node_in, node_out, node_total);

		ret = ovcp_sqlite_exec(openvcp_db, sql_query);
	

		if(ret != SQLITE_OK)
		{
			sqlite3_close(openvcp_db);
			exit(-1);
		}




		/* Node Day Traffic */


		if(timeinfo->tm_hour != 0)
			get_node_traffic(openvcp_db, timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, &day_in, &day_out, &day_total);

		day_in += node_in;
		day_out += node_out;
		day_total += node_total;

		snprintf(sql_query, BUF_SIZE,
				"INSERT OR REPLACE INTO node_traffic VALUES(%d, %d, %d, %lld, %lld, %lld)",
				timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900,
				day_in, day_out, day_total);

		ret = ovcp_sqlite_exec(openvcp_db, sql_query);
		

		if(ret != SQLITE_OK)
		{
			sqlite3_close(openvcp_db);
			exit(-1);
		}

		ovcp_check_trafficlimits(openvcp_db);


		ovcp_log(OVCP_DEBUG, "Next Refresh: [%d s]\n", refresh_delay);

		sleep(refresh_delay);
	}





	sqlite3_close(openvcp_db);

}


static void segv_handle(int sig)
{
	ovcp_log(OVCP_ERROR, "traffic daemon process [%d] received signal 'Segmentation fault'", getpid());
	exit(-1);
}

