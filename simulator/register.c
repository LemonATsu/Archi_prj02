#include "register.h"

void reg_init() {
    int i;
    for(i = 0; i < 32; i ++) {
        reg_IF_ID[i] = 0x00000000;
        reg_ID_EX[i] = 0x00000000;
        reg_EX_ME[i] = 0x00000000;
        reg_ME_WB[i] = 0x00000000;
        reg[i] = 0x00000000;
    }
}

int reg_read(int type, int tar) {
    int result;
    switch(type) {
        case CPU_REG:
            result = reg[tar];
            break;
        case IF_ID:
            result = reg_IF_ID[tar];
            break;
        case ID_EX:
            result = reg_ID_EX[tar];
            break;
        case EX_ME:
            result = reg_EX_ME[tar];
            break;
        case ME_WB:
            result = reg_ME_WB[tar];
            break;
    }

    return result;
}

void reg_write(int type, int tar, int data) {
}
