#ifndef PYOVCPRESPONSE
	#define PYOVCPRESPONSE
	
	
	typedef struct 
	{
		PyObject_HEAD
		struct ovcp_response_st *resp;
	} OVCPResponse;
	
	
	void OVCPResponse_dealloc(OVCPResponse *self);
	PyObject *OVCPResponse_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	int OVCPResponse_init(OVCPResponse *self, PyObject *args, PyObject *kwds);
	PyObject *OVCPResponse_ArrayNew(OVCPResponse* self);
	PyObject *OVCPResponse_ArrayAddString(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_ArrayAddInt(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_ArrayEnd(OVCPResponse* self);
	PyObject *OVCPResponse_StructNew(OVCPResponse* self);
	PyObject *OVCPResponse_StructEnd(OVCPResponse* self);
	PyObject *OVCPResponse_StructAddString(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_StructAddInt(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_StructAddDouble(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_StructAddBool(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_addString(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_addInt(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_addDouble(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_addBool(OVCPResponse* self, PyObject *args);
	PyObject *OVCPResponse_Error(OVCPResponse* self, PyObject *args);
	
	/*
	static PyMemberDef OVCPResponse_members[] = 
	{
		{"first", T_OBJECT_EX, offsetof(OVCPResponse, first), 0, "first name"},
		{"number", T_INT, offsetof(OVCPResponse, number), 0, "noddy number"},
		{NULL} 
	};
	*/


	static PyMethodDef OVCPResponse_methods[] = {
		{"ArrayNew", (PyCFunction)OVCPResponse_ArrayNew, METH_NOARGS, "creates a new array"},
		{"ArrayAddString", (PyCFunction)OVCPResponse_ArrayAddString, METH_VARARGS, "adds a string to the array"},
		{"ArrayAddInt", (PyCFunction)OVCPResponse_ArrayAddInt, METH_VARARGS, "adds an int to the array"},
		{"ArrayEnd", (PyCFunction)OVCPResponse_ArrayEnd, METH_NOARGS, "ends an array"},
		{"StructNew", (PyCFunction)OVCPResponse_StructNew, METH_NOARGS, "creates a new structure"},
		{"StructEnd", (PyCFunction)OVCPResponse_StructEnd, METH_NOARGS, "ends a structure"},
		{"StructAddString", (PyCFunction)OVCPResponse_StructAddString, METH_VARARGS, "adds a string to the structure"},
		{"StructAddInt", (PyCFunction)OVCPResponse_StructAddInt, METH_VARARGS, "adds an int to the structure"},	
		{"StructAddDouble", (PyCFunction)OVCPResponse_StructAddDouble, METH_VARARGS, "adds a double to the structure"},	
		{"StructAddBool", (PyCFunction)OVCPResponse_StructAddBool, METH_VARARGS, "adds a bool to the structure"},	
		{"addString", (PyCFunction)OVCPResponse_addString, METH_VARARGS, "adds a string"},
		{"addInt", (PyCFunction)OVCPResponse_addInt, METH_VARARGS, "adds an int"},
		{"addDouble", (PyCFunction)OVCPResponse_addDouble, METH_VARARGS, "adds a double"},
		{"addBool", (PyCFunction)OVCPResponse_addBool, METH_VARARGS, "adds a bool"},
		{"Error", (PyCFunction)OVCPResponse_Error, METH_VARARGS, "creates an error response"},
		{NULL} /* Sentinel */
	};


	static PyTypeObject OVCPResponseType = 
	{
		PyObject_HEAD_INIT(NULL)
		0,						/* ob_size */
		"ovcp.Response",					/* tp_name */
		sizeof(OVCPResponse),					/* tp_basicsize */
		0,						/* tp_itemsize */
		(destructor)OVCPResponse_dealloc,			/* tp_dealloc */
		0,						/* tp_print */
		0,						/* tp_getattr */
		0,						/* tp_setattr */
		0,						/* tp_compare */
		0,						/* tp_repr */
		0,						/* tp_as_number */
		0,						/* tp_as_sequence */
		0,						/* tp_as_mapping */
		0,						/* tp_hash */
		0,						/* tp_call */
		0,						/* tp_str */
		0,						/* tp_getattro */
		0,						/* tp_setattro */
		0,						/* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* tp_flags */
		"OVCPResponse objects",		 		/* tp_doc */
		0,						/* tp_traverse */
		0,						/* tp_clear */
		0,						/* tp_richcompare */
		0,						/* tp_weaklistoffset */
		0,						/* tp_iter */
		0,						/* tp_iternext */
		OVCPResponse_methods,					/* tp_methods */
		NULL, //OVCPResponse_members,				/* tp_members */
		0,						/* tp_getset */
		0,						/* tp_base */
		0,						/* tp_dict */
		0,						/* tp_descr_get */
		0,						/* tp_descr_set */
		0,						/* tp_dictoffset */
		(initproc)OVCPResponse_init,				/* tp_init */
		0,						/* tp_alloc */
		OVCPResponse_new,					/* tp_new */
	};
	
	

	
#endif
