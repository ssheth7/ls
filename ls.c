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

/* flags  */
int A_allentries;        /* Includes all files present/previous directory  */
int a_allentries;        /* Includes hidden files  */
int c_lastChanged;       /* Shows last changed  */
int d_directories;       /* Shows directories as plain files */
int F_specialsymbols;    /* Adds symbols to special files */
int f_unsorted;          /* Files are unsorted */
int h_humanreadable;     /* Shows sizes in readable format */
int i_inodes;            /* Shows file inodes */
int k_kilobytes;         /* Shows size in kilobytes*/
int l_longformat;        /* Shows long format */                                   
int n_numericalids;      /* Shows IDs numerically */                               
int q_forcenonprintable; /* Forces nonprintable characters */                      
int R_recurse;           /* Recurses into directories */                           
int r_reverseorder;      /* Reverses output*/                                      
int S_sizesorted;        /* Sorts by size */                                       
int s_systemblocks;      /* Shows number of blocks used  */                        
int t_modifiedsorted;    /* Sort by time modified */                               
int u_lastaccess;        /* Shows time of last access  */                          
int w_forcenonprintable; /* Forces raw nonprintable characters  */ 

int EXIT_STATUS;

int parseargs(int, char*[]);
void printother(char*, struct stat);
void printdir(char*, struct stat);
int main(int, char*[]);

int
parseargs(int argc, char **argv)
{	
	printf("Arguments: %d\n", argc);
	int opt, flagsset;

	flagsset = 0;
	while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
		flagsset = 1;
		switch (opt) {
			case 'A':
				A_allentries = 1;
				break;
			case 'a':
				a_allentries = 1;
				break;
			case 'c':
				c_lastChanged = 1;
				u_lastaccess = 0;
				break;
			case 'd':
				d_directories = 1;
				break;
			case 'F':
				F_specialsymbols = 1;
				break;
			case 'f':
				f_unsorted = 1;
				break;
			case 'h':
				h_humanreadable = 1;
				break;
			case 'i':
				i_inodes = 1;
				break;
			case 'k':
				k_kilobytes = 1;
				break;
			case 'l':
				l_longformat = 1;
				n_numericalids = 0;
				break;
			case 'n':
				n_numericalids = 1;
				l_longformat = 0;
				break;
			case 'q':
				q_forcenonprintable = 1;
				w_forcenonprintable = 0;
				break;
			case 'R':
				R_recurse = 1;
				break;
			case 'r':
				r_reverseorder = 1;
				break;
			case 'S':
				S_sizesorted = 1;
				break;
			case 's':
				s_systemblocks = 1;
				break;
			case 't':
				t_modifiedsorted = 1;
				break;
			case 'u':
				u_lastaccess = 1;
				c_lastChanged = 0;
				break;
			case 'w':
				w_forcenonprintable = 1;
				q_forcenonprintable = 0;
				break;
			default: /* Invalid Argument provided  */
				fprintf(stderr, "%s [-AacdFfhiklnqRrSstuw] [file ...]\n", getprogname());
				exit(EXIT_FAILURE);
		}
	}
	return flagsset;
}

void
printother(char* file, struct stat sb){
	printf("%s %d\n", file, sb.st_mode);

}

void
printdir(char* dir, struct stat sb) {
	printf("%s %d\n", dir, sb.st_mode);
}

int
main(int argc, char **argv) {

	int i, argoffset;
	DIR *dp;
	struct dirent *dirp;
	struct stat sb;
	setprogname(argv[0]);
	
	// validate directory/permissions
	// formatting 
	
	argoffset = 0;
	if (parseargs(argc, argv)) {
		argoffset = 1;
	}
	
	/*if ((dp = opendir(argv[1])) == NULL ) {
		fprintf(stderr, "can't open '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (chdir(argv[1]) == -1) {
		fprintf(stderr, "can't chdir to '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}*/

	EXIT_STATUS = 1;
	for (i = 1 + argoffset; i < argc; i++){
		if (stat(argv[i], &sb) == 0) {
			if (S_ISDIR(sb.st_mode)) {
				printdir(argv[i], sb);
			} else {
				printother(argv[i], sb);
			}
		} else {
			printf("%s: cannot access %s: No such file or directory", getprogname(), argv[i]);
			EXIT_STATUS = EXIT_FAILURE;
		}
	}
	if (i == 1 + argoffset) {
		if (stat(".", &sb)!= 0) {
			printf("%s: cannot access .: No such file or directory", getprogname());
		}
		printdir(".", sb);
	}
	
	exit(EXIT_STATUS);
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
	}
	closedir(dp);
	exit(EXIT_SUCCESS);
}
