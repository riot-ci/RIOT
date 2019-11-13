
/*
   Pin assigments for Radio Sensors board revision 2.3/2.4
   using MCUAtMega128rfa1/AtMega256rfr2
 */

#define BUTTON       PB0 /* Rev 2.4 only */
#define ISP_1        PB1 /* SCK */
#define ISP_2        PB2 /* MOSI */
#define ISP_3        PB3 /* MISO */

#define LED_YELLOW   PE3 /* 1k pullup to Vcc */
#define LED_RED      PE4 /* 1k pullup to Vcc */
#define USB_PWR      PB5 /* High if FTDI TTL-USB cable sources */
#define P0           PD2 /* Pulse count input. Pullup via jumper */
#define P1           PD3 /* Pulse count input via optional Comparator */
/* Alternative */
#define RX1           P0 /* RX UART1 */
#define TX1           P1 /* TX UART0 */
#define PWR_1        PE7 /* Programmable power pin Vcc via P-FET */

#define AV_IN        PF0 /* V_IN ADC input. 100k/1M volt. divider */
#define A1           PF1 /* A1 ADC input. 100k/300k volt. divider */
#define A2           PF2 /* A2 ADC input  100k/300k volt. divider */
#define A3           PF3 /* Light sensor A3 ADC input  100k/100k volt. divider */
#define A3_PWR       PF4 /* Light sensor power A3_PWR */

#define RX0          PE0 /* RX UART0 -- FTDI TTL-USB cable */
#define TX0          PE1 /* TX UART0 -- FTDI TTL-USB cable */

#define OW_BUS_0     PD7 /* One-Wire bus w temp/ID Pulled Up. Separate Vcc */
#define OW_BUS_1     PD6 /* One-Wire bus extra Pulled Up. Separate Vcc */

/* AVR ISP standard connected */
/* 16 MHz  xtal */
/* RTC 32.768 Hz  xtal */

#define HUM_PWR      PE2 /* NOT USED */

