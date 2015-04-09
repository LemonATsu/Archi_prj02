#include <stdio.h>
#include <stdlib.h>

int main(void) {
    short x = 0xf000;
    int c= x << 31;
    printf("%d %d %x %x\n", c, x, c, x);
    write(x, c);
}
