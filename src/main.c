/*  OpenVCP
 *  Copyright (C) 2006 Gerrit Wyen <gerrit@t4a.net>
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sqlite3.h>



#include "main.h"
#include "fs.h"
#include "strings.h"
#include "misc.h"
#include "ovcpxml.h"
#include "error.h"
#include "request.h"
#include "modules.h"

struct config_st global_settings;
int kernel_vci_version;
sqlite3 *ovcp_db;




static struct ovcp_session *settle_transmission(int client_socket);
static int authenticate_client(struct ovcp_session *session);
static int handle_request(struct ovcp_session *session);
static void cleanup_socket(int args, void *socket);
static void usage(char *name);
static void signal_handle(int signal);
static void term_handle(void);

#ifdef HAVE_LIBGNUTLS
static gnutls_anon_server_credentials_t anoncred;
static gnutls_dh_params_t dh_params;
#endif


int main(int argc, char **argv)
{

        pid_t daemon_pid = 0;
        char *pid_string;
        unsigned int ret, size;
        unsigned int option;
        int verbose = 0;

        int server_socket, client_socket;
        struct sockaddr_in server_addr, client_addr;
	
        struct ovcp_session *session;



        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(server_addr));

        while ((option = getopt(argc, argv, "v")) != -1)
        {
                switch(option)
                {
                        case 'v': 
                                verbose = 1;
                                break;
                        case 'h':
                                usage(argv[0]);
                                exit(-1);
                                break;
			default:
                                usage(argv[0]);
                                exit(-1);	
                }

        }
        
	
        parse_config(OPENVCP_CONFIG, &global_settings);
                
        ovcp_log_open(global_settings.logfile, global_settings.loglevel, verbose);
 
        ovcp_log(OVCP_INFO, "OpenVCP Daemon %s r%s started", PACKAGE_VERSION, REVISION);
        
                	
	ovcp_log(OVCP_DEBUG, "\nOptions:\n\nIP Address: %s\nPort Number: %s\nLogfile: %s\nLogfile: %d\nStartXID: %s\nVServer Root: %s\nVServer Configs: %s\n"
			"VServer Backups: %s\nVServer Images: %s\nVServer Run Dir: %s\nVserver Cache Dir: %s\nIfaces: %s\nIfbDev: %s\nAdminEmail: %s\n\n",
		 global_settings.ip, global_settings.port, global_settings.logfile, global_settings.loglevel, global_settings.startxid, global_settings.root_dir, global_settings.config_dir,
		 global_settings.backup_dir, global_settings.image_dir, global_settings.run_dir, global_settings.cache_dir, global_settings.ifaces, global_settings.ifbdev, global_settings.adminemail);

        
        
	kernel_vci_version = vci_version();
	ovcp_log(OVCP_DEBUG, "Linux-VServer VCI Version: %x", kernel_vci_version);	

	if(!fs_cgroup_support())
        {
            if(!dir_exist(global_settings.cgroupdir))
                mkdir(global_settings.cgroupdir, 0755);
            execb_cmd("mount", "mount", global_settings.cgroupmountoptions, "-t", "cgroup", "none", global_settings.cgroupdir, NULL);
        }

        signal(SIGCHLD, SIG_IGN);
	
	
	if(fork() == 0)
	{
		traffic_daemon_processe();
		exit(0);
	}
        
        
        
        if(verbose != 1)
        {
                if((daemon_pid = fork()) == -1)
                {
                        ovcp_log(OVCP_ERROR, "Could not fork()");
                        exit(-1);
                }
        }


        if(daemon_pid == 0)
        {
        
        	signal(SIGINT, signal_handle);
        	signal(SIGSEGV, signal_handle);
        	atexit(term_handle);
        	init_modules();

        	#ifdef HAVE_LIBGNUTLS

                        gnutls_global_init ();

                        #ifdef DEBUG
                        	       gnutls_global_set_log_level(6);
                                       gnutls_global_set_log_function(&gnutls_log);
                        #endif
        
                        gnutls_anon_allocate_server_credentials (&anoncred);

                        gnutls_dh_params_init (&dh_params);
                        gnutls_dh_params_generate2 (dh_params, 1024);

                        gnutls_anon_set_server_dh_params (anoncred, dh_params);

                #endif

                server_socket = socket(PF_INET, SOCK_STREAM, 0);
                server_addr.sin_addr.s_addr = inet_addr(global_settings.ip);
                server_addr.sin_port = htons(atoi(global_settings.port));


                fcntl(server_socket, F_SETFD, 1);

                option = 1;
                setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


                ret = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

                if(ret != 0)
                {
                        ovcp_log(OVCP_ERROR, "Could not bind to Port (%s)", strerror(errno));
                        close(server_socket);
                        exit(1);
                }
                


                signal(SIGCHLD, SIG_IGN);
                on_exit(cleanup_socket, &server_socket);


                listen(server_socket, 5);

                size = sizeof(client_addr);


                while(1)
                {

                        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &size);
                        fcntl(client_socket, F_SETFD, 1);

			if(client_socket == -1)
				continue;
			
                        ovcp_log(OVCP_INFO, "Connection from: %s", inet_ntoa(client_addr.sin_addr));

			if((ret = fork()) == 0)
			{
				session = settle_transmission(client_socket);
				
				if(session != NULL)
				{
					session->client_addr = client_addr;
					if(authenticate_client(session) == 1)
					{
						ovcp_log(OVCP_INFO, "%s logged in successfully (tls=%d)", inet_ntoa(session->client_addr.sin_addr), session->tls);
						handle_request(session);
					}
				}
				
				ovcp_close(session);	
				
				exit(0);
			}
			else if(ret == -1)
			{
				ovcp_log(OVCP_ERROR, "Unable to fork()");		
				break;	
			}
			
                        close(client_socket);
                        

                }

                close(server_socket);

                #ifdef HAVE_LIBGNUTLS
                        gnutls_anon_free_server_credentials (anoncred);
                        gnutls_global_deinit ();
                #endif

        }
	else
	{
		pid_string = dec2str(daemon_pid);
		string_append(pid_string, "\n");	

		file_write(global_settings.pid_file, pid_string);

		free(pid_string);
	}


        return 0;
}

static struct ovcp_session *settle_transmission(int client_socket)
{
	struct ovcp_session *session;
	char *cmd, **arguments;
	unsigned int args;
	
	session = (struct ovcp_session *) malloc(sizeof(struct ovcp_session));
	memset(session, 0, sizeof(struct ovcp_session));
	
	session->session_socket = client_socket;

        cmd = ovcp_readline(session);


	if(cmd == NULL)
	{
		free(session);
		return NULL;
	}	
	
	arguments = split_string(cmd, ' ', &args);
	free(cmd);

	if(arguments == NULL || args < 1)
	{
		free_strings(arguments, args);
		free(session);
		return NULL;
	}
	
	if(strcmp(arguments[0], "PLAINTEXT") == 0)
	{
		ovcp_write(session, "OK\n", 4);                                            
	}
	else
	if(strcmp(arguments[0], "TLS") == 0)
	{
		#ifdef HAVE_LIBGNUTLS
		int ret;
		
		ovcp_write(session, "OK\n", 4);
	
		session->tls_session = tls_session_new (client_socket, anoncred);
		ret = gnutls_handshake(*session->tls_session);
			
		ovcp_log(OVCP_DEBUG, "Handshake complete (%d)", ret);
		session->tls = 1;
		#else
		ovcp_write(session, "Not Supported\n", 15);
		free(session);
		session = NULL;
		#endif 
	}
	else 
	{
		ovcp_write(session, "Not Supported\n", 15);
		free(session);
		session = NULL;
	}


	free_strings(arguments, args);

	return session;
}

static int authenticate_client(struct ovcp_session *session)
{
	char *cmd, **arguments;
	unsigned int args;
	
	cmd = ovcp_readline(session);

	if(cmd == NULL)
		return -1;
        
	arguments = split_string(cmd, ' ', &args);
	free(cmd);

	if(arguments == NULL || args < 2)
		return -1;
                	
	if(strcmp(arguments[0], "admin") == 0 
				&& strcmp(arguments[1], global_settings.password) == 0)
	{
		ovcp_write(session, "OK\n", 3);
		free_strings(arguments, args);
	}
	else
	{
		sleep(3);
		ovcp_write(session, "Failed\n", 7);
		free_strings(arguments, args);
		
		return -1;
	}



	return 1;
}

static int handle_request(struct ovcp_session *session)
{

	struct ovcp_request_st *request;
	struct ovcp_response_st *response; 
        char xml_request[BUF_SIZE];
        char *xml_response;
        char *modul, *method;
        int res, i, d=0;
        
        
        fd_set client_fdset;
        struct timeval timeout;

	
	res = sqlite3_open(OPENVCP_DATABASE, &ovcp_db);
	if(res != 0)
	{
		ovcp_log(OVCP_ERROR, "Can't open database: (%s)", sqlite3_errmsg(ovcp_db));
		sqlite3_close(ovcp_db);
		exit(-1);
	}        

        while(1)
        {
                FD_ZERO(&client_fdset);
                FD_SET(session->session_socket, &client_fdset);

                timeout.tv_sec = 4;
                timeout.tv_usec = 50000000;

                res = select(session->session_socket+1, &client_fdset, NULL, NULL, &timeout);

                response = NULL;
		xml_request[0] = 0;
		

                if(res != 0 && res != -1)
                {

                        res = ovcp_read(session, xml_request, BUF_SIZE);
			xml_request[res] = 0;
			
                        ovcp_log(OVCP_DEBUG, "OVCP Request: [%s]", xml_request);
			
			request = ovcp_parse_request(xml_request);

                        if(request != NULL)
                        {
               
				

				modul = strtok(request->methodname, ".");
				method = strtok(NULL, ".");
			
				i = 0;
				response = NULL;
				
				if(modul != NULL && method != NULL)
				{
				
					while(modules[i].name != NULL)
					{
						if(strcmp(modules[i].name, modul) == 0)
						{
							d = 0;
						
							while(modules[i].methods[d].name != NULL)
							{
							
								if(strcmp(modules[i].methods[d].name, method) == 0)
								{
									response = modules[i].methods[d].function(request, modules[i].methods[d].usrdata);
									break;
								}
								d++;
							}			
					
							break;
						}
				
						i++;
					}
				}

				ovcp_free_request(request);

				if(response == NULL)
				{
					if(modules[i].name == NULL)
					{
						response = ovcp_response_error(OVCP_ERROR_MODNOSUP, OVCP_ERROR_MODNOSUP_DESC);
					}
					else
					if(modules[i].methods[d].name == NULL)
					{
						response = ovcp_response_error(OVCP_ERROR_METHNOSUP, OVCP_ERROR_METHNOSUP_DESC);
					}
					else
					{
						response = ovcp_response_error(OVCP_ERROR_INTERNAL, OVCP_ERROR_INTERNAL_DESC);
					}
				}

				
				xml_response = ovcp_response_finalize(response);
				
                                if(xml_response != NULL)
                                {

                                        FD_ZERO(&client_fdset);
                                        FD_SET(session->session_socket, &client_fdset);

                                        timeout.tv_sec = 4;
                                        timeout.tv_usec = 5000000;

                                        res = select(session->session_socket+1, NULL, &client_fdset, NULL, &timeout);

                                        if(res != 0 && res != -1)
                                        {
                                        	ovcp_log(OVCP_DEBUG, "OVCP Response: [%s]", xml_response);
                                        	
                                                ovcp_write(session, xml_response, strlen(xml_response));
					}
					else
					{
						free(xml_response);
						break;
					}


                                        free(xml_response);
                                }
                                else
				{
                                        ovcp_write(session, "INTERNAL ERROR\n", 15);
                                }

                        }
                        else
                        {
                                ovcp_write(session, "Couldn't parse request\n", 24);                     
                                break;
                        }
                }
                else
                {
			break; 
                }

        }
        
	sqlite3_close(ovcp_db);	
       
	return 0; 
}



static void usage(char *name)
{
        printf("Usage: %s [OPTION]\n\n-v\t\tbe verbose, log to stderr\n-h\t\tdisplay this help\n\n", name);
}

static void cleanup_socket(int args, void *socket_ptr)
{
        int *socket = (int *)socket_ptr;

        close(*socket);

}

static void term_handle()
{
	ovcp_free_modules();
	ovcp_log(OVCP_INFO, "Process [%d] terminating", getpid());
}

static void signal_handle(int signal)
{
	if(signal == SIGINT)
	{
		ovcp_log(OVCP_INFO, "Process [%d] received signal 'Interrupt'", getpid());
	}
	if(signal == SIGSEGV)
	{
		ovcp_log(OVCP_ERROR, "Process [%d] received signal 'Segmentation fault'", getpid());
		exit(-1);
	}

	
	exit(0);		
}
