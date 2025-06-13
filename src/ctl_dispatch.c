#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define SOCK_PATH "/run/pidone.sock"
#define MAX_CMDLEN 128
#define MAX_ARGS 2

// Placeholder: Add real service control logic here
void handle_command(const char *cmd, const char *arg, int client_fd)
{
	if (strcmp(cmd, "start") == 0)
	{
		dprintf(client_fd, "Starting service: %s\n", arg);
		// TODO: call supervisor to start service
	}
	else if (strcmp(cmd, "stop") == 0)
	{
		dprintf(client_fd, "Stopping service: %s\n", arg);
		// TODO: call supervisor to stop service
	}
	else if (strcmp(cmd, "restart") == 0)
	{
		dprintf(client_fd, "Restarting service: %s\n", arg);
		// TODO: call supervisor to restart service
	}
	else if (strcmp(cmd, "status") == 0)
	{
		dprintf(client_fd, "Status: (not yet implemented)\n");
		// TODO: show service/process tree
	}
	else if (strcmp(cmd, "enable") == 0)
	{
		dprintf(client_fd, "Enabled service: %s\n", arg);
		// TODO: enable service in boot graph
	}
	else if (strcmp(cmd, "disable") == 0)
	{
		dprintf(client_fd, "Disabled service: %s\n", arg);
		// TODO: disable service in boot graph
	}
	else if (strcmp(cmd, "emergency") == 0)
	{
		dprintf(client_fd, "Entering emergency shell...\n");
		// TODO: spawn rescue shell
	}
	else
	{
		dprintf(client_fd, "Unknown command: %s\n", cmd);
	}
}

void run_control_socket()
{
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket");
		exit(1);
	}
	struct sockaddr_un addr = {0};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
	unlink(SOCK_PATH);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		close(sock);
		exit(1);
	}
	if (listen(sock, 4) < 0)
	{
		perror("listen");
		close(sock);
		exit(1);
	}

	while (1)
	{
		int client = accept(sock, NULL, NULL);
		if (client < 0)
		{
			if (errno == EINTR)
				continue;
			perror("accept");
			break;
		}
		char buf[MAX_CMDLEN] = {0};
		ssize_t n = read(client, buf, sizeof(buf) - 1);
		if (n > 0)
		{
			buf[n] = '\0';
			char *cmd = strtok(buf, " \t\n");
			char *arg = strtok(NULL, " \t\n");
			handle_command(cmd ? cmd : "", arg ? arg : "", client);
		}
		close(client);
	}
	close(sock);
	unlink(SOCK_PATH);
}