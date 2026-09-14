#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#define FILE_PATH	"/etc/SystemVersion"

static void
usage(void)
{
	printf("usage: sw_vers\n"
		"  or:  sw_vers --productName\n"
		"  or:  sw_vers --productVersion\n"
		"  or:  sw_vers --productVersionExtra\n"
		"  or:  sw_vers --buildVersion\n");
	exit(1);
}

static void
FindInText(char *text, const char *key, char *buf, size_t buflen)
{
	if (!text || !key || !buflen)
		return;
	char *str = strstr(text, key);
	if (str)
		str += strlen(key);
	else {
		*buf = '\0';
		return;
	}
	for (; *str == ' ' || *str == ':'; str++);
	while (*str != '\n' && buflen-- > 1) *buf++ = *str++;
	*buf = '\0';
}

int
main(int argc, char *argv[])
{
	int fd;
	char file[1024];
	char ProductName[256];
	char ProductVersion[256];
	char ProductVersionExtra[256];
	char BuildVersion[256];

	if (argc > 2)
		usage();
	if ((fd = open(FILE_PATH, O_RDONLY)) < 0)
		err(EXIT_FAILURE, "%s", FILE_PATH);
	if (read(fd, file, sizeof(file)) < 0)
		err(EXIT_FAILURE, "read");

	FindInText(file, "ProductName", ProductName, sizeof(ProductName));
	FindInText(file, "ProductVersion", ProductVersion, sizeof(ProductVersion));
	FindInText(file, "ProductVersionExtra", ProductVersionExtra, sizeof(ProductVersionExtra));
	FindInText(file, "BuildVersion", BuildVersion, sizeof(BuildVersion));
	close(fd);

	if (argc == 2) {
		if (!strcmp(argv[1], "--productName") || !strcmp(argv[1], "-productName"))
			printf("%s\n", ProductName);
		else if (!strcmp(argv[1], "--productVersion") || !strcmp(argv[1], "-productVersion"))
			printf("%s\n", ProductVersion);
		else if (!strcmp(argv[1], "--productVersionExtra") || !strcmp(argv[1], "-productVersionExtra"))
			printf("%s\n", ProductVersionExtra);
		else if (!strcmp(argv[1], "--buildVersion") || !strcmp(argv[1], "-buildVersion"))
			printf("%s\n", BuildVersion);
		else
			usage();
	} else if (argc == 1) {
		printf( "ProductName:           %s\n"
			"ProductVersion:        %s\n"
			"ProductVersionExtra:   %s\n"
			"BuildVersion:          %s\n",
			ProductName, ProductVersion, ProductVersionExtra, BuildVersion);
	}

	return 0;
}
