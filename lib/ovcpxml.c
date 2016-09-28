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
 
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include "ovcpxml.h"



#ifdef DEBUG_XML
	#define XML_DEBUG(args) printf args
#else
	#define XML_DEBUG(...)
#endif




/* XML Writing */

struct ovcp_response_st *ovcp_response_new(void)
{
	int rc;
  	struct ovcp_response_st *response;

	response = (struct ovcp_response_st *)malloc(sizeof(struct ovcp_response_st));


	response->buf = xmlBufferCreate();
	if (response->buf == NULL) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error creating the xml buffer\n"));
		free(response);
		return NULL;
	}

	response->writer = xmlNewTextWriterMemory(response->buf, 0);
	if (response->writer == NULL) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error creating the xml writer\n"));
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}

  
	rc = xmlTextWriterStartDocument(response->writer, NULL, "utf-8", NULL);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}

	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "methodResponse");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "params");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}
	
	return response;
}

int ovcp_response_array_new(struct ovcp_response_st *response)
{
	int rc;

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "array");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "data");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	return 0;
}

int ovcp_response_array_add_string(struct ovcp_response_st *response, char *string)
{
	int rc;
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "string", BAD_CAST string);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}	
	
	return 0;
}  

int ovcp_response_array_add_int(struct ovcp_response_st *response, int value)
{
	int rc;
	char value_str[12];
	
	snprintf(value_str, 12, "%d", value);
	
	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "int", BAD_CAST value_str);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}  

int ovcp_response_array_end(struct ovcp_response_st *response)
{
	int rc;

	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}

int ovcp_response_struct_new(struct ovcp_response_st *response)
{
	int rc;
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "struct");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	return 0;
}

int ovcp_response_struct_end(struct ovcp_response_st *response)
{
	int rc;

	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}

int ovcp_response_struct_add_string(struct ovcp_response_st *response, char *name, char *string)
{
	int rc;
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "member");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}	
	
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "name", BAD_CAST name);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "string", BAD_CAST string);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}	

	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}	
	
	return 0;
}  

int ovcp_response_struct_add_int(struct ovcp_response_st *response, char *name, int value)
{
	int rc;
	char value_str[12];
	
	snprintf(value_str, 12, "%d", value);
	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "member");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "name", BAD_CAST name);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "int", BAD_CAST value_str);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}

	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
		
	return rc;
}  

int ovcp_response_struct_add_boolean(struct ovcp_response_st *response, char *name, int value)
{
	int rc;
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "member");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "name", BAD_CAST name);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	if(value == 1)
		rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "boolean", BAD_CAST "1");
	else
		rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "boolean", BAD_CAST "0");
		
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}

	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
		
	return rc;
}

int ovcp_response_struct_add_double(struct ovcp_response_st *response, char *name, double value)
{
	int rc;
	char value_str[20];
	
	snprintf(value_str, 20, "%lf", value);
	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "member");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "name", BAD_CAST name);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "double", BAD_CAST value_str);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}  

int ovcp_response_add_string(struct ovcp_response_st *response, char *string)
{
	int rc;

	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "string", BAD_CAST string);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}   

int ovcp_response_add_int(struct ovcp_response_st *response, int value)
{
	int rc;
	char value_str[12];
	
	snprintf(value_str, 12, "%d", value);
	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "int", BAD_CAST value_str);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}

int ovcp_response_add_double(struct ovcp_response_st *response, double value)
{
	int rc;
	char value_str[20];
	
	snprintf(value_str, 20, "%lf", value);
	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
		
	rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "double", BAD_CAST value_str);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}

int ovcp_response_add_boolean(struct ovcp_response_st *response, int value)
{
	int rc;

	
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "param");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		return -1;
	}
	
	if(value == 1)
		rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "boolean", BAD_CAST "1");
	else
		rc = xmlTextWriterWriteElement(response->writer, BAD_CAST "boolean", BAD_CAST "0");
	
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterWriteElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	rc = xmlTextWriterEndElement(response->writer);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndElement\n"));
		return -1;
	}
	
	return 0;
}   

struct ovcp_response_st *ovcp_response_error(int code, char *desc)
{
	int rc;
  	struct ovcp_response_st *response;

	response = (struct ovcp_response_st *)malloc(sizeof(struct ovcp_response_st));


	response->buf = xmlBufferCreate();
	if (response->buf == NULL) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error creating the xml buffer\n"));
		free(response);
		return NULL;
	}

	response->writer = xmlNewTextWriterMemory(response->buf, 0);
	if (response->writer == NULL) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error creating the xml writer\n"));
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}

  
	rc = xmlTextWriterStartDocument(response->writer, NULL, "ISO-8859-1", NULL);
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}

	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "methodResponse");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "fault");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}
	
	rc = xmlTextWriterStartElement(response->writer, BAD_CAST "value");
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterStartElement\n"));
		xmlFreeTextWriter(response->writer);
		xmlBufferFree(response->buf);
		free(response);
		return NULL;
	}
	
	ovcp_response_struct_new(response);
	ovcp_response_struct_add_int(response, "faultCode", code);
	ovcp_response_struct_add_string(response, "faultString", desc);	
	ovcp_response_struct_end(response);
		
	return response;
}
 

char *ovcp_response_finalize(struct ovcp_response_st *response)
{
	int rc;
	char *xml_string;
	
	rc = xmlTextWriterEndDocument(response->writer);
	
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n"));
		return NULL;
	}

	xmlFreeTextWriter(response->writer);

	xml_string = strdup((char *)response->buf->content);

	xmlBufferFree(response->buf);

	free(response);

	return xml_string;
}


int ovcp_free_response(struct ovcp_response_st *response)
{
	int rc;
	
	rc = xmlTextWriterEndDocument(response->writer);
	
	if (rc < 0) 
	{
		XML_DEBUG(("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n"));
		return -1;
	}

	xmlFreeTextWriter(response->writer);

	xmlBufferFree(response->buf);

	free(response);

	return 0;
}







/* XML Reading */


static struct ovcp_data_st *xml_parse_values(xmlTextReaderPtr reader);
static struct ovcp_data_st *xml_parse_struct(xmlTextReaderPtr reader);
static struct ovcp_data_st *xml_parse_array(xmlTextReaderPtr reader);
static struct ovcp_data_st *xml_parse_param(xmlTextReaderPtr reader);
static int ovcp_free_data(struct ovcp_data_st *data);


struct ovcp_data_st *ovcp_request_get_arg(struct ovcp_request_st *request, int id)
{
	int i = 0;
	struct ovcp_data_st *tmpdata;
	
	tmpdata = request->data;
	
	while(tmpdata != NULL)
	{
		if(i == id)
			break;	
		
		tmpdata = tmpdata->next;
		i++;
	}
	
	return tmpdata;
}

struct ovcp_data_st *ovcp_array_get_element(struct ovcp_data_st *array, int id)
{
	int i = 0;
	struct ovcp_data_st *tmpdata;
	
	tmpdata = array->sub;
	
	while(tmpdata != NULL)
	{
		if(i == id)
			break;	
		
		tmpdata = tmpdata->next;
		i++;
	}
	
	return tmpdata;
}

struct ovcp_data_st *ovcp_struct_get_element(struct ovcp_data_st *structure, char *name)
{
	struct ovcp_data_st *data;

	data = ovcp_struct_get_member(structure, name);
	
	if(data != NULL)
		return data->sub;
		
	return NULL;
}

struct ovcp_data_st *ovcp_struct_get_member(struct ovcp_data_st *structure, char *name)
{
	struct ovcp_data_st *tmpdata;
	
	if(structure->sub == NULL)
		return NULL;

	tmpdata = structure->sub;
	
	while(tmpdata != NULL)
	{			
		if(strcmp(tmpdata->data, name) == 0)
			break;	
		
		tmpdata = tmpdata->next;
	}


	if(tmpdata == NULL)
		return NULL;
	else
		return tmpdata;
}

inline void *ovcp_data_get(struct ovcp_data_st *data)
{
	return (data != NULL) ? data->data : NULL;
}

inline void *ovcp_member_get(struct ovcp_data_st *data)
{
	return (data != NULL) ? data->sub : NULL;
}

inline int ovcp_data_is_array(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_ARRAY) : 0;
}

int ovcp_data_is_struct(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_STRUCT) : 0;
}

inline int ovcp_data_is_int(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_INT) : 0;
}

inline int ovcp_data_is_double(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_DOUBLE) : 0;
}

inline int ovcp_data_is_boolean(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_BOOLEAN) : 0;
}

inline int ovcp_data_is_string(struct ovcp_data_st *data)
{
	return (data != NULL) ? (data->type == OVCP_XML_STRING) : 0;
}

static int ovcp_free_data(struct ovcp_data_st *data)
{

	if(data->data != NULL)
		free(data->data);

	if(data->next != NULL)
		ovcp_free_data(data->next);
	if(data->sub != NULL)
		ovcp_free_data(data->sub);
	

	free(data);
	
	return 0;
}


int ovcp_free_request(struct ovcp_request_st *request)
{
	if(request->methodname != NULL)
		free(request->methodname);

	if(request->data != NULL)
		ovcp_free_data(request->data);
	

	free(request);

	
	return 0;
}


struct ovcp_request_st *ovcp_parse_request(char *xmlstr)
{
	if(xmlstr == NULL) return NULL;
	if(strlen(xmlstr) <= 0) return NULL;

	xmlTextReaderPtr reader;
	struct ovcp_request_st *request;
	struct ovcp_data_st *tmpdata = NULL;
	char *p;
	
	request = (struct ovcp_request_st *) malloc(sizeof(struct ovcp_request_st));
	memset(request, 0, sizeof(struct ovcp_request_st));
	
	reader = xmlReaderForMemory(xmlstr, strlen(xmlstr), NULL, NULL, 0);
     
	if (reader == NULL) 
	{
		XML_DEBUG(("Unable to open buffer\n"));
		ovcp_free_request(request);
		return NULL;
	}

	xmlTextReaderRead(reader);
        
        p = (char *) xmlTextReaderConstName(reader);
        
        if(p == NULL)
        {
		XML_DEBUG(("Parsing Error\n"));
		ovcp_free_request(request);
		xmlFreeTextReader(reader);
		return NULL;
	}
        
	if(strcmp(p, "methodCall") == 0)
	{
		xmlTextReaderRead(reader);

		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_request(request);
			xmlFreeTextReader(reader);
			return NULL;
		}
   
		if(strcmp(p, "methodName") == 0)
		{
       	    	  	xmlTextReaderRead(reader);
       		
       			p = (char *) xmlTextReaderConstValue(reader);
        
			if(p == NULL)
			{
				XML_DEBUG(("Parsing Error: methodName missing\n"));
				ovcp_free_request(request);
				xmlFreeTextReader(reader);
				return NULL;
			}
       			
        		request->methodname = strdup(p); 
        		
        		
       			xmlTextReaderRead(reader);
			
			p = (char *) xmlTextReaderConstName(reader);
        
			if(p == NULL)
			{
				XML_DEBUG(("Parsing Error\n"));
				ovcp_free_request(request);
				xmlFreeTextReader(reader);
				return NULL;
			}
			
			if(strcmp(p,"methodName") != 0)
			{
				XML_DEBUG(("Parsing Error: methodName not terminated\n"));
				ovcp_free_request(request);
				xmlFreeTextReader(reader);
				return NULL;
			}
			
	
			xmlTextReaderRead(reader);
			
			p = (char *) xmlTextReaderConstName(reader);
        
			if(p == NULL)
			{
				XML_DEBUG(("Parse Error\n"));
				ovcp_free_request(request);
				xmlFreeTextReader(reader);
				return NULL;
			}
        		
       			if(strcmp(p, "params") != 0)
       			{
       		 		XML_DEBUG(("Parsing Error: params not found\n"));
				ovcp_free_request(request);
				xmlFreeTextReader(reader);
       				return NULL;
       			}
       			
       			
       			while(1)
       			{
       				xmlTextReaderRead(reader);
        			p = (char *) xmlTextReaderConstName(reader);
        
				if(p == NULL)
				{
					XML_DEBUG(("Parsing Error\n"));
					ovcp_free_request(request);
					xmlFreeTextReader(reader);
					return NULL;
				}
				
       				if(strcmp(p, "param") != 0)
       					break;
       			
       				if(request->data == NULL)
       				{
       					request->data = xml_parse_param(reader);
       					tmpdata = request->data;
       				}
       				else
       				{
       					tmpdata->next = xml_parse_param(reader);
       					tmpdata = tmpdata->next;
       				}
       			
       				
       				xmlTextReaderRead(reader);
        			p = (char *) xmlTextReaderConstName(reader);
        
				if(p == NULL)
				{
					XML_DEBUG(("Parsing Error\n"));
					ovcp_free_request(request);
					xmlFreeTextReader(reader);
					return NULL;
				}
				
       				if(strcmp(p, "param") != 0)
       				{
       					XML_DEBUG(("Parsing Error: param tag not terminated [%s]\n", p));
       					ovcp_free_request(request);
       					xmlFreeTextReader(reader);
       					return NULL;
       				}	
       			}
       		}
        
	}
        
        
        
	xmlFreeTextReader(reader);
       

	xmlCleanupParser();
 
 
	xmlMemoryDump();
   
	return request;
}

static struct ovcp_data_st *xml_parse_param(xmlTextReaderPtr reader)
{
	struct ovcp_data_st *data = NULL;
        char *p, *type;
        	
	xmlTextReaderRead(reader);
	p = (char *)xmlTextReaderConstName(reader);
			
	XML_DEBUG(("Param Type: %s\n", p));
				
			
	if(strcmp(p, "value") == 0)
	{
	
		xmlTextReaderRead(reader);


		type = (char *)xmlTextReaderConstName(reader);
		
		
		if(strcmp(type, "struct") == 0)
		{
			data = xml_parse_struct(reader);
		}
		else if(strcmp(type, "array") == 0)
		{
			data = xml_parse_array(reader);
		}
		else
		{
			data = xml_parse_values(reader);
		}
		
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
       
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			return NULL;
		}
						
		if(strcmp(p, "value") != 0)
		{
			XML_DEBUG(("Parsing Error: value not terminated [%s]\n", p));
			return NULL;					
		}
		
	}


	return data;
}

static struct ovcp_data_st *xml_parse_array(xmlTextReaderPtr reader)
{

	struct ovcp_data_st *data, *tmpdata = NULL;
	char *p;
	
	
	data = (struct ovcp_data_st *)malloc(sizeof(struct ovcp_data_st));
	memset(data, 0, sizeof(struct ovcp_data_st));
       					
	data->type = OVCP_XML_ARRAY;
       					
	xmlTextReaderRead(reader);
	p = (char *) xmlTextReaderConstName(reader);
       
	if(p == NULL)
	{
		XML_DEBUG(("Parsing Error\n"));
		ovcp_free_data(data);
		return NULL;
	}
					
	if(strcmp(p, "data") != 0)
	{
		XML_DEBUG(("Parsing Error: data tag not found\n"));
		ovcp_free_data(data);
		return NULL;					
	}
	
	
	while(1)
	{
		

		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
       
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_data(data);
			return NULL;
		}
		
		if(strcmp(p, "data") == 0)
				break;
	
		if(strcmp(p, "value") != 0)
		{
			XML_DEBUG(("Parsing Error: value Tag not found [%s]\n", p));
			ovcp_free_data(data);
			return NULL;					
		}
						
		xmlTextReaderRead(reader);
					
		if(data->sub == NULL)
		{
			data->sub = xml_parse_values(reader);
			tmpdata = data->sub;
		}
		else
		{
			tmpdata->next = xml_parse_values(reader);
			tmpdata = tmpdata->next;
		}
						

						
						
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error: Read Failed\n"));
			ovcp_free_data(data);
			return NULL;
		}
					
		if(strcmp(p, "value") != 0)
		{
			XML_DEBUG(("Parsing Error: Tag not terminated [%s]\n", p));
			ovcp_free_data(data);
			return NULL;					
		} 
			
					
	}

	xmlTextReaderRead(reader);
	p = (char *) xmlTextReaderConstName(reader);
       
	if(p == NULL)
	{
		XML_DEBUG(("Parsing Error\n"));
		ovcp_free_data(data);
		return NULL;
	}
					
	if(strcmp(p, "array") != 0)
	{
		XML_DEBUG(("Parsing Error: array tag not terminated [%s]\n", p));
		ovcp_free_data(data);
		return NULL;					
	}
	
	return data;
}

static struct ovcp_data_st *xml_parse_struct(xmlTextReaderPtr reader)
{
	struct ovcp_data_st *data, *tmpdata = NULL;
	char *p;
	
	data = (struct ovcp_data_st *)malloc(sizeof(struct ovcp_data_st));
	memset(data, 0, sizeof(struct ovcp_data_st));
					
	data->type = OVCP_XML_STRUCT;
					
	while(1)
	{
					
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_data(data);
			return NULL;
		}
				
		if(strcmp(p, "struct") == 0)
				break;
								
	
		if(strcmp(p, "member") != 0)
		{
			XML_DEBUG(("Parsing Error: member Tag not found\n"));
			ovcp_free_data(data);
			return NULL;							
		}
						
		xmlTextReaderRead(reader);
       		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_data(data);
			return NULL;
		}
						
		if(strcmp(p, "name") != 0)
		{
			XML_DEBUG(("Parsing Error: Name Tag not found\n"));
			ovcp_free_data(data);
			return NULL;							
		}
						
					
		xmlTextReaderRead(reader);
       		p = (char *) xmlTextReaderConstValue(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error: name tag empty\n"));
			ovcp_free_data(data);
			return NULL;
		}
						
		if(data->sub == NULL)
		{
			data->sub = (struct ovcp_data_st *)malloc(sizeof(struct ovcp_data_st));
			tmpdata = data->sub;
		}
		else
		{
			tmpdata->next = (struct ovcp_data_st *)malloc(sizeof(struct ovcp_data_st));
			tmpdata = tmpdata->next;							
		}

		memset(tmpdata, 0, sizeof(struct ovcp_data_st));
		tmpdata->type = OVCP_XML_STRUCT_ELEMENT;
		tmpdata->data = strdup(p);
			
						
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_data(data);
			return NULL;
		}
						
		if(strcmp(p, "name") != 0)
		{
			XML_DEBUG(("Parsing Error: Name Tag not found [%s]\n", p));
			ovcp_free_data(data);
			return NULL;							
		}
						
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{	
			XML_DEBUG(("Parsing Error\n"));
			ovcp_free_data(data);
			return NULL;
		}
						
		if(strcmp(p, "value") != 0)
		{
			XML_DEBUG(("Parsing Error: Value Tag not found [%s]\n", p));
			ovcp_free_data(data);
			return NULL;							
		}
					
		xmlTextReaderRead(reader);
		tmpdata->sub = xml_parse_values(reader);
						
						
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
        	if(p == NULL)
		{
			XML_DEBUG(("Parsing Error: Read Failed\n"));
			ovcp_free_data(data);
			return NULL;
		}
					
		if(strcmp(p, "value") != 0)
		{
			XML_DEBUG(("Parsing Error: Tag not terminated [%s]\n", p));
			ovcp_free_data(data);
			return NULL;					
		} 
						
		xmlTextReaderRead(reader);
		p = (char *) xmlTextReaderConstName(reader);
        
		if(p == NULL)
		{
			XML_DEBUG(("Parsing Error: Read Failed\n"));
			ovcp_free_data(data);
			return NULL;
		}
					
		if(strcmp(p, "member") != 0)
		{
			XML_DEBUG(("Parsing Error: Tag not terminated [%s]\n", p));
			ovcp_free_data(data);
			return NULL;					
		} 
	}

	return data;
}


static struct ovcp_data_st *xml_parse_values(xmlTextReaderPtr reader)
{
	struct ovcp_data_st *data;
	char *type, *value;
	char *p;
	
	
	data = (struct ovcp_data_st *)malloc(sizeof(struct ovcp_data_st));
	memset(data, 0, sizeof(struct ovcp_data_st));

	type = (char *)xmlTextReaderConstName(reader);
	xmlTextReaderRead(reader);
	value = (char *)xmlTextReaderConstValue(reader);
	
	if(type == NULL || value == NULL)
	{
		XML_DEBUG(("Parsing Error [%s] [%s]\n", type, value));
		ovcp_free_data(data);
		return NULL;
	}
	
	XML_DEBUG(("Data Type: %s Value: %s\n", type, value));
				
	if(strcmp(type, "int") == 0 || strcmp(type, "i4") == 0)
	{
		data->type = OVCP_XML_INT;
		data->data = malloc(sizeof(int));
		*(int *)data->data = atoi(value);
	}
	else if(strcmp(type, "boolean") == 0)
	{
		data->type = OVCP_XML_BOOLEAN;
		data->data = malloc(sizeof(int));
		
		if(strcmp(value, "1") == 0)
			*(int *)data->data = 1;
		else
			*(int *)data->data = 0;
	}
	else if(strcmp(type, "string") == 0)
	{
		data->type = OVCP_XML_STRING;
		data->data = malloc(strlen(value)+1);
		strcpy(data->data, value);
	}
	else if(strcmp(type, "double") == 0)
	{
		data->type = OVCP_XML_DOUBLE;
		data->data = malloc(sizeof(double));
		*(double *)data->data = strtod(value, NULL);
	}
	
	xmlTextReaderRead(reader);
	p = (char *) xmlTextReaderConstName(reader);
        
        
	if(p == NULL)
	{
		XML_DEBUG(("Parsing Error: Read Failed\n"));
		ovcp_free_data(data);
		return NULL;
	}
					
	if(strcmp(p, type) != 0)
	{
		XML_DEBUG(("Parsing Error: %s tag not terminated [%s]\n", type, p));
		ovcp_free_data(data);
		return NULL;					
	} 
	

	
	return data;
}
