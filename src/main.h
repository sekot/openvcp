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


#include "parseconfig.h"
#include "config.h"


#ifndef MAIN_H
	#define MAIN_H

	#include <sqlite3.h>
	#include <netinet/in.h>
	
        #ifdef HAVE_LIBGNUTLS
                #include <gnutls/gnutls.h>
        #endif

	extern struct config_st global_settings;
	extern int kernel_vci_version;
	extern sqlite3 *ovcp_db;

	
 
 	struct ovcp_session
	{
		int session_socket;
		struct sockaddr_in client_addr;
		int tls;
		
		#ifdef HAVE_LIBGNUTLS
		gnutls_session_t *tls_session;
		#endif
	
	};

#endif
