#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <err.h>

static void usage(void);

int
main(int argc, char *argv[])
{
	DIR *dir;
	char *str;
	int c, verbose = 0;
	struct dirent *ent;
	while ((c = getopt(argc, argv, "v")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 1)
		usage();

	if (!(dir = opendir(argv[0])))
		err(EXIT_FAILURE, "%s", argv[0]);

	while ((ent = readdir(dir))) {
		if (ent->d_name[0] == '.')
			continue;
		if ((str = strrchr(ent->d_name, '.')) && !strcmp(str, ".a")) {
			if (unlink(ent->d_name) != 0)
				err(EXIT_FAILURE, "%s", ent->d_name);
			if (verbose)
				printf("slib-remove: '%s' removed\n", ent->d_name);
		}
	}

	return 0;
}

static void
usage(void)
{
	printf("usage: slib-remove [-v] <LIBDIR>\n");
	exit(EXIT_FAILURE);
}
