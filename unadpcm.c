static int indexTable[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

static int stepTable[89] = { 
	7,	   8,	  9,	 10,	11,	   12,	  13,	 14,	16,	   17, 
	19,	   21,	  23,	 25,	28,	   31,	  34,	 37,	41,	   45, 
	50,	   55,	  60,	 66,	73,	   80,	  88,	 97,	107,   118, 
	130,   143,	  157,	 173,	190,   209,	  230,	 253,	279,   307,
	337,   371,	  408,	 449,	494,   544,	  598,	 658,	724,   796,
	876,   963,	  1060,	 1166,	1282,  1411,  1552,	 1707,	1878,  2066, 
	2272,  2499,  2749,	 3024,	3327,  3660,  4026,	 4428,	4871,  5358,
	5894,  6484,  7132,	 7845,	8630,  9493,  10442, 11487, 12635, 13899, 
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 
};

inline int clamp(int n, int a, int b) { return n < a ? a : n > b ? b : n; }


void unadpcm(unsigned char *src, unsigned char *dst, int dstlen) {
	unsigned char *dstend = dst + dstlen;
	int stepIndex = 0;
	int predictor = 0;

	if (!dstlen)
		return;

	for (;;) {
		unsigned char b = *src++;
		for (int n = 0; n < 2; n++) {
			unsigned char nibble = n ? b >> 4 : b & 0xf;

			int step = stepTable[stepIndex];

			int diff = step >> 3;
			if (nibble & 1) diff += step >> 2;
			if (nibble & 2) diff += step >> 1;
			if (nibble & 4) diff += step;
			if (nibble & 8) diff = -diff;

			predictor = clamp(predictor + diff, -32768, 32767);
			stepIndex = clamp(stepIndex + indexTable[nibble], 0, (sizeof(stepTable) / sizeof(stepTable[0])) - 1);
			*dst++ = (predictor >> 8) ^ 0x80;
			if (dst == dstend)
				return;
		}
	}
}
