#ifndef _PRINT_H_
#define _PRINT_H_

#include <sys/stat.h>

void addsymbols_F(char**, struct stat);
void printdefault(char*);
void printall(char*);
void printraw_q(char*);
void printraw_w(char*);
void printinode_i(struct stat);
void printblocks_s(struct stat, int, int);

#endif /* !_PRINT_H_ */

