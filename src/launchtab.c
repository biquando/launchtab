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
static char *home;        /*  ~                               */
static char *tabpath;     /*  ~/.config/launchtab/launch.tab  */
static char *launchpath;  /*  ~/Library/LaunchAgents          */

static void _install_file(char *path)
{
	/* Read new */
	struct tab newtab = read_tab(path);

	/* Check vaildity */
	if (newtab.invalid) {
		print_err("Aborting. Check %s\n", path);
		exit(EINVAL);
	}

	/* Read old */
	struct tab oldtab = read_tab(tabpath);

	/* Copy file */
	FILE *newfile = fopen(path, "r");
	FILE *oldfile = fopen(tabpath, "w");
	if (!newfile || !oldfile || cpfile(newfile, oldfile) < 0) {
		perror(NULL);
		exit(errno);
	}
	fclose(newfile);
	fclose(oldfile);

	/* Uninstall old */
	uninstall_tab(launchpath, &oldtab);

	/* Install new */
	install_tab(launchpath, &newtab);

	/* Free tabs */
	free_tab(&oldtab);
	free_tab(&newtab);
}

static void _import_tab(char *path)
{
	FILE *tmpf = NULL;

	if (!path) {
		path = str_append(NULL, home);
		path = str_append(path, "/.config/launchtab/temp.XXXX");
		path = mktemp(path);
		tmpf = fopen(path, "w");
		if (!tmpf || cpfile(stdin, tmpf) < 0) {
			perror(NULL);
			exit(errno);
		}
		fflush(tmpf);
	}

	_install_file(path);

	if (tmpf) {
		fclose(tmpf);
		remove(path);
		free(path);
	}
}

static void _edit_tab()
{
	char *path = str_append(NULL, home);
	path = str_append(path, "/.config/launchtab/temp.XXXX");
	path = mktemp(path);
	FILE *tmpf = fopen(path, "w");
	FILE *tabf = fopen(tabpath, "r");
	if (!tabf || !tmpf || cpfile(tabf, tmpf) < 0) {
		perror(NULL);
		exit(errno);
	}
	fflush(tmpf);
	fclose(tabf);

	if (edit_file(path))
		_install_file(path);
	else
		print_info("no changes made\n");

	fclose(tmpf);
	remove(path);
	free(path);
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
	struct tab t = read_tab(tabpath);
	uninstall_tab(launchpath, &t);
	free_tab(&t);
	if (unlink(tabpath) < 0) {
		perror(NULL);
		exit(errno);
	}
}

int main(int argc, char *argv[])
{
	home = getenv("HOME");
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
