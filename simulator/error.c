#include <stdio.h>
#include <stdlib.h>
#include "obj.h"
short error_signal = 0;
void error_init();
void write_to_zero();
void num_overflow(int a, int b);
void addr_overflow();
void misaligned();
void error_output(int cyc, FILE *output);

void error_init() {
    error_signal = 0;
    error_halt = 0;
}

void write_to_zero() {
    error_signal = error_signal | 0x1;
}

void num_overflow(int a, int b) {
    int mask = 0x80000000;
    int sign_a = a & mask;
    int sign_b = b & mask;
    if(sign_a == sign_b) {
        
        int sign = a & mask;
        int temp = (a + b) & mask;
        if(sign != temp) {
            //printf("num_overflow_occur\n");
            error_signal = error_signal | 0x2;
        }
    }
}


void addr_overflow() {
    error_signal = error_signal | 0x14;
}

void misaligned() {
    error_signal = error_signal | 0x18;
}

void error_output(int cyc, FILE *output) {
    int x = 0;
    if(error_signal & 0x1) fprintf(output, "In cycle %d: Write $0 Error\n", cyc);
    if((error_signal >> 1) & 0x1) fprintf(output, "In cycle %d: Number Overflow\n", cyc);
    if((error_signal >> 2) & 0x1) fprintf(output, "In cycle %d: Address Overflow\n", cyc);
    if((error_signal >> 3) & 0x1) fprintf(output, "In cycle %d: Misalignment Error\n", cyc);
    if((error_signal >> 4) & 0x1) error_halt = 1;
    error_signal = 0;
}
