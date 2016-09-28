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


#ifndef STRINGS_H
	#define STRINGS_H

	int free_strings(char **strings, int elements);
	char **split_string(char *request, char separator, unsigned int *args);
	char *concat(char *arg, ...);
	int trim_string(char *string);
	int sconcat(char *buf, int size, ...);
	char *string_append(char *string, char *attachment);
	char *dec2str(int number);
	int trim_slashes(char *string);
	void string_toupper(char *str);

#endif
