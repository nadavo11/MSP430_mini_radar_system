#include  "../header/bsp.h"    // private library - BSP layer
#include  <msp430xG46x.h> // IDE library

////UPDATE14;55
/*______________________________________
 *                                      *
 *         stabalize                    *
 *______________________________________*/

//void stabalize() {
//    int j;
//    FLL_CTL0 |= XCAP14PF;                     // Configure load caps
//
//// Wait for xtal to stabilize
//    do {
//        IFG1 &= ~OFIFG;                           // Clear OSCFault flag
//        for ( j = 0x47; j > 0; j--);             // Time for flag to set
//    } while ((IFG1 & OFIFG));                   // OSCFault flag still set?
//}

/*______________________________________
 *                                      *
 *         GPIO configuration           *
 *______________________________________*/
void GPIOconfig(void){
    //P6DIR &= ~BIT6;             // input -> P6.6 -> AMPLIFIER -> DAC0

    TB1PortSEL |= TB1;
    TB1PortDIR |= TB1;

    TB2PortSEL |= TB2;
    TB2PortDIR &= ~TB2;

    TB3PortSEL |= TB3;
    TB3PortDIR |= TB3;

    TB4PortSEL |= TB4;
    TB4PortDIR |= TB4;

    TA2PortSEL |= TA2;
    TA2PortDIR |= TA2;

    // LCD configuration
    LCD_DATA_WRITE &= ~0xFF;
    LCD_DATA_DIR |= 0xF0;    // P10.4-P10.7 To Output('1')
    LCD_DATA_SEL &= ~0xF0;   // Bit clear P10.4-P10.7
    LCD_CTL_SEL  &= ~0xE0;   // Bit clear P9.5-P9.7
    P5DIR |= 0x02;                            // P5.1 output




    LDR1SEL |= 0x01;                            // Enable A/D channel A0
    LDR2SEL |= 0x02;                            // Enable A/D channel A0

    _BIS_SR(GIE);                     // enable interrupts globally

}

void TimerB_Config(){
    WDTCTL = WDTPW +WDTHOLD;                  // Stop WDT
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps
    int i;
    // Wait for xtal to stabilize
    do
    {
    IFG1 &= ~OFIFG;                           // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);             // Time for flag to set
    }
    while ((IFG1 & OFIFG));                   // OSCFault flag still set?

    //  TB0_CONFIG
    TBCCR0 = MAX_TBR-2;                             // 60 ms Period/2

    //  TB1_CONFIG
    TBCCTL1 |= OUTMOD_7;                       // TBCCR1 toggle/set
    TBCCR1 = 10;                              // TACCR1 PWM duty cycle
    //  TB2_CONFIG
    TBCCTL2 |= CAP | CCIE | CCIS_0 | CM_3 | SCS;                       // TACCR2 toggle/set

    //  TB3_CONFIG
   // TBCCTL3 = OUTMOD_7;                       // TACCR2 toggle/set
  //  TBCCR3 = 1000;                              // TACCR2 PWM duty cycle
    //  TB4_CONFIG
    TBCCTL4 |= OUTMOD_4 | CM_0;                       // TACCR2 toggle/set

    //  TB5_CONFIG

    //  TB6_CONFIG
    TBCTL = TBSSEL_2 + MC_1 + ID_0;                  // SMCLK, up-down mode






    TACCR0 = Periode_20ms_val;                             // 60 ms Period/2
    TACCTL2 = OUTMOD_7;                       // TACCR2 toggle/set
    TACCR2 = 2000;                              // TACCR2 PWM duty cycle

    TACTL = TASSEL_2 + MC_1;
    _BIS_SR(GIE);                     // enable interrupts globally
}

//void delay_us(unsigned int del){
//    TBCCTL4 |= CCIE;
//    if(TBR<Periode_60ms_val-del)
//        TBCCR4 = TBR+del;
//    else TBCCR4 = del-(Periode_60ms_val-TBR);
//    volatile int a=TBCCR4, b=TBR;
//    a++;
//    b++;
//    _BIS_SR(LPM0_bits + GIE);
//
//    TBCCTL4 &= ~CCIE;
//  //  TBCCTL4 = OUTMOD_4 + CCIE;
//}

void delay2(){
    volatile int f;
    f=0;
    f++;
    f--;

}

void delay_us(unsigned int del){
    TBCTL = TBCLR;
    TBCTL |= TBSSEL_2 | MC_1 | ID_0 ;                  // SMCLK, up-down mode
    TBCCTL4 |= CCIE;
    TBCCR4 = TBR+del;
    volatile int a=TBCCR4, b=TBR;
    a++;
    b++;
    _BIS_SR(LPM0_bits + GIE);
    TBCCTL4 &= ~CCIE;
  //  TBCCTL4 = OUTMOD_4 + CCIE;
}

void ADC_config(){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

    ADC12MCTL0 |= INCH_0;
    ADC12MCTL1 |= INCH_1 | EOS;

    ADC12CTL0 = ADC12ON + SHT0_8 + MSC;       // Turn on ADC12, set sampling time
    ADC12CTL1 = SHP + CONSEQ_3;               // Use sampling timer, set mode
    ADC12IE |=  ADC12IV_ADC12IFG0 | ADC12IV_ADC12IFG1;                           // Enable ADC12IFG.0
    ADC12CTL0 |= ENC ;                         // Enable conversions
    __enable_interrupt();                     // Enable interrupts
    ADC12CTL0 |= ADC12SC;                     // Start conversion

}

void ADC_start(){
    ADC12CTL0 |= ENC + ADC12SC;                             // Sampling and conversion start
}
void ADC_stop(){
    ADC12CTL0 &= ~ENC & ~ADC12SC;                             // Sampling and conversion start
}
/** _______________________________________________________________________________________________*
 *                                                                                                *
 *                                                                                                *
 *                               SERVO MOTOR                                                       *
 *                                                                                                *
 * _______________________________________________________________________________________________*
 * **/

//int PERIOD = 512-1;

/*______________________________________
 *                                      *
 *         SERVO config                 *
 *______________________________________*
 *  we use TimerA1.1 as PWM,
 *  output on P1.2
 * *************************************/
//
//void servo_config(){
//    P1DIR |= 0x04;                            // P1.2 output
//    P1SEL |= 0x04;                            // P1.2 TA1 option
//
//    TACCR0 = PERIOD-1;                        // PWM Period
//    TACCTL1 = OUTMOD_7;                       // CCR1 reset/set
//    TACCR1 = 256;                             // CCR1 PWM duty cycle
//    TACTL = TASSEL_1 + MC_1;                  // ACLK, up mode
//
//}


//void servo_stop(void){
//    TACTL = MC_0;                             // stop timer
//}

/*______________________________________
 *                                      *
 *         SERVO config                 *
 *______________________________________*
 *  DUTY -> [15, 75]
 * *************************************/
//void servo_PWM(int DUTY_CYCLE) {
//
//   // TACTL |= TASSEL_1 + MC_1;          //  select: 2 - SMCLK;
//                                            // control: 1 - Up;
//                                            // divider: 3 - /8
//
//    TACCR1 = DUTY_CYCLE;                     // CCR1 count to duty cycle
//
//}

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


//void ultrasonic_config(){
//
//    //trigger: TB1/ P2.2
//    P2DIR |= BIT2;                            // P2.2 output
//    P2SEL |= BIT2;                            // P2.2 TB1 option
//
//    TBCCR0  = 0xFFFF;                         // CCR0 count to 0xFFFF
//    TBCCTL1 = OUTMOD_7;                       // CCR1 reset/set forPWM
//    TBCCR1 = 0xA;                               // CCR1 count to 10 usec
//
//    //timer/ clock:  P2.3/TB2
//    P2DIR &= ~BIT3;                           // P2.3 input
//    P2SEL |= BIT3;                            // P2.3 TB2 option
//
//}


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

//void startTimerB(int x){
//    // count to x*8
//
//    TBCCR0 = 0xFFFE;  // Timer Cycles
//    TBCCR4 = x;        // Timer Cycles
//    TBCTL = TBSSEL_2 + MC_1 + ID_3;
//    TBCCTL4 = CCIE;
//                                        //  select: 2 - SMCLK ;
//                                        //control: 1 - Up  ;
//                                        //divider: 3 - /8
//                                        // no ACLCK, we use SMCLK.
//
//}


/*______________________________________
 *                                      *
 *          Stop Timer A0                *
 *______________________________________*/


//void StopAllTimers(void){
//
//    TA0CTL = MC_3; // halt timer A
//
//}


//-------------------------------------------------------------------------------------
//            Timer A1 PWM configuration
//-------------------------------------------------------------------------------------
//void TIMERB_config(void){
//       //FLL_CTL0 |= XCAP14PF;         // Configure load caps remove later
//    TA0CCTL1 = OUTMOD_4;         // TACCR1 toggle
//    TA0CCR0 = 1000;
//    TA0CTL = TASSEL_2 + MC_1;   //SMCLK, up-mode
//}



//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
//void ADC_config(){
//    ADC12CTL0 = ADC12SHT_2 + ADC12ON + ADC12IE;             // ADC12ON, interrupt enabled
//    ADC12CTL1 = INCH_3 + ADC12SSEL_3;                       // input A3 and SMCL // ADC12CLK/8
//    ADC12AE0 |= BIT3;                                       // P1.3 ADC option select
//}

//void ADC_start(){
//    ADC12CTL0 |= ENC + ADC12SC;                             // Sampling and conversion start
//}

