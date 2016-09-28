/*  OpenVCP
 *  Copyright (C) 2010 Gerrit Wyen <gerrit@ionscale.com>
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
 

#ifndef TASK_H
	#define TASK_H
	
	enum TASK_STATES { OVCP_TASK_WAITING, OVCP_TASK_INPROGRESS, OVCP_TASK_FINISHED};
	
	int ovcp_taskcontrol_init(void);
	int ovcp_task_new(void);
	int ovcp_task_setMsg(int id, char *msg);
	int ovcp_task_setState(int id, int state);
	int ovcp_task_setPercent(int id, float percent);
	char *ovcp_task_getMsg(int id);
	int ovcp_task_getState(int id);
	float ovcp_task_getPercent(int id);
	
	
	
#endif
	
