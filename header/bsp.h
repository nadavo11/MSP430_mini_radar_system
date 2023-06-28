#ifndef _bsp_H_
#define _bsp_H_
////UPDATE14;55
//include  <msp430g2553.h>          // MSP430x2xx
#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal           100

// LCDs abstraction
#define LCD_DATA_WRITE          P10OUT
#define LCD_DATA_DIR            P10DIR
#define LCD_DATA_READ           P10IN
#define LCD_DATA_SEL            P10SEL
#define LCD_CTL_SEL             P9SEL


//uart abstraction
#define TXLED                   BIT0
#define RXLED                   BIT6
#define TXD                     BIT2
#define RXD                     BIT1



//TIMERA0
extern void stopTimerA0(void);
extern void startTimerA1(int freq);
extern void stopTimerA1(void);
extern void startTimerB(int freq);

extern void GPIOconfig(void);
extern void stabalize();

//  servo bsp
extern void servo_config();
extern void servo_stop();
extern void servo_PWM(int DUTY_CYCLE);

//  Ultrasonic bsp
extern void ultrasonic_config();
extern int ultrasonic_measure();



extern void TIMER_A0_config(void);
extern void TIMERB_config(void);
extern void startTimerB();
extern void StopAllTimers(void);
extern void confDMA3(void);


extern void ADC_config();
extern void ADC_start();

extern void UART_init();

#endif


