#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "launchtab.h"
#include "options.h"
#include "style.h"
#include "util.h"
#include "writer.h"

int debug;
int quiet = 0;
static char *tabpath;     /*  ~/.config/launchtab/launch.tab  */
static char *launchpath;  /*  ~/Library/LaunchAgents          */

static void _install_tab()
{
	struct tab t = read_tab(tabpath);
	install_tab(launchpath, &t);
	free_tab(&t);
}

static void _import_tab(char *path)
{
	FILE *fd = path ? fopen(path, "r") : tmpfile();
	if (!fd) {
		perror(NULL);
		exit(errno);
	}

	if (!path && cpfile(stdin, fd) < 0) {
		perror(NULL);
		exit(errno);
	}

	FILE *tabfd = fopen(tabpath, "w");
	if (!tabfd) {
		perror(NULL);
		exit(errno);
	}
	if (cpfile(fd, tabfd) < 0) {
		perror(NULL);
		exit(errno);
	}

	fclose(fd);
	fclose(tabfd);

	_install_tab();
}

static void _edit_tab()
{
	if (!edit_file(tabpath)) {
		print_info("no changes made\n");
		return;
	}
	_install_tab();
}

static void _list_tab()
{
	FILE *fd = fopen(tabpath, "r");
	if (!fd && errno == ENOENT) {
		if (errno == ENOENT)
			print_err("user does not have a launchtab\n");
		else
			perror(NULL);
		exit(errno);
	}

	if (cpfile(fd, stdout) < 0) {
		perror(NULL);
		exit(errno);
	}

	fclose(fd);
}

static void _remove_tab()
{
	if (unlink(tabpath) < 0) {
		perror(NULL);
		exit(errno);
	}
}

int main(int argc, char *argv[])
{
	char *home = getenv("HOME");

	if (!home) {
		print_err("missing $HOME variable.\n");
		exit(ENOENT);
	}

	tabpath = str_append(NULL, home);
	tabpath = str_append(tabpath, "/.config/launchtab/launch.tab");

	launchpath = str_append(NULL, home);
	launchpath = str_append(launchpath, "/Library/LaunchAgents");

	/* mkdir_p: dirname(tabpath) and launchpath */
	size_t tabdirlen = dirname(tabpath, NULL);
	char tmpc = tabpath[tabdirlen];
	tabpath[tabdirlen] = '\0';
	if (mkdir_p(launchpath) < 0) {
		print_err("couldn't create directory: %s\n", launchpath);
		perror(NULL);
		exit(errno);
	}
	if (mkdir_p(tabpath) < 0) {
		print_err("couldn't create directory: %s\n", tabpath);
		perror(NULL);
		exit(errno);
	}
	tabpath[tabdirlen] = tmpc;

	struct taboptions opts = parseopts(argc, argv);
	argc = opts.argc;
	argv = opts.argv;
	debug = opts.debug;

	switch (opts.op) {
	case IMTAB:
		_import_tab(argc > 0 ? argv[0] : NULL);
		break;
	case EDTAB:
		_edit_tab();
		break;
	case LSTAB:
		_list_tab();
		break;
	case RMTAB:
		_remove_tab();
		break;
	default:
		exit(EINVAL);
	}

	free(tabpath);
	free(launchpath);
	return 0;
}
