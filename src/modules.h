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
 
#ifndef MODULES_H
	#define MODULES_H 

	
	struct ovcp_module
	{
		char *name;
		struct ovcp_method *methods;
		int methodscount;
	};
	
	struct ovcp_method
	{
		char *name;
		char **signature;
	        char *help;
		struct ovcp_response_st *(*function)(struct ovcp_request_st*, void*);	
		void *usrdata;
	};
	
	
	
	extern struct ovcp_module *modules;
	
	int ovcp_add_module(char *name);
	int ovcp_add_method(char *module, char *name, struct ovcp_response_st *(*function)(struct ovcp_request_st*));
	int ovcp_add_argmethod(char *module, char *name, struct ovcp_response_st *(*function)(struct ovcp_request_st*, void *), void *usrdata);
	int ovcp_method_sethelp(char *module, char *name, char *help, ...);
	int ovcp_extensions_load(void);
	int ovcp_free_modules(void);
#endif
