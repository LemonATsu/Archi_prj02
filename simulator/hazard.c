#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
#include "hazard.h"
#include "register.h"
void fwd_init() {
    fwd_ID_s = 0;
    fwd_ID_t = 0;
    fwd_ID_type_s = -1;
    fwd_ID_type_t = -1;
    fwd_EX_s = 0;
    fwd_EX_t = 0;
    fwd_EX_type_s = -1;
    fwd_EX_type_t = -1;
    is_fwd_ID = 1;
    is_fwd_EX = 1;
}

// this function will be activated on ID stage.
// it's mainly about recording forwarding data.
// But it will also help branch do their forwarding
//
// for some instructions that don't need to fwd,
// their reg_A, reg_B will be -1,
// so it's impossible to have hazard detect here
//
// if reg_A and reg_B be -1, 
// the only cnd they will have hazard is
// reg_EX = -1 or reg_EX = -1
// but it will fulfill the fwd cnd
int hazard_check(int reg_EX, int reg_ME) {
    int reg_A, reg_B, result = 0, branch = 0;
    int op_cur = CPU.pipeline[0]->opcode;
    int op_ex = CPU.pipeline[1]->opcode;
    int ex_cnd = ((reg_EX != -1) && (reg_EX != 0)); // make sure it is not access 0
    int me_cnd = ((reg_ME != -1) && (reg_ME != 0)); // make sure it is not access 0
    int fwd_des = -1;
    //reg_A is rs, reg_B is rt
    //but for sll/srl/sra, reg_A is rt.
    reg_A = CPU.pipeline[0]->ID_regA;
    reg_B = CPU.pipeline[0]->ID_regB;

    //check if branch or not
    if(op_cur == _beq || op_cur == _bne) {
        branch = 1;
        fwd_des = _ID;
    } else fwd_des = _EX;

    //if they are not equals to -1, it means that it can forward now
    //this will happen only after stall, so the if(ex_cnd) and if(me_cnd) will not be trigger
    //due to the reason that EX is NOP.
    if(branch && !is_fwd_ID) {
        printf("do branch fowarding\n");
        return 0;
    }

    if(ex_cnd) {
        int load = 0;
        if(reg_EX == reg_A || reg_EX == reg_B) {
            //if it is load, need to stall. It's because the data cannot be access at EX stage
            if(is_load(op_ex)) load = 1;
            if(load) result = 1;
            else {   
                fwd_signal(fwd_des, _EX, reg_EX, reg_A, reg_B);
                if(branch) result = 1;
            }
        }
    }
    if(me_cnd) {
        if((reg_ME == reg_A && !ex_cnd) || (reg_ME == reg_B && !ex_cnd)) {
            // branch won't have chance to let ME-WB forward
            // when ID needs to compute, ME-WB might still not available
            // but when ME-WB is available
            // the data is already write back to register(first half cycle)
            if(!branch) fwd_signal(fwd_des, _ME, reg_ME, reg_A, reg_B);
            if(branch) result = 1;
        }
    }

    return result;
}

void clear_fwd(int fwd_to) {
    if(fwd_to == _ID) {
        fwd_ID_s = 0;
        fwd_ID_t = 0;
        fwd_ID_type_s = -1;
        fwd_ID_type_t = -1;
        is_fwd_ID = 1;
    } else {
        fwd_EX_s = 0;
        fwd_EX_t = 0;
        fwd_EX_type_s = -1;
        fwd_EX_type_t = -1;
        is_fwd_EX = 1;
    }
}

//this is use to record data about forwarding
//this data will be used on EXE stage.(branch on ID)
//remember to clear the signal after output to avoid endless forwarding
void fwd_signal(int fwd_to, int type, int reg_fwd, int reg_A, int reg_B) {
    if(reg_fwd == reg_A) {
        //check where to fwd
        if(fwd_to == _ID) { 
            fwd_ID_s = reg_A; //forward to s;
            fwd_ID_type_s = type; //use what kind of reg (EX-ME or ME-WB)
        } else {
            fwd_EX_s = reg_A; //forward to s;
            fwd_EX_type_s = type; //use what kind of reg
        }
    }
    if(reg_fwd == reg_B) {
        if(fwd_to == _ID) {
            fwd_ID_t = reg_B; //forward to t;
            fwd_ID_type_t = type; //use what kind of reg
        } else {
            fwd_EX_t = reg_B; //forward to t;
            fwd_EX_type_t = type; //use what kind of reg
        }
    }

    //is_fwd = 0  pending fwd
    if(fwd_to == _ID) is_fwd_ID = 0;
    else is_fwd_EX = 0;
}



// this function is to give data from reg_EX_ME or reg_ME_WB
// it doesn't care who asking the data
int fwd_unit(int fwd_type) {
    int result;
    //if reg_type = 0 is s, 1 is t
    if(fwd_type == _EX) { //if true, its EX-ME, else ME-WB
        result = reg_read(EX_ME, 0);
    } else {
        result = reg_read(ME_WB, 0);
    }
    return result;
}
