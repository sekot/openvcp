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

#include "ovcpxml.h"

#ifndef GET_H
	#define GET_H


	struct ovcp_response_st *get_xid(struct ovcp_request_st *request);
	struct ovcp_response_st *get_state(struct ovcp_request_st *request);
	struct ovcp_response_st *get_ips(struct ovcp_request_st *request);
	struct ovcp_response_st *get_flags(struct ovcp_request_st *request);
	struct ovcp_response_st *get_schedule(struct ovcp_request_st *request);
	struct ovcp_response_st *get_cgroup(struct ovcp_request_st *request);
	struct ovcp_response_st *get_resources(struct ovcp_request_st *request);
	struct ovcp_response_st *get_mark(struct ovcp_request_st *request);
	struct ovcp_response_st *get_hostname(struct ovcp_request_st *request);
	struct ovcp_response_st *get_space(struct ovcp_request_st *request);
	struct ovcp_response_st *get_rlimit(struct ovcp_request_st *request);
	struct ovcp_response_st *get_image(struct ovcp_request_st *request);
	struct ovcp_response_st *get_uptime(struct ovcp_request_st *request);
	struct ovcp_response_st *get_load(struct ovcp_request_st *request);
	struct ovcp_response_st *get_filter(struct ovcp_request_st *request);

#endif
