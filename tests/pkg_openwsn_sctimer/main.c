#include "stdint.h"
#include "string.h"

#include "ztimer.h"

#include "board.h"
#include "debugpins.h"
#include "sctimer.h"


#define REPETITIONS 100

uint32_t cnt = 0;
uint32_t cb_period[] = { 1, 2, 5, 10, 100, 200};
uint8_t idx = 0;

void cb_compare(void) {

   uint32_t now_32768 = sctimer_readCounter();
   uint32_t now_usec = ztimer_now(ZTIMER_USEC);

   cnt++;

   if (cnt == REPETITIONS) {
       printf("cb_period: %4"PRIu32"ms %"PRIu32"-times ", cb_period[idx], cnt);
       printf("%10"PRIu32" ticks %10"PRIu32" usec\n", now_32768, now_usec);
       idx++;
       idx %= (sizeof(cb_period)/sizeof(cb_period[0]));
       cnt = 0;
   }

   uint32_t next_wakeup = now_32768 + (cb_period[idx] * 32768) / 1000;
   sctimer_setCompare(next_wakeup);

   LED0_TOGGLE;
}


int main(void)
{
   sctimer_init();
   sctimer_set_callback(cb_compare);
   /* set callback as early as possible to trigger early */
   sctimer_setCompare(sctimer_readCounter());
}

