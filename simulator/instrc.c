#include <stdio.h>
#include <stdlib.h>
#include "obj.h"
#include <string.h>
#define _addi 0x08
#define _lw 0x23
#define _lh 0x21
#define _lhu 0x25
#define _lb 0x20
#define _lbu 0x24
#define _sw 0x2B
#define _sh 0x29
#define _sb 0x28
#define _lui 0x0F
#define _andi 0x0C
#define _ori 0x0D
#define _nori 0x0E
#define _slti 0x0A
#define _beq 0x04
#define _bne 0x05
#define _j 0x02
#define _jal 0x03
#define _halt 0x3F


//    func:
#define _add 0x20
#define _sub 0x22
#define _and 0x24
#define _or 0x25
#define _xor 0x26
#define _nor 0x27
#define _nand 0x28
#define _slt 0x2A
#define _sll 0x00
#define _srl 0x02
#define _sra 0x03
#define _jr 0x08
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
}
