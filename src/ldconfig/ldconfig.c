#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage(void);

int
main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "")) != -1) {
		switch (c) {
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	return 0;
}

static void
usage(void)
{
	printf("usage: ldconfig\n");
	exit(EXIT_FAILURE);
}
