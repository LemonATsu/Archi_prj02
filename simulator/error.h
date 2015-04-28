short error_signal = 0;
void error_init();
void write_to_zero();
void num_overflow(int a, int b);
void addr_overflow();
void misaligned();
void error_output(int cyc, FILE *output);
