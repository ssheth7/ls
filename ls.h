#include <sys/stat.h>

int parseargs(int, char*[]);
void splitargs(int, char*[], int);
void formatnondir(char*, struct stat);
void formatdir(char*, struct stat);
void cleanup();
int main(int, char*[]);

struct fileentry;
