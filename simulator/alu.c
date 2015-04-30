#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "register.h"
#include "obj.h"
#include "alu.h"
void alu_unit(int s, int t) {
    struct ins* i = CPU.pipeline[1];
    int opc = i->opcode;
    short c = i->c;
    if(opc) {

        if(opc == _addi) addi(s, c);
        else if(opc == _halt) printf("encounter halt\n");
        else if(is_load(opc) || is_store(opc)) load_store(s, c);
        else if(opc == _lui) lui(c); 
        else if(opc == _andi) andi(s, c);
        else if(opc == _ori) ori(s, c);
        else if(opc == _nori) nori(s, c);
        else if(opc == _slti) slti(s, c);

    } else {
        int func = i->func;
        int shamt = i->shamt;
        
        if(func == _add) add(0, s, t);
        else if(func == _sub) add(1, s, t);
        else if(func == _and) and(0, s, t);
        else if(func == _or) or(0, s, t);
        else if(func == _xor) or(1, s, t);
        else if(func == _nor) or(2, s, t);
        else if(func == _nand) and(1, s, t);
        else if(func == _slt) slt(s, t);
        else if(func == _sll) sll(t, shamt);
        else if(func == _srl) sr(0, t, shamt);
        else if(func == _sra) sr(1, t, shamt);
    }
}



// d, s, t here is not the adress.
// it's the value in reg.
// that is, s is this value reg[s]
// execute will pass the value here
// nand/and means 1 is nand, 0 is and
void add(int mode, int s, int t) {
    if(mode) {
        // sub
        num_overflow(s, t * (-1));
        reg_EX_ME = s - t;
    } else {
        num_overflow(s, t);
        reg_EX_ME = s + t;
    }
}

void and(int mode, int s, int t) {
    int result = s & t;
    // nand/and
    if(mode) reg_EX_ME = ~result;
    else reg_EX_ME = result;
    //if(mode) reg_write(EX_ME, 0, ~result);
    //else reg_write(EX_ME, 0, result);
}

void or(int mode, int s, int t) {
    // nor/xor/or
    if(mode == 2) reg_EX_ME = ~(s | t);
    else if(mode == 1) reg_EX_ME = s ^ t;
    else reg_EX_ME = s | t;
}

void slt(int s, int t) {
    reg_EX_ME = s < t;
}

void sll(int t, int shamt) {
    reg_EX_ME = t << shamt;
}


void sr(int mode, int t, int shamt){
    // sra/srl
    if(mode) {
        reg_EX_ME = t >> shamt;
    } else {
        unsigned int result = t;
        result = result >> shamt; // unsigned int won't do
        reg_EX_ME = result;
    }

}


void addi(int s, short c) {
    // numberoverflow
    num_overflow(s, c);
    reg_EX_ME = s + c;
}

void load_store(int s, short c) {
    // numberoverflow
    num_overflow(s, c);
    reg_EX_ME = s + c;//offset
    // this only do C($s)
    // so it's fit to all load/store
    // read/store will be implement on ME stage
}

void lui(short c) {
    unsigned int tmp = c;
    tmp = tmp << 16;
    reg_EX_ME = tmp;
}

void andi(int s, short c) {
    unsigned int tmp = c & 0x0000ffff;
    reg_EX_ME = s & tmp;
}

void ori(int s, short c) {
    unsigned int tmp = c & 0x0000ffff;
    reg_EX_ME = s | tmp;
}

void nori(int s, short c) {
    unsigned int tmp = c & 0x0000ffff;
    reg_EX_ME = ~(s | tmp);
}

void slti(int s, short c) {
    reg_EX_ME = s < c;
}
