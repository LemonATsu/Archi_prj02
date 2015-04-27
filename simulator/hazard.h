#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
void fwd_ctrl();
void clear_fwd(struct ins* i);
void fwd_signal(int type, int reg_fwd, int reg_A, int reg_B, struct ins* i); 
int hazard_check(int reg_EX, int reg_ME); //return 1 for stall, return 0 for nothing happen
