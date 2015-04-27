#include "hazard.h"


//this function will be activated on ID stage.
//it's mainly about recording forwarding data.
//But it will also help branch do their forwarding
int hazard_check(int reg_EX, int reg_ME) {
    int reg_A, reg_B, result = 0, branch = 0;
    int op_cur = CPU.pipeline[0]->opcode;
    int op_ex = CPU.pipeline[1]->opcode;
    int op_me = CPU.pipeline[2]->opcode;
    int ex_cnd = ((reg_EX != -1) && (reg_EX != 0)); // make sure it is not access 0
    int me_cnd = ((reg_ME != -1) && (reg_ME != 0)); // make sure it is not access 0

    //reg_A is rs, reg_B is rt
    //but for sll/srl/sra, reg_A is rt.
    reg_A = CPU.pipeline[0]->ID_regA;
    reg_B = CPU.pipeline[0]->ID_regB;

    //check if branch or not
    if(op_cur == _beq || op_cur == _bne) branch = 1;

    //if they are not equals to -1, it means that it can forward now
    //this will happen only after stall, so the if(ex_cnd) and if(me_cnd) will not be trigger
    //due to the reason that EX is NOP.
    if(branch && (CPU.pipeline[0]->fwd_type_s != -1 || CPU.pipeline[0]->fwd_type_t != -1)) {
        printf("do branch fowarding\n");
        //clear things about forwarding after snapshot!!!
    }

    if(ex_cnd) {
        int load = 0;
        if(reg_EX == reg_A || reg_EX == reg_B) {
            //if it is load, need to stall. It's because the data cannot be access at EX stage
            if(op_ex == _lw || op_ex == _lh || op_ex == _lhu  || op_ex == _lb || op_ex == _lbu) load = 1;
            if(load) result = 1;
            else {   
                fwd_signal(1, reg_EX, reg_A, reg_B, CPU.pipeline[0]);
                if(branch) result = 1;
            }
        }
    }
    if(me_cnd) {
        if(op_cur == _beq || op_cur == _bne) branch = 1;

        if((reg_ME == reg_A && !ex_cnd) || (reg_ME == reg_B && !ex_cnd)) {
            fwd_signal(0, reg_ME, reg_A, reg_B, CPU.pipeline[0]);
            if(branch) result = 1;
        }
    }

    return result;
}

void clear_fwd(struct ins* i) {
    i->f_s = 0;
    i->f_t = 0;
    i->fwd_type_s = -1;
    i->fwd_type_t = -1;
}

//this is use to record data about forwarding
//this data will be used on EXE stage.(branch on ID)
//remember to clear the signal after output to avoid endless forwarding
void fwd_signal(int type, int reg_fwd, int reg_A, int reg_B, struct ins* i) {
    if(reg_fwd == reg_A) {
        i->f_s = reg_fwd;
        i->fwd_type_s = type;
    }
    if(reg_fwd == reg_B) {
        i->f_t = reg_fwd;
        i->fwd_type_t = type;
    }
}
