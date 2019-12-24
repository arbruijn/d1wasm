#include <emscripten.h>
#include <string.h>
#include <stdlib.h>
#include "bfile.h"

typedef struct bfile {
	int write;
	int pos;
	char *buf;
	int len;
	int size;
	char *fn;
} BFILE;

EM_JS(void *, load_file, (const char *fn, int *size), {
	var mem = Module.HEAPU8, p = fn, a = [], c;
	while ((c = mem[p++]))
		a.push(String.fromCharCode(c));
	var val = loadFile(a.join(""));
	if (!val)
		return 0;
	var retp = Module._malloc(val.length);
	mem.set(val, retp);
	Module.HEAPU32[size>>2] = val.length;
	return retp;
});

EM_JS(void *, save_file, (const char *fn, void *buf, int size), {
	var mem = Module.HEAPU8, p = fn, a = [], c;
	while ((c = mem[p++]))
		a.push(String.fromCharCode(c));
	saveFile(a.join(""), mem.subarray(buf, buf + size));
});

BFILE *bfopen(const char *fn, const char *mode) {
	int write = mode[0] == 'w';
	BFILE *f;
	if (!(f = malloc(sizeof(*f))))
		return NULL;
	f->write = write;
	f->pos = 0;
	if (!write) {
		if (!(f->buf = load_file(fn, &f->len))) {
			free(f);
			return NULL;
		}
	} else {
		if (!(f->buf = malloc(f->size = 4096))) {
			free(f);
			return NULL;
		}
		f->len = 0;
		if (!(f->fn = strdup(fn))) {
			free(f->buf);
			free(f);
			return NULL;
		}
	}
	return f;
}

int bfclose(BFILE *f) {
	if (f->write) {
		save_file(f->fn, f->buf, f->len);
		free(f->fn);
	}
	free(f->buf);
	free(f);
	return 0;
}

int bfread(void *buf, int elmsize, int num, BFILE *f) {
	int left;
	if (f->write)
		return -1;
	num *= elmsize;
	left = f->len - f->pos;
	if (num > left)
		num = left;
	memcpy(buf, f->buf + f->pos, num);
	f->pos += num;
	return num / elmsize;
}

static int bfgrow(BFILE *f, int nsize) {
	char *nbuf;
	if (!(nbuf = realloc(f->buf, nsize)))
		return -1;
	f->buf = nbuf;
	f->size = nsize;
	return 0;
}

int bfwrite(const void *buf, int elmsize, int num, BFILE *f) {
	if (!f->write)
		return -1;
	num *= elmsize;
	if (num + f->pos > f->size) {
		if (bfgrow(f, num + f->pos + f->size))
			return -1;
	}
	memcpy(f->buf + f->pos, buf, num);
	f->pos += num;
	if (f->pos > f->len)
		f->len = f->pos;
	return num / elmsize;
}

int bfseek(BFILE *f, int ofs, int whence) {
	int npos = whence ? whence == 2 ? f->len + ofs : f->pos + ofs : ofs;
	if (npos < 0)
		npos = 0;
	if (npos > f->len) {
		if (f->write) {
			if (npos > f->size && bfgrow(f, npos + f->size))
				return -1;
			memset(f->buf + f->len, 0, npos - f->len);
			f->len = npos;
		} else
			npos = f->len;
	}
	f->pos = npos;
	return 0;
}

int bfeof(BFILE *f) {
	return f->pos == f->len;
}

int bfputs(const char *s, BFILE *f) {
	return bfwrite(s, 1, strlen(s), f);
}

const char *bfgets(char *s, int size, BFILE *f) {
	const char *p;
	int left;
	if (f->write)
		return NULL;
	left = f->len - f->pos;
	if (left > size - 1)
		left = size - 1;
	if ((p = memchr(f->buf + f->pos, '\n', left)))
		left = p - f->buf - f->pos + 1;
	memcpy(s, f->buf + f->pos, left);
	f->pos += left;
	s[left] = 0;
	return s;
}

int bftell(BFILE *f) {
	return f->pos;
}

int bfilelength(BFILE *f) {
	return f->len;
}
