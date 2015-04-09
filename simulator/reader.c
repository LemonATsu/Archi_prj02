#include <stdio.h>
#include <stdlib.h>
#include "obj.h"

void image_reader(struct words *ins_array[], struct words *d_array[]) {

    FILE *i_img_ptr = fopen("iimage.bin", "rb");
    FILE *d_img_ptr = fopen("dimage.bin", "rb");
    
    //Check if iimage.bin exist
    if(i_img_ptr && d_img_ptr) {
        int cnt = 0, d_cnt = 0 ;
        
        printf("successfully reading iimage.bin and dimage.bin\n");
        
        while(!feof(i_img_ptr)) {
            struct words *ins = malloc(sizeof(struct words));
            fread(ins->machine_code, sizeof(int), 1, i_img_ptr); 
            ins_array[cnt] = ins;
            cnt ++; 
        }


        while(!feof(d_img_ptr)) {
            struct words *d_m = malloc(sizeof(struct words));
            fread(d_m->machine_code, sizeof(int), 1, d_img_ptr); 
            d_array[d_cnt] = d_m;
            d_cnt ++; 
        }

        fclose(i_img_ptr);
        fclose(d_img_ptr);
        /*int x = 0;
        For(x = 0; x < cnt; x ++) {
            int y = 0;
            for(y = 0; y < 4; y ++) printf("%02x", ins_array[x].machine_code[y]);
            printf("\n");
        }
        for(x = 0; x < d_cnt; x ++) {
            int y = 0;
            for(y = 0; y < 4; y ++) printf("%02x", d_array[x].machine_code[y]);
            printf("\n");
        }*/
        //printf("reached end\n");
    } else {
        printf("failed: cannot open the file iimage.bin or dimage.bin\n");
        exit(EXIT_FAILURE);
    }
}

void endian_checker() {
    union {
        int i;
        char buf[4];
    } u;
    u.i = 0x12345678;
    if (u.buf[0] == 0x78) printf("env: little endian.\n");
    else printf("env: big endian.\n");
    u.i = u.i >> 4;
    printf("%x\n", u.i);
}
