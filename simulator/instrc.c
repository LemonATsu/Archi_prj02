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
void opn_decoder(char *tar, int opc, int func, int *wb, int *regA, int *regB, int rd, int rs, int rt);
void ins_decoder(struct ins* tar, int bits, int offset);

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
        printf("%d %d\n", i_init->ID_regA, i_init->ID_regB);
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
    tar->ID_regA = -1;
    tar->ID_regB = -1;
    opn_decoder(tar->op_name, tar->opcode, tar->func, &(tar->wb), 
              &(tar->ID_regA), &(tar->ID_regB), tar->rd, tar->rs, tar->rt);

    //double check if it's NOP or not(NOP not always be 0x00000000)
    if(tar->opcode == 0 && tar->func == 0 && tar->rt == 0 && tar->rd == 0)
        strcpy(tar->op_name, "NOP");
}
void opn_decoder(char* tar, int opc, int func, int *wb, 
                 int *regA, int *regB, int rd, int rs, int rt) {
    //give the correspondence opcode/function name to the instruction
    //also, will determine the write back register here.
    if(opc) {
        *wb = rt;
        *regA = rs;
        if(opc == _addi) strcpy(tar, "ADDI");
        else if(opc == _lw) strcpy(tar, "LW");
        else if(opc == _lh) strcpy(tar, "LH");
        else if(opc == _lh) strcpy(tar, "LH");
        else if(opc == _lhu) strcpy(tar, "LHU");
        else if(opc == _lb) strcpy(tar, "LB");
        else if(opc == _lbu) strcpy(tar, "LBU");
        else if(opc == _sw) {strcpy(tar, "SW"); *wb = -1;}
        else if(opc == _sh) {strcpy(tar, "SH"); *wb = -1;}
        else if(opc == _sb) {strcpy(tar, "SB"); *wb = -1;}
        else if(opc == _lui) {strcpy(tar, "LUI"); *regA = -1;}
        else if(opc == _andi) strcpy(tar, "ANDI");
        else if(opc == _ori) strcpy(tar, "ORI");
        else if(opc == _nori) strcpy(tar, "NORI");
        else if(opc == _slti) strcpy(tar, "SLTI");
        else if(opc == _beq) {strcpy(tar, "BEQ"); *wb = -1; *regB = rt;}
        else if(opc == _bne) {strcpy(tar, "BNE"); *wb = -1; *regB = rt;}
        else if(opc == _j) {strcpy(tar, "J"); *wb = -1;}
        else if(opc == _jal) {strcpy(tar, "JAL"); *wb = -1;}
        else if(opc == _halt) {strcpy(tar, "HALT"); *wb = -1; *regA = -1;}
    } else {
        *wb = rd;
        *regA = rs;
        *regB = rt;
        if(func == _add) strcpy(tar, "ADD");
        else if(func == _sub) strcpy(tar, "SUB");
        else if(func == _and) strcpy(tar, "AND");
        else if(func == _or) strcpy(tar, "OR");
        else if(func == _xor) strcpy(tar, "XOR");
        else if(func == _nor) strcpy(tar, "NOR");
        else if(func == _nand) strcpy(tar, "NAND");
        else if(func == _slt) strcpy(tar, "SLT");
        else if(func == _sll) {strcpy(tar, "SLL"); *regA = rt; *regB = -1;}
        else if(func == _srl) {strcpy(tar, "SRL"); *regA = rt; *regB = -1;}
        else if(func == _sra) {strcpy(tar, "SRA"); *regA = rt; *regB = -1;}
        else if(func == _jr) {strcpy(tar, "JR"); *wb = -1; *regB = -1;}
    }

}

