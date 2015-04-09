#include <stdio.h>
#include <stdlib.h>
#include "obj.h"

void reg_init();
int reg_read(int tar);
void reg_write(int tar, int data);

void reg_init() {
    int x;
    for(x = 0; x < 32; x ++) {
        reg[x] = 0x00000000;
    }
}

int reg_read(int tar) {
    return reg[tar];
}

void reg_write(int tar, int data) {
    if(tar == 0) {
        //error write to $0
        write_to_zero();

    } else {
        reg[tar] = data;
    }
}
