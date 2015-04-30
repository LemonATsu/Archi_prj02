#include <stdio.h>
#include <stdlib.h>
#include "obj.h"
#include <string.h>
void ins_init(struct words *i_img[]); 
void ins_decoder(struct ins* tar, int bits, int offset);
void opn_decoder(char *tar, int opc, int func, int *wb, int *regA, int *regB, int rd, int rs, int rt);
void ins_init(struct words *i_img[]) {

    pc = char_to_num(i_img[0]->machine_code);
    ins_num = char_to_num(i_img[1]->machine_code);

    if(ins_num >((MEMO_LIMIT / 4) - 1) || pc > (MEMO_LIMIT - 4)) {
        //error handle
        printf("error: too many ins\n");
    }

    int x = 0, y = 2 + ins_num, max = MEMO_LIMIT / 4;

    //use for bubble insert
    S_NOP = malloc(sizeof(struct ins));
    ins_decoder(S_NOP, 0, 0);
    strcpy(S_NOP->op_name, "NOP");


    //initialize
    for(x = 0; x < max; x ++) {
        struct ins* i_init = malloc(sizeof(struct ins));
        ins_decoder(i_init, 0, x * 4);
        i_memo[x] = i_init;
    }
    //load iimage content;
    for(x = 2; x < y; x ++) {
        //the first instruction is start at pc(initial value)
        int offset = pc + (x - 2) * 4;
        struct ins* i_init = i_memo[offset / 4];
        ins_decoder(i_init, char_to_num(i_img[x]->machine_code), offset);
    }

}

void ins_decoder(struct ins* tar, int bits, int offset) {
    //divide the machine code, put decode it as a instruction
    tar->pc_addr = offset;
    tar->bits = bits; 
    tar->opcode = (tar->bits >> 26) & 0x0000003f;
    tar->rs = (tar->bits >> 21) & 0x0000001f;
    tar->rt = (tar->bits >> 16) & 0x0000001f;
    tar->rd = (tar->bits >> 11) & 0x0000001f;
    tar->shamt = (tar->bits >> 6) & 0x0000001f;
    tar->func = tar->bits & 0x0000003f;
    tar->c = tar->bits & 0x0000ffff;
    tar->j_label = tar->bits & 0x03ffffff;
    tar->wb = -1;
    tar->fwd_id = 0;
    tar->fwd_ex = 0;
    tar->fwd_to_s = -1;
    tar->fwd_to_t = -1;
    tar->value_s = 0;
    tar->value_t = 0;
}
