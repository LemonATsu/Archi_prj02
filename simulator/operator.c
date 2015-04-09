#include "operator.h"


void addi(int s, int t, short c) {
    int dat = reg_read(s);
    num_overflow(dat, c);
    reg_write(t, dat + c);
}

void lw(int s, int t, short c) {
    int dat = reg_read(s);
    num_overflow(dat, c);
    reg_write(t, data_read(dat + c, 4));
}

void lh(int s, int t, short c, int mode) {
    int dat = reg_read(s);
    num_overflow(dat, c);
    if(mode) {
        short int temp = data_read(dat + c, 2);
        reg_write(t, temp);
    } else {
        unsigned int temp = data_read(dat + c, 2);
        reg_write(t, temp);
    }
}
//mode = 1 is lb, mode = 0 is lbu
void lb(int s, int t, short c, int mode) {    
    int dat = reg_read(s);
    num_overflow(dat, c);
    short byte = data_read(dat + c, 1);
    if(mode) {
        short res = byte & 0xff;
        //check eighth byte, if it's 1, than extend it;
        if((byte >> 7 & 0x1) == 1) res = res | 0xffffff00;
        reg_write(t, res);
    } else {
        unsigned int res = byte & 0xff;
        reg_write(t, res);
    }
}

void sw(int s, int t, short c) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);
    num_overflow(dat_s, c);
    data_write(dat_s + c, dat_t, 4);

}

void sh(int s, int t, short c) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);
    num_overflow(dat_s, c);
    int data = dat_t & 0x0000ffff;
    data_write(dat_s + c, data, 2);
}

void sb(int s, int t, short c) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);
    num_overflow(dat_s, c);
    int data = dat_t & 0x000000ff;

    data_write(dat_s + c, data, 1);
}

void lui(int t, short c) {
    reg_write(t, c << 16);
}

void andi(int s, int t, short c) {
    unsigned int temp = c & 0x0000ffff;
    reg_write(t, reg_read(s) & temp); 

}

void ori(int s, int t, short c) {
    int c_un = c & 0x0000ffff;
    reg_write(t, reg_read(s) | c_un); 
}

void nori(int s, int t, short c) {
    int c_un = c & 0x0000ffff;
    reg_write(t, ~(reg_read(s) | c_un)); 
}   

void slti(int s, int t, short c) {
    reg_write(t, (reg_read(s) < c) ? 1 : 0); 
}

void beq(int s, int t, short c) {
    //need to handle overflow;
    num_overflow( pc + 4, c * 4);
    if(reg_read(s) == reg_read(t)) {
        pc += 4 * c;
    }

}

void bne(int s, int t, short c) {
    num_overflow( pc + 4, c * 4);
    if(reg_read(s) != reg_read(t)) {
        pc += 4 * c;
    }
}


void jal(unsigned int c) {
    //printf("jal:%x %x\n", reg[_ra], c * 4 - 4);
    int pc_31_28 = pc & 0xf0000000;
    reg_write(_ra, pc + 4);// we only plus the pc at the end of the cycle.
    pc = (pc_31_28 | (c * 4)) - 4; // start pc is c * 4, but we will plus 4 before output, so sub 4 to fix it
}

void j(unsigned int c) {
    int pc_31_28 = pc & 0xf0000000;
    pc = (pc_31_28 | (c * 4)) - 4; // this will jump to the start of the label,
                    // but the cycle is still not end yet, and
                    // we will plus 4 before entering next cyc
}

void jr(int s) {
    pc = reg_read(s) - 4;
}



//func start here
//mode = 1 is add, = 0 is sub
void add(int s, int t, int d, int mode) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);
    //error handle: overflow ?
    if(mode) {
        num_overflow(dat_s, dat_t);
        reg_write(d, dat_s + dat_t);
    } else {
        num_overflow(dat_s, dat_t * -1);
        reg_write(d, dat_s - dat_t);
    }
}

void and(int s, int t, int d, int mode) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);
    int result = dat_s & dat_t;
    if(mode) reg_write(d, result);
    else reg_write(d, ~result);
}

// mode = 2 is or, = 1 is xor, = 0 is nor
void or(int s, int t, int d, int mode) {
    int dat_s = reg_read(s);
    int dat_t = reg_read(t);

    if(mode == 2) reg_write(d, dat_s | dat_t);
    else if(mode == 1) reg_write(d, dat_s ^ dat_t);
    else reg_write(d, ~(dat_s | dat_t));
}

void slt(int s, int t, int d) {
    reg_write(d, reg_read(s) < reg_read(t));
}

void sll(int t, int d, int shamt) {
    if(t == 0 && d == 0 && shamt == 0) {
        reg[0] = reg[0];
    } else {
        reg_write(d, (reg_read(t) << shamt));
    }
}

//mode = 1 is srl, mode = 0 is sra
void sr(int t, int d, int shamt, int mode) {

    if(mode) {
        unsigned int result = reg_read(t);
        result = result >> shamt;
        reg_write(d, result);
    } else {
        reg_write(d, reg_read(t) >> shamt);
    }
}
