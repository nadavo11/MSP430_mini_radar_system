#ifndef _bsp_H_
#define _bsp_H_
////UPDATE14;55
#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>  // MSP430x4xx

#define   debounceVal           100

#define TAPortSEL         P2SEL
#define TAPortDIR         P2DIR

#define TA0               BIT0
#define TA1               BIT1
#define TA2               BIT4

#define LDR1SEL           P1SEL
#define LDR2SEL           P1SEL

#define LDR0              BIT0
#define LDR1              BIT3

#define TriggerSEL        P2SEL

#define TriggerDIR        P2DIR
#define Trigger_OUT       P2OUT

#define Trigger           BIT3

#define Periode_60ms_val   60000
#define MAX_TBR            65534
//#define deley_4ms_val      4194
#define deley_10us_val       10
#define Periode_20ms_val   21000

// LCDs abstraction
#define LCD_DATA_WRITE          P1OUT
#define LCD_DATA_DIR            P1DIR
#define LCD_DATA_READ           P1IN
#define LCD_DATA_SEL            P1SEL


#define LCD_CTL_SEL             P2SEL
#define LCD_CTL_DIR             P2DIR

void UART_Config();
extern void GPIOconfig(void);
extern void TimerA0_Config();
extern void TimerA1_Config();
extern void delay_us(unsigned int del);
extern void delay2();
extern void ADC_config0();
extern void ADC_config1();
extern void ADC_start();
extern void ADC_stop();

//uart abstraction
#define TXLED                   BIT0
#define RXLED                   BIT6
#define TXD                     BIT2
#define RXD                     BIT1

extern void DCO_config();
#endif


