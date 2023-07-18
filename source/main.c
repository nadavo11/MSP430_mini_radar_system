#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>
////UPDATE14;55
enum FSMstate state;
enum SYSmode lpm_mode;
//unsigned int i = 0;
unsigned int del60ms=Periode_60ms_val;
unsigned int del10us=10;
char st[16]="sdf",ff[16];
extern int temp[2], side;
volatile unsigned int Results[8];
unsigned int Index,g=0;
void main(void){
    P5OUT = 0x00;

    state = state2;       // start in idle state on RESET
    lpm_mode = mode0;     // start in idle state on RESET
    sysConfig();          // Configure GPIO, Stop Timers, Init LCD
    //_BIS_SR(CPUOFF);                          // Enter LPM0
    int a = 510;

    while(1){

        switch(state){
        case state0: //idle - Sleep

     //       enterLPM(mode0);
        break;

        case state1: //PB0 recorder
            while(1){    //servo motor
                a+=9;
                if(a>2310) a=510;
                set_angel(a);       // set CCR3
                delay_us(Periode_60ms_val);
            }
        break;

        case state2: //sonic
            while(1){
               trigger_ultrasonic();
                print_measurments(diff,diff);

//                sprintf(st, "%d", temp[0]);
//               sprintf(ff, "%d", temp[1]);
//               cursor_off;
//               lcd_reset();
//               lcd_puts(st);
//               lcd_puts(ff);
            }
        break;

        case state3:

            while(1){
                LDR_measurement(500);
                Results[0]=(3.3)*(4095-Results[0])/4095;
                Results[1]=(3.3)*(4095-Results[1])/4095;
                print_measurments(Results[0],Results[1]);


            }
        break;
        }

    }

}
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  Results[0] = ADC12MEM0;             // Move result, IFG is cleared
  Results[1] = ADC12MEM1;             // Move result, IFG is cleared

  //__no_operation();                         // SET BREAKPOINT HERE
  ADC12CTL0 &= ~ENC;
  LPM0_EXIT;

}

  
  
  
  
  
  
