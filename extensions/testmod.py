import ovcp

def testmethod(req):
	resp = ovcp.Response()
	dat=req.getArg(1)
	
	if dat == None:
		resp.Error(1, "missing parameter")
		return resp;
		
	if dat.type() != "string":
		resp.Error(2, "wrong parameter")
		return resp;
			
	resp.addString(dat.getContent())
	
	rootdir = ovcp.settings['root_dir']
	confdir = ovcp.settings['config_dir']
	
	resp.addInt(22)
	resp.addString("hello world")
	resp.addDouble(33.223)	
	resp.addString(rootdir)
	resp.addString(confdir)
	resp.addBool(1)	
			
	resp.ArrayNew()
	resp.ArrayAddString("hi")
	resp.ArrayAddInt(32)
	resp.ArrayEnd()
	
	resp.StructNew()
	resp.StructAddBool("test", 1)
	resp.StructAddString("test2", "works")
	resp.StructAddDouble("test3", 22.44)
	resp.StructAddInt("test4", 23)
	resp.StructEnd()

	
	return resp




ovcp.addMethod("vserver", "pytest", testmethod)


