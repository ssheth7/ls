#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <sys/stat.h>

#include <fts.h>
#include <limits.h>
#include <string.h>


typedef struct paddings {
	int blockpadding;
	int grouppadding;
	int inodepadding;
	int linkpadding;
	int sizepadding;
	int userpadding;
} paddings;

void getimmediatechildren(FTSENT*, int, struct paddings*);
int countblocks(FTSENT*, struct paddings*);
void getpaddingsizes(struct stat, struct paddings*);
int countdigits(long);

int lexicosort(const void*, const void*);

int fts_lexicosort(const FTSENT** file1, const FTSENT** file2);
int fts_rlexicosort(const FTSENT** file1, const FTSENT** file2);

int fts_sizesort(const FTSENT** file1, const FTSENT** file2);
int fts_rsizesort(const FTSENT** file1, const FTSENT** file2);

int fts_timemodifiedsort(const FTSENT** file1, const FTSENT** file2);
int fts_rtimemodifiedsort(const FTSENT** file1, const FTSENT** file2);

int fts_lastaccesssort(const FTSENT** file1, const FTSENT** file2);
int fts_rlastaccesssort(const FTSENT** file1, const FTSENT** file2);

int fts_statuschangesort(const FTSENT** file1, const FTSENT** file2);
int fts_rstatuschangesort(const FTSENT** file1, const FTSENT** file2);

#endif /* !_HELPERS_H_  */
