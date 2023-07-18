#ifndef _api_H_
#define _api_H_
////UPDATE14;55
#include  "../header/halGPIO.h"     // private library - HAL layer

extern void sysConfig(void);
extern void set_angel(int phi);
//
extern void lcd_reset();

void trigger_ultrasonic();

extern unsigned int del60ms;
extern unsigned int del10us;

//extern void scan_objects(int t);
//extern void servo_angle(int angle);
/************************************/

#endif







