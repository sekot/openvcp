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
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/wait.h>

#include "main.h"
#include "config.h"
#include "misc.h"
#include "fs.h"
#include "strings.h"
#include "ovcpxml.h"

int ovcp_write(struct ovcp_session *session, char *data, int len)
{
        int ret=-1;


	#ifdef HAVE_LIBGNUTLS
	int transfered=0;

	if(session->tls_session != NULL)
	{

	 	while(transfered < len)
		{
			ret = gnutls_record_send (*(session->tls_session), data+transfered, len-transfered);

			if(ret < 0) break;
			else transfered += ret;
		}
	}
        else
	#endif
		ret = write(session->session_socket, data, len);

	return ret;
}

int ovcp_read(struct ovcp_session *session, char *data, int len)
{
        int ret;

	#ifdef HAVE_LIBGNUTLS
	if(session->tls_session != NULL)
	{
		ret = gnutls_record_recv (*(session->tls_session), data, len);
	}
	else
	#endif
                ret = read(session->session_socket, data, len);

	return ret;
}

char *ovcp_readline(struct ovcp_session *session)
{
	int i = 0;
	int ret;
	char c = 0;
	char *buffer = NULL;

	while(i < 15000)
	{
                #ifdef HAVE_LIBGNUTLS
		if(session->tls_session != NULL)
                      ret = gnutls_record_recv(*(session->tls_session), &c, 1);
                else
                #endif
		      ret = read(session->session_socket, &c, 1);

		if(ret != 1 || c == '\n' || c == '\r')
			break;

		buffer = realloc(buffer, i+2);
		buffer[i] = c;

		i++;
	}

	if(buffer != NULL)
		buffer[i] = 0;

	return buffer;
}

int ovcp_close(struct ovcp_session *session)
{

  	if(session != NULL)
	{

		#ifdef HAVE_LIBGNUTLS

		if(session->tls_session != NULL)
		{
		        gnutls_bye (*(session->tls_session), GNUTLS_SHUT_WR);
		        gnutls_deinit (*(session->tls_session));
		        free(session->tls_session);
		}

		#endif

        	close(session->session_socket);
        	free(session);
        }

        return 0;
}




#ifdef HAVE_LIBGNUTLS

gnutls_session_t *tls_session_new (int client_socket, gnutls_anon_server_credentials_t anoncred)
{
        gnutls_session_t *session;
        const int kx_prio[] = { GNUTLS_KX_ANON_DH, 0 };

        session = (gnutls_session_t *)malloc(sizeof(gnutls_session_t));

        gnutls_init (session, GNUTLS_SERVER);

        gnutls_set_default_priority (*session);
        gnutls_kx_set_priority (*session, kx_prio);

        gnutls_credentials_set (*session, GNUTLS_CRD_ANON, anoncred);

        gnutls_dh_set_prime_bits (*session, 1024);

        gnutls_transport_set_ptr (*session, (gnutls_transport_ptr_t) client_socket);

        return session;
}

void gnutls_log(int level, const char *msg)
{

	 printf("[GNUTLS] [%d] [%s]\n", level, msg);

}

#endif






int vserver_xid(char *name)
{
        char *path, *context_str;
        int xid = 0;

        path = concat(global_settings.config_dir, "/", name, "/context", NULL);

        context_str = file_read(path);

        if(context_str != NULL)
        {
                trim_string(context_str);
                xid = atoi(context_str);

                free(context_str);
        }

        free(path);

        return xid;
}

char *vserver_name(int xid)
{
	DIR *vconfigdir;
	char path[BUF_SIZE];
	struct dirent *vsentry;
	char *xidstr, *name = NULL;

	vconfigdir = opendir(global_settings.config_dir);

        while((vsentry = readdir(vconfigdir)) != NULL)
	{
		if(vsentry->d_name[0] == '.')
			continue;

		sconcat(path, BUF_SIZE, global_settings.config_dir, "/", vsentry->d_name, "/context", NULL);
		xidstr = file_read(path);

		if(xidstr == NULL)
			continue;

		if(xid == atoi(xidstr))
		{
			free(xidstr);
			name = strdup(vsentry->d_name);
			break;
		}

		free(xidstr);
        }

        closedir(vconfigdir);

        return name;
}


int vserver_isrunning(char *name)
{
	char *runfilelink;
	char runfile[BUF_SIZE];
	int len;

	runfilelink = concat(global_settings.config_dir, "/", name, "/run", NULL);

	runfile[0] = 0;
	if ((len = readlink(runfilelink, runfile, sizeof(runfile)-1)) != -1)
		runfile[len] = 0;

	free(runfilelink);


	if(file_exist(runfile))
		return 1;

	return 0;
}

int vserver_exist(char *name)
{

        char *path;

        path = concat(global_settings.config_dir, "/", name, NULL);

        DIR *dirst = opendir(path);

        free(path);

        if(dirst != NULL)
        {
                closedir(dirst);
                return 1;
        }


        return 0;

}
int vserver_lock(char *name)
{
        char *path;

        path = concat(global_settings.config_dir, "/", name, "/locked", NULL);

        if(!file_exist(path))
        {
                file_touch(path);
                free(path);
        }
        else
        {
                free(path);
                return -1;
        }

        return 1;
}


int vserver_unlock(char *name)
{
        char *path;

        path = concat(global_settings.config_dir, "/", name, "/locked", NULL);

        unlink(path);
        free(path);

        return 1;
}


char *exec_cmd(char *cmd, int exectime, ...)
{
        char *output;

        char **argv = NULL, *next_arg;
        va_list ap;
        int i = 0;

        va_start(ap, exectime);
		#ifdef DEBUG
			printf("CMD: ");
		#endif

        while((next_arg = va_arg(ap, char *)) != NULL)
        {
                argv = (char **)realloc(argv, sizeof(char **) * (i+2));

                argv[i] = next_arg;
				#ifdef DEBUG
					printf("%s ", next_arg);
				#endif
                i++;
        }
		#ifdef DEBUG
			printf("\n");
		#endif
        argv[i] = NULL;

        va_end(ap);

        output = exec_cmdv(cmd, exectime, argv);

        free(argv);

        return output;
}

int execb(char *cmd, char **argv)
{
	int pid;
	int status = 0;
	int fds[2];

	if((pid=fork()) == 0)
	{
		#ifndef DEBUG
		if(pipe(fds) == -1) return -1;
		close(1);
		close(2);
		if(dup(fds[1]) == -1) return -1;
		if(dup(fds[1]) == -1) return -1;
		close(fds[1]);
		#endif

		execvp(cmd, argv);
		exit(-1);
	}

	waitpid(pid, &status, 0);


	return status;
}


int execb_cmd(char *cmd, ...)
{

        char **argv = NULL, *next_arg;
        va_list ap;
        int i = 0;
	int res;

        va_start(ap, cmd);
	#ifdef DEBUG
		printf("CMD: ");
	#endif

        while((next_arg = va_arg(ap, char *)) != NULL)
        {
                argv = (char **)realloc(argv, sizeof(char **) * (i+2));
		#ifdef DEBUG
			printf("%s ", next_arg);
                #endif
                argv[i] = next_arg;

                i++;
        }
	#ifdef DEBUG
		printf("\n");
	#endif
        argv[i] = NULL;

        va_end(ap);

        res = execb(cmd, argv);

        free(argv);

        return res;
}

char *exec_cmdv(char *cmd, int exectime, char **argv)
{

        int pipe_fds[2];
        char *output = NULL;
        int ret, res, len = 0, ges = 0;

        struct timeval timeout;
        fd_set fdset;
	int child_pid, child_state;
	int readtimeout;

        ret = pipe(pipe_fds);

        if(( child_pid = fork() ) == 0)
        {
                alarm(exectime);

		close(1);
                close(2);
                if(dup(pipe_fds[1]) == -1) return NULL;
                if(dup(pipe_fds[1]) == -1) return NULL;
                close(pipe_fds[0]);
                close(pipe_fds[1]);

                execvp(cmd, argv);

                perror("execvp");
                exit(-1);
        }
        close(pipe_fds[1]);

	readtimeout = time(NULL) + exectime;

        while(waitpid(child_pid, &child_state, WNOHANG) == 0)
        {
        	if(readtimeout < time(NULL)) break;

                len = 0;

                FD_ZERO(&fdset);
                FD_SET(pipe_fds[0], &fdset);

                timeout.tv_sec = 0;
                timeout.tv_usec = 1000*400;


                res = select(pipe_fds[0]+1, &fdset, NULL, NULL, &timeout);

                if(res != 0 && res != -1)
                {
                        output = (char *)realloc(output, ges+1000+1);
                        len = read(pipe_fds[0], output+ges, 1000);

                        ges += len;
                        output[ges] = 0;
                }
        }

        close(pipe_fds[0]);

	if(output != NULL)
	{
		if(strlen(output) == 0)
		{
			free(output);
			output = NULL;
		}
		else
		{
        		output = (char *)realloc(output, ges+1);
		}
	}



        return output;
}




int die(char *string)
{
        printf("%s", string);
        exit(-1);
}


char **parse_ips(char *name, int *num)
{
        int counter = 0;
        struct dirent *dir_info;
        char *path, *ip;
        DIR *dirst;
	char *subnet;

        char **output = NULL;
        *num = 0;


        path = concat(global_settings.config_dir, "/", name,"/interfaces", NULL);
        dirst = opendir(path);
        free(path);

        if(dirst != NULL)
        {

                while ((dir_info = readdir(dirst)) != NULL)
                {

                        if(strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0)
                        {

                                path = concat(global_settings.config_dir, "/", name,"/interfaces/", dir_info->d_name, NULL);

                                if(dir_exist(path))
                                {
                                        free(path);
                                        path = concat(global_settings.config_dir, "/", name,"/interfaces/", dir_info->d_name, "/ip", NULL);

                                        ip = file_read(path);
                                        if(ip != NULL)
                                        {
                                                trim_string(ip);
						subnet = strstr(ip, "/");
						if(subnet != NULL)
							subnet[0] = 0;

                                                output = realloc(output, (counter+1) * sizeof(char **));
                                                output[counter] = ip;

                                                ovcp_log(OVCP_DEBUG, "IP %s:%d: %s", name, counter, output[counter]);

                                                counter++;
                                        }
                                }

                                free(path);

                        }

                }

                *num = counter;
                closedir(dirst);
        }

        return output;
}


struct iface_entry *parse_ifaces(char *name)
{
        int counter = 0;
        struct dirent *dir_info;
        char *path, *ip, *dev;
        DIR *dirst;
	char *subnet;

        struct iface_entry *output = NULL;


        path = concat(global_settings.config_dir, "/", name,"/interfaces", NULL);
        dirst = opendir(path);
        free(path);

        if(dirst != NULL)
        {

                while ((dir_info = readdir(dirst)) != NULL)
                {

                        if(strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0)
                        {

                                path = concat(global_settings.config_dir, "/", name,"/interfaces/", dir_info->d_name, NULL);

                                if(dir_exist(path))
                                {
                                        free(path);
                                        path = concat(global_settings.config_dir, "/", name,"/interfaces/", dir_info->d_name, "/ip", NULL);

                                        ip = file_read(path);
                                        if(ip != NULL)
                                        {
                                                trim_string(ip);
						subnet = strstr(ip, "/");
						if(subnet != NULL)
							subnet[0] = 0;

                                                output = realloc(output, (counter+1) * sizeof(struct iface_entry));
						memset(&output[counter], 0, sizeof(struct iface_entry));
						output[counter].ip = ip;

						free(path);
                                        	path = concat(global_settings.config_dir, "/", name,"/interfaces/", dir_info->d_name, "/dev", NULL);
						dev = file_read(path);
						if(dev != NULL)
						{
							trim_string(dev);
							output[counter].dev = dev;
						}

                                                counter++;
                                        }
                                }

                                free(path);

                        }

                }

                closedir(dirst);
        }

        if(output != NULL)
        {
        	output = realloc(output, (counter+1) * sizeof(struct iface_entry));
		memset(&output[counter], 0, sizeof(struct iface_entry));
		output[counter].ip = NULL;
		output[counter].dev = NULL;
	}

        return output;
}


int free_ifacelist(struct iface_entry *list)
{
	int i=0;

	if(list == NULL)
		return -1;

	while(list[i].ip != NULL)
	{

		free(list[i].ip);
		if(list[i].dev != NULL) free(list[i].dev);

		i++;
	}

	free(list);

	return 0;
}

struct vsip_entry *vserver_iplist(void)
{
	struct vsip_entry *allips = NULL;
	char **vsips;
	int i, entrysnum, allentrys = 0;

	DIR *dir_ptr;
	struct dirent *dir_entry;
	char path[BUF_SIZE];



	if((dir_ptr = opendir(global_settings.config_dir)) != NULL)
	{

		while((dir_entry = readdir(dir_ptr)) != NULL)
		{


			if(dir_entry->d_name[0] != '.')
			{
				sconcat(path, BUF_SIZE, global_settings.config_dir, "/", dir_entry->d_name, NULL);

				if(dir_exist(path))
				{
					i = 0;
					vsips = parse_ips(dir_entry->d_name, &entrysnum);

					while(i<entrysnum)
					{

						allips = realloc(allips, sizeof(struct vsip_entry)*(allentrys+2));
						allips[allentrys].name = strdup(dir_entry->d_name);
						allips[allentrys].ip = strdup(vsips[i]);

						allentrys++;
						i++;
					}

					free_strings(vsips, entrysnum);

				}
			}

		}


		closedir(dir_ptr);
	}

	if(allips != NULL)
	{
		allips[allentrys].name = NULL;
		allips[allentrys].ip = NULL;
	}

	return allips;
}

int free_iplist(struct vsip_entry *list)
{
	int i=0;

	if(list == NULL)
		return -1;

	while(list[i].name != NULL && list[i].ip != NULL)
	{
		free(list[i].name);
		free(list[i].ip);

		i++;
	}

	free(list);

	return 0;
}

int vci_version(void)
{
	FILE *fd;
	char *tag, *value;
	int part1, part2;
	int version = -1;

	fd = fopen("/proc/virtual/info", "r");

	if(fd != NULL)
	{
		while(!feof(fd) && version == -1)
		{
			if(fscanf(fd, "%as %as\n", &tag, &value) != 2) return -1;

			if(strcmp(tag, "VCIVersion:") == 0)
			{

				sscanf(value, "%x:%x", &part1, &part2);
				version = part1<<16;
				version = version | part2;

			}



			free(tag);
			free(value);
		}

		fclose(fd);
	}




	return version;
}

int kernelmodule_loaded(char *mod)
{
        char name[50+1];
        FILE *modfile;
        int found = 0;

        modfile = fopen("/proc/modules", "r");
	if(modfile == NULL)
		return 0;

        while(!feof(modfile))
        {
               if(fscanf(modfile, "%50s", name) != 1) return 0;

                if(strcmp(name, mod) == 0)
                {
                        found = 1;
                        break;
                }

        }

        fclose(modfile);

        return found;
}


char **node_interfaces(int *count)
{
	FILE *dev_file;
	char word[500+1];
	int i = 0;
	char **devs = NULL;

	*count=0;

	dev_file = fopen("/proc/net/dev", "r");

	if(dev_file != NULL)
	{

		while(!feof(dev_file))
		{
			word[0] = 0;
			if(fscanf(dev_file, "%500s", word) != 1) continue;

			if(strlen(word)>0)
			{
				i = 0;

				while(i != strlen(word))
				{
					if(word[i] == ':')
					{


						word[i] = 0;
						devs = (char **)realloc(devs, sizeof(char *)*(*count+1));
						devs[*count] = strdup(word);
						(*count)++;

						break;
					}

					i++;
				}

			}
		}


		fclose(dev_file);
	}


	return devs;
}


int send_email(char *msg)
{

        int pipe_fds[2];
        int len = 0;

	if(msg == NULL) return 0;

        if(pipe(pipe_fds) != 0) return -1;

        if(fork() == 0)
        {
                alarm(10);
		close(0);
                if(dup(pipe_fds[0]) == -1) return -1;
                close(pipe_fds[0]);
                close(pipe_fds[1]);

                execlp("sendmail", "sendmail", "-t", "-i", NULL);

                exit(-1);
        }
        close(pipe_fds[0]);

        len = write(pipe_fds[1], msg, strlen(msg));

        close(pipe_fds[1]);


       return 0;
}

static FILE *ovcp_logstm;
static int ovcp_logverbose;
static int ovcp_loglevel;

int ovcp_log_open(char *file, int level, int verbose)
{
	ovcp_logstm = fopen(file, "a");
	ovcp_loglevel = level;
	ovcp_logverbose = verbose;

	if(ovcp_logstm == NULL)
		printf("Unable to open logfile\n");
	else setvbuf(ovcp_logstm, NULL, _IOLBF, -1);

	return 1;
}


void ovcp_log(int level, char *fmt, ...)
{
	char msg[BUF_SIZE];
	char cdate[BUF_SIZE];
	char *levelstr = NULL;
	va_list arglist;
	time_t current_time;
	struct tm *timeinfo;



	if(level < ovcp_loglevel)
		return;



	time ( &current_time );
	timeinfo = localtime ( &current_time );

	strftime(cdate, BUF_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

	switch(level)
	{
		case OVCP_DEBUG:
				levelstr = "DEBUG\t";
				break;
		case OVCP_INFO:
				levelstr = "INFO\t";
				break;
		case OVCP_WARNING:
				levelstr = "WARNING\t";
				break;
		case OVCP_ERROR:
				levelstr = "ERROR\t";
				break;
	}
	snprintf(msg, BUF_SIZE, "%s %s -\t %s\n", cdate, levelstr, fmt);

	if(ovcp_logstm != NULL)
	{
		va_start(arglist, fmt);
		vfprintf(ovcp_logstm, msg, arglist);
		va_end(arglist);
	}

	if(ovcp_logverbose == 1)
	{
		va_start(arglist, fmt);
		vprintf(msg, arglist);
		va_end(arglist);
	}
}



int ovcp_sqlite_exec(sqlite3 *db, char *sql_query)
{
	char *sql_error;
	int c = 1,ret;


	while (c <= 10)
	{

		ovcp_log(OVCP_DEBUG, "SQL QUERY[%i]: %s", c, sql_query);
		ret = sqlite3_exec(db, sql_query, NULL, 0, &sql_error);

		ovcp_log(OVCP_DEBUG, "SQL return: %i, message: (%s)", ret, sql_error);

		if(ret != SQLITE_OK)
		{
			sleep(1);
			if(c==10) ovcp_log(OVCP_ERROR, "SQL error (%s)", sql_error);
			sqlite3_free(sql_error);
		}
		else break;

		c++;
	}


	return ret;
}

