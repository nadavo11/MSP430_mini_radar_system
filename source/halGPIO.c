#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "stdint.h"
#include "string.h"
//#include "stdlib.h"


// Global Variables
int j=0;
char *ptr1, *ptr2, *ptr3;
short MSBIFG = 0;
short stateIFG = 1; // 0-state changed -> send state(pb pressed)
int rotateIFG = 1;
unsigned int delay_time = 500;
const unsigned int timer_half_sec = 65535;
unsigned int i = 0;
unsigned int tx_index;
char counter_str[4];
short Vr[] = {0, 0}; //Vr[0]=Vry , Vr[1]=Vrx
const short state_changed[] = {1000, 1000}; // send if button pressed - state changed
char stringFromPC[80];
char file_content[80];
int ExecuteFlag = 0;
int FlashBurnIFG = 0;
int SendFlag = 0;
int startRotateLEDs = 0x10;
int* rotateLEDs = &startRotateLEDs;
int counter = 514;
char step_str[4];
char finish_str[3] = "FIN";
int curr_counter = 0;
short finishIFG = 0;
//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 

	ADCconfig();
	StopAllTimers();
	UART_init();
    GPIOconfig();
}
//--------------------------------------------------------------------
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


//--------------------------------------------------------------------
// 				Set Byte to Port
//--------------------------------------------------------------------
void print2RGB(char ch){
    RGBArrPortOut = ch;
} 

//--------------------------------------------------------------------
//              Send FINISH to PC
//--------------------------------------------------------------------
void send_finish_to_PC(){
    finishIFG = 1;
    tx_index = 0;
    UCA0TXBUF = finish_str[tx_index++];
    IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
    __bis_SR_register(LPM0_bits + GIE); // Sleep
    START_TIMERA0(10000);
    finishIFG = 0;
}

//--------------------------------------------------------------------
//              Send degree to PC
//--------------------------------------------------------------------
void send_degree_to_PC(){
    tx_index = 0;
    UCA0TXBUF = step_str[tx_index++];
    IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
    __bis_SR_register(LPM0_bits + GIE); // Sleep
    START_TIMERA0(10000);
}

//---------------------------------------------------------------------
//            General Function
//---------------------------------------------------------------------
//-----------------------------------------------------------------------
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    if (tmp == 0){
        str[strSize] = '0';
        return;
    }
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

//-----------------------------------------------------------------------
uint32_t hex2int(char *hex) {
    uint32_t val = 0;
    int o;
    for(o=0; o<2; o++) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
//-----------------------------------------------------------------------
void motorGoToPosition(uint32_t stepper_degrees, char script_state){

    int clicks_cnt;
    uint32_t step_counts;
    uint32_t calc_temp;
    calc_temp = stepper_degrees * counter;
    step_counts = (calc_temp / 360); // how much clicks to wanted degree

    //RK code
    int diff = step_counts - curr_counter;
    if(0 <= diff){ //move CW
        for (clicks_cnt = 0; clicks_cnt < diff; clicks_cnt++){
            curr_counter++;
            Stepper_clockwise(150);
            START_TIMERA0(10000);
            //send data only if FINISH or stepper_deg (state 6)
            if(script_state == '6'){
                int2str(step_str, curr_counter);
                send_degree_to_PC(); }
        }
        if (script_state == '7') {
            int2str(step_str, curr_counter);
           // sprintf(step_str, "%d", curr_counter);
            send_degree_to_PC(); }
        sprintf(step_str, "%s", "FFFF"); // add finish flag
        send_degree_to_PC();
    }
    else{ // move CCW
        for (clicks_cnt = diff; clicks_cnt < 0; clicks_cnt++){
            curr_counter--;
            Stepper_counter_clockwise(150);
            START_TIMERA0(10000);
            //send data only if FINISH or stepper_deg (state 6)
            if(script_state == '6'){
                int2str(step_str, curr_counter);
          //      sprintf(step_str, "%d", curr_counter);
                send_degree_to_PC(); }
        }
        if (script_state == '7') {
            int2str(step_str, curr_counter);
        //    sprintf(step_str, "%d", curr_counter);
            send_degree_to_PC(); }
        sprintf(step_str, "%s", "FFFF"); // add finish flag
        send_degree_to_PC();
    }
}
//--------------------------------------------------------------------
//              Print Byte to 8-bit LEDs array
//--------------------------------------------------------------------
void print2LEDs(unsigned char ch){
    LEDsArrPortOut = ch;
}

//----------------------Count Timer Calls---------------------------------
void timer_call_counter(){

    unsigned int num_of_halfSec;

    num_of_halfSec = (int) delay_time / half_sec;
    unsigned int res;
    res = delay_time % half_sec;
    res = res * clk_tmp;

    for (i=0; i < num_of_halfSec; i++){
        TIMER_A0_config(timer_half_sec);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }

    if (res > 1000){
        TIMER_A0_config(res);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
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
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            Start Timer With counter
//---------------------------------------------------------------------
void START_TIMERA0(unsigned int counter){
    TIMER_A0_config(counter);
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}
//---------------------------------------------------------------------
//            Start Timer1 With counter
//---------------------------------------------------------------------
void START_TIMERA1(unsigned int counter){
    TIMER_A1_config(counter);
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}
// ------------------------------------------------------------------
//                     Polling delays
//---------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}
//---------------**************************----------------------------
//               Interrupt Services Routines
//---------------**************************----------------------------
//*********************************************************************
//                        TIMER A0 ISR
//*********************************************************************
#pragma vector = TIMER0_A0_VECTOR // For delay
__interrupt void TimerA_ISR (void)
{
    StopAllTimers();
    LPM0_EXIT;
}

//*********************************************************************
//                        TIMER A ISR
//*********************************************************************
#pragma vector = TIMER1_A0_VECTOR // For delay
__interrupt void Timer1_A0_ISR (void)
{
    if(!TAIFG) { StopAllTimers();
    LPM0_EXIT;
    }
}

//*********************************************************************
//                         ADC10 ISR
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
   LPM0_EXIT;
}

//-------------------ATAN2- Fixed point - returns degrees---------------------------
int16_t atan2_fp(int16_t y_fp, int16_t x_fp)
{
    int32_t coeff_1 = 45;
    int32_t coeff_1b = -56; // 56.24;
    int32_t coeff_1c = 11;  // 11.25
    int16_t coeff_2 = 135;

    int16_t angle = 0;

    int32_t r;
    int32_t r3;

    int16_t y_abs_fp = y_fp;
    if (y_abs_fp < 0)
        y_abs_fp = -y_abs_fp;

    if (y_fp == 0)
    {
        if (x_fp >= 0)
        {
            angle = 0;
        }
        else
        {
            angle = 180;
        }
    }
    else if (x_fp >= 0)
    {
        r = (((int32_t)(x_fp - y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) /
((int32_t)(x_fp + y_abs_fp));

        r3 = r * r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = (int16_t) (     coeff_1 + ((coeff_1b * r + r3) >>
MULTIPLY_FP_RESOLUTION_BITS)   );
    }
    else
    {
        r = (((int32_t)(x_fp + y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) /
((int32_t)(y_abs_fp - x_fp));
        r3 = r * r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = coeff_2 + ((int16_t)    (((coeff_1b * r + r3) >>
MULTIPLY_FP_RESOLUTION_BITS))   );
    }

    if (y_fp < 0)
        return (360-angle);     // negate if in quad III or IV
    else
        return (angle);
}


//***********************************TX ISR******************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(state == state3 && finishIFG == 1){  // For script
        UCA0TXBUF = finish_str[tx_index++];                 // TX next character

        if (tx_index == sizeof step_str - 1) {   // TX over?
            tx_index=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;
            LPM0_EXIT;
        }
    }

    if (state == state3 && finishIFG == 0){  // For script
        UCA0TXBUF = step_str[tx_index++];                 // TX next character

        if (tx_index == sizeof step_str - 1) {   // TX over?
            tx_index=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;
            LPM0_EXIT;
        }
    }
    else if (state==state2 && stateStepp==stateStopRotate){
        UCA0TXBUF = counter_str[tx_index++];                 // TX next character

        if (tx_index == sizeof counter_str - 1) {   // TX over?
            tx_index=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;
            LPM0_EXIT;
        }
    }
    else if (stateIFG && state == state1){  // Send Push Button state
        if(MSBIFG) UCA0TXBUF = (state_changed[i++]>>8) & 0xFF;
        else UCA0TXBUF = state_changed[i] & 0xFF;
        MSBIFG ^= 1;

        if (i == 2) {  // TX over?
            i=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            START_TIMERA0(10000);
            stateIFG = 0;
            LPM0_EXIT;
        }
    }
    else if(!stateIFG && state == state1){ //send data for painter!!
        if(MSBIFG) UCA0TXBUF = (Vr[i++]>>8) & 0xFF;
        else UCA0TXBUF = Vr[i] & 0xFF;
        MSBIFG ^= 1;

        if (i == 2) {  // TX over?
            i=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            START_TIMERA0(10000);
            LPM0_EXIT;
        }
    } else{
        strcpy(stringFromPC, "Wallak\n");
        UCA0TXBUF = stringFromPC[tx_index++];                 // TX next character

        if (tx_index == sizeof stringFromPC - 1) {   // TX over?
            tx_index=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            LPM0_EXIT;
        }

    }
}
//***********************************RX ISR******************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    stringFromPC[j] = UCA0RXBUF;  // Get Whole string from PC
    j++;
    // This if to get the file data. Added 'Z' to the end of the data in the python file, acts like ack
    if (stringFromPC[j-1] == 'Z'){
        j = 0;
        SendFlag = 0;
        strcpy(file_content, stringFromPC);
     //   ExecuteFlag = 1;
    }
    // This if to get the file name
    if (!SendFlag && stringFromPC[j-1] == '\x0a'){
        for (i=0; i < j; i++){
            file.file_name[i] = stringFromPC[i];
        }
        SendFlag = 1;
        j = 0;
    }
    if (stringFromPC[j-1] == 'W'){ //pointer for 1st selected file
        FlashBurnIFG = 1;
        ptr1 = (char*) 0x1000;
        file.file_ptr[0]=ptr1;
        file.num_of_files = 1;
        j = 0;
    }
    if (stringFromPC[j-1] == 'X'){ //pointer for 2nd selected file
        FlashBurnIFG = 1;
        ptr2 = (char*) 0x1040;
        file.file_ptr[1]=ptr2;
        file.num_of_files = 2;
        j = 0;
    }
    if (stringFromPC[j-1] == 'Y'){ //pointer for 3rd selected file
        FlashBurnIFG = 1;
        ptr3 = (char*) 0x1080;
        file.file_ptr[2]=ptr3;
        file.num_of_files = 3;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
    }

    if (stringFromPC[j-1] == 'T'){ //index of executed list
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 1;
    }
    if (stringFromPC[j-1] == 'U'){
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 2;
    }
    if (stringFromPC[j-1] == 'V'){
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 3;
    }


    // If's for states
    if (stringFromPC[0] == 'm') {state = state0; stateStepp=stateDefault; rotateIFG = 0; j = 0;}
    else if (stringFromPC[0] == 'P') { state = state1; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  //Was p
    else if (stringFromPC[0] == 'C') { state = state2; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  //Was c
    else if (stringFromPC[0] == 's') { state = state3; stateStepp=stateDefault; rotateIFG = 0; j = 0;}

    else if (stringFromPC[0] == 'A'){ stateStepp = stateAutoRotate; rotateIFG = 1; j = 0;}// Auto Rotate
    else if (stringFromPC[0] == 'M'){ stateStepp = stateStopRotate; rotateIFG = 0; j = 0;}// Stop Rotate
    else if (stringFromPC[0] == 'J'){ stateStepp = stateJSRotate; j = 0;}// JoyStick Rotatefixed pmsp430


    LPM0_EXIT;
}

//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void Joystick_handler(void){
      IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
      delay(debounceVal);

      if(JoyStickIntPend & BIT5){ //int at P1.5
          stateIFG = 1; // send state!
          JoyStickIntPend &= ~BIT5;
      }
      IE2 |= UCA0TXIE;                       // enable USCI_A0 TX interrupt
}
