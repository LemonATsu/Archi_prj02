// func
void alu_unit(int s, int t);

void add(int mode, int s, int t); // sub/add (1, 0)
void and(int mode, int s, int t); // nand/and
void or(int mode,  int s, int t); // nor/xor/or
void slt(int s, int t); 
void sll(int t, int shamt);
void sr(int mode, int t, int shamt); // sra/srl


void addi(int s, short c);
void load_store(int s, short c);

void lui(short c);
void andi(int s, short c);
void ori(int s, short c);
void nori(int s, short c);
void slti(int s, short c);
// beq, bne

void jr(int s); // not sure
void j(unsigned int c);
void jal(unsigned int c);




/*

void addi(int s, int t, short c);
void lw(int s, int t, short c);
void lh(int s, int t, short c, int mode); // lh/lhu
void lb(int s, int t, short c, int code);
void sw(int s, int t, short c);
void sh(int s, int t, short c); 
void sb(int s, int t, short c);
void lui(int t, short c);
void andi(int s, int t, short c);
void ori(int s, int t, short c);
void nori(int s, int t, short c);
void slti(int s, int t, short c);
void beq(int s, int t, short c);
void bne(int s, int t, short c);

void j(unsigned int c);
void jal(unsigned int c);
void jr(int s);


// func
void add(int s, int t, int d, int mode); // add/ sub
void and(int s, int t, int d, int mode);// and/nand
void or(int s, int t, int d, int mode);// or/xor/nor
void slt(int s, int t, int d);
void sll(int t, int d, int shamt);
void sr(int t, int d, int shamt, int mode); // srl/sra*/
