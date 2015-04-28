#ifndef _REGISTERH_
#define CPU_REG 0
#define IF_ID 1
#define ID_EX 2
#define EX_ME 3
#define ME_WB 4
#endif
int reg_IF_ID;
int reg_ID_EX;
int reg_EX_ME;
int reg_ME_WB;
int reg[33];
int reg_temp[33];
void reg_init();

int reg_read(int type, int tar);
void reg_write(int type, int tar, int data);
