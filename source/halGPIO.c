#include  "../header/halGPIO.h"
unsigned int EndOfRecord = 0;

unsigned volatile int temp[2],i=0,j=0,diff,msc_cnt=0;
char message[40];
char new_x[50];
char script[64];
char c;
int First_Time = 0x01;
int count=0;

void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03)
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}

void enable_interrupts(){
  _BIS_SR(GIE);
}

void disable_interrupts(){
  _BIC_SR(GIE);
}

void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}

void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}

void lcd_puts(const char * s){
    int i=0;
    while(i<16 &&*s!='\0'){
        lcd_data(*s++);
        i++;
    }
    lcd_new_line;
    i=0;
    while(i<16 && *s!='\0'){
            lcd_data(*s++);
            i++;
            if((int)*(s)=='\0') i=16;
    }


}

void lcd_putrow(const char * s){
    int i=0;
    while(i<16 &&*s!='\0'){
        lcd_data(*s++);
        i++;
    }
}

void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}

void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}

void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);
    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}

void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}

void lcd_print_voltage(int num) {
    int thousands,hundreds,tens,ones;
    thousands = (num / 1000) + 0x30; // 0x30 is ACCI of '0'
    num %= 1000;
    hundreds = (num / 100) + 0x30;
    num %= 100;
    tens = (num / 10) + 0x30;
    num %= 10;
    ones = num + 0x30;
    lcd_data((char)thousands);
    lcd_data((char)hundreds);
    lcd_data((char)tens);
    lcd_data((char)ones);
}

void start_msg(){
  msc_cnt = 0;
  UCA0TXBUF = message[msc_cnt++];
  IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt

}

void start_PWM(){
  TA1CCTL1 |= CCIE;
}

void stop_PWM(){
  TA1CCTL1 &= ~CCIE;
}
/*
  TA1.1 ISR for capture and PWM


*/
// TA0_A1 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA1IV,0x0A))
  {
      case  TA1IV_NONE: break;              // Vector  0:  No interrupt
      case  TA1IV_TACCR2:                   // Vector  2:  TACCR1 CCIFG
          temp[i] = TA1CCR2;
          i += 1;
         // TA1CCTL1 &= ~CCIFG;
          
          if (i==2) {
              if(temp[0] < temp[1]){
                  diff = temp[1] - temp[0];
              }
              else{
                  //max value of TBR is 131071, in hex 0xFFFF
                  diff = 0xFFFF - temp[0] + temp[1];
              }
              i=0;
              LPM0_EXIT;
          }
          
        break;
      case TA1IV_TACCR1:        
        //TA1CCTL1 &= ~CCIFG;
        LPM0_EXIT;
        break;             // Vector  4:  TACCR2 CCIFG
      case TA1IV_6: break;                  // Vector  6:  Reserved CCIFG
      case TA1IV_8: break;                  // Vector  8:  Reserved CCIFG
      case TA1IV_TAIFG: break;              // Vector 10:  TAIFG
      default: 	break;
  }
}
//  #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//  #pragma vector = TIMER1_A1_VECTOR
//  __interrupt void Timer_A1 (void)
//  #elif defined(__GNUC__)
//  void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
//  #else
//  #error Compiler not supported!
//  #endif
//  {
//    switch(__even_in_range(TA1IV,0x0A)){
//        case TA1IV_NONE:
//            break;
//        /****** capture ISR *****
//         *
//         *      TB2 acts in capture
//         * *********************/
//        case TA1IV_TACCR1:           //CAPTURE ISR  
//          TA1CCTL1 &= ~CCIFG;
//          LPM0_EXIT;
//          
//            break;
//          
//        case TA1IV_TACCR2:           //CAPTURE ISR        
//              temp[i] = TA1CCR2;
//              i += 1;
//             // TA1CCTL1 &= ~CCIFG;
//          
//          if (i==2) {
//              if(temp[0] < temp[1]){
//                  diff = temp[1] - temp[0];
//              }
//              else{
//                  //max value of TBR is 131071, in hex 0xFFFF
//                  diff = 0xFFFF - temp[0] + temp[1];
//              }
//              i=0;
//              LPM0_EXIT;
//          }
//          break;
//        
//
//        }
//  }


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  LPM0_EXIT;
}



//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector = TIMER0_A1_VECTOR
//__interrupt void Timer_A01 (void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A01 (void)
//#else
//#error Compiler not supported!
//#endif
//{
//  TA0CCTL0 &= ~CCIE;           
//
//  LPM0_EXIT;
//
//}

/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                      TX ISR                                                     *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * handles the UART serial  communication module (USCI) Transmission interrupt                     *
 *                                                                                                 *
 *_________________________________________________________________________________________________*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)8565
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  c = message[msc_cnt++];                 // TX next character
  UCA0TXBUF = c;
  if (c == '\n')              // TX over?
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
    LPM0_EXIT;
}
/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                      RX ISR                                                     *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * handles the UART serial  communication module (USCI) Receiving  interrupt                       *
 *                                                                                                 *
 *_________________________________________________________________________________________________*/

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
//  IE2 &= ~UCA0RXIE;
  if(state==scriptmode){
    c = UCA0RXBUF;
    if(c == 'r')
        state = state0;
    else{
        script[j++] = c;
        if (j== 64 || c == '\n'){
            j = 0;
            //state = state0;
            //IE2 |= UCA0RXIE;
            LPM0_EXIT;

        }
        
    }

  }     else if  (UCA0RXBUF == '0')                     // '0' received?
        {state = state0; }      // Set state1

         else if (UCA0RXBUF == '1')                // '1' received?
        {state = state1; }      // Set state2

        else if (UCA0RXBUF == '2')                // '2' received?
        {state = state2; }      // Set state2

        else if (UCA0RXBUF == '3')                // '3' received?
        { state = state3; }   // Set state3

        else if(UCA0RXBUF == '4'){
            state=state4;

            IE2 |= UCA0TXIE;
            UCA0TXBUF ='4';
        }


        else if (UCA0RXBUF == '5')                // '5' received?
        {state = state5; }    // Set state5

        else if (UCA0RXBUF == '6')                // '6' received?
        {state = state6; }
            // Set state6
        else if (UCA0RXBUF == 's'){                 // 's' received?
            state = scriptmode;
        }


        else if (UCA0RXBUF == '8')                // '8' received?
        {state = state0; }                         // Set state0
 // IE2 |= UCA0RXIE;
  if (state!=state4){
    switch(lpm_mode){
        case mode0:
            LPM0_EXIT; // must be called from ISR only
            break;
        case mode1:
            LPM1_EXIT; // must be called from ISR only
            break;
        case mode2:
            LPM2_EXIT; // must be called from ISR only
            break;
        case mode3:
            LPM3_EXIT; // must be called from ISR only
            break;
        case mode4:
            LPM4_EXIT; // must be called from ISR only
            break;
    }
  }
}


// ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(CPUOFF);
}
