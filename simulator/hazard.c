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
int hazard_check(int reg_EX, int reg_ME, int reg_A, int reg_B) {
    int result = 0, branch = 0;
    struct ins* i = CPU.pipeline[0];
    int op_cur = i->opcode, func_cur = i->func;
    int op_ex = CPU.pipeline[1]->opcode;
    int op_me = CPU.pipeline[2]->opcode;
    int ex_cnd = ((reg_EX != -1) && (reg_EX != 0)) && (reg_EX == reg_A || reg_EX == reg_B); // make sure it is not access 0
    int me_cnd = ((reg_ME != -1) && (reg_ME != 0)) && !(reg_EX == reg_ME) && (reg_ME == reg_A || reg_ME == reg_B); // make sure it is not access 0
    int fwd_des = -1;
    //reg_A is rs, reg_B is rt
    //but for sll/srl/sra, reg_A is rt.

    //check if branch(jump) or not
    if(is_branch(_ID) || (op_cur == 0x00 && func_cur == _jr)) {
        branch = 1;
        fwd_des = _ID;
    } else fwd_des = _EX;

    //if they are not equals to -1, it means that it can forward now
    //this will happen only after stall, so the if(ex_cnd) and if(me_cnd) will not be trigger
    //due to the reason that EX is NOP.
    if(branch && i->fwd_id) {
        //printf("do branch fowarding\n");
        return 0;
    }

    //refresh forwarding message
    if(i->fwd_id || i->fwd_ex) {
        i->fwd_id = 0;
        i->fwd_ex = 0;
        i->fwd_to_s = -1;
        i->fwd_to_t = -1;
    }


    if(ex_cnd && notNOP(1)) {
        int load = 0;
            //if it is load, need to stall. It's because the data cannot be access at EX stage
            if(is_load(op_ex)) result = 1;
            else {   
                fwd_signal(fwd_des, _EX, reg_EX, reg_A, reg_B);
                if(branch) result = 1;
            }
    }
    if(me_cnd && notNOP(2)) {
            // branch won't have chance to let ME-WB forward
            // when ID needs to compute, ME-WB might still not available
            // but when ME-WB is available
            // the data is already write back to register(first half cycle)
            if(!branch) fwd_signal(fwd_des, _ME, reg_ME, reg_A, reg_B);
            else {
                // special case. If load, it cannot fwd from EX_DM
                // if it's normal operation, and then it can do fwd;
                if(is_load(op_me)) result = 1;
                else fwd_signal(_ID, _EX, reg_ME, reg_A, reg_B);
            }
    }

    return result;
}


//this is use to record data about forwarding
//this data will be used on EXE stage.(branch on ID)
//remember to clear the signal after output to avoid endless forwarding
void fwd_signal(int fwd_to, int type, int reg_fwd, int reg_A, int reg_B) {
    struct ins* i = CPU.pipeline[0];
    if(reg_fwd == reg_A) i->fwd_to_s = type;
    if(reg_fwd == reg_B) i->fwd_to_t = type;

    if(fwd_to == _ID) i->fwd_id = 1;
    else i->fwd_ex = 1;
}



// this function is to give data from reg_EX_ME or reg_ME_WB
// it doesn't care who asking the data
int fwd_unit(int fwd_type) {
    int result;
    //if reg_type = 0 is s, 1 is t
    if(fwd_type == _EX) result = reg_read(EX_ME, 0);
    else result = reg_read(ME_WB, 0);
    return result;
}
