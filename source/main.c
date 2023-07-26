#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>

enum FSMstate state;
enum Stepperstate stateStepp;
enum SYSmode lpm_mode;


char *msg = "Wallak";
void main(void){

  state = stateJSRotate;  // start in idle state on RESET
  stateStepp = stateDefault;
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Init ADC


  UCA0TXBUF = 'a' & 0xFF;
  MSBIFG = 1;
  IE2 |= UCA0TXIE;
  while(1){
	switch(state){

	case state0: //   StepperUsingJoyStick
	    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	    switch(stateStepp){
	    case stateAutoRotate:
	        while(rotateIFG){ curr_counter++; Stepper_clockwise(200); }
	        break;

        case stateStopRotate:
            break;

        case stateJSRotate:
            counter = 514;
            StepperUsingJoyStick();
            break;
        case stateDefault:
         //   counter = 514;
        //    motorGoToPosition(360, 1);
            __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
            break;
	    }
	    break;

	case state1: // Paint
	    JoyStickIntEN |= BIT5;
	 //   IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	    while (state == state1){
	        JoyStickADC_Painter();
	    }
        JoyStickIntEN &= ~BIT5;
	    break;

	case state2: // Calibrate
        IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

        switch(stateStepp){
        case stateDefault:
            __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
            break;

        case stateAutoRotate: // start rotate
            counter = 0;
            while(rotateIFG) { Stepper_clockwise(100); counter++; }
            break;

        case stateStopRotate: // stop and set phi
            calibrate();
            break;
        }
	    break;

	case state3:  //Script
        IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	    while ( state == state3){
	        ScriptFunc();
	    }
		break;
		
	case state4: //

		break;

    case state5: //

        break;

    case state6: //

        break;

    case state7: //

        break;

    case state8: //

        break;
		
	}
  }
}

  
  
  
  
  
  
