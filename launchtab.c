#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_EDITOR "nano"
#define CONFIG ".config/"
#define TABDIR "launchtab/"
#define TAB    "launch.tab"
#define TABPATH CONFIG TABDIR TAB

void make_dirs(const char *home)
{
	char *path;

	path = malloc(strlen(home) + sizeof "/" CONFIG TABDIR);
	if (!path)
		exit(6);

	strcpy(path, home);

	strcpy(path + strlen(home), "/" CONFIG);
	mkdir(path, 0755);

	strcpy(path + strlen(home) + sizeof "/" CONFIG - 1, TABDIR);
	mkdir(path, 0755);

	free(path);
}

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
	if (child < 0)
		exit(1);
	if (child == 0) {
		execlp(editor, editor, file, NULL);
		exit(2);
	}
	wait(NULL);

	if (stat(file, &statbuf) == 0)
		newtime = statbuf.st_mtimespec;

	if (oldtime.tv_sec == newtime.tv_sec
			&& oldtime.tv_nsec == newtime.tv_nsec) {
		return -1; /* tab file was not modified */
	}

	return open(file, O_RDONLY);
}

int main(void)
{
	int fd;
	char *home = getenv("HOME");
	char *tabpath;

	if (!home)
		return 5;

	tabpath = malloc(strlen(home) + sizeof "/" TABPATH);
	strcpy(tabpath, home);
	strcpy(tabpath + strlen(home), "/" TABPATH);

	make_dirs(home);
	fd = edit_file(tabpath);
	if (fd < 0)
		return 0;

	close(fd);
	free(tabpath);
	write(1, "file was modified\n", 18);
	return 0;
}
