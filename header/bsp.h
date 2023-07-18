#ifndef _bsp_H_
#define _bsp_H_
////UPDATE14;55
//include  <msp430g2553.h>          // MSP430x2xx
#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal           100

#define TB1PortSEL         P2SEL
#define TB1PortDIR         P2DIR


#define TB2PortSEL         P2SEL
#define TB2PortDIR         P2DIR

#define TB3PortSEL         P3SEL
#define TB3PortDIR         P3DIR

#define TB4PortSEL         P3SEL
#define TB4PortDIR         P3DIR

#define TB5PortSEL         P3SEL
#define TB5PortDIR         P3DIR

#define TB6PortSEL         P3SEL
#define TB6PortDIR         P3DIR

#define TA2PortSEL         P2SEL
#define TA2PortDIR         P2DIR




#define TB1                BIT2
#define TB2                BIT3
#define TB3                BIT4
#define TB4                BIT5
#define TB5                BIT6
#define TB6                BIT7

#define TA2                BIT0



#define LDR1SEL            P6SEL
#define LDR2SEL            P6SEL




#define Periode_60ms_val   60000
#define MAX_TBR            65534
//#define deley_4ms_val      4194
#define deley_10us_val       10
#define Periode_20ms_val   21000

// LCDs abstraction
#define LCD_DATA_WRITE          P10OUT
#define LCD_DATA_DIR            P10DIR
#define LCD_DATA_READ           P10IN
#define LCD_DATA_SEL            P10SEL
#define LCD_CTL_SEL             P9SEL


//uart abstraction
//#define TXLED                   BIT0
//#define RXLED                   BIT6
//#define TXD                     BIT2
//#define RXD                     BIT1
//



extern void GPIOconfig(void);
extern void TimerB_Config();
extern void delay_us(unsigned int del);
extern void delay2();

//  servo bsp
//extern void servo_config();
//extern void servo_stop();
//extern void servo_PWM(int DUTY_CYCLE);

//  Ultrasonic bsp
//extern void ultrasonic_config();
//extern int ultrasonic_measure();


//
//extern void TIMER_A0_config(void);
//extern void TIMERB_config(void);
//extern void startTimerB();
//extern void StopAllTimers(void);
//extern void confDMA3(void);

//
extern void ADC_config();
extern void ADC_start();
extern void ADC_stop();

//
//extern void UART_init();

#endif


