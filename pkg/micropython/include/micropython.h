#ifndef MICROPYTHON_H
#define MICROPYTHON_H

#include <stdint.h>

#ifndef MP_RIOT_HEAPSIZE
#define MP_RIOT_HEAPSIZE (8U*1024)
#endif

void mp_riot_init(char* heap, size_t heap_size);
//static inline mp_riot_deinit(void) { mp_deinit(); }
void mp_do_str(const char *src, int len);

#endif /* MICROPYTHON_H */
