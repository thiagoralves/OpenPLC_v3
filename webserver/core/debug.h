#ifndef DEBUG_H
#define DEBUG_H

void set_endianness(uint8_t value);

uint16_t get_var_count(void);

//__attribute__((weak))
size_t get_var_size(size_t);// {return 0;}

//__attribute__((weak))
void *get_var_addr(size_t);// {return 0;}

//__attribute__((weak))
void force_var(size_t, bool, void *);// {}

//__attribute__((weak))
void set_trace(size_t, bool, void *);// {}

//__attribute__((weak))
void trace_reset(void);// {}

#endif
