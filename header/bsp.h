#ifndef _bsp_H_
#define _bsp_H_
////UPDATE14;55
//include  <msp430g2553.h>          // MSP430x2xx
#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal           100

// LCDs abstraction
#define LCD_DATA_WRITE          P1OUT
#define LCD_DATA_DIR            P1DIR
#define LCD_DATA_READ           P1IN
#define LCD_DATA_SEL            P1SEL
#define LCD_CTL_SEL             P2SEL


//uart abstraction
#define TXLED                   BIT0
#define RXLED                   BIT6
#define TXD                     BIT2
#define RXD                     BIT1

//  Keypad abstraction
#define KeypadPortSel         P10SEL
#define KeypadPortDIR         P10DIR
#define KeypadPortOUT         P10OUT
#define KeypadPortIN         P10IN
#define KeypadIRQPort         P2IN
#define KeypadIRQIntPend      P2IFG
#define KeypadIRQIntEn        P2IE
#define KeypadIRQIntEdgeSel   P2IES
#define KeypadIRQPortSel      P2SEL
#define KeypadIRQPortDir      P2DIR
#define KeypadIRQPortOut      P2OUT


//// PushButton 3 abstraction for Main Lab
//#define PB3sArrPort         P2IN
//#define PB3sArrIntPend      P2IFG
//#define PB3sArrIntEn        P2IE
//#define PB3sArrIntEdgeSel   P2IES
//#define PB3sArrPortSel      P2SEL
//#define PB3sArrPortDir      P2DIR
//#define PB3sArrPortOut      P2OUT

// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PBsArrPortOut      P1OUT
#define PB0                0x01   // P1.0
#define PB1                0x02  // P1.1
#define PB2                0x04  // P1.2
#define PB3                0x10  // P2.0

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
extern void StopAllTimers(void);
extern void confDMA3(void);


extern void ADC_config();
extern void ADC_start();

extern void UART_init();

#endif


