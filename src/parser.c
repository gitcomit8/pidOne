#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 512
#define MAX_KEY 64
#define MAX_VAL 256

typedef struct service_conf
{
	char name[MAX_VAL];
	char exec[MAX_VAL];
	char type[MAX_KEY];
	char requires[MAX_VAL];
	char after[MAX_VAL];
	char restart[MAX_KEY];
	int restart_delay;
	char user[MAX_KEY];
	char rlimits[MAX_VAL];
	char seccomp[MAX_KEY];
} service_conf;

// INI parser for single file
int parse_service_file(const char *path, service_conf *svc)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return -1;
	char line[MAX_LINE];
	while (fgets(line, sizeof(line), f))
	{
		char key[MAX_KEY], val[MAX_VAL];
		if (line[0] == '#' || line[0] == '\n')
			continue;
		if (sscanf(line, "%63[^ =] = \"%255[^\"]\"", key, val) == 2 ||
			sscanf(line, "%63[^ =] = %255[^\n]", key, val) == 2)
		{
			if (strcmp(key, "name") == 0)
				strncpy(svc->name, val, sizeof(svc->name));
			else if (strcmp(key, "exec") == 0)
				strncpy(svc->exec, val, sizeof(svc->exec));
			else if (strcmp(key, "type") == 0)
				strncpy(svc->type, val, sizeof(svc->type));
			else if (strcmp(key, "requires") == 0)
				strncpy(svc->requires, val, sizeof(svc->requires));
			else if (strcmp(key, "after") == 0)
				strncpy(svc->after, val, sizeof(svc->after));
			else if (strcmp(key, "restart") == 0)
				strncpy(svc->restart, val, sizeof(svc->restart));
			else if (strcmp(key, "restart_delay") == 0)
				svc->restart_delay = atoi(val);
			else if (strcmp(key, "user") == 0)
				strncpy(svc->user, val, sizeof(svc->user));
			else if (strcmp(key, "rlimits") == 0)
				strncpy(svc->rlimits, val, sizeof(svc->rlimits));
			else if (strcmp(key, "seccomp") == 0)
				strncpy(svc->seccomp, val, sizeof(svc->seccomp));
		}
	}
	fclose(f);
	return 0;