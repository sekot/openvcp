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


#include <sqlite3.h>


#ifndef FILTER_H
	#define FILTER_H


	#ifndef IPTABLES_VERSION
		#define IPTABLES_VERSION  "0.3.6"
	#endif

	#ifndef IPTABLES_LIB_DIR
		#define IPTABLES_LIB_DIR "/lib/iptables"
	#endif

	struct ovcp_ipt_rule
	{
		enum
		{
			IPT_DIRECTION_IN,
			IPT_DIRECTION_OUT

		} direction;

		enum
		{
			IPT_PROTO_TCP,
			IPT_PROTO_UDP,
			IPT_PROTO_ICMP,
                        IPT_PROTO_ALL

		} proto;

		char *srcip; int srcport;
		char *destip; int destport;

		enum
		{
			IPT_TARGET_ACCEPT,
			IPT_TARGET_DROP,
			IPT_TARGET_REJECT

		} target;

		enum
		{
			IPT_MATCH_NONE,
			IPT_MATCH_LIMIT,
			IPT_MATCH_STATE,
			IPT_MATCH_ICMP

		} match;

		enum
               {
                       IPT_STATE_NEW  = 0x0000000f,
                       IPT_STATE_ESTABLISHED = 0x000000f0,
                       IPT_STATE_RELATED = 0x00000f00,

               } match_value;
	};


	int ovcp_create_chains(void);
	int ovcp_create_tables(sqlite3 *openvcp_db);
	int ovcp_store_rule(char *name, struct ovcp_ipt_rule rule);
	int ovcp_iptable_rule(struct ovcp_ipt_rule *rule);

#endif
