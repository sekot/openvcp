#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "main.h"
#include "move.h"
#include "fs.h"
#include "strings.h"
#include "misc.h"

int move_vserver(char *name, char *ip)
{
	char *tmpsrc, *tmpdest;

	tmpdest = concat(global_settings.config_dir, "/", name, NULL);

	if(!dir_exist(tmpdest))
	{
		mkdir(tmpdest, 0755);
	}
	free(tmpdest);

	tmpdest = concat(global_settings.config_dir, "/", name, "/.", NULL);
	tmpsrc = concat(ip, ":", tmpdest, NULL);

	execb_cmd("rsync", "rsync", "-a",	"-e", "ssh", "--delete", "--force", tmpsrc, tmpdest, NULL);

  free(tmpsrc); free(tmpdest);

  tmpdest = concat(global_settings.root_dir, "/", name, NULL);

	if(!dir_exist(tmpdest))
	{
		mkdir(tmpdest, 0755);
	}
	free(tmpdest);

  tmpdest = concat(global_settings.root_dir, "/", name, "/.", NULL);
	tmpsrc = concat(ip, ":", tmpdest, NULL);

	execb_cmd("rsync", "rsync", "-a",	"-e", "ssh", "--delete", "--force", tmpsrc, tmpdest, NULL);

  free(tmpsrc); free(tmpdest);

	return 1;
}

