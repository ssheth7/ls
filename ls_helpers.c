#include <string.h>

#include "ls_helpers.h"

int 
lexicosort(const void* str1, const void* str2) {
	return strcmp(*(const char **) str1, *(const char **) str2);
}
