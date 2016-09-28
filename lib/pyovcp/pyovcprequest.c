#include <Python.h>
#include "ovcpxml.h"
#include "pyovcp.h"
#include "pyovcprequest.h"
#include "pyovcpdata.h"


void OVCPRequest_dealloc(OVCPRequest* self)
{

	self->ob_type->tp_free((PyObject*)self);
}


PyObject *OVCPRequest_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	OVCPRequest *self;
	self = (OVCPRequest *)type->tp_alloc(type, 0);
	

	return (PyObject *)self;
}


int OVCPRequest_init(OVCPRequest *self, PyObject *args)
{
	char *xmlstr = NULL;	
	
	if(!PyArg_ParseTuple(args, "|s", &xmlstr))
		return -1;
	
	if(xmlstr != NULL)
		self->req = ovcp_parse_request(xmlstr);
	
	return 0;
}


PyObject *OVCPRequest_getArg(OVCPRequest *self, PyObject *args)
{
	int id;
	struct ovcp_data_st *data;
	OVCPData *objdata=NULL;

	
		
	if(!PyArg_ParseTuple(args, "i", &id))
		return NULL;
		
	data = ovcp_request_get_arg(self->req, id);
	
	if(data == NULL)
		Py_RETURN_NONE;
	
	objdata = (OVCPData *)createPythonObject("ovcp", "Data", NULL);

	if(objdata != NULL)
		objdata->data = data;
	else Py_RETURN_NONE;
	
		
	return (PyObject *)objdata;

}




