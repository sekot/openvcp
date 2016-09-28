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



#ifndef USERBACKUP_H
	#define USERBACKUP_H

	struct ovcp_response_st *get_userbackups(struct ovcp_request_st *request);
	struct ovcp_response_st *create_userbackups(struct ovcp_request_st *request);
	struct ovcp_response_st *restore_userbackups(struct ovcp_request_st *request);
	struct ovcp_response_st *refresh_userbackups(struct ovcp_request_st *request);
	struct ovcp_response_st *delete_userbackups(struct ovcp_request_st *request);

#endif
