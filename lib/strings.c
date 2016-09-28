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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "strings.h"


char **split_string(char *request, char separator, unsigned int *args)
{

	char *p;
	char **array = NULL;

	*args = 0;

	if(request == NULL)
		return NULL;

	p = request;

	while(1)
	{
		if((*p == separator || *p == 0) && p != request)
		{
			array = (char **)realloc(array, sizeof(char **) * ( (*args) + 1));
			array[*args] = (char *)malloc(p-request+1);

			strncpy(array[*args], request, p-request);
			array[*args][p-request] = 0;

			while(*p == separator)
					p++;

			request = p;

			(*args)++;
		}

		if(*p == 0)
			break;

		p++;
	}


	return array;
}



int free_strings(char **strings, int elements)
{
	int i = 0;

	while(i != elements)
	{
		free(strings[i]);

		i++;
	}

	free(strings);

	return 1;
}


char *concat(char *first_arg, ...)
{
	va_list list;
	char *string, *arg;
	int len = 0;

	len = strlen(first_arg);

	string = (char *)malloc(len+1);
	strcpy(string, first_arg);

	va_start(list, first_arg);

	while((arg = va_arg(list, char *)) != NULL)
	{
		len += strlen(arg);
		string = (char *)realloc(string, len+1);
		strcat(string, arg);
	}

	va_end(list);

	return string;
}



int trim_string(char *string)
{
	int i;

	if(string == NULL)
		return -1;

	i = strlen(string)-1;

	while(i >= 0)
	{
		if(string[i] == '\n' || string[i] == '\r'
			|| string[i] == '\t' || string[i] == ' ')
		{
			string[i] = 0;
		}
		else
		{
			break;
		}

		i--;
	}

	return i;
}


int trim_slashes(char *string)
{
	int i;

	if(string == NULL)
		return -1;

	i = strlen(string)-1;

	while(i > 0)
	{
		if(string[i] == '/')
			string[i] = 0;
		else break;

		i--;
	}

	return i;
}


char *string_append(char *string, char *attachment)
{
	int len;

	if(string != NULL && attachment != NULL)
	{
		len = strlen(string);
		string = realloc(string, len + strlen(attachment)+1);
		memcpy(string+len, attachment, strlen(attachment)+1);
	}


	return string;
}



void string_toupper(char *str)
{
	int i = 0;

	if(str == NULL) return;

	while(i < strlen(str))
	{

		str[i] = toupper(str[i]);
		i++;
	}
}



int sconcat(char *buf, int size, ...)
{
	char *arg;
	int len = 0, pos = 0;
	va_list list;

	size = size - 1;

	if(buf != NULL)
	{

		va_start(list, size);

		while((arg = va_arg(list, char *)) != NULL)
		{
			len = strlen(arg);

			if(pos < size)
			{
				if(size-pos < len)
					len = size-pos;

				memcpy(buf+pos, arg, len);

				pos += len;
			}
			else break;
		}

		va_end(list);

		buf[pos] = 0;
	}

	return pos;
}


char *dec2str(int number)
{
	char *str;

	str = (char *)malloc(12);

	snprintf(str, 11, "%d", number);

	return str;
}
