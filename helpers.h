#include <sys/stat.h>

#include <fts.h>
#include <limits.h>
#include <string.h>

int lexicosort(const void*, const void*);

int fts_lexicosort(const FTSENT** file1, const FTSENT** file2);
int fts_rlexicosort(const FTSENT** file1, const FTSENT** file2);

int fts_sizesort(const FTSENT** file1, const FTSENT** file2);
int fts_rsizesort(const FTSENT** file1, const FTSENT** file2);

int fts_timemodifiedsort(const FTSENT** file1, const FTSENT** file2);
int fts_rtimemodifiedsort(const FTSENT** file1, const FTSENT** file2);

int fts_lastaccesssort(const FTSENT** file1, const FTSENT** file2);
int fts_rlastaccesssort(const FTSENT** file1, const FTSENT** file2);
