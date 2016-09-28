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

#ifndef MISC_H
        #define MISC_H

	#include "main.h"
	
	struct iface_entry
	{
		char *dev;
		char *ip;				
	};
	
	struct vsip_entry
	{
		char *name;
		char *ip;				
	};

        int ovcp_write(struct ovcp_session *session, char *data, int len);
        int ovcp_read(struct ovcp_session *session, char *data, int len);
        char *ovcp_readline(struct ovcp_session *session);
        int ovcp_close(struct ovcp_session *session);
        
	#ifdef HAVE_LIBGNUTLS
	gnutls_session_t *tls_session_new (int client_socket, gnutls_anon_server_credentials_t anoncred);        
	void gnutls_log(int level, const char *msg);
	#endif
	
	int vci_version(void);
	char *vserver_name(int xid);
	
        int vserver_exist(char *name);
        int vserver_lock(char *name);
        int vserver_unlock(char *name);
        int vserver_isrunning(char *name);

	struct iface_entry *parse_ifaces(char *name);
	int free_ifacelist(struct iface_entry *list);
	
        char **parse_ips(char *name, int *num);
        int vserver_xid(char *name);

	int execb(char *cmd, char **argv);
	int execb_cmd(char *cmd, ...);
        char *exec_cmd(char *cmd, int exectime, ...);
        char *exec_cmdv(char *cmd, int exectime, char **argv);

        int die(char *string);

	struct vsip_entry *vserver_iplist(void);
	int free_iplist(struct vsip_entry *list);
	
	int kernelmodule_loaded(char *mod);
	char **node_interfaces(int *count);
	
	int send_email(char *msg);
	
	enum ovcploglevel { OVCP_DEBUG, OVCP_INFO, OVCP_WARNING, OVCP_ERROR };
	
	int ovcp_log_open(char *file, int level, int verbose);
	void ovcp_log(int level, char *fmt, ...);
	
	
	int ovcp_sqlite_exec(sqlite3 *db, char *sql_qry);
	
#endif

