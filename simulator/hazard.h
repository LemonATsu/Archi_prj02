void fwd_init();
int fwd_unit(int fwd_type);
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


