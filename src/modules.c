/*  OpenVCP
 *  Copyright (C) 2009 Gerrit Wyen <gerrit@t4a.net>
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

#include <Python.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "ovcpxml.h"
#include "modules.h"
#include "strings.h"
#include "misc.h"
#include "pyovcp/pyovcp.h"

struct ovcp_module *modules = 0;
static int modulescount = 0;

#define OVCP_EXTENSION_DIR OPENVCP_DATA"/extensions"

int ovcp_add_module(char *name)
{
	modulescount++;
	
	modules = (struct ovcp_module *) realloc(modules, sizeof(struct ovcp_module) * (modulescount+1));

	modules[modulescount-1].name = strdup(name);
	modules[modulescount-1].methods = NULL;
	modules[modulescount-1].methodscount = 0;
	
	memset(&modules[modulescount], 0, sizeof(struct ovcp_module));

	return 1;
}



int ovcp_add_method(char *module, char *name, struct ovcp_response_st *(*function)(struct ovcp_request_st*))
{
	return ovcp_add_argmethod(module, name, (struct ovcp_response_st *(*)(struct ovcp_request_st*, void *)) function, NULL);
}

int ovcp_add_argmethod(char *module, char *name, struct ovcp_response_st *(*function)(struct ovcp_request_st*, void *), void *usrdata)
{
	int i=0;
	
	while(modules[i].name != NULL)
	{
		if(strcmp(modules[i].name, module) == 0)
		{
			modules[i].methodscount++;
			
			modules[i].methods = (struct ovcp_method *)realloc(modules[i].methods, sizeof(struct ovcp_method)*(modules[i].methodscount+1));
			
			memset(&modules[i].methods[modules[i].methodscount-1], 0, sizeof(struct ovcp_method));
			memset(&modules[i].methods[modules[i].methodscount], 0, sizeof(struct ovcp_method));
			modules[i].methods[modules[i].methodscount-1].name = strdup(name);
			modules[i].methods[modules[i].methodscount-1].function = function;
			modules[i].methods[modules[i].methodscount-1].usrdata =  usrdata;
		
			break;
		}
		
		i++;
	}

	if(modules[i].name == NULL) return 0;
	
	ovcp_log(OVCP_INFO, "Loaded method: %s.%s", module, name);
	
	return 1;
}


int ovcp_method_sethelp(char *module, char *name, char *help, ...)
{

	int i=0, j;
	
	while(modules[i].name != NULL)
	{
		if(strcmp(modules[i].name, module) == 0)
		{
			j=0;
			
			while(modules[i].methods[j].name != NULL)
			{
				if(strcmp(modules[i].methods[j].name, name) == 0)
				{
					modules[i].methods[j].help = strdup(help);
				}
				
				j++;
			}	
		}
		
		i++;
	}	
	
	return 1;
}


int free_method(struct ovcp_method *method)
{
	if(method->name != NULL) free(method->name);
	if(method->help != NULL) free(method->help);
	
	return 1;
}

int free_module(struct ovcp_module *module)
{
	int i = 0;
	
	if(module->methods != NULL)
	{
		while(module->methods[i].name != NULL)
		{
			free_method(&module->methods[i]);
			i++;
		}
		
		free(module->methods);
	}
	
	if(module->name != NULL) free(module->name);

	return 1;	
}

int ovcp_free_modules()
{
	int i=0;
	
	while(modules[i].name != NULL)
	{
		free_module(&modules[i]);
		i++;
	}
	
	free(modules);

	/* Interpreter beenden */
	Py_Finalize();
	
	return 1;
}


int ovcp_extensions_load()
{
	DIR *dir_ptr;
	struct dirent *dir_entry;
	char path[BUF_SIZE];
	FILE * fmod;
	
	/* Interpreter initialisieren */
	Py_Initialize();
	init_pyovcp();

	
	if((dir_ptr = opendir(OVCP_EXTENSION_DIR)) != NULL)
	{

		while((dir_entry = readdir(dir_ptr)) != NULL)
		{


			if(dir_entry->d_name[0] != '.')
			{
				sconcat(path, BUF_SIZE, OVCP_EXTENSION_DIR, "/", dir_entry->d_name, NULL);

				fmod = fopen(path,"r");
				PyRun_SimpleFile(fmod, path);
				fclose(fmod);
			}

		}


		closedir(dir_ptr);
	}
	
	return 1;
}



