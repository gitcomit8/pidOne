#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define MAX_RESTARTS 5

typedef struct service
{
	pid_t pid;
	int restart_count;
} service;

service *find_service_by_pid(pid_t pid)
{
	// TODO: implement real lookup
	static service dummy;
	dummy.pid = pid;
	dummy.restart_count = 0;
	return &dummy;
}

void start_service(service *svc)
{
	// TODO: implement actual service start logic
	printf("Starting service with PID placeholder\n");
	svc->pid = fork();
	if (svc->pid == 0)
	{
		// Replace with actutal exec logic
		execlp("/bin/sleep", "sleep", "60", NULL);
		_exit(127);
	}
}

void handle_service_exit(service *svc)
{
	if (svc->restart_count++ < MAX_RESTARTS)
	{
		int delay = 1 << svc->restart_count;
		srand(time(NULL) ^ getpid());
		int jitter = rand() % 1000;
		sleep(delay + jitter / 1000);
		start_service(svc);
	}
	else
	{
		printf("Service exceeded max restarts, not restarting\n");
	}
}

void supervise_loop()
{
	int status;
	while (1)
	{
		pid_t pid = waitpid(-1, &status, WNOHANG);
		if (pid > 0)
		{
			service *svc = find_service_by_pid(pid);
			if (WIFEXITED(status) || WIFSIGNALED(status))
			{
				handle_service_exit(svc);
			}
		}
		usleep(100000);
	}
}