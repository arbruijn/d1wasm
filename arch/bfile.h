typedef struct bfile BFILE;

BFILE *bfopen(const char *, const char *);
int bfclose(BFILE *);
int bfread(void *, int, int, BFILE *);
int bfwrite(const void *, int, int, BFILE *);
int bfseek(BFILE *, int, int);

int bfeof(BFILE *f);
int bfputs(const char *s, BFILE *f);
const char *bfgets(char *s, int size, BFILE *f);
int bftell(BFILE *f);
int bfilelength(BFILE *f);

