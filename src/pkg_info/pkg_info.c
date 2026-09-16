/*
 *  Copyright (c) 2026, littlefly365
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <err.h>

const char SYSPKG[] = "/usr/syspkg";

static void usage(void);
static void list_pkgs(void);
static void about_pkg(const char *pkgname);

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

	switch (argc) {
	case 0:
		list_pkgs();
		break;
	case 1:
		about_pkg(argv[0]);
		break;
	default:
		usage();
	}

	return 0;
}

static void
list_pkgs(void)
{
	DIR* dir;
	struct dirent *ent;
	if (!(dir = opendir(SYSPKG))) {
		fprintf(stderr, "pkg_info: The package database '%s' does not exist.\n", SYSPKG);
		exit(EXIT_FAILURE);
	}

	while ((ent = readdir(dir))) {
		if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
			continue;
		puts(ent->d_name);
	}
}

static void
printinfo_pkg(const char *pkgname, const char *key, const char *key_file)
{
	int fd = 0;
	char buf[512];
	snprintf(buf, sizeof(buf), "%s/%s/%s", SYSPKG, pkgname, key_file);

	if ((fd = open(buf, O_RDONLY)) == -1) {
		fprintf(stderr, "pkg_info: The package '%s' is not installed.\n", pkgname);
		exit(EXIT_FAILURE);
	}

	char value[128];
	if (read(fd, value, sizeof(value)) <= 0)
		err(EXIT_FAILURE, "read");

	printf("%s:   %s\n", key, value);
}

static void
about_pkg(const char *pkgname)
{
	printinfo_pkg(pkgname, "Name", "NAME");
	printinfo_pkg(pkgname, "Version", "VERSION");
	printinfo_pkg(pkgname, "Release", "RELEASE");
	printinfo_pkg(pkgname, "Description", "DESCRIPTION");
	printinfo_pkg(pkgname, "Architecture", "ARCHITECTURE");
	printinfo_pkg(pkgname, "Source", "SOURCE");
	printinfo_pkg(pkgname, "License", "LICENSE");
	printinfo_pkg(pkgname, "Maintainer", "MAINTAINER");
	printinfo_pkg(pkgname, "Last Update", "LAST_UPDATE");
}

static void
usage(void)
{
	fprintf(stderr, "usage: pkg_info [pkgname]\n");
	exit(EXIT_FAILURE);
}
