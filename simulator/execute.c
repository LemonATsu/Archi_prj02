#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "obj.h"
//one extra reg to make sure it is safe.

void cycle_output(int cyc, FILE *output);
void execute();

void execute() {
    int x = 0;
    int halt_spot = false;
    int cyc = 0;
    FILE *snap_file = fopen("snapshot.rpt", "w");
    FILE *err_file = fopen("error_dump.rpt", "w");
    error_init();

    reg_init();
    
    while(!halt_spot) {
        if(!cyc) {
            reg[_sp] = sp;
            cycle_output(cyc, snap_file);
            cyc ++;
            if(ins_num == 0) break;
            continue;
        }

        struct ins* cur_ins = i_memo[pc / 4]; 
        int opcode = (cur_ins->bits >> 26) & 0x0000003f;
        int code = cur_ins->bits;
        //printf("current pc, stack:%x %x qqq\n", pc, reg[_sp]);        
        if(opcode) {
        //I or J type
            int s = (code >> 21) & 0x0000001f;
            int t = (code >> 16) & 0x0000001f;
            short c = code & 0x0000ffff;
            unsigned int tar = code & 0x03ffffff;
            switch(opcode) {
                case _halt:
                    halt_spot = true;
                    break;
                case _addi:
                    addi(s, t, c);
                    break;
                case _lw:
                    lw(s, t, c);
                    break;
                case _lh:
                    lh(s, t, c, 1); //arg2: 1 for lh, 0 for lhu;
                    break;
                case _lhu:
                    lh(s, t, c, 0); //arg2: 1 for lh, 0 for lhu;
                    break;
                case _lb:
                    lb(s, t, c, 1); //arg2: 1 for lb, 0 for lhu;
                    break;
                case _lbu:
                    lb(s, t, c, 0); //arg2: 1 for lb, 0 for lhu;
                    break;
                case _sw:
                    sw(s, t, c);
                    break;
                case _sh:
                    sh(s, t, c);
                    break;
                case _sb:
                    sb(s, t, c);
                    break;
                case _lui:
                    lui(t, c);
                    break;
                case _andi:
                    andi(s, t, c);
                    break;
                case _ori:
                    ori(s, t, c);
                    break;
                case _nori:
                    nori(s, t, c);
                    break;
                case _slti:
                    slti(s, t, c);
                    break;
                case _beq:
                    beq(s, t, c);
                    break;
                case _bne:
                    bne(s, t, c);
                    break;
                case _j:
                    j(tar);
                    break;
                case _jal:
                    jal(tar);
                    break;
            }
        } else {
            int func = cur_ins->bits & 0x0000003f;
            int s = (code >> 21) & 0x0000001f;
            int t = (code >> 16) & 0x0000001f;
            int d = (code >> 11) & 0x0000001f;
            int shamt = (code >> 6) & 0x0000001f;
            switch(func) {
                case _add:
                    add(s, t, d, 1); //arg2: 1 for add, 0 for sub;
                    break;
                case _sub:
                    add(s, t, d, 0); //arg2: 1 for add, 0 for sub;
                    break;
                case _and:
                    and(s, t, d, 1); //arg2: 1 for and, 0 for nand;
                    break;
                case _or:
                    or(s, t, d, 2); // arg2 : 2 for or, 1 for xor, 0 for nor;
                    break;
                case _xor:
                    or(s, t, d, 1); // arg2 : 2 for or, 1 for xor, 0 for nor;
                    break;
                case _nor:
                    or(s, t, d, 0); // arg2 : 2 for or, 1 for xor, 0 for nor;
                    break;
                case _nand:
                    and(s, t, d, 0); //arg2: 1 for and, 0 for nand;
                    break;
                case _slt:
                    slt(s, t, d);
                    break;
                case _sll:
                    sll(t, d, shamt);
                    break;
                case _srl:
                    sr(t, d, shamt, 1); //arg2: 1 for srl, 0 for sra
                    break;
                case _sra:
                    sr(t, d, shamt, 0); //arg2: 1 for srl, 0 for sra
                    break;
                case _jr:
                    jr(s);
                    break;
            }
        } 

        error_output(cyc, err_file);
        if(halt_spot || error_halt || cyc == 500001) break;
        //output
        pc += 4;
        cycle_output(cyc, snap_file);
        cyc ++;
    }

    fclose(snap_file);
    fclose(err_file);
}


void cycle_output(int cyc, FILE *output) {
    int x = 0;

    fprintf(output, "cycle %d\n", cyc);
    for(x = 0; x < 32; x ++) fprintf(output, "$%02d: 0x%08X\n", x, reg[x]);
    fprintf(output, "PC: 0x%08X\n\n\n", pc);

}
