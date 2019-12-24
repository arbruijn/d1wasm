#include "cfile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct CFILE {
	char *data;
	int size;
	int pos;
} CFILE;

typedef struct hogfile {
	char	name[13];
	char *	offset;
	int 	length;
} hogfile;

#define MAX_HOGFILES 250

hogfile HogFiles[MAX_HOGFILES];
char Hogfile_initialized = 0;
int Num_hogfiles = 0;

hogfile AltHogFiles[MAX_HOGFILES];
char AltHogfile_initialized = 0;
int AltNum_hogfiles = 0;
char AltHogFilename[64];

char AltHogDir[64];
char AltHogdir_initialized = 0;

extern char *hog_data, *pig_data;
extern int hog_len, pig_len;

void cfile_init_hogfile(char *fname, hogfile * hog_files, int * nfiles ) {
	char *p = hog_data;
	char *end = hog_data + hog_len;

	if ( strncmp( p, "DHF", 3 ) )	{
		return;
	}
	p += 3;

	while( p < end )	
	{	
		if ( *nfiles >= MAX_HOGFILES ) {
			printf( "ERROR: HOGFILE IS LIMITED TO %d FILES\n",  MAX_HOGFILES );
			return;
		}
		if (p + 13 > end)
			return;
		memcpy(hog_files[*nfiles].name, p, 13 );
		p += 13;
		int len = *((int *)p);
		p += 4;
		if (p + len > end)
			return;
		hog_files[*nfiles].length = len;
		hog_files[*nfiles].offset = p;
		*nfiles = (*nfiles) + 1;
		// Skip over
		p += len;
	}
}

CFILE * cfopen(char * filename, char * mode) {
	if (strcasecmp(filename, "descent.pig") == 0) {
		CFILE *cfile = malloc ( sizeof(CFILE) );
		if ( cfile == NULL )
			return NULL;
		cfile->size = pig_len;
		cfile->data = pig_data;
		cfile->pos = 0;
		return cfile;
	}

	if ( !Hogfile_initialized ) 	{
		Num_hogfiles = 0;
		cfile_init_hogfile( "descent.hog", HogFiles, &Num_hogfiles );
		Hogfile_initialized = 1;
	}

	for (int i=0; i<Num_hogfiles; i++ )	{
		if ( !strcasecmp( HogFiles[i].name, filename ))	{
			CFILE *cfile = malloc ( sizeof(CFILE) );
			if ( cfile == NULL )
				return NULL;
			cfile->size = HogFiles[i].length;
			cfile->data = HogFiles[i].offset;
			cfile->pos = 0;
			return cfile;
		}
	}
	return NULL;

}

int cfilelength( CFILE *fp ) { return fp->size; }
void *cfiledata( CFILE *fp ) { return fp->data; }
size_t cfread( void * buf, size_t elsize, size_t nelem, CFILE * fp ) {
	int len = (int)(elsize * nelem);
	int left = fp->size - fp->pos;
	if (len < 0)
		len = 0;
	if (len > left) {
		len = left;
		nelem = len / elsize;
	}
	memcpy(buf, fp->data + fp->pos, len);
	fp->pos += len;
	return nelem;
}
void cfclose( CFILE * cfile ) { free(cfile); }
int cfgetc( CFILE * fp ) { return fp->pos < fp->size ? fp->data[fp->pos++] : EOF; }
int cfseek( CFILE *fp, long int offset, int where ) {
	int new_pos = (int)offset;
	if (where == SEEK_CUR)
		new_pos += fp->pos;
	else if (where == SEEK_END)
		new_pos += fp->size;
	if (new_pos < 0)
		new_pos = 0;
	if (new_pos > fp->size)
		new_pos = fp->size;
	fp->pos = new_pos;
	return 0;
}
int cftell( CFILE * fp ) { return fp->pos; }
char * cfgets( char * buf, size_t n, CFILE * fp ) {
	char *s = fp->data + fp->pos;
	int left = fp->size - fp->pos;
	if (!left || !n || (left == 1 && *s == '\x1a'))
		return NULL;
	int rd = n >= 2 ? n - 2 : n - 1;
	if (rd >= left) {
		if (s[left - 1] == '\x1a')
			left--;
		rd = left;
	}
	char *p = memchr(s, '\n', rd);
	if (!p && rd < left) {
		if (s[rd] == '\n')
			p = s + rd;
		else if (s[rd] == '\r' && rd + 1 < left && s[rd + 1] == '\n')
			p = s + rd + 1;
	}
	if (p) {
		rd = p - s;
		fp->pos += rd + 1;
		if (p[-1] == '\r')
			rd--;
	} else
		fp->pos += rd;
	memcpy(buf, s, rd);
	if (rd < left)
		buf[rd++] = '\n';
	buf[rd] = 0;
	return buf;
}

int cfexist( char * filename );	// Returns true if file exists on disk (1) or in hog (2).

// Allows files to be gotten from an alternate hog file.
// Passing NULL disables this.
void cfile_use_alternate_hogfile( char * name ) { }

// All cfile functions will check this directory if no file exists
// in the current directory.
void cfile_use_alternate_hogdir( char * path ) { }

