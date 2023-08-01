#include  "../header/bsp.h"    // private library - BSP layer
//#include  <msp430xG46x.h> // IDE library
#include  <msp430g2553.h>          // MSP430x2xx
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

    TAPortSEL |= TA1;
    TAPortDIR |= TA1;

    TAPortSEL |= TA2;
    TAPortDIR &= ~TA2;    

    // LCD configuration
    LCD_DATA_WRITE &= ~0xF0;
    LCD_DATA_DIR |= 0xF0;    // P10.4-P10.7 To Output('1')
    LCD_DATA_SEL &= ~0xF0;   // Bit clear P10.4-P10.7
    LCD_CTL_SEL  &= ~0xE0;   // Bit clear P9.5-P9.7
    LCD_CTL_DIR  |= 0xE0;   // Bit clear P9.5-P9.7
    
    TriggerDIR |= Trigger;                            // P5.1 output
    TriggerSEL &= ~Trigger;
    Trigger_OUT &= ~Trigger;


    LDR1SEL |= LDR0;                            // Enable A/D channel A0
    LDR2SEL |= LDR1;                            // Enable A/D channel A0

    P1DIR &= ~BIT0 + ~BIT3;
    _BIS_SR(GIE);                     // enable interrupts globally

}

void TimerA1_Config(){
    WDTCTL = WDTPW +WDTHOLD;                  // Stop WDT
    //  TB0_CONFIG
    TA1CCR0 = MAX_TBR-3;                             // 60 ms Period/2

    //  TB1_CONFIG
    TA1CCTL1 |= OUTMOD_6 ;                       // TBCCR1 toggle/set
    TA1CCR1 = MAX_TBR-2;                              // TACCR1 PWM duty cycle
    
    //  TB2_CONFIG
    TA1CCTL2 |= CAP | CCIS_0 | CM_3 | SCS;                       // TACCR2 toggle/set
    /// removed ccie
 
    /**
     * WHY SHULD THEY HAVE CCIE>? MAYBE WE GO TO IDLE, WOULD THEY SSTILL NEED IT?
     * */
    TA1CTL |= TASSEL_2 | MC_1 | CCIE;          // counts to CCR0

//    TA1CCTL2 = CAP + CM_3 + CCIE + SCS + CCIS_0;
//                                            // TA0CCR1 Capture mode; CCI1A; Both
//                                            // Rising and Falling Edge; interrupt enable
//  TA1CTL |= TASSEL_2 + MC_2 + TACLR;        // SMCLK, Cont Mode; start timer

  
    _BIS_SR(GIE);                     // enable interrupts globally
}

void TimerA0_Config(){
    WDTCTL = WDTPW +WDTHOLD;                  // Stop WDT

    //  TB0_CONFIG
    TA0CCR0 = MAX_TBR-2;                             // 60 ms Period/2

    //  TB1_CONFIG
    TACTL |= TASSEL_2 | MC_1 | CCIE;          // counts to CCR0 //WHY DOES IT NEED CCIE RIGHT NOW??
    //TACTL |= TASSEL_2 | MC_1;
    _BIS_SR(GIE);                     // enable interrupts globally
}
/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                DCO config                                                       *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * sets the clock frequency of the microcontroller to                                              *
 * 1 MHz using the DCO (Digitally Controlled Oscillator).                                          *
 *                                                                                                 *
 *_________________________________________________________________________________________________*/

void DCO_config() {

    if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
        while(1);                               // do not load, trap CPU!!


    DCOCTL = 0;                               // Select lowest DCOx and MODx settings

    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO Frequency Range
    DCOCTL = CALDCO_1MHZ;                      // Set DCO specific frequency within the range



    //P2DIR = 0xFF;                             // All uuu.x outputs
    //P2OUT = 0;                                // All P2.x reset
    P1SEL = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
//    P1DIR |= RXLED + TXLED;
    P1OUT &= 0x01;
}
void UART_Config() {
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  
  UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
  UCA0BR0 = 104;                            // UART Baud Rate Control registers
  UCA0BR1 = 0x00;                           // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}


void delay_us(unsigned int del){
    TACTL = TACLR;
    TACTL |= TASSEL_2 | MC_1 | ID_0;                        // SMCLK, up-down mode

    TA0CCTL0 = CCIE;                                        // TACCR0 interrupt enabled
    TACCR0 = TAR+del;


    __bis_SR_register(LPM0_bits + GIE);
    TACTL &= ~CCIE;



  //  TBCCTL4 = OUTMOD_4 + CCIE;
}
void ADC_config0(){
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;             // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_0 + ADC10SSEL_3;                       // input A3 and SMCL // ADC10CLK/8

    ADC10AE0 |= BIT0;                                       // P1.3 ADC option select
}
void ADC_config1(){
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;             // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_3 + ADC10SSEL_3;                       // input A3 and SMCL // ADC10CLK/8
    ADC10AE0 |= BIT3;                                       // P1.3 ADC option select
}
void ADC_start(){
    ADC10CTL0 |= ENC + ADC10SC;                             // Sampling and conversion start
    //__bis_SR_register(LPM0_bits + GIE);

}

void ADC_stop(){
   // ADC10CTL0 &= ~ENC;                             // Sampling and conversion start
   // ADC10CTL0 &= ~ADC10SC;
    ADC10CTL0   =0x00;
    ADC10CTL1   =0x00;
    ADC10AE0    =0x00;
}

