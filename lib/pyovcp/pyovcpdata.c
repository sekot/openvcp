#include <Python.h>
#include "ovcpxml.h"
#include "pyovcp.h"
#include "pyovcpdata.h"


void OVCPData_dealloc(OVCPData* self)
{

	self->ob_type->tp_free((PyObject*)self);
}


PyObject *OVCPData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	OVCPData *self;
	self = (OVCPData *)type->tp_alloc(type, 0);

	
	return (PyObject *)self;
}


int OVCPData_init(OVCPData *self, PyObject *args, PyObject *kwds)
{
	
	return 0;
}


PyObject *OVCPData_type(OVCPData *self, PyObject *args)
{
	char *type = NULL;
	
	if(ovcp_data_is_array(self->data))
		type = "array";
	else
	if(ovcp_data_is_struct(self->data))
		type = "struct";
	else
	if(ovcp_data_is_int(self->data))
		type = "int";
	else
	if(ovcp_data_is_double(self->data))
		type = "double";
	else
	if(ovcp_data_is_boolean(self->data))
		type = "boolean";
	else
	if(ovcp_data_is_string(self->data))
		type = "string";		

	if(type == NULL) Py_RETURN_NONE;
	else
	return Py_BuildValue("s", type);
}


PyObject *OVCPData_getElement(OVCPData *self, PyObject *args)
{
	int id;
	char *name;
	OVCPData *objdata;
	
	struct ovcp_data_st *data = NULL;
	
	if(ovcp_data_is_array(self->data))
	{
		if(!PyArg_ParseTuple(args, "i", &id))
			return NULL;

		data = ovcp_array_get_element(self->data, id);
	}
	else
	if(ovcp_data_is_struct(self->data))
	{
		if(!PyArg_ParseTuple(args, "s", &name))
			return NULL;
			
		data = ovcp_struct_get_element(self->data, name);
	}
	
	if(data == NULL) Py_RETURN_NONE;
	
	objdata = (OVCPData *)createPythonObject("ovcp", "Data", NULL);

	if(objdata != NULL)
		objdata->data = data;
	else Py_RETURN_NONE;

	return (PyObject *)objdata;
}


PyObject *OVCPData_getContent(OVCPData *self)
{
	PyObject *content=NULL;
	
	void *data = ovcp_data_get(self->data);
	
	if(ovcp_data_is_int(self->data))
		return PyInt_FromLong( (long)*((int *)data) );
	else
	if(ovcp_data_is_double(self->data))
		return PyFloat_FromDouble( *((double *)data) );
	else
	if(ovcp_data_is_boolean(self->data))
		return PyBool_FromLong( (long)*((int *)data) );
	else
	if(ovcp_data_is_string(self->data))
		return PyString_FromString((char *)data);

	if(content == NULL) Py_RETURN_NONE;

	return content;
}




