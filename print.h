#ifndef _PRINT_H_
#define _PRINT_H_

#include <sys/stat.h>

void addsymbols_F(char**, struct stat);
void printdefault(char*);
void printall(char*);
void printraw_q(char*);
void printinode_i(struct stat);
void printblocks_s(int, int, int);
void printlong_l(char*, char*, struct stat);
#endif /* !_PRINT_H_ */

