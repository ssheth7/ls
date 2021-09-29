#include <sys/stat.h>

#include <limits.h>
#include <string.h>

struct file {
	char entryname[PATH_MAX] ;
	struct stat sb;
};

int lexicosort(const void*, const void*);
