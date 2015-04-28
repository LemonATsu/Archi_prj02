#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
#include "hazard.h"
#include "register.h"
int reg_EX = -1, reg_ME = -1;
void execute() {
    int x = 0, cyc = 0;
    int halt_spot = false;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    CPU_init();
    fwd_init();
    //error_init();
    while(1) {
        halt_cnt = 0;
        stall = 0;
        reg_EX = -1;
        reg_ME = -1;
        //WB;
        //MEM;
        EX();
        
        ID(reg_EX, reg_ME);

        IF(stall);

        cycle_output( cyc, stall, snap_file);
        if(!stall) pc += 4;
        cyc ++;
        if(halt_cnt == 5) break;
    }
    fclose(snap_file);
    fclose(err_file);
}


void EX() {
    struct ins* i = CPU.pipeline[1];
    if(i->wb != -1) {
        reg_EX = i->wb;
    }
}

void ID(int reg_EX, int reg_ME) {
    if(strcmp(CPU.pipeline[0]->op_name, "NOP")) {
        int h_c;
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

void cycle_output(int cyc, int stall, FILE *output) {
    int i;

    fprintf(output, "cycle %d\n", cyc);

    for(i = 0; i < 32; i ++) {
        fprintf(output, "$%0d: 0x%08X\n", i, reg[i]);
    }
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
                if(fwd_ID_type_s) fprintf(output, " fwd_EX-DM_rs_$%d", fwd_ID_s);
                else fprintf(output, " fwd_DM-WB_rs_$%d", fwd_ID_s);
            }
            if(fwd_ID_t) {
                if(fwd_ID_type_t) fprintf(output, " fwd_EX-DM_rt_$%d", fwd_ID_t);
                else fprintf(output, " fwd_DM-WB_rt_$%d", fwd_ID_t);
            }
            //clear after print
            clear_fwd();
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
