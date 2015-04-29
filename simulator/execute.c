#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
#include "hazard.h"
#include "alu.h"
#include "register.h"
int reg_EX = -1, reg_ME = -1;
//flush, branch, jump, not complete yet

void execute() {
    int x = 0, cyc = 0;
    int halt_spot = false;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    CPU_init();
    fwd_init();
    error_init();
    reg_init();
    while(1) {
        error_output(cyc, err_file);       
        if(error_halt) break;
        halt_cnt = 0;
        stall = 0;
        flush = 0;
        pc_jump = 0;
        reg_EX = -1;
        reg_ME = -1;

        //save the register content for later output.
        reg_copy();

        //this stage will only be perform when it's not NOP
        if(notNOP(_WB) && notHALT(_WB)) WB();
        if(notNOP(_ME) && notHALT(_ME)) ME();
        if(notNOP(_EX) && notHALT(_EX)) EX();
        if(notNOP(_ID) && notHALT(_ID)) ID(reg_EX, reg_ME);
        IF(stall);



        reg_output(cyc, stall ,snap_file);
        cycle_output(stall, snap_file);
        if(halt_cnt == 5) break;
        if(!stall) pc += 4;
        if(flush) {pc = pc_jump; printf("jump\n");}
        cyc ++;
    }
    fclose(snap_file);
    fclose(err_file);
}


void WB() {
    struct ins* i = CPU.pipeline[3];
    reg_write(CPU_REG, i->wb, reg_read(ME_WB, 0));
}

void ME() {
    struct ins* i = CPU.pipeline[2];
    int opc = i->opcode;

    if(is_load(opc)) {
        int result = 0;
        int sign = 0;
        short int tmp = 0;
        int offset = reg_read(EX_ME, 0);
        switch(opc) {
            case _lw: 
                result = data_read(offset, 4);
                break;
            case _lh: 
                tmp = data_read(offset, 2);
                result = tmp;
                break;
            case _lhu: 
                result = data_read(offset, 2);
                break;
            case _lb:
                tmp = data_read(offset, 1);
                sign = (tmp >> 7) & 1;
                if(sign) tmp = tmp | 0xffffff00;
                result = data_read(offset, 1);
                break;
            case _lbu:
                result = data_read(offset, 1);
                break;
        }
        reg_ME = i->wb;
        reg_write(ME_WB, 0, result);

    } else if(is_store(opc)) {
        int data = i->rt;
        int offset = reg_read(EX_ME, 0);
        switch(opc) {
            case _sw:
                data_write(offset, data, 4);
                break;
            case _sh:
                data = data & 0x0000ffff;
                data_write(offset, data, 2);
                break;
            case _sb:
                data = data & 0x000000ff;
                data_write(offset, data, 1);
                break;
        }
        //reg_ME isn't in use.
        reg_ME = -1;
    } else {

        if(i->wb != -1) {
            //has compute something to wait for write back
            reg_write(ME_WB, 0, reg_read(EX_ME, 0));
        }
        //reg_ME isn't in use.
        reg_ME = -1;
    }
}

void EX() {
    struct ins* i = CPU.pipeline[1];
    //printf("%d %s\n", i->wb, i->op_name);
    if(i->wb != -1) {
        int a = 0, b = 0;

        //check the forwarding status. if = 0, it needs fwd
        if(!is_fwd_EX) {
            //do fwd get a, b;
            if(fwd_EX_s != 0) a = fwd_unit(fwd_EX_type_s);
            if(fwd_EX_t != 0) b = fwd_unit(fwd_EX_type_t);

        } else {
            if(i->ID_regA != -1) a = reg_read(CPU_REG, i->ID_regA);
            if(i->ID_regB != -1) b = reg_read(CPU_REG, i->ID_regB);
        }
        //special case : sw
        if(is_store(CPU.pipeline[1]->opcode)) {
            i->rt = b;//sw
        }
        reg_EX = i->wb;

        //call ALU to work
        alu_unit(a, b);
    }
}

void ID(int reg_EX, int reg_ME) {
    struct ins* i = CPU.pipeline[0];
    int h_c;
    h_c = hazard_check(reg_EX, reg_ME);
    if(!h_c) { 
        int a = reg_read(CPU_REG, i->rs), b = reg_read(CPU_REG, i->rt);

        if(!is_fwd_ID) {
            if(fwd_ID_s) a = fwd_unit(fwd_ID_type_s);
            if(fwd_ID_t) b = fwd_unit(fwd_ID_type_t);
        }
        if(is_branch(_ID)) {

            if((i->opcode == _beq) && (a == b)) {
                // BEQ
                do_flush();
                pc_jump = pc + 4 + (i->c * 4);
            } else if((i->opcode == _bne) &&(a != b)) {
                // BNE
                do_flush();
                pc_jump = pc + 4 + (i->c * 4);
            }

        } else if(is_jump(_ID)) {
            int pc_31_28 = pc & 0xf0000000;
            do_flush();

            if(i->func == _jr) pc_jump = a; // a = reg[s]
            else if(i->opcode == _j) pc_jump = (pc_31_28 | (i->c * 4));
            else {
                pc_jump = (pc_31_28 | (i->c * 4));
                // write to reg(Does it needs to fwd?) 
                reg_write(CPU_REG, _ra, pc);
            }
        }
    }
    else do_stall();
}

void IF(int stall) {
    int x;
    for(x = 4; x > 1; x --) {
        CPU.pipeline[x] = CPU.pipeline[x - 1];
        if(CPU.pipeline[x]->opcode == _halt) {halt_cnt++;}
    }
    //if stall, ID will be keep at [0]
    if(!stall) {
        CPU.pipeline[1] = CPU.pipeline[0];
        CPU.pipeline[0] = i_memo[pc / 4];
        if(CPU.pipeline[1]->opcode == _halt) halt_cnt++;
        if(CPU.pipeline[0]->opcode == _halt) halt_cnt++;
    } else {
        CPU.pipeline[1] = S_NOP;
    }
}

void reg_output(int cyc, int stall, FILE *output) {
    int i;
    fprintf(output, "cycle %d\n", cyc);
    for(i = 0; i < 32; i ++) {
        fprintf(output, "$%02d: 0x%08X\n", i, reg_temp[i]);
    }
    fprintf(output, "PC: 0x%08X\n", pc);
}
void cycle_output(int stall, FILE *output) {


    if(!stall) {
        //stall only happen in IF & ID
        if(!flush) fprintf(output, "IF: 0x%08X\n", CPU.pipeline[0]->bits);
        else fprintf(output, "IF: 0x%08X to_be_flushed\n", CPU.pipeline[0]->bits);

        fprintf(output, "ID: %s", CPU.pipeline[1]->op_name);

        if(!is_fwd_ID && notNOP(1)) fwd_output(_ID, output);
        fprintf(output, "\n"); 

    } else {
        // stall happen
        fprintf(output, "IF: 0x%08X to_be_stalled\n", i_memo[pc / 4]->bits);
        fprintf(output, "ID: %s to_be_stalled\n", CPU.pipeline[0]->op_name); 
    }

    fprintf(output, "EX: %s", CPU.pipeline[2]->op_name); 
    if(!is_fwd_EX && notNOP(2)) fwd_output(_EX, output);    
    fprintf(output, "\n"); 

    fprintf(output, "DM: %s\n", CPU.pipeline[3]->op_name); 
    fprintf(output, "WB: %s\n", CPU.pipeline[4]->op_name); 
    fprintf(output, "\n\n"); 
}

void fwd_output(int fwd_to, FILE *output) {
    int reg_ps, reg_pt;
    int type_s, type_t;
    if(fwd_to == _ID) {
        printf("fwd to ID\n");
        reg_ps = fwd_ID_s; //the $s need to be fwd in ID
        reg_pt = fwd_ID_t;
        type_s = fwd_ID_type_s; //type of fwd: EX-ME or ME-WB 
        type_t = fwd_ID_type_t; 
    } else {
        reg_ps = fwd_EX_s;
        reg_pt = fwd_EX_t;
        type_s = fwd_EX_type_s;
        type_t = fwd_EX_type_t;
    }

    if(reg_ps) {
        if(type_s == _EX) fprintf(output, " fwd_EX-DM_rs_$%d", reg_ps);
        else fprintf(output, " fwd_DM-WB_rs_$%d", reg_ps);
    }
    if(reg_pt) {
        if(type_t == _EX) fprintf(output, " fwd_EX-DM_rt_$%d", reg_pt);
        else fprintf(output, " fwd_DM-WB_rt_$%d", reg_pt);
    }
    //clear after print
    clear_fwd(fwd_to);
}

void CPU_init() {
    int i = 0;
    for(i = 0; i < 5; i ++)
        CPU.pipeline[i] = S_NOP;
}

void do_stall() {
    stall = 1;
} // let other component can stall

void do_flush() {
    flush = 1;
}
