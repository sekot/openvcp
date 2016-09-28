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





#ifndef REQUEST_H
	#define REQUEST_H

	#include "node.h"
	#include "set.h"
	#include "get.h"
	#include "traffic.h"
	#include "setup.h"
  #include "move.h"
	#include "ovcpxml.h"
	#include "bandwidth.h"
	#include "userbackup.h"

	#define OVCP_XML_RPC_API_VERSION "1.0"

        struct ovcp_response_st *system_list_methods(struct ovcp_request_st *request);
        struct ovcp_response_st *system_method_signature(struct ovcp_request_st *request);
        struct ovcp_response_st *system_method_help(struct ovcp_request_st *request);
        struct ovcp_response_st *system_get_api_version(struct ovcp_request_st *request);
        struct ovcp_response_st *vserver_rescue(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_control_start(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_control_stop(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_list(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_setup_image(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_setup_config(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_traffic(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_traffic_total(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_traffic_hour(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_remove(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_resources_hour(struct ovcp_request_st *request);
	struct ovcp_response_st *vserver_move(struct ovcp_request_st *request);

	int init_modules(void);
#endif
