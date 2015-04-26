#include <stdio.h>
#include <stdlib.h>
#define CPU_REG 0
#define IF_ID 1
#define ID_EX 2
#define EX_ME 3
#define ME_WB 4
int reg_IF_ID[33];
int reg_ID_EX[33];
int reg_EX_ME[33];
int reg_ME_WB[33];
int reg[33];

void reg_init();

int reg_read(int type, int tar);
void reg_write(int type, int tar, int data);
