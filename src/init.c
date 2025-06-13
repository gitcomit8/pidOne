#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <sys/sysmacros.h>
#include "parser.h"
#include "graph.h"
#include "supervisor.h"
#include "ctl_dispatch.h"

#define SERVICE_DIR "/etc/pidone.d"
#define die(msg)     \
	do               \
	{                \
		perror(msg); \
		exit(1);     \
	} while (0)
#define MAX_SERVICES 128

static void setup_filesystems()
{
	if (mount("none", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL) < 0)
		die("mount /proc");
	if (mount("none", "/sys", "sysfs", 0, NULL) < 0)
		die("mount /sys");
	if (mount("tmpfs", "/run", "tmpfs", MS_NODEV | MS_NOEXEC, 0) < 0)
		die("mount /run");
}

static void setup_devices()
{
	struct stat st;
	if (stat("/dev/console", &st) < 0)
	{
		if (mknod("/dev/console", S_IFCHR | 0600, makedev(5, 1)) < 0)
			die("mknod /dev/console");
	}
	if (mount("devtmpfs", "/dev", "devtmpfs", MS_NOSUID, 0) < 0)
		die("mount /dev");
}

static int load_all_services(service_conf *services, int max)
{
	DIR *d = opendir(SERVICE_DIR);
	if (!d)
	{
		perror("opendir");
		return 0;
	}
	struct dirent *de;
	int count = 0;
	char path[PATH_MAX];
	while ((de = readdir(d)) && count < max)
	{
		if (de->d_type != DT_REG)
			continue;
		snprintf(path, sizeof(path), "%s/%s", SERVICE_DIR, de->d_name);
		if (parse_service_file(path, &services[count]) == 0)
		{
			count++;
		}
		else
		{
			fprintf(stderr, "Failed to parse %s\n", path);
		}
	}
	closedir(d);
	return count;
}

static void sigchld_handler(int signo)
{
	// Supervisor handles process reaping
}

int main(int argc, char *argv[])
{
	if (getpid() != 1)
	{
		fprintf(stderr, "This program must be run with PID 1\n");
		return 1;
	}

	signal(SIGCHLD, sigchld_handler);
	setup_filesystems();
	setup_devices();

	// Control socket server
	pthread_t ctl_tid;
	if (pthread_create(&ctl_tid, NULL, run_control_socket, NULL) != 0)
		die("pthread_create");

	service_conf services[MAX_SERVICES];
	int svc_count = load_all_services(services, MAX_SERVICES);
	if (svc_count == 0)
	{
		fprintf(stderr, "No services found in %s\n", SERVICE_DIR);
	}

	service_graph g = build_service_graph(services, svc_count);
	resolve_dependencies(&g, start_service);
	supervise_loop();
	return 0;
}