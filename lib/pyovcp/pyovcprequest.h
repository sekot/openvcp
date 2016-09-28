#ifndef PYOVCPREQUEST
	#define PYOVCPREQUEST
	
	
	typedef struct 
	{
		PyObject_HEAD
		struct ovcp_request_st *req;
		int number;
	} OVCPRequest;
	
	
	void OVCPRequest_dealloc(OVCPRequest *self);
	PyObject *OVCPRequest_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	int OVCPRequest_init(OVCPRequest *self, PyObject *args);
	PyObject *OVCPRequest_name(OVCPRequest* self);
	PyObject *OVCPRequest_getArg(OVCPRequest *self, PyObject *args);
	
	
	/*
	static PyMemberDef OVCPRequest_members[] = 
	{
		{"first", T_OBJECT_EX, offsetof(OVCPRequest, first), 0, "first name"},
		{"last", T_OBJECT_EX, offsetof(OVCPRequest, last), 0,	 "last name"},
		{"number", T_INT, offsetof(OVCPRequest, number), 0, "noddy number"},
		{NULL} 
	};
	*/


	static PyMethodDef OVCPRequest_methods[] = {
		{"getArg", (PyCFunction)OVCPRequest_getArg, METH_VARARGS, "Returns the argument at position n"},
		{NULL} /* Sentinel */
	};


	static PyTypeObject OVCPRequestType = 
	{
		PyObject_HEAD_INIT(NULL)
		0,						/* ob_size */
		"ovcp.Request",					/* tp_name */
		sizeof(OVCPRequest),					/* tp_basicsize */
		0,						/* tp_itemsize */
		(destructor)OVCPRequest_dealloc,			/* tp_dealloc */
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
		"Request objects",		 		/* tp_doc */
		0,						/* tp_traverse */
		0,						/* tp_clear */
		0,						/* tp_richcompare */
		0,						/* tp_weaklistoffset */
		0,						/* tp_iter */
		0,						/* tp_iternext */
		OVCPRequest_methods,					/* tp_methods */
		NULL, //OVCPRequest_members,				/* tp_members */
		0,						/* tp_getset */
		0,						/* tp_base */
		0,						/* tp_dict */
		0,						/* tp_descr_get */
		0,						/* tp_descr_set */
		0,						/* tp_dictoffset */
		(initproc)OVCPRequest_init,				/* tp_init */
		0,						/* tp_alloc */
		OVCPRequest_new,					/* tp_new */
	};
	
	

	
#endif
