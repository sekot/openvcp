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


#ifndef OVCPXML_H
	#define OVCPXML_H	
	
	
	#include <libxml/encoding.h>
	#include <libxml/xmlwriter.h>

	enum ovcp_xml_datatypes {OVCP_XML_STRUCT, OVCP_XML_STRUCT_ELEMENT, OVCP_XML_ARRAY, OVCP_XML_INT, OVCP_XML_BOOLEAN, OVCP_XML_STRING, OVCP_XML_DOUBLE};
	
	#ifndef TRUE
		#define TRUE	1
		#define FALSE	0
	#endif
	
	struct ovcp_response_st
	{
		xmlTextWriterPtr writer;
		xmlBufferPtr buf;
	};

	struct ovcp_request_st
	{
		char *methodname;
		struct ovcp_data_st *data;	
	};
	
	struct ovcp_data_st
	{
		int type;
		void *data;
		
		struct ovcp_data_st *next, *sub;
	};

	struct ovcp_response_st *ovcp_response_new(void);
	int ovcp_response_array_new(struct ovcp_response_st *response);
	int ovcp_response_array_add_string(struct ovcp_response_st *response, char *string);
	int ovcp_response_array_add_int(struct ovcp_response_st *response, int value);
	int ovcp_response_array_end(struct ovcp_response_st *response);
	int ovcp_response_struct_new(struct ovcp_response_st *response);
	int ovcp_response_struct_end(struct ovcp_response_st *response);
	int ovcp_response_struct_add_string(struct ovcp_response_st *response, char *name, char *string);
	int ovcp_response_struct_add_int(struct ovcp_response_st *response, char *name, int value);
	int ovcp_response_struct_add_double(struct ovcp_response_st *response, char *name, double value);
	int ovcp_response_struct_add_boolean(struct ovcp_response_st *response, char *name, int value);
	int ovcp_response_add_string(struct ovcp_response_st *response, char *string);
	int ovcp_response_add_int(struct ovcp_response_st *response, int value);
	int ovcp_response_add_double(struct ovcp_response_st *response, double value);
	int ovcp_response_add_boolean(struct ovcp_response_st *response, int value);
	struct ovcp_response_st *ovcp_response_error(int code, char *desc);
	char *ovcp_response_finalize(struct ovcp_response_st *response);
	int ovcp_free_response(struct ovcp_response_st *response);

	struct ovcp_request_st *ovcp_parse_request(char *xmlstr);
	struct ovcp_data_st *ovcp_request_get_arg(struct ovcp_request_st *request, int id);
	struct ovcp_data_st *ovcp_array_get_element(struct ovcp_data_st *array, int id);
	struct ovcp_data_st *ovcp_struct_get_element(struct ovcp_data_st *structure, char *name);
	struct ovcp_data_st *ovcp_struct_get_member(struct ovcp_data_st *structure, char *name);
	inline void *ovcp_data_get(struct ovcp_data_st *data);
	inline void *ovcp_member_get(struct ovcp_data_st *data);
	inline int ovcp_data_is_array(struct ovcp_data_st *data);
	inline int ovcp_data_is_struct(struct ovcp_data_st *data);
	inline int ovcp_data_is_int(struct ovcp_data_st *data);
	inline int ovcp_data_is_double(struct ovcp_data_st *data);
	inline int ovcp_data_is_boolean(struct ovcp_data_st *data);
	inline int ovcp_data_is_string(struct ovcp_data_st *data);
	int ovcp_free_request(struct ovcp_request_st *request);
	
#endif
