#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void JoyStickADC_Painter();
extern void JoyStickADC_Steppermotor();
extern void Stepper_clockwise(long);
extern void Stepper_counter_clockwise(long);
extern void blinkRGB();
extern void clear_counters();
extern void change_delay_time();
extern void calibrate();
extern void ScriptFunc();
extern void ExecuteScript();
extern void ClearLEDsRGB();

extern int16_t Vrx;
extern int16_t Vry;






#endif







