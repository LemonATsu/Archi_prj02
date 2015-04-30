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
    for(i = 0; i < 32; i ++) reg[i] = 0x00000000;
    reg[_sp] = sp;
}

int reg_read(int type, int tar) {
    int result;
    if(type ==CPU_REG) result = reg[tar];
    else if(type == EX_ME) result = reg_EX_ME;
    else if(type == ME_WB) result = reg_ME_WB;

    return result;
}

void reg_write(int type, int tar, int data) {

    if(type == CPU_REG)
        if(tar == 0) write_to_zero();
        else reg[tar] = data;
    else if(type == EX_ME) reg_EX_ME = data;
    else if(type == ME_WB) reg_ME_WB = data;

}
