#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
#include "hazard.h"
#include "alu.h"
#include "register.h"
int reg_EX = -1, reg_ME = -1;
void execute() {
    int x = 0, cyc = 0;
    int halt_spot = false;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    CPU_init();
    fwd_init();
    error_init();
    while(1) {
        halt_cnt = 0;
        stall = 0;
        reg_EX = -1;
        reg_ME = -1;

        reg_copy();
        if(notNOP(CPU.pipeline[3]->op_name))WB();
        if(notNOP(CPU.pipeline[2]->op_name))ME();
        if(notNOP(CPU.pipeline[1]->op_name))EX();
        if(notNOP(CPU.pipeline[0]->op_name))ID(reg_EX, reg_ME);
        IF(stall);

        error_output(cyc, err_file);       

        if(error_halt) break;
        
        reg_output(cyc, stall ,snap_file);
        cycle_output(stall, snap_file);
        if(halt_cnt == 5) break;
        if(!stall) pc += 4;
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
        int result = 0;
    } else {

        if(i->wb != -1) {
        //has compute something to wait for write back
            reg_write(ME_WB, 0, reg_read(EX_ME, 0));
        }
    }
}

void EX() {
    struct ins* i = CPU.pipeline[1];
    //printf("%d %s\n", i->wb, i->op_name);
    if(i->wb != -1 && strcmp(i->op_name, "NOP")) {
        int a = 0, b = 0;

        //check the forwarding status. if = 0, it needs fwd
        if(!is_fwd_EX) {
            if(fwd_EX_s != 0) a = fwd_unit(fwd_EX_type_s);
            if(fwd_EX_t != 0) b = fwd_unit(fwd_EX_type_t);
            printf("EX doing FWD %d %d\n", a, b);

            //do fwd get a, b;
        } else {
            if(i->ID_regA != -1) a = reg_read(CPU_REG, i->ID_regA);
            if(i->ID_regB != -1) b = reg_read(CPU_REG, i->ID_regB);
        }
        reg_EX = i->wb;
        
        //call ALU to work
        alu_unit(a, b);
    }
}

void ID(int reg_EX, int reg_ME) {
    if(strcmp(CPU.pipeline[0]->op_name, "NOP")) {
        int h_c;
        if(!is_fwd_ID) {
            int a = 0, b = 0;
            if(fwd_ID_s) a = fwd_unit(fwd_ID_type_s);
            if(fwd_ID_t) b = fwd_unit(fwd_ID_type_t);
            printf("ID doing FWD %d %d\n", a, b);
        }
        h_c = hazard_check(reg_EX, reg_ME);
        if(h_c) do_stall();
    }
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
}
void cycle_output(int stall, FILE *output) {

    fprintf(output, "PC: 0x%08X\n", pc);

    if(!stall) {
        //stall only happen in IF & ID
        fprintf(output, "IF: 0x%08X\n", CPU.pipeline[0]->bits);
        fprintf(output, "ID: %s", CPU.pipeline[1]->op_name);

        if((!strcmp(CPU.pipeline[1]->op_name, "BEQ") || !strcmp(CPU.pipeline[1]->op_name, "BNE")) 
                && !is_fwd_ID) {
            //no forward will occur here except branch
            //if it's BEQ or BNE and it need fwd
            if(fwd_ID_s) {
                if(fwd_ID_type_s == _EX) fprintf(output, " fwd_EX-DM_rs_$%d", fwd_ID_s);
                else fprintf(output, " fwd_DM-WB_rs_$%d", fwd_ID_s);
            }
            if(fwd_ID_t) {
                if(fwd_ID_type_t == _EX) fprintf(output, " fwd_EX-DM_rt_$%d", fwd_ID_t);
                else fprintf(output, " fwd_DM-WB_rt_$%d", fwd_ID_t);
            }
            //clear after print
            clear_fwd(_ID);
        }
        fprintf(output, "\n"); 
    } else {
        // stall happen
        fprintf(output, "IF: 0x%08X to_be_stalled\n", i_memo[pc / 4]->bits);
        fprintf(output, "ID: %s to_be_stalled\n", CPU.pipeline[0]->op_name); 
    }

    fprintf(output, "EX: %s\n", CPU.pipeline[2]->op_name); 
    fprintf(output, "DM: %s\n", CPU.pipeline[3]->op_name); 
    fprintf(output, "WB: %s\n", CPU.pipeline[4]->op_name); 
    fprintf(output, "\n\n"); 
}

void CPU_init() {
    int i = 0;
    for(i = 0; i < 5; i ++)
        CPU.pipeline[i] = S_NOP;
}

void do_stall() {
    stall = 1;
} // let other component can stall
