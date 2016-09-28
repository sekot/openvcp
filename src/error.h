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


#ifndef ERROR_H
	#define ERROR_H

	#define OVCP_ERROR_OUTOFMEM	0
	#define OVCP_ERROR_ARGMISS 	1
	#define OVCP_ERROR_WRONGTYPE 	2
	#define OVCP_ERROR_VSNOTEXIST 	3
	#define OVCP_ERROR_FILEMISSING	4
	#define OVCP_ERROR_DIRNCREATED	5
	#define OVCP_ERROR_FLAGNFOUND	6
	#define OVCP_ERROR_IFACENEXIST	7
	#define OVCP_ERROR_WRONGRLIMIT	8
	#define OVCP_ERROR_INTERNAL	9
	#define OVCP_ERROR_VSLOCKED	10
	#define OVCP_ERROR_DBERROR	11
	#define OVCP_ERROR_UABLCPYIMG	12
	#define OVCP_ERROR_UABLCRTCONF	13
	#define OVCP_ERROR_VSNRUNING	14
	#define OVCP_ERROR_CODNSETPW	15
	#define OVCP_ERROR_MODNOSUP	16
	#define OVCP_ERROR_METHNOSUP	17
	#define OVCP_ERROR_DIRNOSLASH	18
	#define OVCP_ERROR_UNABLSPACE	19
	#define OVCP_ERROR_RULENPERM	20
	#define OVCP_ERROR_BACKUPFAIL	21
	#define OVCP_ERROR_NORESCUEIMG	22
	#define OVCP_ERROR_UABLRESTORE	23
	#define OVCP_ERROR_NOTSTOPPED	24
	#define OVCP_ERROR_UABLDELETE	25
	#define OVCP_ERROR_BCKDIRNFUOD	26
	#define	OVCP_ERROR_CGROUP		27
	#define	OVCP_ERROR_VSEXIST	28
	#define	OVCP_ERROR_WRONGARG	29

	#define OVCP_ERROR_OUTOFMEM_DESC	"Out of Memory."
	#define OVCP_ERROR_ARGMISS_DESC 	"Argument Missing."
	#define OVCP_ERROR_WRONGTYPE_DESC	"Argument has the wrong Type."
	#define OVCP_ERROR_VSNOTEXIST_DESC 	"VServer does not exist."
	#define OVCP_ERROR_FILEMISSING_DESC	"File missing."
	#define OVCP_ERROR_DIRNCREATED_DESC	"Couldn't create Directory."
	#define OVCP_ERROR_FLAGNFOUND_DESC	"Flag not found."
	#define OVCP_ERROR_IFACENEXIST_DESC	"Interface does not exist."
	#define OVCP_ERROR_WRONGRLIMIT_DESC	"A rlimit by this name does not exist."
	#define OVCP_ERROR_INTERNAL_DESC	"Internal Error."
	#define OVCP_ERROR_VSLOCKED_DESC	"VServer currently locked."
	#define OVCP_ERROR_DBERROR_DESC		"Database Error."
	#define OVCP_ERROR_UABLCPYIMG_DESC	"Unable to copy image."
	#define OVCP_ERROR_UABLCRTCONF_DESC	"Unable to create config."
	#define OVCP_ERROR_VSNRUNING_DESC	"VServer not running."
	#define OVCP_ERROR_CODNSETPW_DESC	"Couldn't set root password."
	#define OVCP_ERROR_MODNOSUP_DESC	"Modul not supported."
	#define OVCP_ERROR_METHNOSUP_DESC	"Method not supported."
	#define OVCP_ERROR_DIRNOSLASH_DESC	"Directory doesn't start with a slash"
	#define OVCP_ERROR_UNABLSPACE_DESC	"Unable to lookup diskspace"
	#define OVCP_ERROR_RULENPERM_DESC	"Rule not permitted for this VServer"
	#define OVCP_ERROR_BACKUPFAIL_DESC	"Unable to make a backup"
	#define OVCP_ERROR_NORESCUEIMG_DESC	"Rescue Image not found"
	#define OVCP_ERROR_UABLRESTORE_DESC	"Unable to restore backup"
	#define OVCP_ERROR_NOTSTOPPED_DESC	"VServer shouldn't be running"
	#define OVCP_ERROR_UABLDELETE_DESC	"Unable to delete directory"
	#define OVCP_ERROR_BCKDIRNFUOD_DESC	"User backup directory doesn't exist"
	#define	OVCP_ERROR_CGROUP_DESC		"cgroup not supported"
	#define OVCP_ERROR_VSEXIST_DESC 	"VServer does exist."
	#define	OVCP_ERROR_WRONGARG_DESC  "Argument incorrect."
#endif


