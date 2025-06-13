#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/run/pidOne.sock"
#define MAX_CMD 64
#define MAX_ARG 64

void usage()
{
	fprintf(stderr,
			"Usage: pid1ctl [command] [service]\n"
			"Commands\n"
			"  start      Launch service\n"
			"  stop       Send SIGTERM → SIGKILL\n"
			"  restart    Stop + Start\n"
			"  status     Show process tree\n"
			"  enable     Add to boot graph\n"
			"  disable    Remove from boot graph\n"
			"  emergency  Enter rescue shell\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		usage();
		return 1;
	}

	char cmd[MAX_CMD] = {0};
	char arg[MAX_ARG] = {0};
	strncpy(cmd, argv[1], MAX_CMD - 1);
	if (argc > 2)
		strncpy(arg, argv[2], MAX_ARG - 1);

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket");
		return 1;
	}

	struct sockaddr_un addr = {0};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("connect");
		close(sock);
		return 1;
	}

	if (write(sock, cmd, strlen(cmd)) < 0)
	{
		perror("write cmd");
		close(sock);
		return 1;
	}
	if (strlen(arg) > 0)
	{
		if (write(sock, " ", 1) < 0)
		{
			perror("write space");
			close(sock);
			return 1;
		}
		if (write(sock, arg, strlen(arg)) < 0)
		{
			perror("write arg");
			close(sock);
			return 1;
		}
	}
	if (write(sock, "\n", 1) < 0)
	{
		perror("write newline");
		close(sock);
		return 1;
	}

	char buf[1024];
	ssize_t n;
	while ((n = read(sock, buf, sizeof(buf) - 1)) > 0)
	{
		buf[n] = '\0';
		printf("%s", buf);
	}

	close(sock);
	return 0;
}