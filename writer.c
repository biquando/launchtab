#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "launchtab.h"
#include "style.h"
#include "writer.h"
#include "util.h"

const char *caltype[5] = {
	"Minute",
	"Hour",
	"Day",
	"Month",
	"Weekday"
};

const char *fdtype[3] = {
	"In",
	"Out",
	"Err"
};

int edit_file(const char *file)
{
	char *editor = getenv("EDITOR");
	struct timespec oldtime = {0};
	struct timespec newtime = {0};
	struct stat statbuf;

	if (!editor)
		editor = DEFAULT_EDITOR;

	if (stat(file, &statbuf) == 0)
		oldtime = statbuf.st_mtimespec;

	pid_t child = fork();
	if (child < 0) {
		perror(NULL);
		exit(errno);
	}
	if (child == 0) {
		execlp(editor, editor, file, NULL);
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

	if (stat(file, &statbuf) == 0)
		newtime = statbuf.st_mtimespec;

	if (oldtime.tv_sec == newtime.tv_sec
			&& oldtime.tv_nsec == newtime.tv_nsec) {
		return 0; /* tab file was not modified */
	}

	return 1;
}

void write_plist(char *launchpath, struct rule r)
{
	if (!r.id || !r.command) {
		print_warn("rule is missing a command: %s\n", r.id);
		print_info("  skipping rule...\n");
		return;
	}

	char *path = try_malloc(strlen(launchpath) + strlen(r.id)
			+ sizeof ".plist");
	strcpy(path, launchpath);
	strcpy(path + strlen(path), "/");
	strcpy(path + strlen(path), r.id);
	strcpy(path + strlen(path), ".plist");

	print_dbg("path: %s\n", path);

	FILE *f = fopen(path, "w");
	if (!f) {
		print_warn("couldn't open file %s\n", path);
		print_info("  skipping rule...\n");
		return;
	}

	fprintf(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\"\n"
		"  \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
		"<plist version=\"1.0\">\n"
		"<dict>\n"
		"    <key>Label</key>\n"
		"    <string>%s</string>\n", r.id);
	fprintf(f,
		"    <key>ProgramArguments</key>\n"
		"    <array>\n"
		"        <string>/bin/bash</string>\n"
		"        <string>-c</string>\n"
		"        <string>exec %s</string>\n"
		"    </array>\n", r.command);

	/* Interval */
	if (r.interval) {
		fprintf(f,
			"    <key>StartInterval</key>\n"
			"    <integer>%s</integer>\n", r.interval);
	}

	/* Single calendar */
	if (r.ncal == 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <dict>\n");
		for (int e = 0; e < 5; e++) {
			if (!r.cal->ent[e])
				continue;
			fprintf(f,
				"        <key>%s</key>\n"
				"        <integer>%s</integer>\n",
				caltype[e], r.cal->ent[e]);
		}
		fprintf(f, "    </dict>\n");
	}

	/* Multiple calendars */
	if (r.ncal > 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <array>\n");
		for (int c = 0; c < r.ncal; c++) {
			fprintf(f, "        <dict>\n");
			for (int e = 0; e < 5; e++) {
				if (!r.cal[c].ent[e])
					continue;
				fprintf(f,
					"            <key>%s</key>\n"
					"            <integer>%s</integer>\n",
					caltype[e], r.cal[c].ent[e]);
			}
			fprintf(f, "        </dict>\n");
		}
		fprintf(f, "    </array>\n");
	}

	/* Environment variables */
	if (r.nvar > 0)
		fprintf(f,
			"    <key>EnvironmentVariables</key>\n"
			"    <dict>\n");
	for (int v = 0; v < r.nvar; v++) {
		fprintf(f,
			"        <key>%s</key>\n"
			"        <string>%s</string>\n",
			r.varlabels[v], r.varvalues[v]);
	}
	if (r.nvar > 0)
		fprintf(f, "    </dict>\n");

	/* File descriptors */
	for (int n = 0; n < 3; n++) {
		if (!r.fd[n])
			continue;
		fprintf(f,
			"    <key>Standard%sPath</key>\n"
			"    <string>%s</string>\n",
			fdtype[n], r.fd[n]);
	}

	if (r.verbatim)
		fprintf(f, "%s", r.verbatim);

	fprintf(f, "</dict>\n"
		"</plist>\n");

	free(path);
	fclose(f);

	print_info("Wrote rule: %s\n", r.id);
}
