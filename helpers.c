/*
 * Contains sorting helper methods for ls.c
*/
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"
#include "ls.h"


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


void
getimmediatechildren(FTSENT* children, int parentlevel, struct paddings *paddings) 
{
	while(children != NULL) {
		if (children->fts_level == parentlevel + 1) {
			formatentry(0, children->fts_name, children->fts_path,
			  *children->fts_statp, *paddings);
		}
		children = children->fts_link;
	}
}

int 
countblocks(FTSENT *children, struct paddings *paddings)
{
	int blocks, blocksum, printallflags, size, total;
	
	printallflags = A_allentries || a_allentries; 
	blocksum = 0;
	size = 0;
	total = 0;

	while (children != NULL) {
		while(children != NULL) {
			if (printallflags || strncmp(children->fts_name, ".", 1) != 0) {
				
				blocks = children->fts_statp->st_blocks;
				size = children->fts_statp->st_size;	
				total += size;
				blocksum += blocks;
	
				getpaddingsizes(*children->fts_statp, paddings); 
			}
			children = children->fts_link;
		}
	}
	if (h_humanreadable == 1) {
		return total;
	}
	return blocksum;
}

void
getpaddingsizes(struct stat sb, struct paddings *paddings) 
{
	int digitlen, gid, links, uid;
	blkcnt_t blocks;
	ino_t inode;
	off_t size;
	struct group *group;
	struct passwd *passwd;

	blocks = sb.st_blocks;
	size = sb.st_size;
	uid = sb.st_uid;
	gid = sb.st_gid;
	inode = sb.st_ino;
	links = sb.st_nlink;
	digitlen = 0;
	
	digitlen = countdigits(blocks);
	if (digitlen > paddings->block) {
		paddings->block = digitlen;
	}
	digitlen = countdigits(size);
	if (digitlen > paddings->size) {
		paddings->size = digitlen;
	}
	digitlen = countdigits(links);
	if (digitlen > paddings->link) {
		paddings->link = digitlen;
	}
	if (n_numericalids == 1 || (passwd = getpwuid(uid)) == NULL) {
		digitlen = countdigits(uid);
	} else {
		digitlen = strlen(passwd->pw_name);
	}
	if (digitlen > paddings->user) {
		paddings->user = digitlen;
	}

	if (n_numericalids == 1 || (group = getgrgid(gid)) == NULL) {
		digitlen = countdigits(gid);	
	} else {
		digitlen = strlen(group->gr_name);

	}

	if (digitlen > paddings->group) {
		paddings->group = digitlen;
	}
	
	digitlen = countdigits(inode);
	if (digitlen > paddings->inode) {
		paddings->inode = digitlen;
	}
}
int
countdigits(long digit)
{
	int numdigits;

	numdigits = 0;
	while (digit > 0){
		numdigits++;
		digit/=10;
	}
	return numdigits;
}
int
lexicosort(const void* str1, const void* str2) 
{
	return strcmp(*(const char **) str1, *(const char **) str2);
}

int 
fts_lexicosort(const FTSENT** file1, const FTSENT** file2)
{
	
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int 
fts_rlexicosort(const FTSENT** file1, const FTSENT** file2)
{
	
	return -1 * (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int 
fts_sizesort(const FTSENT** file1, const FTSENT** file2)
{
	off_t file1size = (*file1)->fts_statp->st_size;
	off_t file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return 1;
	} 
	if (file1size > file2size) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int 
fts_rsizesort(const FTSENT** file1, const FTSENT** file2)
{
	off_t file1size = (*file1)->fts_statp->st_size;
	off_t file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return -1;
	} 
	if (file1size > file2size) {
		return 1;
	}
	return fts_rlexicosort(file1, file2);
}

int
fts_timemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_mtime;
	time_t file2time = (*file2)->fts_statp->st_mtime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rtimemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_mtime;
	time_t file2time = (*file2)->fts_statp->st_mtime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_lastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_atime;
	time_t file2time = (*file2)->fts_statp->st_atime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rlastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_atime;
	time_t file2time = (*file2)->fts_statp->st_atime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return 0;
}

int
fts_statuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_ctime;
	time_t file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rstatuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_ctime;
	time_t file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return fts_lexicosort(file1, file2);
}
