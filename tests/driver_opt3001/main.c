#include <stdio.h>
#include <inttypes.h>

#include "opt3001_params.h"
#include "opt3001.h"
#include "xtimer.h"

int main(void){

  opt3001_t dev;
  uint16_t crf;
  uint16_t rawlux;
  float convl;

  puts("OPT3001 test application\n");
  printf("Initializing OPT3001 sensor at I2C_%i ... ", opt3001_params[0].i2c_dev);

  if (opt3001_init(&dev, &opt3001_params[0]) != OPT3001_OK) {
      puts("init device [ERROR]");
      return -1;
  }

  if (opt3001_set_active(&dev) != OPT3001_OK) {
      puts("set active [ERROR]");
      return -1;
  }

  puts("[SUCCESS]\n");

  //int pre, now;

  while(1){

      //pre = xtimer_now_usec();

      opt3001_read(&dev, &crf, &rawlux);

      //now = xtimer_now_usec();

      //printf("TIME: %d\n", (now-pre));

      if (crf) {
          printf("Raw lux data: %5d\n", rawlux);
          opt3001_convert(rawlux, &convl);
          printf("Lux data: %d\n", (int)(convl*100));
      } else {
          puts("conversion in progress ...");
      }

  }

  return 0;
}
