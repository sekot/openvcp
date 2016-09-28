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


#ifndef CONFIG_H


	#define CONFIG_H

	#ifndef BUF_SIZE
		#define BUF_SIZE 1000
	#endif
	
	#ifndef OPENVCP_CONFIG
		#define OPENVCP_CONFIG "/etc/openvcpd.conf"
	#endif
	
	#ifndef OPENVCP_DATA
		#define OPENVCP_DATA "/usr/share/openvcpd"
	#endif
	
	#ifndef OPENVCP_DATABASE
		#define OPENVCP_DATABASE OPENVCP_DATA"/db"
	#endif

	#ifndef START_XID
		#define START_XID "7000"
	#endif 

	#ifndef XID_COUNTER
		#define XID_COUNTER OPENVCP_DATA"/xidcounter"
	#endif 

	#ifndef REVISION
		#define REVISION "0"
	#endif
#endif
