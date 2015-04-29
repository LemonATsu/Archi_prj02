#include <stdio.h>
#include <stdlib.h>
#include "register.h"
#include "execute.h"
#include "obj.h"
void reg_init() {
    int i;
    reg_IF_ID = 0x00000000;
    reg_ID_EX = 0x00000000;
    reg_EX_ME = 0x00000000;
    reg_ME_WB = 0x00000000;
    for(i = 0; i < 32; i ++) {
        reg[i] = 0x00000000;
        reg_temp[i] = 0x00000000;
    }
    reg[_sp] = sp;
}

void reg_copy() {
    int i;
    for(i = 0; i < 32; i ++) {
        reg_temp[i] = reg[i];
    }
}

int reg_read(int type, int tar) {
    int result;
    switch(type) {
        case CPU_REG:
            result = reg[tar];
            break;
        case EX_ME:
            result = reg_EX_ME;
            break;
        case ME_WB:
            result = reg_ME_WB;
            break;
    }

    return result;
}

void reg_write(int type, int tar, int data) {
    switch(type) {
        case CPU_REG:
            if(tar == 0) write_to_zero();
            else reg[tar] = data;
            break;
        case EX_ME:
            reg_EX_ME = data;
            break;
        case ME_WB:
            reg_ME_WB = data;
            break;
    }
}
