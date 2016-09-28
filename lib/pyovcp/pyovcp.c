#include <Python.h>
#include "pyovcp.h"
#include "pyovcprequest.h"
#include "pyovcpresponse.h"
#include "pyovcpdata.h"






#ifndef SHAREDOBJ
#include "../../src/main.h"
#include "../../src/modules.h"


struct ovcp_response_st *ovcp_call_pythonmod(struct ovcp_request_st *req, void *func)
{
	OVCPResponse *respobj;
	OVCPRequest *reqobj;
	struct ovcp_response_st *resp = NULL;
	PyObject *funcargobj;
		
	reqobj = (OVCPRequest *)createPythonObject("ovcp", "Request", NULL);
	reqobj->req = req;

	funcargobj = Py_BuildValue("(O)", (PyObject *)reqobj);
	
	respobj = (OVCPResponse *) PyObject_CallObject((PyObject *)func, funcargobj);
	
	if(respobj != NULL)
	{
		if(strcmp(respobj->ob_type->tp_name, "ovcp.Response") == 0)
		{
			resp = respobj->resp;
			respobj->resp = NULL;
		}
		
		Py_DECREF(respobj);
	}
	
	if(PyErr_Occurred() != NULL)
		PyErr_Print();
	
	return resp;
}


PyObject *OVCP_addMethod(PyObject* self, PyObject *args)
{
	PyObject *func;
	char *module, *name;
	
	if(!PyArg_ParseTuple(args, "ssO", &module, &name, &func))
		return NULL;
	Py_XINCREF(func);
	
	ovcp_add_argmethod(module, name, ovcp_call_pythonmod, (void *)func);	

	Py_RETURN_TRUE;	
}


PyObject *ovcp_create_settingsdict(void)
{
	PyObject *dict = PyDict_New();
	
	PyDict_SetItem(dict, PyString_FromString("ip"), PyString_FromString(global_settings.ip));
	PyDict_SetItem(dict, PyString_FromString("port"), PyString_FromString(global_settings.port));
	PyDict_SetItem(dict, PyString_FromString("startxid"), PyString_FromString(global_settings.startxid));
	PyDict_SetItem(dict, PyString_FromString("root_dir"), PyString_FromString(global_settings.root_dir));
	PyDict_SetItem(dict, PyString_FromString("config_dir"), PyString_FromString(global_settings.config_dir));
	PyDict_SetItem(dict, PyString_FromString("backup_dir"), PyString_FromString(global_settings.userbackup_dir));
	PyDict_SetItem(dict, PyString_FromString("image_dir"), PyString_FromString(global_settings.run_dir));
	PyDict_SetItem(dict, PyString_FromString("run_dir"), PyString_FromString(global_settings.ip));
	PyDict_SetItem(dict, PyString_FromString("cache_dir"), PyString_FromString(global_settings.cache_dir));
	PyDict_SetItem(dict, PyString_FromString("ifaces"), PyString_FromString(global_settings.ifaces));
	PyDict_SetItem(dict, PyString_FromString("ifbdev"), PyString_FromString(global_settings.ifbdev));
	PyDict_SetItem(dict, PyString_FromString("adminemail"), PyString_FromString(global_settings.adminemail));	
	
	return dict;
}

#else


PyMODINIT_FUNC initovcp(void)
{
	init_pyovcp();
}


#endif


static PyMethodDef module_methods[] = {

	#ifndef SHAREDOBJ
	{"addMethod", (PyCFunction)OVCP_addMethod, METH_VARARGS, "adds a method"},
	#endif

	{NULL} /* Sentinel */
};


void init_pyovcp(void)
{
	PyObject *m;
	
	if (PyType_Ready(&OVCPRequestType) < 0)
		return;
	if (PyType_Ready(&OVCPResponseType) < 0)
		return;
	if (PyType_Ready(&OVCPDataType) < 0)
		return;
		
	m = Py_InitModule3("ovcp", module_methods,
					"python module that allows to write extensions for openvcp in python");
	if (m == NULL)
	  return;
	  
	Py_INCREF(&OVCPRequestType);
	Py_INCREF(&OVCPResponseType);
	Py_INCREF(&OVCPDataType);
			
	PyModule_AddObject(m, "Request", (PyObject *)&OVCPRequestType);
	PyModule_AddObject(m, "Response", (PyObject *)&OVCPResponseType);
	PyModule_AddObject(m, "Data", (PyObject *)&OVCPDataType);
	
	#ifndef SHAREDOBJ
	PyModule_AddObject(m, "settings", ovcp_create_settingsdict());
	#endif
}




PyObject *createPythonObject(char *module, char *attr, PyObject *args)
{
	PyObject *objdata=NULL;
	PyObject *pModule, *pFunc;
	
	pModule = PyImport_ImportModule(module);
	pFunc = PyObject_GetAttrString(pModule, attr);

	if (pFunc && PyCallable_Check(pFunc)) 
		objdata = PyObject_CallObject(pFunc, args);

	Py_XDECREF(pFunc);
	Py_XDECREF(pModule);

	return objdata;
}




