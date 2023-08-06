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
volatile unsigned int Results[2];
unsigned int Index,g=0;

char value[64];   
int data1=0,data2=0,data3=0,data4;

int y=0;

int z=0;
//segB[]="0X1040",segC[]="0X1080";

void main(void){
    P2OUT = 0x00;

    state = state1;       // start in idle state on RESET
    lpm_mode = mode0;     // start in idle state on RESET
    sysConfig();          // Configure GPIO, Stop Timers, Init LCD
    //_BIS_SR(CPUOFF);                          // Enter LPM0
    int a = 0;
    int inc = 3;

    while(1){

        switch(state){
        case state0: //idle - Sleep
            lcd_reset();
            _BIS_SR(LPM0_bits + GIE);

        break;

        case state1: //PB0 recorder
            TimerA0_Config();

            a = 0;
            while(state == state1){    //servo motor
                a+=inc;
                if(a>179 -inc || a < -inc){
                  inc*=-1;
                  a+=4*inc;
                }


                set_angel(a);       // set CCR3
                for(z=0; z<4; z++){
                  delay_us(Periode_60ms_val);
                }

                LDR_measurement(Results);
                trigger_ultrasonic();
                Results[0]=(Results[0]+Results[1])/2;
                //Results[0]*=0.1;
             //   print_measurments(Results[0] ,Results[1]);
                delay_us(Periode_60ms_val);
                TA1CCTL2 &= ~CCIE;
                TA1CCTL1 &= ~CCIE;
                TA1CCTL0 &= ~CCIE;
                TA0CTL &= ~TAIE;
                //diff=diff/10;
                diff=diff*0.69;
                print_measurments(Results[0]  ,diff);

                sendFormatMessage(a,Results[0] ,Results[1],diff);
                delay_us(1500);
                //DelayUs(1600);
                
                sprintf(value, "%d|%d|%d|%d", diff,a,Results[0],Results[1]);
                write_SegC(value, segments[(seg)%3]);                    // Write segment C, increment value
                // copy_C2D();                             // Copy segment C to D
                __no_operation(); 
            //    stop_PWM();

            }
        break;

        case state2: //sonic
            while(1){
               trigger_ultrasonic();
                _BIS_SR(LPM0_bits + GIE);

                sprintf(st, "%d", diff);

               cursor_off;
               lcd_reset();
               lcd_puts(st);
               delay_us(Periode_60ms_val);
            }
        break;

        case state3:

            while(1){
                LDR_measurement(Results);
                print_measurments(Results[0] ,Results[1]);
            }
        break;
        case state4:
            while(1){
                send_msg();

                _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 w/ interrupt

            }
        break;
        case scriptmode:
            while(state == scriptmode){
                lcd_reset();
                lcd_puts("script mode");


                IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt

                _BIS_SR(LPM0_bits + GIE);                 // Enter LPM3 w/ interrupt
                delay_us(1500);
                if(state==scriptmode){
                    //cast_script();
                    write_SegC(script, segments[(seg)%3]);

                }
                                   // Write segment C, increment value
                ExecuteScript((char*)segments[(seg)%3]);
                // copy_C2D();                             // Copy segment C to D
                lcd_data(0x30+seg);
                TA1CCTL2 &= ~CCIE;
                TA1CCTL1 &= ~CCIE;
                TA1CCTL0 &= ~CCIE;
                TA0CTL &= ~TAIE;

            }
            break;
        case state6:
          while(state == state6)                                  // Repeat forever
          {
            sprintf(value, "%d|%d|%d|%d", data1++,data2++,data3++,data4++);
            write_SegC(value, segments[(seg++)%3]);                    // Write segment C, increment value
           // copy_C2D();                             // Copy segment C to D
            __no_operation();                       // SET BREAKPOINT HERE
          }
        }

    }

}


  
  
  
  
  
  
