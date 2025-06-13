#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define MAX_LINE 512

static void trim(char *str)
{
	char *end;
	while (isspace((unsigned char)*str))
		str++;
	if (*str == 0)
		return;
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;
	*(end + 1) = 0;
}

int parse_service_file(const char *path, service_conf *svc)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return -1;
	memset(svc, 0, sizeof(service_conf));
	char line[MAX_LINE];
	while (fgets(line, sizeof(line), f))
	{
		char key[MAX_KEY], val[MAX_VAL];
		char *eq = strchr(line, '=');
		if (!eq)
			continue;
		*eq = 0;
		strncpy(key, line, MAX_KEY - 1);
		key[MAX_KEY - 1] = 0;
		strncpy(val, eq + 1, MAX_VAL - 1);
		val[MAX_VAL - 1] = 0;
		trim(key);
		trim(val);

		if (val[0] == '"' && val[strlen(val) - 1] == '"')
		{
			memmove(val, val + 1, strlen(val) - 2);
			val[strlen(val) - 2] = 0;
		}

		if (strcmp(key, "name") == 0)
			strncpy(svc->name, val, sizeof(svc->name) - 1);
		else if (strcmp(key, "exec") == 0)
			strncpy(svc->exec, val, sizeof(svc->exec) - 1);
		else if (strcmp(key, "type") == 0)
			strncpy(svc->type, val, sizeof(svc->type) - 1);
		else if (strcmp(key, "requires") == 0)
			strncpy(svc->requires, val, sizeof(svc->requires) - 1);
		else if (strcmp(key, "after") == 0)
			strncpy(svc->after, val, sizeof(svc->after) - 1);
		else if (strcmp(key, "restart") == 0)
			strncpy(svc->restart, val, sizeof(svc->restart) - 1);
		else if (strcmp(key, "restart_delay") == 0)
			svc->restart_delay = atoi(val);
		else if (strcmp(key, "user") == 0)
			strncpy(svc->user, val, sizeof(svc->user) - 1);
		else if (strcmp(key, "rlimits") == 0)
			strncpy(svc->rlimits, val, sizeof(svc->rlimits) - 1);
		else if (strcmp(key, "seccomp") == 0)
			strncpy(svc->seccomp, val, sizeof(svc->seccomp) - 1);
	}
	fclose(f);
	// Validate
	if (svc->name[0] == '\0' || svc->exec[0] == '\0')
		return -2;
	return 0;
}