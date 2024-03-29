#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "launchtab.h"
#include "style.h"
#include "util.h"

void *try_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr) {
		perror(NULL);
		exit(errno);
	}
	return ptr;
}

void *try_realloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (!ptr) {
		perror(NULL);
		exit(errno);
	}
	return ptr;
}


char *trim_leading(char *str)
{
	if (str)
		while (isspace(*str)) str++;
	return str;
}

char *trim_trailing(char *str)
{
	if (str && str[0]) {
		char *end = str + strlen(str) - 1;
		while (end > str && isspace(*end)) end--;
		end[1] = '\0';
	}
	return str;
}

char *trim(char *str)
{
	return trim_trailing(trim_leading(str));
}

/* Append str2 to str1. str1 must be NULL or dynamically allocated.
 * Returns the new string, reallocated from str1. */
char *str_append(char *str1, char *str2)
{
	int l1 = str1 ? strlen(str1) : 0;
	int l2 = strlen(str2);
	str1 = try_realloc(str1, l1 + l2 + 1);
	strcpy(str1 + l1, str2);
	return str1;
}

/* Returns the value associated with label, or NULL if none. */
char *find_value(char *label, char **labels, char **values, unsigned int n)
{
	for (int i = 0; i < n; i++) {
		if (strcmp(label, labels[i]) == 0)
			return (char *) values[i];
	}
	return NULL;
}

/* Escapes an xml-style string. str must be reallocable. */
char *escape_xml(char *str)
{
	int i, len = strlen(str);
	for (i = len - 1; i >= 0; i--) {
		char *replacement;
		switch (str[i]) {
		case '<':
			replacement = "&lt;";
			break;
		case '>':
			replacement = "&gt;";
			break;
		case '&':
			replacement = "&amp;";
			break;
		case '\'':
			replacement = "&apos;";
			break;
		case '"':
			replacement = "&quot;";
			break;
		default:
			replacement = NULL;
		}

		if (replacement) {
			int repl_len = strlen(replacement);
			str = try_realloc(str, len + repl_len);
			memmove(&str[i + repl_len], &str[i + 1], len - i + 1);
			memmove(&str[i], replacement, repl_len);
			len = len + repl_len - 1;
		}
	}
	return str;
}


int mkdir_p(const char *path)
{
	char *buf = try_malloc(strlen(path) + 1);
	strcpy(buf, path);

	for (char *p = buf; *p != '\0'; p++) {
		if (*p != '/' || p == buf)
			continue;

		*p = '\0';
		if (mkdir(buf, 0755) < 0 && errno != EEXIST)
			return -1;
		*p = '/';
	}
	if (mkdir(buf, 0755) < 0 && errno != EEXIST)
		return -1;

	return 0;
}

size_t dirname(const char *path, char *output)
{
	size_t i;
	if (!path)
		return 0;
	i = strlen(path);

	/* strip trailing slashes */
	while (i > 0 && path[i - 1] == '/')
		i--;

	/* strip last entry */
	while (i > 0 && path[i - 1] != '/')
		i--;

	/* strip trailing slashes */
	while (i > 0 && path[i - 1] == '/')
		i--;

	if (output) {
		strncpy(output, path, i);
		output[i] = '\0';
	}
	return i;
}

int cpfile(FILE *src, FILE *dst)
{
	char buf[4096];
	rewind(src);
	rewind(dst);

	while (!feof(src)) {
		size_t nbytes = fread(buf, 1, sizeof buf, src);
		if (nbytes > 0 && fwrite(buf, 1, nbytes, dst) != nbytes) {
			return -1;
		}
	}

	return 0;
}


void print_dbg(char *format, ...)
{
	if (!debug)
		return;

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_warn(char *format, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, format);
	fprintf(stderr, LTWARN(""));
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_warnl(char *format, unsigned lineno, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, lineno);
	fprintf(stderr, LTWARNL(""), lineno);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_err(char *format, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, format);
	fprintf(stderr, LTERR(""));
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_errl(char *format, unsigned lineno, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, lineno);
	fprintf(stderr, LTERRL(""), lineno);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_info(char *format, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, format);
	fprintf(stderr, LTINFO(""));
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void print_infol(char *format, unsigned lineno, ...)
{
	if (quiet)
		return;

	va_list ap;
	va_start(ap, lineno);
	fprintf(stderr, LTINFOL(""), lineno);
	vfprintf(stderr, format, ap);
	va_end(ap);
}
