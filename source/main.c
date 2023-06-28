#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>
////UPDATE14;55
enum FSMstate state;
unsigned int KB;
enum SYSmode lpm_mode;
unsigned int i = 0;


void main(void){
    //realtime
    char my_s[100]  ="Wise machines are dramatically more valuable than machines that just store andretrieve information";
    char my_d[100];


    state = state1;       // start in idle state on RESET
    lpm_mode = mode0;     // start in idle state on RESET
    sysConfig();          // Configure GPIO, Stop Timers, Init LCD


        while(1){

            switch(state){
            case state0: //idle - Sleep

                enterLPM(mode0);
                break;

            case state1: //PB0 recorder
                scan_objects(x);
                break;


        }
    }
  
}

  
  
  
  
  
  
