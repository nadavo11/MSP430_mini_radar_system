#include  "../header/bsp.h"    // private library - BSP layer
#include  <msp430xG46x.h> // IDE library

////UPDATE14;55
/*______________________________________
 *                                      *
 *         stabalize                    *
 *______________________________________*/

void stabalize() {
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps

// Wait for xtal to stabilize
    do {
        IFG1 &= ~OFIFG;                           // Clear OSCFault flag
        for (i = 0x47FF; i > 0; i--);             // Time for flag to set
    } while ((IFG1 & OFIFG));                   // OSCFault flag still set?
}
/*______________________________________
 *                                      *
 *         GPIO configuration           *
 *______________________________________*/
void GPIOconfig(void){
    // LCD configuration
    LCD_DATA_WRITE &= ~0xFF;
    LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
    LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
    LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7

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

    TACCR0 = PERIOD-1;                           // PWM Period
    TACCTL1 = OUTMOD_7;                       // CCR1 reset/set
    TACCR1 = 256;                             // CCR1 PWM duty cycle
    TACTL = MC_0;                             // stop timer
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

    TACTL = TASSEL_2 + MC_1 + ID_3;          //  select: 2 - SMCLK;
                                            // control: 1 - Up;
                                            // divider: 3 - /8

    TACCTL1 = OUTMOD_7;                      // TACCR1 PWM
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
    P1SEL |= BIT2;                            // P2.2 TB1 option

    TBCCR0  = 0xFFFF;                         // CCR0 count to 0xFFFF
    TBCCTL1 = OUTMOD_7;                       // CCR1 reset/set forPWM
    TBCCR1 = A;                               // CCR1 count to 10 usec

    //timer/ clock:  P2.3/TB2
    P2DIR &= ~BIT3;                           // P2.3 input
    P2SEL |= BIT3;                            // P2.3 TB2 option

}

volatile float diff;
volatile int temp[2];
int i = 0;


int ultrasonic_measure(){
    volatile float distance;

    TBCTL = TBSSEL_2+MC_1;                      // SMCLK, upmode
    TBCCTL2 = CAP | CCIE |CCIS_1 |CM_3 | SCS;   // capture mode,
                                                // interrupt enable,
                                                // capture input select: 0 - CCIxB ?????🪲🪲🪲;
                                                // capture mode: 3 - both edges;
                                                // sync capture source
    __BIS_SR(LPM0_bits + GIE);                  // Enter LPM0 w/ interrupt

    distance = diff/58;
    dst_int = floor(distance);
    return dst_int;
}


// Timer_B interrupt service routine -- Toggle P5.1
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B (void)
{
    temp[i] = TBCCR2;
    i += 1;
    TBCCTL2 &= ~CCIFG ;
    if (i==2) {
        diff= temp[i-1]-temp[i-2];
        i=0;
    }
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






/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                DCO config                                                       *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * sets the clock frequency of the microcontroller to                                              *
 * 1 MHz using the DCO (Digitally Controlled Oscillator).                                          *
 *                                                                                                 *
 *_________________________________________________________________________________________________*/

/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                      UART init                                                  *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * initializes the UART communication module (USCI) for serial communication.                      *
 *  - UART CLK  - SMCLK                                                                            *
 *_________________________________________________________________________________________________*/
void UART_init() {
    UCA0CTL1 |= UCSSEL_2;                     // UART CLK <- SMCLK

    // configure the transmission rate to 9600 baud
    UCA0BR0 = 104;                           // UART Baud Rate Control registers
    UCA0BR1 = 0x00;                           // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
   // __bis_SR_register(LPM0_bits + GIE);       // Enter LPM3 w/ int until Byte RXed
}
