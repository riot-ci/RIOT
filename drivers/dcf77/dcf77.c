/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dcf77
 * @{
 *
 * @file
 * @brief       Device driver implementation for the dcf 77
 *              longwave time signal and standard-frequency radio station
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "assert.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dcf77.h"
#include "dcf77_params.h"


#define ENABLE_DEBUG    (1)
#include "debug.h"


/* Every pulse send by the DCF longer than 200ms new Minute is beginning */
#define PULSE_START_HIGH_THRESHOLD       (1500000) /*~1000ms*/
/* Every pulse send by the DCF longer than 100ms is interpreted as 1 */
#define PULSE_WIDTH_THRESHOLD       (140000U)  /*~150ms*/
/* If an expected pulse is not detected within 1s, something is wrong */
#define TIMEOUT                     (2500000) /*~0200ms*/
/* The DCF sensor cannot measure more than once a second */
#define DATA_HOLD_TIME              (US_PER_SEC)

#define READING_CYCLE               59



/**
 * @brief   Wait until the pin @p pin has level @p expect
 *
 * @param   pin     GPIO pin to wait for
 * @param   expect  Wait until @p pin has this logic level
 * @param   timeout Timeout in µs
 *
 * @retval  0       Success
 * @retval  -1      Timeout occurred before level was reached
 */
static inline int _wait_for_level(gpio_t pin, bool expect, unsigned timeout)
{
    while ((gpio_read(pin) != expect) && (timeout!=0)) {
        xtimer_usleep(1);
        timeout--;
    }

    return (timeout > 0) ? 0 : -1;
}


/**
 * @brief   get the bit of a single second
 *
 *
 * @param[in]  dev        Device Params of DCF77
 * @param[in]  startBit   first binary value
 * @param[in]  endBit     expected binary value
 * @param[in]  threshold  threshold to pass
 *
 * @retval `DCF77_OK`         Success
 * @retval `DCF77_TIMEOUT`    Reading data timed out (bad receiving)
 */
static uint8_t _read(bool *dest,gpio_t pin, bool startBit, bool endBit, uint32_t threshold){

  uint32_t start=0, end =0;
  if (_wait_for_level(pin, startBit, TIMEOUT)) {
      DEBUG("Timeout wait for %d\n",startBit);
      return DCF77_TIMEOUT;
  }
  start = xtimer_now_usec();

  if (_wait_for_level(pin, endBit, TIMEOUT)) {
      DEBUG("Timeout wait for %d\n", endBit);
      return DCF77_TIMEOUT;

  }
  end = xtimer_now_usec();

  if((end - start) > threshold ){
    *dest = 1;
  }else{
    *dest = 0;
  }
  return DCF77_OK;
}


static inline void _reset(dcf77_t *dev)
{
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_set(dev->params.pin);
}



/**
 * @brief   Decode the current Bit and save the values
 *
 *
 * @param[in]  bitNumber  Current Bit
 * @param[in]  result     value of the Current Bit
 * @param[in]  *data      Datastruct of the DCF77
 *
 * @retval `DCF_TIMEOUT`    Reading data timed out
 * @retval `DCF77_NOCSUM`   Parity Error
 * @retval `DCF77_OK`       Success
 */

static int8_t _decodeInformation(uint8_t bitNumber, uint8_t result, dcf77_data_t *data){
  switch (bitNumber) {
    case  0:
      printf("### S T A R T  N E W  M I N U T E\n");
      break;
    case WHEATER:
      DEBUG("### WEATHER DATA\n"); break;
    case CALLING:
      DEBUG("### CALLBIT RESERVE ANTENNA\n"); break;
    case MEZ_MESZ_SHIFT:
      DEBUG("### ANNOUNCEMENT SWITCH MEZ/MESZ\n"); break;
    case MEZ:
      DEBUG("### 0 = MEZ | 1 = MESZ\n");
      data->mesz = data->mesz +result;
      data->mesz = data->mesz <<1;
      break;
    case MESZ:
      DEBUG("### 0 = MESZ | 1 = MEZ\n");
      data->mesz = data->mesz +result;
      DEBUG("MESZ COUNTER: %d\n",data->mesz);
      break;
    case SHIFT_SECOND:
      DEBUG("### 1 = LEAP SECOND AT END OF HOUR\n"); break;
    case TIME_INFORMATION:
      DEBUG("### BEGIN TIMEINFORMATION\n");
      if(result!=1){
        DEBUG("### ERROR BIT 20 NOT 1...\n");
        return DCF77_NOCSUM;
        }
      break;
    case MINUTE_SINGLE:
      DEBUG("### MINUTE SINGLE\n");
      data->minute = data->minute+(POTENZY1*result);
      data->parity += result;
      break;
    case MINUTE_SECOND:
      DEBUG("### MINUTE SECOND\n");
      data->minute = data->minute+(POTENZY2*result);
      data->parity += result;
      break;
    case MINUTE_QUAD:
      DEBUG("### MINUTE QUAD\n");
      data->minute = data->minute+(POTENZY4*result);
      data->parity += result;
      break;
    case MINUTE_EIGHT:
      DEBUG("### MINUTE EIGHT\n");
      data->minute = data->minute+(POTENZY8*result);
      data->parity += result;
      break;
    case MINUTE_TENNER:
      DEBUG("### MINUTE TENNER\n");
      data->minute = data->minute+(POTENZY10*result);
      data->parity += result;
      break;
    case MINUTE_TEWENTIES:
      DEBUG("### MINUTE TWENTIES\n");
      data->minute = data->minute+(POTENZY20*result);
      data->parity += result;
      break;
    case MINUTE_FOURTIES:
      DEBUG("### MINUTE FOURTIES\n");
      data->minute = data->minute+(POTENZY40*result);
      data->parity += result;
      break;
    case MINUTE_PR:
      DEBUG("### PARITY MINUTE\n");
      if(((data->parity%2==0)&&(result==0))^
      ((data->parity%2==1)&&(result==1))){
        DEBUG("### PARITY OK\nMINUTES: %d\n",data->minute);
        data->parity=0;
        break;
      }else{
        DEBUG("### PARITY ERROR\nMINUTES: %d PARITY: %d\n",data->minute,result);
        return DCF77_NOCSUM;
      }
      break;
    case HOUR_SINGLE:
      DEBUG("### HOUR SINGLE\n");
      data->hours = data->hours+(POTENZY1*result);
      data->parity += result;
      break;
    case HOUR_SECOND:
      DEBUG("### HOUR_SECOND\n");
      data->hours = data->hours+(POTENZY2*result);
      data->parity += result;
      break;
    case HOUR_QUAD:
      DEBUG("### HOUR QUAD\n");
      data->hours = data->hours+(POTENZY4*result);
      data->parity += result;
      break;
    case HOUR_EIGHT:
      DEBUG("### HOUR EIGHT\n");
      data->hours = data->hours+(POTENZY8*result);
      data->parity += result;
      break;
    case HOUR_TENNER:
      DEBUG("### HOUR TENNER\n");
      data->hours = data->hours+(POTENZY10*result);
      data->parity += result;
      break;
    case HOUR_TEWENTIES:
      DEBUG("### HOUR TWENTIES\n");
      data->hours = data->hours+(POTENZY20*result);
      data->parity += result;
      break;
    case HOUR_PR:
      DEBUG("### PARITY HOUR\n");
      if(((data->parity%2==0)&&(result==0))^
      ((data->parity%2==1)&&(result==1))){
        DEBUG("### PARITY OK\nHOUR: %d\n",data->hours);
        data->parity=0;
        break;
      }else{
        DEBUG("### PARITY ERROR\n");
        return DCF77_NOCSUM;
      }
    case DAY_SINGLE:
      DEBUG("### DAY SINGLE\n");
      data->day = data->day+(POTENZY1*result);
      break;
    case DAY_SECOND:
      DEBUG("### DAY SECOND\n");
      data->day = data->day+(POTENZY2*result);
      break;
    case DAY_QUAD:
      DEBUG("### DAY QUAD\n");
      data->day = data->day+(POTENZY4*result);
      break;
    case DAY_EIGHT:
      DEBUG("### DAY EIGHT\n");
      data->day = data->day+(POTENZY8*result);
      break;
    case DAY_TENNER:
      DEBUG("### DAY TENNER\n");
      data->day = data->day+(POTENZY10*result);
      break;
    case DAY_TEWENTIES:
      DEBUG("### DAY TWENTIES\n");
      data->day = data->day+(POTENZY20*result);
      DEBUG("###CALENDERDAY: %d\n",data->day);
      break;
    case WEEKDAY_SINGLE:
      DEBUG("### WEEKDAY SINGLE\n");
      data->weekday = data->weekday+(POTENZY1*result);
      break;
    case WEEKDAY_SECOND:
      DEBUG("### WEEKDAY SECOND\n");
      data->weekday = data->weekday+(POTENZY2*result);
      break;
    case WEEKDAY_QUAD:
      DEBUG("### WEEKDAY QUAD\n");
      data->weekday = data->weekday+(POTENZY4*result);
      DEBUG("###WEEKDAY: %d\n",data->weekday);
      break;
    case MONTH_SINGLE:
      DEBUG("### MONTH SINGLE\n");
      data->month = data->month+(POTENZY1*result);
      break;
    case MONTH_SECOND:
      DEBUG("### MONTH SECOND\n");
      data->month = data->month+(POTENZY2*result);
      break;
    case MONTH_QUAD:
      DEBUG("### MONTH QUAD\n");
      data->month = data->month+(POTENZY4*result);
      break;
    case MONTH_EIGHT:
      DEBUG("### MONTH EIGHT\n");
      data->month = data->month+(POTENZY8*result);
      break;
    case MONTH_TENNER:
      DEBUG("### MONTH TENNER\n");
      data->month = data->month+(POTENZY10*result);
      DEBUG("###MONTH: %d\n",data->month);
      break;
    case YEAR_SINGLE:
      DEBUG("### YEAR SINGLE\n");
      data->year = data->year+(POTENZY1*result);
      data->parity += result;
      break;
    case YEAR_SECOND:
      DEBUG("### YEARH SECOND\n");
      data->year = data->year+(POTENZY2*result);
      data->parity += result;
      break;
    case YEAR_QUAD:
      DEBUG("### YEAR QUAD\n");
      data->year = data->year+(POTENZY4*result);
      data->parity += result;
      break;
    case YEAR_EIGHT:
      DEBUG("### YEAR EIGHT\n");
      data->year = data->year+(POTENZY8*result);
      data->parity += result;
      break;
    case YEAR_TENNER:
      DEBUG("### YEAR TENNER\n");
      data->year = data->year+(POTENZY10*result);
      data->parity += result;
      break;
    case YEAR_TEWENTIES:
      DEBUG("### YEAR TWENTIES\n");
      data->year = data->year+(POTENZY20*result);
      data->parity += result;
      break;
    case YEAR_FOURTIES:
      DEBUG("### YEAR FOURTIES\n");
      data->year = data->year+(POTENZY40*result);
      data->parity += result;
      break;
    case YEAR_EIGHTIES:
      DEBUG("### YEAR EIGHTIES\n");
      data->year = data->year+(POTENZY80*result);
      data->parity += result;
      break;
    case YEAR_PR:
      DEBUG("### PARITY YEAR\n");
      if(((data->parity%2==0)&&(result==0))^
      ((data->parity%2==1)&&(result==1))){
        DEBUG("### PARITY OK\nYEAR: %d\n",data->year);
        data->parity=0;
        break;
      }else{
        DEBUG("### PARITY ERROR\n");
        return DCF77_NOCSUM;
      }
    default:
      break;
}
DEBUG("Bit %d is %d\n",bitNumber,result);
return DCF77_OK;
}

/**
 * @brief   Initalize the Device
 *
 * @param   dev     device
 * @param   params  device_params
 *
 * @retval  DCF77_OK             Success
 * @retval  DCF77_INIT_ERROR     Error in initalization
 */

int dcf77_init(dcf77_t *dev, const dcf77_params_t *params)
{
    DEBUG("dcf77_init\n");

    /* check parameters and configuration */
    assert(dev && params);
    dev->params = *params;
    if(gpio_init(dev->params.pin, dev->params.in_mode)!=DCF77_NOCSUM){
    xtimer_usleep(2000 * US_PER_MS);

    DEBUG("dcf77_init: success\n");
    return DCF77_OK;
  }else{
    DEBUG("dcf77_init: failed\n");
    return DCF77_INIT_ERROR;
  }
}


/**
 * @brief   Read a complete minute.
 *
 * @param   dev           device
 * @param   minute        minute
 * @param   hour          hour
 * @param   weekday       weekday
 * @param   calenderday   calenderday
 * @param   month         month
 * @param   year          year
 * @param   mesz          mesz
 *
 * @retval  DCF77_OK          Success
 * @retval  DCF77_TIMEOUT     Timeout occurred before level was reached
 * @retval  DCF77_NOCSUM      Parity Bits arent correct
 */

int dcf77_read(dcf77_t *dev, uint8_t *minute, uint8_t *hour, uint8_t *weekday,
  uint8_t *calenderday, uint8_t *month, uint8_t *year, uint8_t *mesz){
  assert(dev);
  dcf77_data_t dcf77_data={};
  uint32_t start=0, end =0;
  bool result=false;

  /*Wait for the Start (Bit 0)*/
  while((end - start)<=PULSE_START_HIGH_THRESHOLD){
  if (_wait_for_level(dev->params.pin, 0, TIMEOUT)) {
     DEBUG("Timeout wait for LOW\n");
     return DCF77_TIMEOUT;
  }
  start = xtimer_now_usec();

  if (_wait_for_level(dev->params.pin, 1, TIMEOUT)) {
     DEBUG("Timeout wait for HIGH\n");
     return DCF77_TIMEOUT;

  }
  end = xtimer_now_usec();
  }

  /* Start with reading Bit 0*/

  for (int i = 0; i < READING_CYCLE; i++) {
    if(_read(&result,dev->params.pin,1,0,PULSE_WIDTH_THRESHOLD)==DCF77_OK){
     if(_decodeInformation(i, result, &dcf77_data)==DCF77_NOCSUM){
       return DCF77_NOCSUM;
     }
   }
  }
  *minute = dcf77_data.minute;
  *hour = dcf77_data.hours;
  *weekday = dcf77_data.weekday;
  *calenderday = dcf77_data.day;
  *month = dcf77_data.month;
  *year = dcf77_data.year;
  *mesz = dcf77_data.mesz;

  return DCF77_OK;
}
