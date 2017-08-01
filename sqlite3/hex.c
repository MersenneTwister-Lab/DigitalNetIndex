#include <stdio.h>
#include <stdint.h>

static uint8_t table[256];

int main()
{
    for (int i = 0; i < 9; i++) {
        table[i + '0'] = i;
    }
    for (int i = 0; i < 6; i++) {
        table[i + 'a'] = i + 10;
        table[i + 'A'] = i + 10;
    }
    printf("{");
    for (int i = 0; i < 256; i++) {
        printf("%d, ", table[i]);
    }
    printf("};\n");
}
