int unlink(const char *path);
//int stricmp(const char *a, const char *b);
//int strnicmp(const char *a, const char *b, int n);
#define isatty(fd) 0
int filelength(int fd);
void _makepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext);
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
