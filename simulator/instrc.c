#include <stdio.h>
#include <stdlib.h>
#include "obj.h"

void ins_init(struct words *i_img[]); 

void ins_init(struct words *i_img[]) {
    pc = char_to_num(i_img[0]->machine_code);
    ins_num = char_to_num(i_img[1]->machine_code);
    //printf("pc: 0x%08x, #of ins: 0x%08x\n", pc, ins_num);
    
    if(ins_num >((MEMO_LIMIT / 4) - 1) || pc > (MEMO_LIMIT - 4)) {
        //error handle
        printf("error: too many ins\n");
    }

    int x = 0, y = 2 + ins_num, max = MEMO_LIMIT / 4;
    //initialize
    for(x = 0; x < max; x ++) {
        struct ins* i_init = malloc(sizeof(struct ins));
        i_init->pc_addr = x * 4;
        i_init->bits = 0x00000000;
        i_memo[x] = i_init;
    }
    //load iimage contend;
    for(x = 2; x < y; x ++) {
        //the first instruction is start at pc(initial value)
        int offset = pc + (x - 2) * 4;
        struct ins* i_init = i_memo[offset / 4];
        i_init->pc_addr = offset;
        i_init->bits = char_to_num(i_img[x]->machine_code); 
    }
    /* 
    for(x = 0; x < MEMO_LIMIT / 4; x ++) {
        struct ins* i_check = i_memo[x];
        printf("PC: 0x%08x(%d), bits: 0x%08x\n", i_check->pc_addr, x, i_check->bits);

    }*/
}
