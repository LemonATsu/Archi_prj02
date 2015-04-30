#include <stdio.h>
#include <stdlib.h>
#include "obj.h"

void test_printer(struct words* a[], struct words* b[]);

int main(int argc, char **argv){
    //start reading data
    struct words *ins_array[300000];
    struct words *d_array[300000];
    image_reader(ins_array, d_array);
    //printf("start data init\n");
    data_init(d_array);
    //printf("start ins init\n");
    ins_init(ins_array);
    //printf("start execute\n");
    execute();
    //data_write(12, 0x56781234);
    //printf("i know sp too: 0x%08x, with data: %08x\n", sp, data_read(12));
    //test_printer(ins_array, d_array);
}

void test_printer(struct words* a[], struct words* b[]) {
    // for check ins_array and d_array 
    int i = 0, x = 0;
    unsigned int out = 0;
    printf("a:\n");
    while(a[i]) {
        printf("%08x", char_to_num(a[i]->machine_code));
        printf("\n");
        i ++;
    }
    i = 0;
    printf("b:\n");
    while(b[i]) {
        printf("%08x", char_to_num(b[i]->machine_code));
        printf("\n");
        i ++;
    }
}
