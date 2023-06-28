#include  "../header/halGPIO.h"     // private library - HAL layer
////UPDATE14;55
// Global Variables
unsigned int EndOfRecord = 0;
int j=0;
unsigned int KBIFG = 0;
char LED_STATE = 0x1;
char new_x[5];
int x_pending = 0;
unsigned int x = 500;
int current_buzz = 0;


/*__________________________________________________________
 *                                                          *
 *             System Configuration                       *
 *__________________________________________________________*/
void sysConfig(void){
    stabalize();
	GPIOconfig();

    servo_config();

	//StopAllTimers();


    //UART_init();
}
//____________________________________________________________

/*__________________________________________________________
 *                                                          *
 *            General Function - No need                       *
 *__________________________________________________________*/

void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}
/*__________________________________________________________
 *                                                          *
 *            Enter LPM mode                                *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *             Enable interrupts                       *
 *__________________________________________________________*/

void enable_interrupts(){
  _BIS_SR(GIE);
}
/*__________________________________________________________
 *                                                          *
 *             Disable interrupts                      *
 *__________________________________________________________*/

void disable_interrupts(){
  _BIC_SR(GIE);
}
/*__________________________________________________________
 *                                                          *
 *            LCD                       *
 *__________________________________________________________*/
/*__________________________________________________________
 *                                                          *
 *           send a command to the LCD                       *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *          send data to the LCD                      *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *          write a string of chars to the LCD                     *
 *__________________________________________________________*/

void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

/******************************************************************
 * Displays a two-digit integer on the LCD screen.
 * If the integer is less than 10, a leading zero is displayed.
 *
 * @param num The integer to display (should be between 0 and 99).
 */
void lcd_print_num(int num) {
  // Extract the tens and ones digits
  int tens = num / 10 + 0x30;
  int ones = num % 10 + 0x30;

  // Display the tens digit

  lcd_data((char)tens);


  // Display the ones digit
  lcd_data((char)ones);
}


void lcd_print_voltage(int num) {
    // Extract the tens and ones digits

    int thousands = num / 1000 + 0x30;
    int res = num % 1000;

    int hundreds = res / 100 + 0x30;
    res = num % 100;

    int tens = res / 10 + 0x30;
    res = res % 10;

    int ones = res % 10 + 0x30;

    // Display the thousands digit
    lcd_data((char)thousands);

    // Display the hundreds digit
    lcd_data((char)hundreds);

    // Display the tens digit
    lcd_data((char)tens);

    // Display the ones digit
    lcd_data((char)ones);
}


/*__________________________________________________________
 *                                                          *
 *          initialize the LCD                     *
 *__________________________________________________________*/

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

/*__________________________________________________________
 *                                                          *
 *         lcd strobe functions                     *
 *__________________________________________________________*/
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}

/*__________________________________________________________
 *                                                          *
 *         Polling delays                     *
 *__________________________________________________________*/
/*__________________________________________________________
 *                                                          *
 *         Delay usec functions                     *
 *__________________________________________________________*/
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
/*__________________________________________________________
 *                                                          *
 *          Delay msec functions                      *
 *__________________________________________________________*/

void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}



/********************************************************************************
 *                                                                              *
 *                          TimerA0 ISR                                         *
 *                                                                              *
 *______________________________________________________________________________*/


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

    /*__________________________________________________________
     *                                                          *
     *            Exit from a given LPM                         *
     *__________________________________________________________*/
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
            TACTL = MC_0+TACLR;
}
/*__________________________________________________________
 *                                                          *
 *            Exit from a given LPM                       *
 *__________________________________________________________*/



/*__________________________________________________________
 *                                                          *
 *            delay for multiple TA calls                   *
 *__________________________________________________________*/

void delay(unsigned int t){
    unsigned int num_of_halfSec;

    num_of_halfSec = (int) t / 500;
    unsigned int res;
    res = t % 500;
    res = res * 131;

    for (i=0; i < num_of_halfSec; i++){
        startTimerA0(0xFFFF);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }

    if (res > 1000){
        startTimerA0(res);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }
}

/*__________________________________________________________
 *                                                          *
 *            Exit from a given LPM                         *
 *__________________________________________________________*/

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}


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
    if(state == state7) UCA0TXBUF = '7'; //print menu in PC
    else if (x_pending) UCA0TXBUF = '4';
    else UCA0TXBUF = 'F';   // Finish
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt


}

/** _______________________________________________________________________________________________*
 *                                                                                                 *
 *                                      RX ISR                                                     *
 *                                                                                                 *
 *  -----------------------------------------------------------------------------------------------*
 * handles the UART serial  communication module (USCI) Receiving  interrupt                       *
 *                                                                                                 *
 *_________________________________________________________________________________________________*/
//#pragma vector=USART1RX_VECTOR
//__interrupt void USART1_rx (void)
//{
//    while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready?
//    TXBUF1 = RXBUF1;                          // RXBUF1 to TXBUF1
//
//
//    switch(lpm_mode){
//        case mode0:
//            LPM0_EXIT; // must be called from ISR only
//            break;
//        case mode1:
//            LPM1_EXIT; // must be called from ISR only
//            break;
//        case mode2:
//            LPM2_EXIT; // must be called from ISR only
//            break;
//        case mode3:
//            LPM3_EXIT; // must be called from ISR only
//            break;
//        case mode4:
//            LPM4_EXIT; // must be called from ISR only
//            break;
//    }
//}
//

/******************************************************************************
 *
 *                          Buttons ISR
 *
 *____________________________________________________________________________

******************************************************************************/
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
    int k;
    for (k=0;k<100;k++);

//            selector of transition between states


      current_buzz = (current_buzz + 1 )%4;
      PBsArrIntPend &= ~PB0;



//            Exit from a given LPM

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


