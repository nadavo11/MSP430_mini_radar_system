#ifndef _api_H_
#define _api_H_
////UPDATE14;55
#include  "../header/halGPIO.h"     // private library - HAL layer


void LDR_measurement(unsigned volatile int arr[]);
void print_measurments(unsigned volatile int LLDR , unsigned volatile int RLDR);
void send_msg();
void start_PWM();


extern void sysConfig(void);
extern void set_angel(int phi);
//
extern void lcd_reset();

void trigger_ultrasonic();

extern unsigned int del60ms;
extern unsigned int del10us;

extern void sendFormatMessage( int a, int b, int c, int d);
extern void UART_send();

//extern void scan_objects(int t);
//extern void servo_angle(int angle);
/************************************/

#endif







