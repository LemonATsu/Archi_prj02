#include <stdio.h>
#include <stdlib.h>
#include "report.h"
#include "obj.h"
#include "execute.h"
#include "register.h"
#include "hazard.h"




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

        if(notNOP(1) && CPU.pipeline[1]->fwd_id) fwd_output(_ID, output);
        fprintf(output, "\n"); 

    } else {
        // stall happen
        fprintf(output, "IF: 0x%08X to_be_stalled\n", i_memo[pc / 4]->bits);
        fprintf(output, "ID: %s to_be_stalled\n", CPU.pipeline[0]->op_name); 
    }

    fprintf(output, "EX: %s", CPU.pipeline[2]->op_name); 
    if(notNOP(2) && CPU.pipeline[2]->fwd_ex) fwd_output(_EX, output);
    fprintf(output, "\n"); 

    fprintf(output, "DM: %s\n", CPU.pipeline[3]->op_name); 
    fprintf(output, "WB: %s\n", CPU.pipeline[4]->op_name); 
    fprintf(output, "\n\n"); 
}

void fwd_output(int fwd_to, FILE *output) {
    struct ins* i;

    if(fwd_to == _ID) {
        i = CPU.pipeline[1];
        i->fwd_id = 0;
    } else {
        i = CPU.pipeline[2];
        i->fwd_ex = 0;
    }

    if(i->fwd_to_s != -1) {
        if(i->fwd_to_s == _EX) fprintf(output, " fwd_EX-DM_rs_$%d", i->rs);
        else fprintf(output, " fwd_DM-WB_rs_$%d", i->rs);
        i->fwd_to_s = -1;
    }
    if(i->fwd_to_t != -1) {
        if(i->fwd_to_t == _EX) fprintf(output, " fwd_EX-DM_rt_$%d", i->rt);
        else fprintf(output, " fwd_DM-WB_rt_$%d", i->rt);
        i->fwd_to_t = -1;
    }
}
