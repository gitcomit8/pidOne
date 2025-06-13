#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include "graph.h"
#include "supervisor.h"

#define MAX_SERVICES 128

typedef struct
{
	node *svc_node;
	service_conf conf;
	pid_t pid;
	int restart_count;
} running_service;

static running_service services[MAX_SERVICES];
static int n_services = 0;

static running_service *find_service_by_pid(pid_t pid)
{
	for (int i = 0; i < n_services; ++i)
		if (services[i].pid == pid)
			return &services[i];
	return NULL;
}

void start_service(node *svc_node)
{
	if (!svc_node)
		return;
	running_service *rs = NULL;
	for (int i = 0; i < n_services; ++i)
		if (services[i].svc_node == svc_node)
		{
			rs = &services[i];
			break;
		}
	if (!rs)
	{
		rs = &services[n_services++];
		rs->svc_node = svc_node;
		// In real use, pass conf pointer instead of copying, for now just stub
		memset(&rs->conf, 0, sizeof(rs->conf));
		strncpy(rs->conf.name, svc_node->name, sizeof(rs->conf.name) - 1);
	}
	pid_t pid = fork();
	if (pid == 0)
	{
		// Child
		execlp("/bin/sh", "sh", "-c", rs->conf.exec[0] ? rs->conf.exec : "sleep 60", NULL);
		_exit(127);
	}
	else if (pid > 0)
	{
		rs->pid = pid;
		rs->restart_count = 0;
		printf("Started service %s (pid %d)\n", rs->conf.name, pid);
	}
	else
	{
		perror("fork");
	}
}

static void restart_service(running_service *rs)
{
	if (rs->restart_count++ < 5)
	{
		printf("Restarting service %s\n", rs->conf.name);
		start_service(rs->svc_node);
	}
	else
	{
		printf("Service %s failed too many times; not restarting\n", rs->conf.name);
	}
}

void supervise_loop()
{
	int status;
	while (1)
	{
		pid_t pid = wait(&status);
		if (pid > 0)
		{
			running_service *svc = find_service_by_pid(pid);
			if (svc)
			{
				printf("Service %s (pid %d) terminated\n", svc->conf.name, pid);
				if (WIFEXITED(status) || WIFSIGNALED(status))
				{
					if (strcmp(svc->conf.restart, "always") == 0)
						restart_service(svc);
				}
			}
		}
	}
}