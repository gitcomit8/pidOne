#ifndef PARSER_H
#define PARSER_H

#define MAX_KEY 64
#define MAX_VAL 256

typedef struct service_conf
{
	char name[MAX_VAL];
	char exec[MAX_VAL];
	char type[MAX_KEY];
	char
		requires[
			MAX_VAL];
	char after[MAX_VAL];
	char restart[MAX_KEY];
	int restart_delay;
	char user[MAX_KEY];
	char rlimits[MAX_VAL];
	char seccomp[MAX_KEY];
} service_conf;

int parse_service_file(const char *path, service_conf *svc);

#endif // PARSER_H