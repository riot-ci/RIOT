/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is imported from [1] and slightly modified to work with
 * the RIOT build system.
 *
 * [1] https://github.com/torvalds/linux/blob/6ba1b005ffc388c2aeaddae20da29e4810dea298/scripts/basic/fixdep.c
 */

/*
 * "Optimize" a list of dependencies as spit out by gcc -MD
 * for the kernel build
 * ===========================================================================
 *
 * Author       Kai Germaschewski
 * Copyright    2002 by Kai Germaschewski  <kai.germaschewski@gmx.de>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 * It is invoked as
 *
 *   fixdep <dep_file> <object_file> <config_dir>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static void usage(void)
{
	fprintf(stderr, "Usage: fixdep <depfile> <target> <deps_dir>\n");
	exit(1);
}

/*
 * In the intended usage of this program, the stdout is redirected to .*.cmd
 * files. The return value of printf() and putchar() must be checked to catch
 * any error, e.g. "No space left on device".
 */
static void xprintf(const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vprintf(format, ap);
	if (ret < 0) {
		perror("fixdep");
		exit(1);
	}
	va_end(ap);
}

static void xputchar(int c)
{
	int ret;

	ret = putchar(c);
	if (ret == EOF) {
		perror("fixdep");
		exit(1);
	}
}

/*
 * Print out a dependency path from a symbol name
 */
static void print_dep(const char *m, int slen, const char *dir)
{
	int c, prev_c = '/', i;

	xprintf("    $(wildcard %s/", dir);
	for (i = 0; i < slen; i++) {
		c = m[i];
		if (c == '_')
			c = '/';
		else
			c = tolower(c);
		if (c != '/' || prev_c != '/')
			xputchar(c);
		prev_c = c;
	}
	xprintf(".h) \\\n");
}

struct item {
	struct item	*next;
	unsigned int	len;
	unsigned int	hash;
	char		name[];
};

#define HASHSZ 256
static struct item *hashtab[HASHSZ];

static unsigned int strhash(const char *str, unsigned int sz)
{
	/* fnv32 hash */
	unsigned int i, hash = 2166136261U;

	for (i = 0; i < sz; i++)
		hash = (hash ^ str[i]) * 0x01000193;
	return hash;
}

/*
 * Lookup a value in the configuration string.
 */
static int is_defined_config(const char *name, int len, unsigned int hash)
{
	struct item *aux;

	for (aux = hashtab[hash % HASHSZ]; aux; aux = aux->next) {
		if (aux->hash == hash && aux->len == len &&
		    memcmp(aux->name, name, len) == 0)
			return 1;
	}
	return 0;
}

/*
 * Add a new value to the configuration string.
 */
static void define_config(const char *name, int len, unsigned int hash)
{
	struct item *aux = malloc(sizeof(*aux) + len);

	if (!aux) {
		perror("fixdep:malloc");
		exit(1);
	}
	memcpy(aux->name, name, len);
	aux->len = len;
	aux->hash = hash;
	aux->next = hashtab[hash % HASHSZ];
	hashtab[hash % HASHSZ] = aux;
}

/*
 * Record the use of a CONFIG_* word.
 */
static void use_config(const char *m, int slen, const char *deps_dir)
{
	unsigned int hash = strhash(m, slen);

	if (is_defined_config(m, slen, hash))
	    return;

	define_config(m, slen, hash);
	print_dep(m, slen, deps_dir);
}

/* test if s ends in sub */
static int str_ends_with(const char *s, int slen, const char *sub)
{
	int sublen = strlen(sub);

	if (sublen > slen)
		return 0;

	return !memcmp(s + slen - sublen, sub, sublen);
}

static void parse_config_file(const char *p, const char *deps_dir)
{
	const char *q, *r;
	const char *start = p;

	while ((p = strstr(p, "CONFIG_"))) {
		if (p > start && (isalnum(p[-1]) || p[-1] == '_')) {
			p += 7;
			continue;
		}
		p += 7;
		q = p;
		while (isalnum(*q) || *q == '_')
			q++;
		if (str_ends_with(p, q - p, "_MODULE"))
			r = q - 7;
		else
			r = q;
		if (r > p)
                        use_config(p, r - p, deps_dir);
		p = q;
	}
}

static void *read_file(const char *filename)
{
	struct stat st;
	int fd;
	char *buf;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "fixdep: error opening file: ");
		perror(filename);
		exit(2);
	}
	if (fstat(fd, &st) < 0) {
		fprintf(stderr, "fixdep: error fstat'ing file: ");
		perror(filename);
		exit(2);
	}
	buf = malloc(st.st_size + 1);
	if (!buf) {
		perror("fixdep: malloc");
		exit(2);
	}
	if (read(fd, buf, st.st_size) != st.st_size) {
		perror("fixdep: read");
		exit(2);
	}
	buf[st.st_size] = '\0';
	close(fd);

	return buf;
}

/* Ignore certain dependencies */
static int is_ignored_file(const char *s, int len)
{
	return str_ends_with(s, len, "generated/autoconf.h");
}

/*
 * Important: The below generated source_foo.o and deps_foo.o variable
 * assignments are parsed not only by make, but also by the rather simple
 * parser in scripts/mod/sumversion.c.
 */
static void parse_dep_file(char *m, const char *target, const char *deps_dir)
{
	char *p;
	int is_last, is_target;
	int saw_any_target = 0;
	int is_first_dep = 0;
	void *buf;

	while (1) {
		/* Skip any "white space" */
		while (*m == ' ' || *m == '\\' || *m == '\n')
			m++;

		if (!*m)
			break;

		/* Find next "white space" */
		p = m;
		while (*p && *p != ' ' && *p != '\\' && *p != '\n')
			p++;
		is_last = (*p == '\0');
		/* Is the token we found a target name? */
		is_target = (*(p-1) == ':');
		/* Don't write any target names into the dependency file */
		if (is_target) {
			/* The /next/ file is the first dependency */
			is_first_dep = 1;
		} else if (!is_ignored_file(m, p - m)) {
			*p = '\0';

			/*
			 * Do not list the source file as dependency, so that
			 * kbuild is not confused if a .c file is rewritten
			 * into .S or vice versa. Storing it in source_* is
			 * needed for modpost to compute srcversions.
			 */
			if (is_first_dep) {
				/*
				 * If processing the concatenation of multiple
				 * dependency files, only process the first
				 * target name, which will be the original
				 * source name, and ignore any other target
				 * names, which will be intermediate temporary
				 * files.
				 */
				if (!saw_any_target) {
					saw_any_target = 1;
					xprintf("deps_%s :=\\\n  %s \\\n",
						target, m);
				}
				is_first_dep = 0;
			} else {
				xprintf("  %s \\\n", m);
			}

			buf = read_file(m);
			parse_config_file(buf, deps_dir);
			free(buf);
		}

		if (is_last)
			break;

		/*
		 * Start searching for next token immediately after the first
		 * "whitespace" character that follows this token.
		 */
		m = p + 1;
	}

	if (!saw_any_target) {
		fprintf(stderr, "fixdep: parse error; no targets found\n");
		exit(1);
	}

	xprintf("\n%s: $(deps_%s)\n\n", target, target);
	xprintf("$(deps_%s):\n", target);
}

int main(int argc, char *argv[])
{
        const char *depfile, *target, *deps_dir;
	void *buf;

	if (argc != 4)
		usage();

	depfile = argv[1];
	target = argv[2];
	deps_dir = argv[3];

	buf = read_file(depfile);
	parse_dep_file(buf, target, deps_dir);
	free(buf);

	return 0;
}
