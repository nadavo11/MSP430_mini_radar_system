#include  "../header/bsp.h"    // private library - BSP layer
#include  <msp430xG46x.h> // IDE library

////UPDATE14;55
/*______________________________________
 *                                      *
 *         stabalize                    *
 *______________________________________*/

void stabalize() {
    int j;
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps

// Wait for xtal to stabilize
    do {
        IFG1 &= ~OFIFG;                           // Clear OSCFault flag
        for ( j = 0x47; j > 0; j--);             // Time for flag to set
    } while ((IFG1 & OFIFG));                   // OSCFault flag still set?
}
/*______________________________________
 *                                      *
 *         GPIO configuration           *
 *______________________________________*/
void GPIOconfig(void){

    // LCD configuration
    LCD_DATA_WRITE &= ~0xFF;
    LCD_DATA_DIR |= 0xF0;    // P10.4-P10.7 To Output('1')
    LCD_DATA_SEL &= ~0xF0;   // Bit clear P10.4-P10.7
    LCD_CTL_SEL  &= ~0xE0;   // Bit clear P9.5-P9.7

    _BIS_SR(GIE);                     // enable interrupts globally

}

/** _______________________________________________________________________________________________*
 *                                                                                                *
 *                                                                                                *
 *                               SERVO MOTOR                                                       *
 *                                                                                                *
 * _______________________________________________________________________________________________*
 * **/
int PERIOD = 512-1;
/*______________________________________
 *                                      *
 *         SERVO config                 *
 *______________________________________*
 *  we use TimerA1.1 as PWM,
 *  output on P1.2
 * *************************************/

void servo_config(){
    P1DIR |= 0x04;                            // P1.2 output
    P1SEL |= 0x04;                            // P1.2 TA1 option

    TACCR0 = PERIOD-1;                        // PWM Period
    TACCTL1 = OUTMOD_7;                       // CCR1 reset/set
    TACCR1 = 256;                             // CCR1 PWM duty cycle
    TACTL = TASSEL_1 + MC_1;                  // ACLK, up mode

}

void servo_stop(void){
    TACTL = MC_0;                             // stop timer
}
/*______________________________________
 *                                      *
 *         SERVO config                 *
 *______________________________________*
 *  DUTY -> [15, 75]
 * *************************************/
void servo_PWM(int DUTY_CYCLE) {

   // TACTL |= TASSEL_1 + MC_1;          //  select: 2 - SMCLK;
                                            // control: 1 - Up;
                                            // divider: 3 - /8

    TACCR1 = DUTY_CYCLE;                     // CCR1 count to duty cycle

}

/** _______________________________________________________________________________________________*
 *                                                                                                *
 *                                                                                                *
 *                               Ultrasonic                                                       *
 *                                                                                                *
 * _______________________________________________________________________________________________*
 *
 *   ACLK = 32kHz, SMCLK = MCLK = TBCLK = default DCO ~1048kHz.
 *  //Connections
 *  P2.3 ECHO
 *  P2.2 TRIGGER
 * ***********************************************************************************************/


void ultrasonic_config(){

    //trigger: TB1/ P2.2
    P2DIR |= BIT2;                            // P2.2 output
    P2SEL |= BIT2;                            // P2.2 TB1 option

    TBCCR0  = 0xFFFF;                         // CCR0 count to 0xFFFF
    TBCCTL1 = OUTMOD_7;                       // CCR1 reset/set forPWM
    TBCCR1 = 0xA;                               // CCR1 count to 10 usec

    //timer/ clock:  P2.3/TB2
    P2DIR &= ~BIT3;                           // P2.3 input
    P2SEL |= BIT3;                            // P2.3 TB2 option

}


/** _______________________________________________________________________________________________*
*                                                                                                *
*                                                                                                *
*                               TIMER DELAY                                                       *
*                                                                                                *
* _______________________________________________________________________________________________*
        *

* ***********************************************************************************************/
/*______________________________________
 *                                      *
 *              Timer B4                 *
 *      x*1msec configuration           *
 *______________________________________*/

//              StartTimer For Count Down

void startTimerB(int x){
    // count to x*8

    TBCCR0 = 0xFFFE;  // Timer Cycles
    TBCCR4 = x;        // Timer Cycles
    TBCTL = TBSSEL_2 + MC_1 + ID_3;
    TBCCTL4 = CCIE;
                                        //  select: 2 - SMCLK ;
                                        //control: 1 - Up  ;
                                        //divider: 3 - /8
                                        // no ACLCK, we use SMCLK.

}


/*______________________________________
 *                                      *
 *          Stop Timer A0                *
 *______________________________________*/


void StopAllTimers(void){

    TA0CTL = MC_3; // halt timer A

}


//-------------------------------------------------------------------------------------
//            Timer A1 PWM configuration
//-------------------------------------------------------------------------------------
void TIMERB_config(void){
       //FLL_CTL0 |= XCAP14PF;         // Configure load caps remove later
    TA0CCTL1 = OUTMOD_4;         // TACCR1 toggle
    TA0CCR0 = 1000;
    TA0CTL = TASSEL_2 + MC_1;   //SMCLK, up-mode
}



//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
//void ADC_config(){
//    ADC12CTL0 = ADC12SHT_2 + ADC12ON + ADC12IE;             // ADC12ON, interrupt enabled
//    ADC12CTL1 = INCH_3 + ADC12SSEL_3;                       // input A3 and SMCL // ADC12CLK/8
//    ADC12AE0 |= BIT3;                                       // P1.3 ADC option select
//}

void ADC_start(){
    ADC12CTL0 |= ENC + ADC12SC;                             // Sampling and conversion start
}

