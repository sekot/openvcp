#ifndef PYOVCPDATA
	#define PYOVCPDATA
	
	
	typedef struct 
	{
		PyObject_HEAD
		struct ovcp_data_st *data;
	} OVCPData;
	
	
	void OVCPData_dealloc(OVCPData *self);
	PyObject *OVCPData_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	int OVCPData_init(OVCPData *self, PyObject *args, PyObject *kwds);
	PyObject *OVCPData_type(OVCPData *self, PyObject *args);
	PyObject *OVCPData_getContent(OVCPData *self);
	PyObject *OVCPData_getElement(OVCPData *self, PyObject *args);
	
	/*
	static PyMemberDef OVCPData_members[] = 
	{
		{"first", T_OBJECT_EX, offsetof(OVCPData, first), 0, "first name"},
		{"number", T_INT, offsetof(OVCPData, number), 0, "noddy number"},
		{NULL} 
	};
	*/


	static PyMethodDef OVCPData_methods[] = {
		{"type", (PyCFunction)OVCPData_type, METH_VARARGS, "returns the type"},
		{"getElement", (PyCFunction)OVCPData_getElement, METH_VARARGS, "returns an element of an array or a structure"},
		{"getContent", (PyCFunction)OVCPData_getContent, METH_NOARGS, "returns the content"},
		{NULL} /* Sentinel */
	};


	static PyTypeObject OVCPDataType = 
	{
		PyObject_HEAD_INIT(NULL)
		0,						/* ob_size */
		"ovcp.Data",					/* tp_name */
		sizeof(OVCPData),					/* tp_basicsize */
		0,						/* tp_itemsize */
		(destructor)OVCPData_dealloc,			/* tp_dealloc */
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
		"OVCPData objects",		 		/* tp_doc */
		0,						/* tp_traverse */
		0,						/* tp_clear */
		0,						/* tp_richcompare */
		0,						/* tp_weaklistoffset */
		0,						/* tp_iter */
		0,						/* tp_iternext */
		OVCPData_methods,					/* tp_methods */
		NULL, //OVCPData_members,				/* tp_members */
		0,						/* tp_getset */
		0,						/* tp_base */
		0,						/* tp_dict */
		0,						/* tp_descr_get */
		0,						/* tp_descr_set */
		0,						/* tp_dictoffset */
		(initproc)OVCPData_init,				/* tp_init */
		0,						/* tp_alloc */
		OVCPData_new,					/* tp_new */
	};
	
	

	
#endif
