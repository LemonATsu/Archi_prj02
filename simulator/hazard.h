#ifndef _HAZARDH_
#define fwd_ID 0
#define fwd_EX 1
#define is_load(A) A==_lw || A ==_lh || A ==_lhu || A==_lb || A==_lbu
#endif
void fwd_init();
void fwd_ctrl(int fwd_to, int type, int);
void clear_fwd();
void fwd_signal(int fwd_to, int type, int reg_fwd, int reg_A, int reg_B); 
int hazard_check(int reg_EX, int reg_ME); //return 1 for stall, return 0 for nothing happen


int fwd_ID_s;
int fwd_ID_t;
int fwd_ID_type_s;
int fwd_ID_type_t;
int fwd_EX_s;
int fwd_EX_t;
int fwd_EX_type_s;
int fwd_EX_type_t;
int is_fwd_ID;
int is_fwd_EX;


