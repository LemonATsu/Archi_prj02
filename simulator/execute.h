//define opcode/func here.
//also, about some register
#define true 1
#define false 0
//    register:
#define _zero 0
#define _at 1
#define _v0 2
#define _v1 3
#define _a0 4
#define _a1 5
#define _a2 6
#define _a3 7
#define _t0 8
#define _t1 9
#define _t2 10
#define _t3 11
#define _t4 12
#define _t5 13
#define _t6 14
#define _t7 15
#define _s0 16
#define _s1 17
#define _s2 18
#define _s3 19
#define _s4 20
#define _s5 21
#define _s6 22
#define _s7 23
#define _t8 24
#define _t9 25
#define _k0 26
#define _k1 27
#define _gp 28
#define _sp 29
#define _fp 30
#define _ra 31
//    opcode:
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
//  stage:(IF is the last stage in implement. all the stage will -1(ID:1->0))
#define _ID 0
#define _EX 1
#define _MEM 2
#define _WB 3

void cycle_output(int cyc, FILE *output);
void execute();
void CPU_init();
int stall;
int halt_cnt = 0;
struct cpu CPU;

void IF(int stall);
void ID();
void EX();
void MEM();
void WB();

void do_stall();
void do_fwd(int type_from, int type_to);
