/* This still very simple ls clone illustrates the use of the stat(2)
 * family of functions to print out some information about the files in
 * the given directory.
 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ls.h"

int main(int, char*[]);
void parseargs(int, char*[]);

void
parseargs(int argc, char **argv)
{	
	int opt;	
	while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
		switch (opt) {
			
			case 'A':
				break;
			case 'a':
				break;
			case 'c':
				break;
			case 'd':
				break;
			case 'F':
				break;
			case 'f':
				break;
			case 'h':
				break;
			case 'i':
				break;
			case 'k':
				break;
			case 'l':
				break;
			case 'n':
				break;
			case 'q':
				break;
			case 'R':
				break;
			case 'r':
				break;
			case 's':
				break;
			case 't':
				break;
			case 'u':
				break;
			case 'w':
				break;
			default: /* Invalid Argument provided  */
				fprintf(stderr, "%s [-AacdFfhiklnqRrSstuw] [file ...]\n", getprogname());
				exit(EXIT_FAILURE);
		}	
	}
}

int
main(int argc, char **argv) {

	DIR *dp;
	struct dirent *dirp;
	setprogname(argv[0]);

	parseargs(argc, argv);
	exit(EXIT_SUCCESS);
	if ((dp = opendir(argv[1])) == NULL ) {
		fprintf(stderr, "can't open '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (chdir(argv[1]) == -1) {
		fprintf(stderr, "can't chdir to '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	while ((dirp = readdir(dp)) != NULL ) {
		struct stat sb;
		if (stat(dirp->d_name, &sb) == -1) {
			fprintf(stderr, "Can't stat %s: %s\n", dirp->d_name,
						strerror(errno));

			if (lstat(dirp->d_name, &sb) == -1) {
				fprintf(stderr,"Can't stat %s: %s\n", dirp->d_name,
						strerror(errno));
				continue;
			}
		}

		printf("%s: ", dirp->d_name);
		if (S_ISREG(sb.st_mode))
			printf("regular file");
		else if (S_ISDIR(sb.st_mode))
			printf("directory");
		else if (S_ISCHR(sb.st_mode))
			printf("character special");
		else if (S_ISBLK(sb.st_mode))
			printf("block special");
		else if (S_ISFIFO(sb.st_mode))
			printf("FIFO");
		else if (S_ISLNK(sb.st_mode))
			printf("symbolic link");
		else if (S_ISSOCK(sb.st_mode))
			printf("socket");
		else
			printf("unknown");

		printf(" -- according to stat\n");

		if (lstat(dirp->d_name, &sb) == -1) {
			fprintf(stderr,"Can't stat %s: %s\n", dirp->d_name,
						strerror(errno));
			continue;
		}

		printf("%s: ", dirp->d_name);
		if (S_ISREG(sb.st_mode))
			printf("regular file");
		else if (S_ISDIR(sb.st_mode))
			printf("directory");
		else if (S_ISCHR(sb.st_mode))
			printf("character special");
		else if (S_ISBLK(sb.st_mode))
			printf("block special");
		else if (S_ISFIFO(sb.st_mode))
			printf("FIFO");
		else if (S_ISLNK(sb.st_mode))
			printf("symbolic link");
		else if (S_ISSOCK(sb.st_mode))
			printf("socket");
		else
			printf("unknown");

		printf(" -- according to lstat\n\n");
	}

	closedir(dp);
	exit(EXIT_SUCCESS);
}
