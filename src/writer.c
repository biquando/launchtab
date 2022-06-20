#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "launchtab.h"
#include "style.h"
#include "util.h"
#include "writer.h"

static const char *caltype[5] = {
	"Minute",
	"Hour",
	"Day",
	"Month",
	"Weekday"
};

static const char *fdtype[3] = {
	"In",
	"Out",
	"Err"
};

int edit_file(char *path)
{
	char *editor = getenv("EDITOR");
	struct timespec oldtime = {0};
	struct timespec newtime = {0};
	struct stat statbuf;

	if (!editor)
		editor = DEFAULT_EDITOR;

	if (stat(path, &statbuf) == 0)
		oldtime = statbuf.st_mtimespec;

	pid_t child = fork();
	if (child < 0) {
		perror(NULL);
		exit(errno);
	}
	if (child == 0) {
		execlp(editor, editor, path, NULL);
		perror(NULL);
		exit(errno);
	}

	int status;
	if (wait(&status) < 0) {
		perror(NULL);
		exit(errno);
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status)) {
		print_warn("%s exited abnormally\n", editor);
	}

	if (stat(path, &statbuf) == 0)
		newtime = statbuf.st_mtimespec;

	if (oldtime.tv_sec == newtime.tv_sec
			&& oldtime.tv_nsec == newtime.tv_nsec) {
		return 0; /* tab file was not modified */
	}

	return 1;
}

void write_plist(char *path, struct tab *t, struct rule *r)
{
	print_dbg("writing plist: %s\n", path);

	FILE *f = fopen(path, "w");
	if (!f) {
		print_warn("couldn't open file %s\n", path);
		print_info("  skipping rule...\n");
		return;
	}

	/* Look for $SHELL environment variable */
	char *shell = find_value("SHELL", r->varlabels, r->varvalues, r->nvars);
	if (!shell) {
		shell = find_value("SHELL", t->varlabels_glob,
				t->varvalues_glob, t->nvars_glob);
	}
	if (!shell)
		shell = DEFAULT_SHELL;

	fprintf(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\"\n"
		"  \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
		"<plist version=\"1.0\">\n"
		"<dict>\n"
		"    <key>Label</key>\n"
		"    <string>%s</string>\n", r->id);
	fprintf(f,
		"    <key>ProgramArguments</key>\n"
		"    <array>\n"
		"        <string>%s</string>\n"
		"        <string>-c</string>\n"
		"        <string>exec %s</string>\n"
		"    </array>\n", shell, r->command ? r->command : "");

	/* Interval */
	if (r->interval) {
		fprintf(f,
			"    <key>StartInterval</key>\n"
			"    <integer>%s</integer>\n", r->interval);
	}

	/* Single calendar */
	if (r->ncals == 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <dict>\n");
		for (int e = 0; e < 5; e++) {
			if (!r->cal->ent[e])
				continue;
			fprintf(f,
				"        <key>%s</key>\n"
				"        <integer>%s</integer>\n",
				caltype[e], r->cal->ent[e]);
		}
		fprintf(f, "    </dict>\n");
	}

	/* Multiple calendars */
	if (r->ncals > 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <array>\n");
		for (int c = 0; c < r->ncals; c++) {
			fprintf(f, "        <dict>\n");
			for (int e = 0; e < 5; e++) {
				if (!r->cal[c].ent[e])
					continue;
				fprintf(f,
					"            <key>%s</key>\n"
					"            <integer>%s</integer>\n",
					caltype[e], r->cal[c].ent[e]);
			}
			fprintf(f, "        </dict>\n");
		}
		fprintf(f, "    </array>\n");
	}

	/* Environment variables */
	if (r->nvars > 0 || t->nvars_glob > 0) {
		fprintf(f,
			"    <key>EnvironmentVariables</key>\n"
			"    <dict>\n");
	}
	for (int v = 0; v < t->nvars_glob; v++) { /* Global envars */
		/* Only add global variables if they aren't locally set */
		if (!find_value(t->varlabels_glob[v],
		               r->varlabels,
		               r->varvalues,
		               r->nvars)) {
			fprintf(f,
				"        <key>%s</key>\n"
				"        <string>%s</string>\n",
				t->varlabels_glob[v], t->varvalues_glob[v]);
		}
	}
	for (int v = 0; v < r->nvars; v++) { /* Local envars */
		fprintf(f,
			"        <key>%s</key>\n"
			"        <string>%s</string>\n",
			r->varlabels[v], r->varvalues[v]);
	}
	if (r->nvars > 0 || t->nvars_glob > 0)
		fprintf(f, "    </dict>\n");

	/* File descriptors */
	for (int n = 0; n < 3; n++) {
		if (!r->fd[n])
			continue;
		fprintf(f,
			"    <key>Standard%sPath</key>\n"
			"    <string>%s</string>\n",
			fdtype[n], r->fd[n]);
	}

	if (r->verbatim)
		fprintf(f, "%s", r->verbatim);

	fprintf(f, "</dict>\n"
		"</plist>\n");

	free(path);
	fclose(f);

	print_info("Wrote rule: %s\n", r->id);
}


struct tab read_tab(char *path)
{
	struct tab t = {0};
	FILE *f = fopen(path, "r");
	if (f) {
		lex_tab(f, &t);
		fclose(f);
	}

	return t;
}

void debug_tab(struct tab *t)
{
	if (!debug)
		return;

	fprintf(stderr, "\n=================================\n\n");
	for (int v = 0; v < t->nvars_glob; v++) {
		fprintf(stderr, "Global: %s = %s\n",
				t->varlabels_glob[v], t->varvalues_glob[v]);
	}
	for (int i = 0; i < t->nrules; i++) {
		struct rule r = t->rules[i];
		fprintf(stderr, "[%s]\n", r.id);
		fprintf(stderr, "%s\n", r.command);
		if (r.interval)
			fprintf(stderr, "Interval: %s\n", r.interval);
		for (int c = 0; c < r.ncals; c++) {
			fprintf(stderr, "Calendar:");
			for (int e = 0; e < 5; e++) {
				fprintf(stderr, " %s", r.cal[c].ent[e]);
			}
			fprintf(stderr, "\n");
		}
		for (int v = 0; v < r.nvars; v++) {
			fprintf(stderr, "Variable: %s = %s\n",
					r.varlabels[v], r.varvalues[v]);
		}
		fprintf(stderr, "stdin: %s\n", r.fd[0]);
		fprintf(stderr, "stdout: %s\n", r.fd[1]);
		fprintf(stderr, "stderr: %s\n", r.fd[2]);
		fprintf(stderr, "verbatim: %s\n", r.verbatim);
	}
}

void free_tab(struct tab *t)
{
	/* Free rules */
	for (int i = 0; i < t->nrules; i++) {
		struct rule r = t->rules[i];
		free(r.id);
		free(r.command);
		free(r.cal);
		for (int v = 0; v < r.nvars; v++) {
			free(r.varlabels[v]);
			free(r.varvalues[v]);
		}
		free(r.varlabels);
		free(r.varvalues);
		free(r.fd[0]);
		free(r.fd[1]);
		free(r.fd[2]);
		free(r.verbatim);
	}
	free(t->rules);

	/* Free global envars */
	for (int i = 0; i < t->nvars_glob; i++) {
		free(t->varlabels_glob[i]);
		free(t->varvalues_glob[i]);
	}
	free(t->varlabels_glob);
	free(t->varvalues_glob);
}

static char *unload_str =
"'\n[ -z \"$LID\" ] ||"
" launchctl unload \"$HOME/Library/LaunchAgents/$LID.plist\" 2>&1 |"
" grep -q '^Unload failed' &&"
" 1>&- 2>&- launchctl bootout \"gui/$UID/$LID\"";

void uninstall_tab(char *launchpath, struct tab *t)
{
	for (int i = 0; i < t->nrules; i++) {
		struct rule *r = &t->rules[i];

		/* Unload rules */
		char *unload = str_append(NULL, "LID='");
		unload = str_append(unload, r->id);
		unload = str_append(unload, unload_str);
		system(unload);
		free(unload);

		/* Remove plists */
		char *plist = str_append(NULL, launchpath);
		plist = str_append(plist, "/");
		plist = str_append(plist, r->id);
		plist = str_append(plist, ".plist");
		if (unlink(plist) < 0 && errno != ENOENT) {
			perror(NULL);
			exit(errno);
		}
	}
}

static char *load_str =
"'\n1>&- 2>&- launchctl load \"$HOME/Library/LaunchAgents/$LID.plist\"";

void install_tab(char *launchpath, struct tab *t)
{
	debug_tab(t);
	for (int i = 0; i < t->nrules; i++) {
		struct rule *r = &t->rules[i];

		/* Create plists */
		char *plist = str_append(NULL, launchpath);
		plist = str_append(plist, "/");
		plist = str_append(plist, r->id);
		plist = str_append(plist, ".plist");
		write_plist(plist, t, r);

		/* Load rules */
		char *load = str_append(NULL, "LID='");
		load = str_append(load, r->id);
		load = str_append(load, load_str);
		system(load);
		free(load);
	}
}
