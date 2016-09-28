/*  OpenVCP
 *  Copyright (C) 2007 Gerrit Wyen <gerrit@t4a.net>
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


#include <getopt.h>
#include <sys/errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <time.h>

#include "libiptc/libiptc.h"

#include "config.h"
#include "filter.h"
#include "misc.h"


int ovcp_create_chains(void)
{
	struct iptc_handle *h;
	const char *chain, *target_name;
	char intablename[] = "openvcp_in";
	char outtablename[] = "openvcp_out";
	char *iptargv[6];
	const struct ipt_entry *e;
	int ret, found = 0;




	h = iptc_init("filter");

	if (!h)
	{
		ovcp_log(OVCP_ERROR, "IPTables error (%s)", iptc_strerror(errno));
		return -1;
	}


	for (chain = iptc_first_chain(h); chain && found != 0xff; chain = iptc_next_chain(h))
	{
		if(strcmp(chain, intablename) == 0)
			found |= 0x0f;
		if(strcmp(chain, outtablename) == 0 )
			found |= 0xf0;
	}


	if((found & 0x0f) != 0x0f)
	{

		if(iptc_create_chain(intablename, h) == 0)
			return -1;

	}

	if((found & 0xf0) != 0xf0)
	{


		if(iptc_create_chain(outtablename, h) == 0)
			return -1;
	}

	if(!iptc_commit(h))
		return -1;


	h = iptc_init("filter");

	for (e = iptc_first_rule("INPUT", h); e; e = iptc_next_rule(e, h))
	{
		target_name = iptc_get_target(e, h);
		if(strcmp(intablename, target_name) == 0)
			break;
	}


	if(e == NULL)
	{
		iptargv[0] = "iptables";
		iptargv[1] = "-A";
		iptargv[2] = "INPUT";
		iptargv[3] = "-j";
		iptargv[4] = intablename;
		iptargv[5] = NULL;

		ret = execb("iptables", iptargv);
		if (ret != 0) return -1;

	}



	for (e = iptc_first_rule("OUTPUT", h); e; e = iptc_next_rule(e, h))
	{
		target_name = iptc_get_target(e, h);
		if(strcmp(outtablename, target_name) == 0)
			break;
	}


	if(e == NULL)
	{
		iptargv[0] = "iptables";
		iptargv[1] = "-A";
		iptargv[2] = "OUTPUT";
		iptargv[3] = "-j";
		iptargv[4] = outtablename;
		iptargv[5] = NULL;

		ret = execb("iptables", iptargv);
		if (ret != 0) return -1;

	}


	if(!iptc_commit(h))
		return -1;



	return 0;
}


int ovcp_iptable_rule(struct ovcp_ipt_rule *rule)
{

	int ret;

	int i = 1;
	char *iptargv[21];
	char srcport[6], destport[6], proto[5];
	char match_value[30] = "";




	iptargv[0] = "openvcp";

	iptargv[i] = "-A";
	i++;

	if(rule->direction == IPT_DIRECTION_IN)
		iptargv[i] = "openvcp_in";
	else
		iptargv[i] = "openvcp_out";
	i++;

        if(rule->proto != IPT_PROTO_ALL)
        {
            iptargv[i] = "-p";
            i++;

            if(rule->proto == IPT_PROTO_TCP)
                    strcpy(proto, "tcp");
            else if(rule->proto == IPT_PROTO_UDP)
                    strcpy(proto, "udp");
            else
                    strcpy(proto, "icmp");
            
            iptargv[i] = proto;
            i++;
        }

	if(rule->srcip != NULL)
	{
		iptargv[i] = "-s";
		i++;
		iptargv[i] = rule->srcip;
		i++;
	}

	if(rule->srcport != -1)
	{
		iptargv[i] = "--sport";
		i++;
		snprintf(srcport, 6, "%d", rule->srcport);
		iptargv[i] = srcport;
		i++;
	}

	if(rule->destip != NULL)
	{
		iptargv[i] = "-d";
		i++;
		iptargv[i] = rule->destip;
		i++;
	}

	if(rule->destport != -1)
	{
		iptargv[i] = "--dport";
		i++;
		snprintf(destport, 6, "%d", rule->destport);
		iptargv[i] = destport;
		i++;
	}

	iptargv[i] = "-j";
	i++;

	if(rule->target == IPT_TARGET_DROP)
		iptargv[i] = "DROP";
	else if(rule->target == IPT_TARGET_REJECT)
		iptargv[i] = "REJECT";
	else
		iptargv[i] = "ACCEPT";
	i++;


	if(rule->match == IPT_MATCH_LIMIT)
	{
		iptargv[i] = "-m";
		i++;

		iptargv[i] = "limit";
		i++;

		iptargv[i] = "--limit";
		i++;

		snprintf(match_value, 30, "%d/s", rule->match_value);
		iptargv[i] = match_value;
		i++;
	}

	if(rule->match == IPT_MATCH_STATE)
	{
		iptargv[i] = "-m";
		i++;

		iptargv[i] = "state";
		i++;

		iptargv[i] = "--state";
		i++;

		if((rule->match_value & IPT_STATE_NEW) ==  IPT_STATE_NEW)
		{
			strcpy(match_value, "NEW");
		}
		if((rule->match_value & IPT_STATE_ESTABLISHED) ==  IPT_STATE_ESTABLISHED)
		{
			if(match_value[0] != 0)
				strcat(match_value, ",ESTABLISHED");
			else
				strcpy(match_value, "ESTABLISHED");

		}
		if((rule->match_value & IPT_STATE_RELATED) ==  IPT_STATE_RELATED)
		{
			if(match_value[0] != 0)
				strcat(match_value, ",RELATED");
			else
				strcpy(match_value, "RELATED");
		}

		iptargv[i] = match_value;
		i++;
	}

	if(rule->match == IPT_MATCH_ICMP)
	{
		iptargv[i] = "--icmp-type";
		i++;

		snprintf(match_value, 30, "%d", rule->match_value);
		iptargv[i] = match_value;
		i++;
	}


	#ifdef DEBUG
	int d = 0;
	while(d < i)
	{
		printf("[%s] ", iptargv[d]);

		d++;
	}
	printf("\n");
	#endif

	iptargv[i] = NULL;
	ret = execb("iptables", iptargv);


	if (ret != 0)
	{
		ovcp_log(OVCP_ERROR, "IPtables error");
		return -1;
	}


	return 0;
}


int ovcp_create_tables(sqlite3 *openvcp_db)
{
	char **sql_result;
	int i, nrow, ncolumn, ret;
	char *sql_error;
	struct ovcp_ipt_rule rule;
	struct iptc_handle *h;

	h = iptc_init("filter");

	if (!h)
	{
		ovcp_log(OVCP_ERROR, "IPTables error (%s)", iptc_strerror(errno));
		return -1;
	}

	iptc_flush_entries("openvcp_in", h);
	iptc_flush_entries("openvcp_out", h);

	ret = iptc_commit(h);

	if (!ret)
	{
		ovcp_log(OVCP_ERROR, "IPtables error: %s", iptc_strerror(errno));
		return -1;
	}


	ret = sqlite3_get_table(openvcp_db, "SELECT * FROM iptable_rules", &sql_result, &nrow, &ncolumn, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		return -1;
	}

	i = 1;


	while(i != nrow+1)
	{

		if(strcmp("INPUT", sql_result[i*(ncolumn)+0]) == 0)
		{
			rule.direction = IPT_DIRECTION_IN;
		}
		else
		{
			rule.direction = IPT_DIRECTION_OUT;
		}

		if(strcmp("tcp", sql_result[i*(ncolumn)+1]) == 0)
		{
			rule.proto = IPT_PROTO_TCP;
		}
		else if(strcmp("udp", sql_result[i*(ncolumn)+1]) == 0)
		{
			rule.proto = IPT_PROTO_UDP;
		}
		else if(strcmp("icmp", sql_result[i*(ncolumn)+1]) == 0)
		{
			rule.proto = IPT_PROTO_ICMP;
		}
                else
                {
                    rule.proto = IPT_PROTO_ALL;
                }

		if(strcmp(sql_result[i*(ncolumn)+2], "(null)") == 0)
			rule.srcip = NULL;
		else
			rule.srcip = sql_result[i*(ncolumn)+2];

		rule.srcport = atoi(sql_result[i*(ncolumn)+3]);

		if(strcmp(sql_result[i*(ncolumn)+4], "(null)") == 0)
			rule.destip = NULL;
		else
			rule.destip = sql_result[i*(ncolumn)+4];

		rule.destport = atoi(sql_result[i*(ncolumn)+5]);


		if(strcmp("DROP", sql_result[i*(ncolumn)+6]) == 0)
		{
			rule.target = IPT_TARGET_DROP;
		}
		else if(strcmp("REJECT", sql_result[i*(ncolumn)+6]) == 0)
		{
			rule.target = IPT_TARGET_REJECT;
		}
		else
		{
			rule.target = IPT_TARGET_ACCEPT;
		}

		rule.match = IPT_MATCH_NONE;

		if(strcmp("LIMIT", sql_result[i*(ncolumn)+7]) == 0)
		{
			rule.match = IPT_MATCH_LIMIT;
		}
		if(strcmp("STATE", sql_result[i*(ncolumn)+7]) == 0)
		{
			rule.match = IPT_MATCH_STATE;
		}

		rule.match_value = 0;

		if(rule.match == IPT_MATCH_STATE)
		{
			if(strstr(sql_result[i*(ncolumn)+8], "ESTABLISHED"))
			{
				rule.match_value = rule.match_value ^ IPT_STATE_ESTABLISHED;
			}
			if(strstr(sql_result[i*(ncolumn)+8], "NEW"))
			{
				rule.match_value = rule.match_value ^ IPT_STATE_NEW;
			}
			if(strstr(sql_result[i*(ncolumn)+8], "RELATED"))
			{
				rule.match_value = rule.match_value ^ IPT_STATE_RELATED;
			}
		}
		else
		{
			rule.match_value = atoi(sql_result[i*(ncolumn)+8]);
		}


		ovcp_iptable_rule(&rule);

		i++;
	}




	sqlite3_free_table(sql_result);



	return 0;
}



int ovcp_store_rule(char *name, struct ovcp_ipt_rule rule)
{
	char sql_query[BUF_SIZE];
	char *sql_error;
	sqlite3 *openvcp_db;
	char *direction = "", *proto = "", *target = "", *match = "";
	char match_value[25] = "";
	int ret;

	ret = sqlite3_open(OPENVCP_DATABASE, &openvcp_db);

	if(ret != 0)
	{
		ovcp_log(OVCP_ERROR, "Can't open database: (%s)", sqlite3_errmsg(openvcp_db));
		sqlite3_close(openvcp_db);
		return -1;
	}

	if(rule.direction == IPT_DIRECTION_IN)
		direction = "INPUT";
	else
		direction = "OUTPUT";

	if(rule.proto == IPT_PROTO_TCP)
		proto = "tcp";
	else if(rule.proto == IPT_PROTO_ICMP)
		proto = "icmp";
	else if(rule.proto == IPT_PROTO_UDP)
		proto = "udp";
        else
            proto = "any";

	if(rule.target == IPT_TARGET_DROP)
		target = "DROP";
	else if(rule.target == IPT_TARGET_REJECT)
		target = "REJECT";
	else
		target = "ACCEPT";

	if(rule.match != IPT_MATCH_NONE)
	{
		if(rule.match == IPT_MATCH_LIMIT)
		{
			match = "LIMIT";
			snprintf(match_value, 25, "%d", rule.match_value);
		}
		else if(rule.match == IPT_MATCH_ICMP)
		{
			match = "ICMPTYPE";
			snprintf(match_value, 25, "%d", rule.match_value);
		}
		else
		{
			match = "STATE";

			if((rule.match_value & IPT_STATE_NEW) ==  IPT_STATE_NEW)
			{
				strcpy(match_value, "NEW");
			}
			if((rule.match_value & IPT_STATE_ESTABLISHED) ==  IPT_STATE_ESTABLISHED)
			{
				if(match_value[0] != 0)
					strcat(match_value, ",ESTABLISHED");
				else
					strcpy(match_value, "ESTABLISHED");
			}
			if((rule.match_value & IPT_STATE_RELATED) ==  IPT_STATE_RELATED)
			{
				if(match_value[0] != 0)
					strcat(match_value, ",RELATED");
				else
					strcpy(match_value, "RELATED");
			}

		}
	}

	snprintf(sql_query, BUF_SIZE, "INSERT OR REPLACE INTO iptable_rules('direction', 'proto', 'srcip', 'srcport', 'destip', "
		"'destport', 'target', 'match', 'match-value', 'name') VALUES('%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', '%s')",
		direction, proto, rule.srcip, rule.srcport, rule.destip, rule.destport, target, match, match_value, name);

	ret = sqlite3_exec(openvcp_db, sql_query, NULL, 0, &sql_error);

	if(ret != SQLITE_OK)
	{
		ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
		sqlite3_free(sql_error);
		sqlite3_close(openvcp_db);
		return -1;
	}


	sqlite3_close(openvcp_db);


	return 0;
}
