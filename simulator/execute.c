#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
#include "register.h"
void execute() {
    int x = 0, cyc = 0;
    int halt_spot = false;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    CPU_init();
    //error_init();
    while(1) {
        halt_cnt = 0;
        stall = 0;
        //WB;
        //MEM;
        //EX
        //ID
        //IF
        IF(stall);
        cycle_output( cyc, snap_file);
        if(!stall) pc += 4;
        cyc ++;
        if(halt_cnt == 5) break;
    }
    fclose(snap_file);
    fclose(err_file);
}

void ID() {
}

void IF(int stall) {
    int x;
    for(x = 4; x > 0; x --) {
        if(!stall) CPU.pipeline[x] = CPU.pipeline[x - 1];
        else {
            CPU.pipeline[x] = S_NOP;
            break;
        }
        if(CPU.pipeline[x]->opcode == _halt) {halt_cnt++;}
    }
    if(!stall) {
        CPU.pipeline[0] = i_memo[pc / 4];
        if(CPU.pipeline[0]->opcode == _halt) halt_cnt++;
    }
}

void cycle_output(int cyc, FILE *output) {
    int i;
    fprintf(output, "cycle %d\n", cyc);
    for(i = 0; i < 32; i ++) {
        fprintf(output, "$%0d: 0x%08X\n", i, reg[i]);
    }
        fprintf(output, "PC: 0x%08X\n", pc);
    fprintf(output, "IF: 0x%08X\n", CPU.pipeline[0]->bits);
    fprintf(output, "ID: %s\n", CPU.pipeline[1]->op_name); 
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

void do_stall() {stall = 1;} // let other component trigger stall
