/*
 * Contains print helper functions for ls.c
*/

#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "print.h"


/* flags  */
extern int A_allentries;        /* Includes all files but current/previous directory  */
extern int a_allentries;        /* Includes hidden files  */
extern int c_lastchanged;       /* Shows last changed  */
extern int d_directories;       /* Shows directories as plain files */
extern int F_specialsymbols;    /* Adds symbols to special files */
extern int f_unsorted;          /* Files are unsorted */
extern int h_humanreadable;     /* Shows sizes in readable format */
extern int i_inodes;            /* Shows file inodes */
extern int k_kilobytes;         /* Shows size in kilobytes*/
extern int l_longformat;        /* Shows long format */                                   
extern int n_numericalids;      /* Shows IDs numerically */                               
extern int q_forcenonprintable; /* Forces nonprintable characters */                      
extern int R_recurse;           /* Recurses into directories */                           
extern int r_reverseorder;      /* Reverses output*/                                      
extern int S_sizesorted;        /* Sorts by size */                                       
extern int s_systemblocks;      /* Shows number of blocks used  */                        
extern int t_modifiedsorted;    /* Sort by time modified */                               
extern int u_lastaccess;        /* Shows time of last access  */                          
extern int w_forcenonprintable; /* Forces raw nonprintable characters  */ 

extern int BLOCKSIZE;
extern int EXIT_STATUS;

#define CEIL(w, x, y) (1 + (w - 1)/(x / y))

void
addsymbols_F(char** entry, struct stat sb)
{
	int entrymode;
	char appended;
	char* modifiedentry;
	
	if ((modifiedentry = malloc(strlen(*entry) + 2)) == NULL) {
		(void)fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
	}
	
	entrymode = sb.st_mode;
			
	if (S_ISDIR(entrymode)) {
		appended = '/';
	} else if (S_ISLNK(entrymode)) {
		appended = '@';
	} else if (S_ISSOCK(entrymode)) {
		appended = '=';
	} else if(S_ISFIFO(entrymode)) {
		appended = '|';
	} else if (S_ISWHT(entrymode)) {
		appended = '%';
	} else if (S_IEXEC & entrymode) {
		appended = '*';
	} else {
		free(modifiedentry);
		return;
	}
	snprintf(modifiedentry, strlen(*entry) + 2, "%s%c", *entry, appended);
	*entry = modifiedentry;
	free(modifiedentry);
}

void
printdefault(char* entry)
{
	(void)printf("%s\n", entry);
}

void
printraw_q(char* entry)
{
	int index;

	index = 0;

	while(entry[index]) {
		if (isprint(entry[index])) {
			(void)printf("%c", entry[index]);
		} else {
			(void)printf("?");
		}
		index++;
	}
	(void)printf("\n");
}

void
printraw_w(char* entry)
{
	int index;

	index = 0;
	while(entry[index]) {
		if (isprint(entry[index])) {
			(void)printf("%c", entry[index]);
		} else {
			(void)printf("%c", entry[index]);
		}
		index++;
	}
	(void)printf("\n");
}

void
printinode_i(struct stat sb) 
{
	(void)printf("%ld ", sb.st_ino);
}

void
printblocks_s(int blocks, int size, int isEntry)
{
	int humanizeflags;
	char buf[6];
	if (!h_humanreadable) {
		if (isEntry == 1) {
			(void)printf("%d ", CEIL(blocks, BLOCKSIZE, 512));
		} else {
			(void)printf("total %d\n", CEIL(blocks, BLOCKSIZE, 512));
		}
		return;	
	}
	
	humanizeflags = HN_DECIMAL | HN_B | HN_NOSPACE;
	if (humanize_number(buf, sizeof(buf), size , " ",  HN_AUTOSCALE, humanizeflags) == -1) {
		(void)fprintf(stderr, "Humanize function failed: %s\n", strerror(errno)); 
		EXIT_STATUS = EXIT_FAILURE;
	}
	if (isEntry == 1) {
		(void)printf("%s ", buf);
	} else {
		(void)printf("total %s\n", buf);
	}
}

// Edit for n option
// Edit for h, k options
// Edit for c, u options
// Make print pretty
void
printlong_l(char* entry, struct stat sb) 
{
	int entrymode, numlinks, currentyear;
	char permbuf[12];
	char timebuf[13];
	time_t filetime;
	uid_t uid;
	gid_t gid;
	struct tm *currenttime;
	struct group *group;
	struct passwd *passwd;
	
	entrymode = sb.st_mode;
	(void)strmode(entrymode, permbuf);
	permbuf[11] == '\0';	
	numlinks = sb.st_nlink;
	uid = sb.st_uid;
	gid = sb.st_gid;

	if ((group = getgrgid(gid)) == NULL) {
		(void)fprintf(stderr, "Could not get guid of %s: %s\n", strerror(errno));
		EXIT_STATUS = EXIT_FAILURE;
	} 
	if ((passwd = getpwuid(uid)) == NULL) {
		(void)fprintf(stderr, "Could not get pid of %s: %s\n", strerror(errno));
		EXIT_STATUS = EXIT_FAILURE;
	}

	filetime = time(NULL);
	currenttime = localtime(&filetime);
	currentyear = currenttime->tm_year + 1900;
	
	if (c_lastchanged) {
		filetime = sb.st_ctime;
	} else if (u_lastaccess) {
		filetime = sb.st_atime;
	} else {
		filetime = sb.st_mtime;
	
	}
	
	if (strftime(timebuf, sizeof(timebuf), "%b %d %k:%M", localtime(&filetime)) == 0) {
		fprintf(stderr, "Could not format date of %s.\n", entry); 
		EXIT_STATUS = EXIT_FAILURE;
	}
	timebuf[12] = '\0';
	printf("%s %d %s %s %d %s %s\n", 
	permbuf, numlinks, passwd->pw_name, group->gr_name, sb.st_size, timebuf, entry);

	

}
