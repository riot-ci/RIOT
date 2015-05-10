#ifndef MICROPYTHON_H
#define MICROPYTHON_H

#include <stdint.h>

#define MP_RIOT_HEAPSIZE (8*1024)

void mp_riot_init(char* stack_start, int stack_size, char* heap, int heap_size);
//static inline mp_riot_deinit(void) { mp_deinit(); }
void mp_do_str(const char *src, int len);

#endif /* MICROPYTHON_H */
