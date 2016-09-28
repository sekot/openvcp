#ifndef PYOVCP_H
	#define PYOVCP_H
	
	
	PyObject *createPythonObject(char *module, char *attr, PyObject *args);
	void init_pyovcp(void);
	
#endif
