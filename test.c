#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

extern __declspec(dllimport) int __stdcall VeryClassicInit(intptr_t tex);
extern __declspec(dllimport) void __stdcall VeryClassicFrame(int kc, int ka);

int main() {
    uint32_t *buf = malloc(320 * 200 * 4);
    int ret = VeryClassicInit((intptr_t)buf);
    printf("ret %d\n", ret);
    fflush(stdout);
    for (int i = 0; i < 10000; i++) {
        VeryClassicFrame(32,32);
        printf(".");
        fflush(stdout);
    }
}
