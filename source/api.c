#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
//#include "msp430xG46x.h"
#include  <msp430g2553.h>          // MSP430x2xx
#include "stdio.h"
int fdfd=0;

void UART_send(){
    //while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    IE2 |= UCA0TXIE;                       // Disable USCI_A0 TX interrupt
    UCA0TXBUF = message[msc_cnt];                      // TX -> next character
    __bis_SR_register(LPM0_bits);

}

void sendFormatMessage( int a, int b, int c, int d) {
    // the format is |phi|ldr1|ldr2|us|\n
    snprintf(message, 30, "|%d|%d|%d|%d|\n", a, b, c, d);
    msc_cnt=0;
    UART_send();
}

void sysConfig(){
    GPIOconfig();
    TimerA0_Config();
    TimerA1_Config();
    lcd_init();
    DCO_config();
    UART_Config();

}

void set_angel(int phi){
    phi = phi*10 +510;
    TA1CCR1=phi;
    start_PWM();
}

void lcd_reset(){
    lcd_clear();
    lcd_home();
}

void long_delay(){
    delay_us(del60ms);
    delay_us(del60ms);
    delay_us(del60ms);
    delay_us(del60ms);
}

void LDR_measurement(unsigned volatile int arr[]){

    LDR1SEL |= LDR0;                            // Enable A/D channel A0
    LDR2SEL |= LDR1;                            // Enable A/D channel A0

    P1DIR |= BIT0 + BIT3;

        TACTL &= ~CCIE;

        fdfd++;
        ADC_config0();
        ADC_start();
        //ADC_stop();
        __bis_SR_register(LPM0_bits + GIE);
        ADC10CTL0 &= ~ADC10ON;                    // Disable ADC10 interrupt

        arr[0]=ADC10MEM;

        fdfd++;
        ADC_config1();
        ADC_start();

        __bis_SR_register(LPM0_bits + GIE);
        //ADC_stop();
        ADC10CTL0 &= ~ADC10ON;                    // Disable ADC10 interrupt

        arr[1]=ADC10MEM;
        fdfd++;
        
        long_delay();
        lcd_reset();
        fdfd++;
}

void trigger_ultrasonic(){
    delay_us(del60ms);
    TA1CCTL2 |=CCIE;

    delay_us(20);
    Trigger_OUT |= 0x08;
    delay_us(20);
    Trigger_OUT &= ~0x08;

    _BIS_SR(LPM0_bits + GIE);
    // remove interrupt enable
    TA1CCTL2 &= ~CCIE;
}

void print_measurments(unsigned int LLDR , unsigned int RLDR){
    char L[16],R[16];
    sprintf(L, "%d", LLDR);
    sprintf(R, "%d", RLDR);
    cursor_off;
    lcd_reset();
    lcd_putrow("RLDR: ");
    lcd_putrow(L);
    if(LLDR>RLDR) lcd_putrow(" LS");
    else lcd_putrow(" RS");
    lcd_new_line;
    lcd_putrow("LLDR: ");
    lcd_putrow(R);
    long_delay();
}

void send_msg(){
    int ss=12345;
    sprintf(message, "%d s", ss);
    start_msg();

}
