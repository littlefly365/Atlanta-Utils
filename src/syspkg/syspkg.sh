#!/bin/sh
# Copyright (c) 2026, littlefly365
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SYSPKG="/usr/syspkg"
SYSRECIPEDIR="/usr/sysrecipes"
FETCHDIR="/usr/src"

PREFIX=/usr
SYSLIBDIR=/lib
LIBDIR=$PREFIX/lib
LIBEXECDIR=$PREFIX/libexec
BINDIR=$PREFIX/bin
SBINDIR=$PREFIX/sbin
SYSCONFDIR=/etc

def_gnu_args="--prefix=$PREFIX --libdir=$LIBDIR --bindir=$BINDIR --sbindir=$SBINDIR --sysconfdir=$SYSCONFDIR"

usage()
{
	printf >&2 "usage: syspkg <pkgname>...\n"
	exit 1
}

check_recipe()
{
	if [ ! -f "$SYSRECIPEDIR/$1.recipe" ]; then
		printf >&2 "syspkg: The packages '$1' does not exist or is not part of the base system.\n"
		exit 1
	else
		. "$SYSRECIPEDIR/$1.recipe"
	fi

	if [ -z "$pkgname" ] || [ -z "$pkgver" ] || [ -z "$pkgrel" ] || [ -z "$source" ] || [ -z "$build_style" ]; then
		printf >&2 "syspkg: The recipe for the package '$1' does not have the enough information.\n"
		exit 1
	fi
}

post_install_step()
{
	:
}

executing_recipe()
{
	file="$(basename $source)"
	mkdir -p "$FETCHDIR" && cd "$FETCHDIR"

	if [ ! -f "./$file" ]; then
		wget "$source"
	fi

	tar xf "$file" && cd "$pkgname-$pkgver"

	# configure step
	args=""
	case "$build_style" in
		gnu_configure|configure)
			if [ -v "$extra_args" ]; then
				args="$def_gnu_args $extra_args"
			elif [ -v "$configure_args" ]; then
				args="$configure_args"
			else
				args="$def_gnu_args"
			fi

			if [ -x ../configure ]; then
				../configure $args
			elif [ -x ./configure ]; then
				./configure $args
			else
				printf >&2 "syspkg: The configure script does not exist in the current directory.\n"
				exit 1
			fi
		;;
		*)
			break;
	esac

	# build step
	case "$build_style" in
		gnu_configure|configure)
			make -j$(nproc) || exit 1
		;;
		*)
			break
		;;
	esac

	# install step
	case "$build_style" in
		gnu_configure|configure)
			make install || exit 1
		;;
		*)
			break
		;;
	esac

	post_install_step

	cd .. && rm -rf "$pkgname-$pkgver"

	if command -v slib-remove >/dev/null; then
		slib-remove $LIBDIR
	fi

	# Create METADATA
	mkdir -p "$SYSPKG/$pkgname"
	printf "$pkgver" > "$SYSPKG/$pkgname/VERSION"
	printf "$pkgrel" > "$SYSPKG/$pkgname/RELEASE"
	printf "$pkgdesc" > "$SYSPKG/$pkgname/DESCRIPTION"
	printf "$source" > "$SYSPKG/$pkgname/SOURCE"
	printf "$license" > "$SYSPKG/$pkgname/LICENSE"
	printf "$(date)" > "$SYSPKG/$pkgname/LAST_UPDATE"

	printf "syspkg: '$1 $pkgver' has been installed successfully\n"
}

main()
{
	if [ $(id -un) != "root" ]; then
		printf >&2 "syspkg: You need root permissions.\n"
		exit 1
	fi

	while [ $# -gt 0 ]; do
		case "$1" in
			-*)
				printf >&2 "syspkg: invalid option '$1'\n"
				usage
			;;
			*)
				break;
			;;
		esac
	done

	if [ $# == 0 ]; then
		usage
	fi

	while [ $# -gt 0 ]; do
		check_recipe "$1"
		executing_recipe "$1"
		shift
	done
}

main $@
