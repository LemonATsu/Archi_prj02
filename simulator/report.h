int need_o_ex;
int need_o_id;
int o_ex_s, o_ex_t;
int o_ex_st, o_ex_tt;
int o_id_s, o_id_t;
int o_id_st, o_id_tt;

void o_init(int fwd_to);
void o_sync_fwd(int fwd_to);
void fwd_output(int fwd_to, FILE *output);
void reg_output(int cyc, int stall, FILE *output);
void cycle_output(int stall, FILE *output);
