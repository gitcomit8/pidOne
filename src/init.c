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
#include <sys/sysmacros.h>

#define die(msg)     \
	do               \
	{                \
		perror(msg); \
		exit(1);     \
	} while (0)

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

int main(int argc, char *argv[])
{
	if (getpid() != 1)
	{
		fprintf(stderr, "This program must be run with PID 1\n");
		return 1;
	}

	setup_filesystems();
	setup_devices();

	// TODO: handoff to supervisor
	while (1)
	{
		pause();
	}
	return 0;
}