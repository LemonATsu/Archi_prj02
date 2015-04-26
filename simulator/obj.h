#include <stdio.h>
#define MEMO_LIMIT 1024
int sp;
int data_num;
int pc;
int ins_num;
int error_halt;
// actually only contains MEMO_LIMIT/4 instructions
struct ins* i_memo[MEMO_LIMIT];
struct ins* S_NOP;
struct words{
    unsigned char machine_code[5];
};

struct data{
    int addr;
    int dat[5];
};

struct ins{
    int pc_addr; //6 bits
    int bits;
    int opcode;
    int rs;
    int rt;
    int rd;
    int shamt;
    int func;
    short c;
    unsigned int j_label;
    char op_name[4];
};

struct cpu {
    struct ins* pipeline[6]; 
};
