#include <stdio.h>
#include <stdlib.h>
#include "report.h"
#include "obj.h"
#include "execute.h"
#include "register.h"
#include "hazard.h"



void o_init(int fwd_to) {
    if(fwd_to == _EX) {
        need_o_ex = 0;
        o_ex_s = 0;
        o_ex_t = 0;
        o_ex_st = -1;
        o_ex_tt = -1;
    } else {
        need_o_id = 0;
        o_id_s = 0;
        o_id_t = 0;
        o_id_st = -1;
        o_id_tt = -1;
    }
}

void o_sync_fwd(int fwd_to) {
    if(fwd_to == _EX) {
        need_o_ex = 1;
        o_ex_s = fwd_EX_s;
        o_ex_t = fwd_EX_t;
        o_ex_st = fwd_EX_type_s;
        o_ex_tt = fwd_EX_type_t;
    } else {
        need_o_id = 1;
        o_id_s = fwd_ID_s;
        o_id_t = fwd_ID_t;
        o_id_st = fwd_ID_type_s;
        o_id_tt = fwd_ID_type_t;
    }
}

void reg_output(int cyc, int stall, FILE *output) {
    int i;
    fprintf(output, "cycle %d\n", cyc);
    for(i = 0; i < 32; i ++) {
        fprintf(output, "$%02d: 0x%08X\n", i, reg[i]);
    }
    fprintf(output, "PC: 0x%08X\n", pc);
}
void cycle_output(int stall, FILE *output) {



    if(!stall) {
        //stall only happen in IF & ID
        if(!flush) fprintf(output, "IF: 0x%08X\n", CPU.pipeline[0]->bits);
        else fprintf(output, "IF: 0x%08X to_be_flushed\n", i_memo[pc / 4]->bits);

        fprintf(output, "ID: %s", CPU.pipeline[1]->op_name);

        if(need_o_id && notNOP(1)) fwd_output(_ID, output);
        fprintf(output, "\n"); 

    } else {
        // stall happen
        fprintf(output, "IF: 0x%08X to_be_stalled\n", i_memo[pc / 4]->bits);
        fprintf(output, "ID: %s to_be_stalled\n", CPU.pipeline[0]->op_name); 
    }

    fprintf(output, "EX: %s", CPU.pipeline[2]->op_name); 
    if(need_o_ex && notNOP(2)) fwd_output(_EX, output);
    fprintf(output, "\n"); 

    fprintf(output, "DM: %s\n", CPU.pipeline[3]->op_name); 
    fprintf(output, "WB: %s\n", CPU.pipeline[4]->op_name); 
    fprintf(output, "\n\n"); 
}

void fwd_output(int fwd_to, FILE *output) {
    int reg_ps, reg_pt;
    int type_s, type_t;
    if(fwd_to == _ID) {
        //printf("fwd to ID\n");
        reg_ps = o_id_s; //the $s need to be fwd in ID
        reg_pt = o_id_t;
        type_s = o_id_st; //type of fwd: EX-ME or ME-WB 
        type_t = o_id_tt; 
        o_init(_ID);
    } else {
        reg_ps = o_ex_s;
        reg_pt = o_ex_t;
        type_s = o_ex_st;
        type_t = o_ex_tt;
        o_init(_EX);
    }
    if(reg_ps) {
        if(type_s == _EX) fprintf(output, " fwd_EX-DM_rs_$%d", reg_ps);
        else fprintf(output, " fwd_DM-WB_rs_$%d", reg_ps);
    }
    if(reg_pt) {
        if(type_t == _EX) fprintf(output, " fwd_EX-DM_rt_$%d", reg_pt);
        else fprintf(output, " fwd_DM-WB_rt_$%d", reg_pt);
    }
}
