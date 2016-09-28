#include <Python.h>
#include "ovcpxml.h"
#include "pyovcpresponse.h"


void OVCPResponse_dealloc(OVCPResponse* self)
{

	if(self->resp != NULL)
		ovcp_free_response(self->resp);
	
	self->ob_type->tp_free((PyObject*)self);
}


PyObject *OVCPResponse_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	OVCPResponse *self;
	self = (OVCPResponse *)type->tp_alloc(type, 0);
	
	if (self != NULL)
	{
		self->resp = ovcp_response_new();
		if(self->resp == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}
	}
	
	return (PyObject *)self;
}


int OVCPResponse_init(OVCPResponse *self, PyObject *args, PyObject *kwds)
{

	return 0;
}




PyObject *OVCPResponse_ArrayNew(OVCPResponse* self)
{
	int res;
	
	res = ovcp_response_array_new(self->resp);
	
	return PyInt_FromLong(res);
}


PyObject *OVCPResponse_ArrayAddString(OVCPResponse* self, PyObject *args)
{
	int res;
	char *str;
	
	if(!PyArg_ParseTuple(args, "s", &str))
		return NULL;
		
	res = ovcp_response_array_add_string(self->resp, str);
	
	
	
	return PyInt_FromLong(res);	
}
	
	
PyObject *OVCPResponse_ArrayAddInt(OVCPResponse* self, PyObject *args)
{
	int res, num;
	
	if(!PyArg_ParseTuple(args, "i", &num))
		return NULL;
		
	res = ovcp_response_array_add_int(self->resp, num);
	
	
	
	return PyInt_FromLong(res);	
}

PyObject *OVCPResponse_ArrayEnd(OVCPResponse* self)
{
	int res;
	
	res = ovcp_response_array_end(self->resp);
	
	return PyInt_FromLong(res);
}
	
	
PyObject *OVCPResponse_StructNew(OVCPResponse* self)
{
	int res;
	
	res = ovcp_response_struct_new(self->resp);
	
	return PyInt_FromLong(res);
}
		
PyObject *OVCPResponse_StructEnd(OVCPResponse* self)
{
	int res;
	
	res = ovcp_response_struct_end(self->resp);
	
	return PyInt_FromLong(res);
}

PyObject *OVCPResponse_StructAddString(OVCPResponse* self, PyObject *args)
{
	int res;
	char *name, *str;
	
	if(!PyArg_ParseTuple(args, "ss", &name, &str))
		return NULL;
		
	res = ovcp_response_struct_add_string(self->resp, name, str);
	
	
	
	return PyInt_FromLong(res);	
}

PyObject *OVCPResponse_StructAddInt(OVCPResponse* self, PyObject *args)
{
	int res;
	char *name;
	int num;
	
	if(!PyArg_ParseTuple(args, "si", &name, &num))
		return NULL;
		
	res = ovcp_response_struct_add_int(self->resp, name, num);
	
	
	
	return PyInt_FromLong(res);	
}

PyObject *OVCPResponse_StructAddDouble(OVCPResponse* self, PyObject *args)
{
	int res;
	char *name;
	double num;
	
	if(!PyArg_ParseTuple(args, "sd", &name, &num))
		return NULL;
		
	res = ovcp_response_struct_add_double(self->resp, name, num);
	
	
	
	return PyInt_FromLong(res);	
}

PyObject *OVCPResponse_StructAddBool(OVCPResponse* self, PyObject *args)
{
	int res;
	char *name;
	int num;
	
	if(!PyArg_ParseTuple(args, "si", &name, &num))
		return NULL;
		
	res = ovcp_response_struct_add_boolean(self->resp, name, num);
	
	
	
	return PyInt_FromLong(res);	
}

PyObject *OVCPResponse_addString(OVCPResponse* self, PyObject *args)
{
	int res;
	char *str;
	
	if(!PyArg_ParseTuple(args, "s", &str))
		return NULL;
		
	res = ovcp_response_add_string(self->resp, str);
	
	return PyInt_FromLong(res);
}

PyObject *OVCPResponse_addInt(OVCPResponse* self, PyObject *args)
{
	int res, num;
	
	if(!PyArg_ParseTuple(args, "i", &num))
		return NULL;
		
	res = ovcp_response_add_int(self->resp, num);
	
	return PyInt_FromLong(res);
}

PyObject *OVCPResponse_addDouble(OVCPResponse* self, PyObject *args)
{
	int res;
	double num;
	
	if(!PyArg_ParseTuple(args, "d", &num))
		return NULL;
		
	res = ovcp_response_add_double(self->resp, num);
	
	return PyInt_FromLong(res);
}

PyObject *OVCPResponse_addBool(OVCPResponse* self, PyObject *args)
{
	int res, num;
	
	if(!PyArg_ParseTuple(args, "i", &num))
		return NULL;
		
	res = ovcp_response_add_boolean(self->resp, num);
	
	return PyInt_FromLong(res);
}

PyObject *OVCPResponse_Error(OVCPResponse* self, PyObject *args)
{
	int code;
	char *desc;

	
	if(!PyArg_ParseTuple(args, "is", &code, &desc))
		return NULL;
	
	if(self->resp != NULL)
		ovcp_free_response(self->resp);
			
	self->resp = ovcp_response_error(code, desc);

	return PyInt_FromLong(1);
}


	




