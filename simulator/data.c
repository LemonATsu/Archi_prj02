#include <stdio.h>
#include <stdlib.h>
#include "obj.h"


//address < 1024 -> have 256 words at most
static struct data* d_memo_data[1025];
//unsigned int sp;
//unsigned int data_num;
// global data has been put in obj.h


void data_init(struct words *d_img[]);
int data_read(int addr, int byte);
void data_write(int addr, int data, int byte);
//short int data_read_half(int addr);
//short int data_read_byte(int addr);
//void data_write_byte(int addr, int data);
int char_to_num(unsigned char code[]);

void data_init(struct words *d_img[]) {
    sp = char_to_num(d_img[0]->machine_code);
    data_num = char_to_num(d_img[1]->machine_code);
    //printf("stack pts: 0x%08x\n", sp);
    //printf("# of data: 0x%08x\n", data_num);

    //error type: address overflow, needs to halt
    if(data_num > ((MEMO_LIMIT / 4) - 1) || d_img[3 + data_num]) {
        //error handle
        //the error condition are still unsure
        //must halt
        printf("%d data_num\n", data_num);
        printf("error handle ...\n");
        if(d_img[3 + data_num]) {
            printf("More data then expect\n");
        } else {
            printf("Too many data\n");
            exit(EXIT_FAILURE);
        }
    }
    int x, y = 2 + data_num;
    for(x = 2; x < y; x++) {
        //save data into D memory, details are still unsure
        //printf("data: 0x%08x\n", char_to_num(d_img[x]->machine_code));
        struct data* read_dat = malloc(sizeof(struct data));
        d_memo_data[x - 2] = read_dat;
        if(!d_img[x]) {
            printf("fatal error! num of data in D memory doesn't match!\n");
            exit(EXIT_FAILURE);
        }

        int temp = char_to_num(d_img[x]->machine_code), y;
        for(y = 0; y < 4; y ++) {
            read_dat->dat[y] = (temp >> (8 * (3 - y))) & 0x000000ff;
        }
        read_dat->addr = (x - 2) * 4;
    }

   // initialize to zero
   x = MEMO_LIMIT / 4;
   for(y = y - 2; y < x; y ++) {
        int z;
        struct data* init_dat = malloc(sizeof(struct data));
        d_memo_data[y] = init_dat;
        for(z = 0; z < 4; z ++) {
            init_dat->dat[z] = 0x00000000;
        }
        init_dat->addr = y * 4;
   }
}

int data_read(int addr, int byte) {
    int result = 0, tar, fix = 3 - (4 - byte), out = 0;
    struct data* d_read;
    if((addr + byte - 1) >= MEMO_LIMIT || (addr >= MEMO_LIMIT) || (addr < 0)) {
        //error handle
        addr_overflow();
        out = 1;
    }
    if(
        (addr % 4 != 0 && byte == 4) ||
        (addr % 2 != 0 && byte == 2)){
        misaligned();
        out = 1;
    }
    if(out) return 0;

    if(addr == 0) {
        tar = 0;
    }  else {
        tar = addr / 4;
    }

    d_read = d_memo_data[tar];

    if(d_read) {
        int x = addr % 4, c;
        for(c = 0; c < byte; c ++) {
            result += d_read->dat[x + c] << (8 * ((byte - 1) - c));
        }
    }

    return result;
}



void data_write(int addr, int data, int byte){
    int out = 0;
    struct data* d_write;
    unsigned int tar;
    if((addr + byte - 1) >= MEMO_LIMIT || (addr >= MEMO_LIMIT) || addr < 0) {
        //error handle
        addr_overflow();
        out = 1;
    }
    if(
        (addr % 4 != 0 && byte == 4) ||
        (addr % 2 != 0 && byte == 2)) {
        misaligned();
        out = 1;
    }
    if(out) return;
 
    if(addr == 0) {
        tar = 0;
    } else {
        tar = addr / 4;
    }

    d_write = d_memo_data[tar];

    if(d_write) {
        int c;
        int x = addr % 4;
        for(c = 0; c < byte; c ++) {
            d_write->dat[x + c] = (data >> (8 * ((byte - 1) - c))) & 0x000000ff;
        }
    }
    else {
        //error handle
        printf("error: D memory out of bound(write)\n");
    }
}



int char_to_num(unsigned char code[]) {
    unsigned int result = 0;
    int x = 0;
    for(x = 0; x < 4; x++) {
        int i = code[x];
        //each char is 8bits, shift 8;
        result += (i << ((3 - x) * 8));
    }
    return result;
}

int sign_extend(int data, int sign_bit) {
    int result;
    if(data >> (sign_bit - 1) & 0x1 == 0x1) {
        if(sign_bit == 16) result = data | 0xffff0000;
        else if(sign_bit == 8) result = data | 0xffffff00;
    } else {
        if(sign_bit == 16) result = data | 0x00000000;
        else if(sign_bit == 8) result = data | 0x00000000;
    }
    return result;
}
/*short int data_read_half(int addr) {
    int tar;
    short int result = 0;
    struct data* d_read;

    if(addr > MEMO_LIMIT || addr < 0 || addr % 2 != 0) {
        //error handle
   
    }
    else if(addr == 0) tar = 0;
    else tar = addr / 4;
    d_read = d_memo_data[tar];
    if(addr % 4 != 0) {
        result = ((d_read->dat[2] << 8) + d_read->dat[3]) & 0x0000ffff;
    } else {
        result = ((d_read->dat[0] << 8) + d_read->dat[1]) & 0x0000ffff;
    }
    return result;
}

short int data_read_byte(int addr) {
    int tar;
    short int result = 0;
    struct data* d_read;
    if(addr > MEMO_LIMIT || addr < 0) {
        //error handle
   
    }
    else if(addr == 0) tar = 0;
    else tar = addr / 4;

    int offset = addr % 4;
    d_read = d_memo_data[tar];
    result = d_read->dat[offset];
    return result;
}*/
/*void data_write_half(int addr, int data){
    struct data* d_write;
    unsigned int tar;
    if(addr >= MEMO_LIMIT || addr < 0 || addr % 2 != 0) {
        //error handle
        printf("error: D memory out of bound(half): %08x, %x, %x\n", addr,data, pc);
        exit(EXIT_FAILURE);
    } else if(addr % 2 != 0) {
        //error handle
    }
    else if(addr == 0) tar = 0;
    else tar = addr / 4;

    d_write = d_memo_data[tar];
    if(d_write) {
        int offset = addr % 4;
        d_write->dat[offset] = (data >> 8) & 0x000000ff;
        d_write->dat[offset + 1] = data & 0x000000ff;
    } else {
        //error handle
    }

}
void data_write_byte(int addr, int data){
    struct data* d_write;
    unsigned int tar;
    if(addr >= MEMO_LIMIT || addr < 0) {
        //error handle
        printf("error: D memory out of bound(byte): %08x\n", addr);
        exit(EXIT_FAILURE);
    }
    else if(addr == 0) tar = 0;
    else tar = addr / 4;

    d_write = d_memo_data[tar];

    if(d_write) { 
        int offset = addr % 4;
        d_write->dat[offset] = data & 0x000000ff;
    } else {
        //error handle
    }
}*/
