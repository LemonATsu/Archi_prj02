#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "obj.h"
#include "hazard.h"
#include "alu.h"
#include "report.h"
#include "register.h"
int reg_EX = -1, reg_ME = -1;
int dec_regA = -1, dec_regB = -1;
void execute() {
    int x = 0, cyc = 0;
    int halt_spot = false;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    CPU_init();
    fwd_init();
    error_init();
    reg_init();
    o_init(_ID);
    o_init(_EX);
    while(1) {
        g_cyc = cyc;
        error_output(cyc, err_file);       
        if(error_halt) break;

        halt_cnt = 0;
        stall = 0;
        reg_EX = -1;
        reg_ME = -1;

        reg_output(cyc, stall ,snap_file);

        //this stage will only be perform when it's not NOP
        if(notNOP(_WB) && notHALT(_WB)) WB();
        if(notNOP(_ME) && notHALT(_ME)) ME();
        if(notNOP(_EX) && notHALT(_EX)) EX();
        ID(reg_EX, reg_ME);
        IF(stall);

        cycle_output(stall, snap_file);

        for(x = 4; x >= 0; x --) if(CPU.pipeline[x]->opcode == _halt) halt_cnt++;

        if(halt_cnt == 5) break;

        if(!stall) pc += 4;


        if(flush) do_flush();

        cyc ++;

        if(cyc >= 1000000) break;
        //if(cyc >= 12) break;
    }
    fclose(snap_file);
    fclose(err_file);
}


void WB() {
    struct ins* i = CPU.pipeline[3];
    if(i->wb != -1) reg_write(CPU_REG, i->wb, reg_read(ME_WB, 0));
}

void ME() {
    struct ins* i = CPU.pipeline[2];
    int opc = i->opcode;

    if(is_load(opc)) {
        int result = 0;
        int sign = 0;
        short int tmp = 0;
        int offset = reg_read(EX_ME, 0);
        if(opc == _lw) { //lw
            result = data_read(offset, 4);
        } else if(opc == _lh) { //lh
            tmp = data_read(offset, 2);
            result = tmp;
        } else if(opc == _lhu) { //lhu
            result = data_read(offset, 2);
        } else if(opc == _lb) { //lb
            tmp = data_read(offset, 1);
            sign = (tmp >> 7) & 1;
            if(sign) tmp = tmp | 0xffffff00;
            result = data_read(offset, 1);
        } else if(opc == _lbu) { //lbu
            result = data_read(offset, 1);
        }

        reg_ME = i->wb;
        reg_write(ME_WB, 0, result);

    } else if(is_store(opc)) {
        int data = i->rt;
        int offset = reg_read(EX_ME, 0);
        if(opc == _sw) { //sw
            data_write(offset, data, 4);
        } else if(opc == _sh) { //sh
            data = data & 0x0000ffff;
            data_write(offset, data, 2);
        } else if(opc == _sb) { //sb
            data = data & 0x000000ff;
            data_write(offset, data, 1);
        }
        //reg_ME isn't in use.
        reg_ME = -1;
    } else {
         
        //has compute something to wait for write back
        if(i->wb != -1) reg_write(ME_WB, 0, reg_read(EX_ME, 0));
        //reg_ME isn't in use.
        reg_ME = -1;
    }
}

void EX() {
    struct ins* i = CPU.pipeline[1];
    //printf("%d %s\n", i->wb, i->op_name);
    if(i->wb != -1 || is_store(i->opcode) || is_load(i->opcode)) {
        int a = 0, b = 0;

        if(dec_regA != -1) a = reg_read(CPU_REG, dec_regA);
        if(dec_regB != -1) b = reg_read(CPU_REG, dec_regB);
        //check the forwarding status. if = 0, it needs fwd
        if(!is_fwd_EX) {
            //do fwd get a, b;
            if(fwd_EX_s != 0) a = fwd_unit(fwd_EX_type_s);
            if(fwd_EX_t != 0) b = fwd_unit(fwd_EX_type_t);

            clear_fwd(_EX);

        }
        //special case : sw
        if(is_store(i->opcode)) {
            i->rt = b;//sw
        }
        reg_EX = i->wb;
        if(i->opcode == _jal) reg_write(EX_ME, _ra, i->pc_addr + 4);


        //call ALU to work
        alu_unit(a, b);
    }
}

void ID(int reg_EX, int reg_ME) {
    struct ins* i = CPU.pipeline[0];
    int h_c;
    dec_regA = -1;
    dec_regB = -1;

    ID_decoder();

    if(!notNOP(_ID) || !notHALT(_ID)) return;

    h_c = hazard_check(reg_EX, reg_ME, dec_regA, dec_regB);

    if(!h_c) { 
        int a = reg_read(CPU_REG, i->rs), b = reg_read(CPU_REG, i->rt);
        if(!is_branch(_ID) && !is_jump(_ID)) return;
        if(!is_fwd_ID) {
            if(fwd_ID_s) a = fwd_unit(fwd_ID_type_s);
            if(fwd_ID_t) b = fwd_unit(fwd_ID_type_t);

            clear_fwd(_ID);
        }
        if(is_branch(_ID)) {
            if((i->opcode == _beq) && (a == b)) {
                // BEQ
                //printf("beq %d %d %d %x\n", a, b, i->c, i->pc_addr);
                be_flush();
                pc_jump = i->pc_addr + (i->c * 4);
            } else if((i->opcode == _bne) &&(a != b)) {
                // BNE
                //printf("bne %d %d %d %x\n", a, b, i->c, i->pc_addr);
                be_flush();
                pc_jump = i->pc_addr + (i->c * 4);
            }

        } else if(is_jump(_ID)) {
            int pc_31_28 = pc & 0xf0000000;
            be_flush();

            if(i->func == _jr) pc_jump = a; // a = reg[s]
            else if(i->opcode == _j) pc_jump = (pc_31_28 | (i->j_label * 4));
            else pc_jump = (pc_31_28 | (i->j_label * 4));
        }
    }
    else do_stall();
}

void IF(int stall) {
    int x;
    for(x = 4; x > 1; x --) CPU.pipeline[x] = CPU.pipeline[x - 1];
    //if stall, ID will be keep at [0]
    if(!stall) {
        CPU.pipeline[1] = CPU.pipeline[0];
        if(!flush) CPU.pipeline[0] = i_memo[pc / 4];
    } else CPU.pipeline[1] = S_NOP;
}


void CPU_init() {
    int i = 0;
    for(i = 0; i < 5; i ++)
        CPU.pipeline[i] = S_NOP;
}

void do_stall() {stall = 1;} // let other component can stall

void be_flush() {flush = 1;}

void do_flush() {
    pc = pc_jump + 4;
    CPU.pipeline[0] = S_NOP;
    flush = 0;
}

void ID_decoder() {
    //give the correspondence opcode/function name to the instruction
    //also, will determine the write back register here.
    struct ins* i = CPU.pipeline[0];
    int opc = i->opcode;
    int func = i->func;
    if(opc) {
        i->wb = i->rt;
        dec_regA = i->rs;
        if(opc == _addi) strcpy(i->op_name, "ADDI");
        else if(opc == _lw) strcpy(i->op_name, "LW");
        else if(opc == _lh) strcpy(i->op_name, "LH");
        else if(opc == _lh) strcpy(i->op_name, "LH");
        else if(opc == _lhu) strcpy(i->op_name, "LHU");
        else if(opc == _lb) strcpy(i->op_name, "LB");
        else if(opc == _lbu) strcpy(i->op_name, "LBU");
        else if(opc == _sw) {strcpy(i->op_name, "SW"); i->wb = -1;}
        else if(opc == _sh) {strcpy(i->op_name, "SH"); i->wb = -1;}
        else if(opc == _sb) {strcpy(i->op_name, "SB"); i->wb = -1;}
        else if(opc == _lui) {strcpy(i->op_name, "LUI"); dec_regA = -1;}
        else if(opc == _andi) strcpy(i->op_name, "ANDI");
        else if(opc == _ori) strcpy(i->op_name, "ORI");
        else if(opc == _nori) strcpy(i->op_name, "NORI");
        else if(opc == _slti) strcpy(i->op_name, "SLTI");
        else if(opc == _beq) {strcpy(i->op_name, "BEQ"); i->wb = -1; dec_regB = i->rt;}
        else if(opc == _bne) {strcpy(i->op_name, "BNE"); i->wb = -1; dec_regB = i->rt;}
        else if(opc == _j) {strcpy(i->op_name, "J"); i->wb = -1;}
        else if(opc == _jal) {strcpy(i->op_name, "JAL"); i->wb = 31;}
        else if(opc == _halt) {strcpy(i->op_name, "HALT"); i->wb = -1; dec_regA = -1;}
    } else {
        i->wb = i->rd;
        dec_regA = i->rs;
        dec_regB = i->rt;
        if(func == _add) strcpy(i->op_name, "ADD");
        else if(func == _sub) strcpy(i->op_name, "SUB");
        else if(func == _and) strcpy(i->op_name, "AND");
        else if(func == _or) strcpy(i->op_name, "OR");
        else if(func == _xor) strcpy(i->op_name, "XOR");
        else if(func == _nor) strcpy(i->op_name, "NOR");
        else if(func == _nand) strcpy(i->op_name, "NAND");
        else if(func == _slt) strcpy(i->op_name, "SLT");
        else if(func == _sll) {strcpy(i->op_name, "SLL"); dec_regA = -1;}
        else if(func == _srl) {strcpy(i->op_name, "SRL"); dec_regA = -1;}
        else if(func == _sra) {strcpy(i->op_name, "SRA"); dec_regA = -1;}
        else if(func == _jr) {strcpy(i->op_name, "JR"); i->wb = -1; dec_regB = -1;}
    }

    if(i->opcode == 0 && i->shamt == 0 && i->func == 0 && i->rt == 0 && i->rd == 0)
        strcpy(i->op_name, "NOP");

}
