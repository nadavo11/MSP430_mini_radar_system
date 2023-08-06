#ifndef _api_H_
#define _api_H_
////UPDATE14;55
#include  "../header/halGPIO.h"     // private library - HAL layer
#define segment_size 64;

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
extern char  value[64]; 
extern char temp1[64];

extern void sendFormatMessage( int a, int b, int c, int d);
extern void UART_send();
extern void copy_C2D (void);
extern void write_SegC (char* value, int seg);
extern void flash_config();
extern int hexCharToInt(char c);
extern void inc_lcd(int x);
extern int hexChar2ToInt(char c[2]);
extern void dec_lcd(int x);
extern void ExecuteScript(char* Flash_ptrscript);
extern void script_delete();



//extern void scan_objects(int t);
//extern void servo_angle(int angle);
/************************************/

#endif







