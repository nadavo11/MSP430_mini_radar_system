#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
//#include "msp430xG46x.h"
#include  <msp430g2553.h>          // MSP430x2xx
#include "stdio.h"
int fdfd=0;
int t=50;
char temp1[64];
int iii=0;
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
  //  TimerA1_Config();
    lcd_init();
    DCO_config();
    UART_Config();

}

void set_angel(int phi){
    TA1CTL=TACLR;
    TA1CCR0 = MAX_TBR-3;                             // 60 ms Period/2
    TA1CCTL1 |= OUTMOD_6 ;                       // TBCCR1 toggle/set

    TA1CTL = TASSEL_2 | MC_1 ;          // counts to CCR0
    phi = phi*10 +510;
    TA1CCR1=phi;
    TA1CTL |= CCIE;
  //  start_PWM();
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
    TA1CTL=TACLR;
    TA1CCR0 = MAX_TBR-3;                             // 60 ms Period/2
    TA1CCTL2 = CAP | CCIS_0 | CM_3 | SCS;                       // TACCR2 toggle/set

    TA1CTL = TASSEL_2 | MC_1 | ID_2;          // counts to CCR0
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
void print_US(unsigned int LLDR , unsigned int RLDR){
    char L[16],R[16];
    sprintf(L, "%d", LLDR); // diff
    sprintf(R, "%d", RLDR); //ang
    cursor_off;
    lcd_reset();
    lcd_putrow("distance: ");
    if(LLDR>90||LLDR<0)
        lcd_putrow("No object");
    else lcd_putrow(L);
    if(LLDR>RLDR) lcd_putrow(" LS");
    else lcd_putrow(" RS");
    lcd_new_line;
    lcd_putrow("angle: ");
    lcd_putrow(R);
    long_delay();
}

void send_msg(){
    int ss=12345;
    sprintf(message, "%d s", ss);
    start_msg();

}

void flash_config(){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
  value[0] = '\0';                                // initialize value

}
\
void write_SegC (char* value, int seg){
  char *Flash_ptr;                          // Flash pointer
  unsigned int i;

  Flash_ptr = (char *) seg;              // Initialize Flash pointer
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment

  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i=0; i<64; i++)
  {
    *Flash_ptr++ = *value++;                   // Write value to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void copy_C2D (void){
  char *Flash_ptrC;                         // Segment C pointer
  char *Flash_ptrD;                         // Segment D pointer
  unsigned int i;

  Flash_ptrC = (char *) 0x1040;             // Initialize Flash segment C pointer
  Flash_ptrD = (char *) 0x1080;             // Initialize Flash segment D pointer
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  *Flash_ptrD = 0;                          // Dummy write to erase Flash segment D
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i=0; i<64; i++)
  {
    *Flash_ptrD++ = (*Flash_ptrC++ +1);          // copy value segment C to segment D
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void inc_lcd(int x){
    int count=0;
    while(count<x){
        lcd_reset();
        lcd_putrow("INC_LCD");
        lcd_new_line;

        lcd_print_num(count++);
        wait(t);
    }
}

void dec_lcd(int x){
    int count=x;
    while(count>0){
        lcd_reset();
        lcd_putrow("DEC_LCD");
        lcd_new_line;

        lcd_print_num(count--);
        wait(t);
    }
}

void rra_lcd(char x){
    int count=0;
    int b;
    while(count<16){
        lcd_reset();
        for(b = 0; b < count; b++)
            lcd_data(' ');
        lcd_data(x);
        wait(t);
        count++;
    }
    while(count>0){

        lcd_reset();
        lcd_new_line;

        for(b = 0; b < 16; b++)
            lcd_data(' ');
        lcd_data(x);
        wait(t);
        count--;
    }
}

void servo_scan(int la, int ra){

    lcd_reset();
    lcd_putrow("Servo Scan");
    TA1CCTL2 &= ~CCIE;
              TA1CCTL1 &= ~CCIE;
              TA1CCTL0 &= ~CCIE;
              TA0CTL &= ~TAIE;
    int y;
    int ang=la, incr=3;
    set_angel(ang);       // set CCR3
    for(y=0; y<25; y++){
        delay_us(Periode_60ms_val);
    }
    while(ang<ra){
        ang+=incr;
        if(ang>3*(la-ra)/4||ang<(la-ra)/4)
            ang+=incr;

        set_angel(ang);       // set CCR3
        for(iii=0; iii<4; iii++){
            delay_us(Periode_60ms_val);
        }


        trigger_ultrasonic();
        diff=diff*0.069;
        print_US(diff,ang);
        delay_us(Periode_60ms_val);

        TA1CCTL2 &= ~CCIE;
        TA1CCTL1 &= ~CCIE;
        TA1CCTL0 &= ~CCIE;
        TA0CTL &= ~TAIE;

        for(y=0; y<25; y++){
            delay_us(Periode_60ms_val);
        }
        //sendFormatMessage(diff,diff ,diff,diff);


        stop_PWM();

    }

}
//---------------Execute Script--------------------------------

void ExecuteScript(char* Flash_ptrscript)
{
    char OPC[2], Operand1[2], Operand2[2];
    unsigned int Oper2ToInt, X,X2, start, stop, y;
    // read script from flash
//    if (flag_script)
//        Flash_ptrscript = file.file_ptr[file.num_of_files - 1];
//    flag_script = 0;
    //Flash_ptrscript++;
    for (y = 0; y < 64;)
    {

        OPC[0] = *Flash_ptrscript++;
        OPC[1] = *Flash_ptrscript++;

        y +=2;

        switch (OPC[1])
        {
            case '1':
                /**     INC_LCD     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;

                X = hexChar2ToInt(Operand1);
                y +=2;

                //-----------------------------------

                inc_lcd(X);
                break;

            case '2':
                /**     DEC_LCD     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;

                X = hexChar2ToInt(Operand1);
                y +=2;
                //---------------------------
                dec_lcd(X);
                break;
            case '3':
                /**     RRA_LCD     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;

                X = hexChar2ToInt(Operand1);
                y +=2;
                //---------------------------
                //   sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
                rra_lcd(X);
                break;

            case '4':
                /**     SET DELAY     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;


                y +=2;
                //---------------------------
                t = hexChar2ToInt(Operand1);
                break;

            case '5':
                /**     CLEAR LCD     */
                lcd_reset();
                break;

            case '6': //point stepper motor to degree p
                /**     servo DEG     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;
                y +=2;
                X = hexChar2ToInt(Operand1);
                //---------------------------
                servo_scan(X,X+1);
                wait(t);



                break;
            case '7': //scan area between angle l to r
                /**     DEC_LCD     */
                Operand1[0] = *Flash_ptrscript++;
                Operand1[1] = *Flash_ptrscript++;

                Operand2[0] = *Flash_ptrscript++;
                Operand2[1] = *Flash_ptrscript++;

                X  = hexChar2ToInt(Operand1);
                X2 = hexChar2ToInt(Operand2);
                y+=4;
                //---------------------------
                servo_scan(X,X2);

                break;
            case '8': // go sleep
                break;

        }
    }
}

int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1; // Invalid character
}
int hexChar2ToInt(char c[2]){
    int highNibble = hexCharToInt(c[0]);
    int lowNibble = hexCharToInt(c[1]);
    return (highNibble << 4) | lowNibble;
}


void cast_script(){
    int highNibble;
    int lowNibble;
    int len = strlen(script) -1;
    int q, w;
    for (q = 2, w = 0; q < len -1; q += 2, w++) {
        highNibble = hexCharToInt(script[q]);
        lowNibble = hexCharToInt(script[q + 1]);
    }
    temp1[w] = (highNibble << 4) | lowNibble;
}

void script_delete(){
    int q;
    for(q = 0; q < 64; q++ )
        script[q] = '\0';
}


