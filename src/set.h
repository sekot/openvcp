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

#ifndef SET_H
	#define SET_H


	struct ovcp_response_st *set_rlimit(struct ovcp_request_st *request);
	struct ovcp_response_st *set_flag_add(struct ovcp_request_st *request);
	struct ovcp_response_st *set_flag_remove(struct ovcp_request_st *request);
	struct ovcp_response_st *set_mark(struct ovcp_request_st *request);
	struct ovcp_response_st *set_schedule(struct ovcp_request_st *request);
	struct ovcp_response_st *set_cgroup(struct ovcp_request_st *request);
	struct ovcp_response_st *add_ip(struct ovcp_request_st *request);
	struct ovcp_response_st *remove_ip(struct ovcp_request_st *request);
	struct ovcp_response_st *set_space(struct ovcp_request_st *request);
	struct ovcp_response_st *set_rootpw(struct ovcp_request_st *request);
	struct ovcp_response_st *set_hostname(struct ovcp_request_st *request);
	struct ovcp_response_st *set_filter(struct ovcp_request_st *request);
	struct ovcp_response_st *del_filter(struct ovcp_request_st *request);


#endif
