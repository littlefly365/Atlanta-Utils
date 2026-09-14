#include <sys/types.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <err.h>

#if !defined(__linux__) || !defined(__x86_64__)
#error "This only works properly on x86_64 Linux"	/* We only care about Linux on x86_64 machines */
#endif

#if defined(__GLIBC__)
#define DEFAULT GLIBC
#else
#define DEFAULT MUSL
#endif

static const char *LD_MUSL = "/lib/ld-musl-x86_64.so.1";

static const int GLIBC = 1;
static const int MUSL = 2;

static void usage(void);
static int ldyd_main(const char *file, int mode);
static int is_script(const char *file);

int
main(int argc, char *argv[])
{
	int c, ret = 0;
	int mode = DEFAULT;
	while ((c = getopt(argc, argv, "Gm")) != -1) {
		switch (c) {
		case 'G':
			mode = GLIBC;
			break;
		case 'm':
			mode = MUSL;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	for (int i = 0; i < argc; i++) {
		if (argc > 1) {
			printf("%s:\n", argv[i]);
		}

		if (ldyd_main(argv[i], mode) != 0)
			ret = 1;
	}

	return ret;
}

static int
ldyd_main(const char *file, int mode)
{
	char buf[PATH_MAX];
	if (!strrchr(file, '/')) {
		snprintf(buf, sizeof(buf), "./%s", file);
	} else {
		snprintf(buf, sizeof(buf), "%s", file);
	}

	if (access(buf, F_OK) != 0) {
		fprintf(stderr, "ldyd: %s: No such file or directory\n", buf);
		return 1;
	}

	if (access(buf, X_OK) != 0) {
		fprintf(stderr, "ldyd: warning: you do not have execution permission for '%s'\n", buf);
		goto no_exec;
	}

	if (is_script(buf)) {
		goto no_exec;
	}

	if (mode == GLIBC)
		setenv("LD_TRACE_LOADED_OBJECTS", "1", 1);

	pid_t pid = fork();
	if (pid == -1) {
		err(EXIT_FAILURE, "fork");
	}

	if (pid == 0) {
		if (mode == GLIBC) {
			if (execl(buf, buf, NULL) != 0)
				err(EXIT_FAILURE, "%s", buf);
		} else {
			if (execl(LD_MUSL, LD_MUSL, "--list", buf, NULL) != 0)
				err(EXIT_FAILURE, "%s", LD_MUSL);
		}
	}

	waitpid(pid, NULL, 0);	/* I don't care about the exit code */
	goto end;

no_exec:
	fprintf(stderr, "\tnot a dynamic executable\n");
	return 1;

end:
	return 0;
}

static int
is_script(const char *file)
{
	int fd;
	char buf[2];
	fd = open(file, O_RDONLY);
	if (fd == -1)
		return 0;
	if (read(fd, buf, 2) != 2) {
		close(fd);
		return 0;
	}

	close(fd);

	if (buf[0] == '#' && buf[1] == '!')
		return 1;

	return 0;
}

static void
usage(void)
{
	printf("usage: ldyd [-Gm] FILE...\n");
	exit(1);
}
