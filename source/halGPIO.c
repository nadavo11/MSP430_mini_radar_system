#include  "../header/halGPIO.h"
unsigned int EndOfRecord = 0;

int temp[2],i=0,diff;

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

//
//#pragma vector=PORT1_VECTOR  // For Push Buttons
//  __interrupt void PBs_handler(void){
//      DelayUs(debounceVal);
//
//	if(PBsArrIntPend & PB0){
//	    lcd_clear();
//	    state = state1;
//	    PBsArrIntPend &= ~PB0;
//	}
//    else if(PBsArrIntPend & PB1){
//        lcd_clear();
//        state = state2;
//        lcd_home();
//	    PBsArrIntPend &= ~PB1;
//    }
//    else if(PBsArrIntPend & PB2){
//        lcd_clear();
//        state = state3;
//	    PBsArrIntPend &= ~PB2;
//
//    }
//
//    else if(PBsArrIntPend & PB3){
//
//           state = state4;
//           PBsArrIntPend &= ~PB3;
//       }
//    else PBsArrIntPend &= 0x0F;
//
//        switch(lpm_mode){
//		case mode0:
//		 LPM0_EXIT;
//		 break;
//
//		case mode1:
//		 LPM1_EXIT;
//		 break;
//
//		case mode2:
//		 LPM2_EXIT;
//		 break;
//
//        case mode3:
//		 LPM3_EXIT;
//		 break;
//
//        case mode4:
//		 LPM4_EXIT;
//		 break;
//	}
//
//}
//

  #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = TIMER0_B1_VECTOR
  __interrupt void Timer_B (void)
  #elif defined(__GNUC__)
  void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) Timer_B (void)
  #else
  #error Compiler not supported!
  #endif
  {
    switch(TBIV){
        case TBIV_TBCCR1:
            TBIV &= ~TBIV_TBCCR1;
            LPM0_EXIT;

            break;
        /****** capture ISR *****
         *
         *      TB2 acts in capture
         * *********************/
        case TBIV_TBCCR2:           //CAPTURE ISR
            TBIV &= ~TBIV_TBCCR2;
            if(i==0) {
                // clear TBR to start counting from zero
                //TBR = 0
                //TBCTL |= TBCLR;
                //rising edge
                //revcord count value
            }
                temp[i] = TBCCR2;
                i += 1;
                TBCCTL2 &= ~CCIFG;
            
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

        case TBIV_TBCCR3:
            TBIV &= ~TBIV_TBCCR3;
            break;

        case TBIV_TBCCR4:           // delay ISR
            TBIV &= ~TBIV_TBCCR4;
            LPM0_EXIT;
            break;

        case TBIV_TBCCR5:
            TBIV &= ~TBIV_TBCCR5;
            break;

        case TBIV_TBCCR6:
            TBIV &= ~TBIV_TBCCR6;
            break;
        }
  }


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

}


//    switch(lpm_mode){
//        case mode0:
//            LPM0_EXIT; // must be called from ISR only
//            break;
//
//        case mode1:
//            LPM1_EXIT; // must be called from ISR only
//            break;
//
//        case mode2:
//            LPM2_EXIT; // must be called from ISR only
//            break;
//
//        case mode3:
//            LPM3_EXIT; // must be called from ISR only
//            break;
//
//        case mode4:
//            LPM4_EXIT; // must be called from ISR only
//            break;
//        }

          //TBCTL &= ~TBIFG; // Clear the interrupt flag
          //TBCTL = TBCLR;




//  #pragma vector = DMA_VECTOR
//  __interrupt void DMA_ISR (void){
//      LPM0_EXIT;
//      _BIC_SR(DMAIV);
//  }
