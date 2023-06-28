#include  <msp430xG46x.h> // IDE library
//#include <msp430.h>
#include  "../header/bspGPIO.h"    // private library
////UPDATE14;55
void InitGPIO(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LEDs 8-bit Array Port configuration
  LEDsArrayPortDir |= 0xff;
  LEDsArrayPort = 0x00;   			// clear LEDs port
  LEDsArrayPortSel &= ~0xC0; 		// only for "MSP430G2xx3"
  
  // PBs 4-bit Array Port configuration  
  PBsArrayPortSel &= ~0x03;             // PB.0,PB.1 as GPIO 
  PBsArrayPortDir &= ~0x03;             // PB.0,PB.1 as GPIO-input
  PBsArrayIntPending = 0x00;            // clear all interrupts pending
  PBsArrayIntEnable |= 0x03;            // enable PB.0,PB.1 interrupts 
  PBsArrayIntEdgeSel |=0x03;            // PB.0,PB.1 as falling-down edge select
}

 

 
  

