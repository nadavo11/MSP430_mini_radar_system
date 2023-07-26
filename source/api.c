
#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "math.h"

int flag_script = 1;
int16_t Vrx = 0;
int16_t Vry = 0;

//-------------------------------------------------------------
//                StepperUsingJoyStick
//-------------------------------------------------------------
void JoyStickADC_Painter(){
    JoyStickIntEN &= ~BIT5; // allow interrupt only in the end of cycle
    i = 0;
    if(!stateIFG) { //send data
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
        ADC10SA = &Vr;                        // Data buffer start
        ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, ADC10_ISR will force exit

        UCA0TXBUF = Vr[i] & 0xFF;
        MSBIFG = 1;
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, will exit when finish tx

    }

    else if (stateIFG) { //send state
        UCA0TXBUF = state_changed[i] & 0xFF;
        MSBIFG = 1;
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, will exit when finish tx


        START_TIMERA0(5000); // wait for PC to get and sync after all the debounce and interrupt delay
        JoyStickIntPend &= ~BIT5;

    }

    JoyStickIntEN |= BIT5; // allow interrupt only in the end of cycle
}
//-------------------------------------------------------------
//                StepperUsingJoyStick
//-------------------------------------------------------------
void JoyStickADC_Steppermotor(){
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
        ADC10SA = &Vr;                        // Data buffer start
        ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, ADC10_ISR will force exit

}
//-----------For Flash-------------------------------------
void ScriptFunc() {

    if(FlashBurnIFG){
        FlashBurnIFG=0;
        FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
   //     ScriptData();
        file.file_size[file.num_of_files - 1] = strlen(file_content) - 1;
        write_Seg();
        send_finish_to_PC(); // send ACK to PC
        IE2 |= UCA0RXIE;
    }
    if(ExecuteFlag){
        ExecuteFlag=0;
        flag_script = 1;
        delay_time = 500;  // delay default time
        ExecuteScript();
        send_finish_to_PC(); // finish script
    }
    __bis_SR_register(LPM0_bits + GIE);
}

//---------------Execute Script--------------------------------
void ExecuteScript(void)
{
    char *Flash_ptrscript;                         // Segment pointer
    char OPCstr[10], Operand1Flash[20], Operand2Flash[20];
    unsigned int Oper2ToInt, X, start, stop, y;
    if (flag_script)
        Flash_ptrscript = file.file_ptr[file.num_of_files - 1];
    flag_script = 0;
  //  IE2 != ~UCA0RXIE; // Now added by mg
   // for (y = 0; y <= file.file_size[file.num_of_files - 1];)
    for (y = 0; y < 64;)
    {
        OPCstr[0] = *Flash_ptrscript++;
        OPCstr[1] = *Flash_ptrscript++;
        y = y + 2;
        switch (OPCstr[1])
        {
        case '1':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
//            sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt > 0)
            {
                blinkRGB();    //blink RGB LED
                Oper2ToInt--;
            }
            break;

        case '2':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
      //      sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt)
            {
                rlc_leds(rotateLEDs);           //rotate left
                Oper2ToInt--;
            }
            break;
        case '3':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
         //   sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt)
            {
                rrc_leds(rotateLEDs);         //rotate right
                Oper2ToInt--;
            }
            break;
        case '4':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
       //     sscanf(Oper1fFromFlash, "%2x", &delay_time); //set delay
            delay_time = hex2int(Operand1Flash);
            delay_time = delay_time * 10 ; //its in unit of 10ms
            break;
        case '5':
            ClearLEDsRGB();
            break;
        case '6': //point stepper motor to degree p
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
        //    sscanf(Oper1fFromFlash, "%2x", &X);
            X = hex2int(Operand1Flash);
            motorGoToPosition(X, OPCstr[1]);

            break;
        case '7': //scan area between angle l to r
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Operand2Flash[0] = *Flash_ptrscript++;
            Operand2Flash[1] = *Flash_ptrscript++;
            y = y + 2;
        //    sscanf(Oper1fFromFlash, "%2x", &X);
            X = hex2int(Operand1Flash);
            start = X;
            motorGoToPosition(X, OPCstr[1]);
       //     sscanf(Oper2fFromFlash, "%2x", &X);
            X = hex2int(Operand2Flash);
            stop = X;
            motorGoToPosition(X, OPCstr[1]);

            break;
        case '8': // go sleep
//            state = state0;
            break;

        }
    }
}
//-------------------------------------------------------------
//*************************************************************
//****************Script Functions*****************************
//-------------------------------------------------------------
//                1. Blink RGB
//-------------------------------------------------------------
void blinkRGB(){
    int colors[] = {0b000, 0b001, 0b01000000, 0b01000001, 0b10000000, 0b10000001, 0b11000000, 0b11000001}; // According the P1 pins
    unsigned int i = 0;
    for(i=0; i<8;){
        print2RGB(colors[i]);
        timer_call_counter();
        i = (i + 1) % 9;
    }
}
//-------------------------------------------------------------
//                2. Rotate left LEDS
//-------------------------------------------------------------
void rlc_leds(int* rotateLEDs){
        unsigned int i;
    LEDsArrPortOut = 0x00;
    for(i=0; i<4; i++){
        print2LEDs(*rotateLEDs);
        *rotateLEDs = (*rotateLEDs*2)%255;
        if (*rotateLEDs == 1)
            *rotateLEDs = 0x10;
     //   START_TIMERA0(65000);  // Timer stopped working
        timer_call_counter();
    }
}

//-------------------------------------------------------------
//                3. Rotate right LEDS
//-------------------------------------------------------------
void rrc_leds(int* rotateLEDs){
        unsigned int i;
    LEDsArrPortOut = 0x00;
    *rotateLEDs = 128;
    for(i=0; i<4; i++){
        print2LEDs(*rotateLEDs);
        *rotateLEDs = (*rotateLEDs/2)%255;
        if (*rotateLEDs == 1)
            *rotateLEDs = 0x10;
     //   START_TIMERA0(40000);
        timer_call_counter();
    }
}

//-------------------------------------------------------------
//                5. Clear all LEDS+RGB
//-------------------------------------------------------------
void ClearLEDsRGB(){
    LEDsArrPortOut &= ~0xF0;
    RGBArrPortOut &= ~(BIT7 + BIT6 + BIT0);
}

//*************************************************************
//*************************************************************
//-------------------------------------------------------------
//                Stepper clockwise
//-------------------------------------------------------------
void Stepper_clockwise(long speed_Hz){
    int speed_clk;
    // 1 step clockwise of stepper - 50Hz
//    speed_clk = 131072/speed_Hz;
    speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
    StepmotorPortOUT = 0x01; // out = 0001
    START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
    StepmotorPortOUT = 0x08; // out = 1000
    START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
    StepmotorPortOUT = 0x04; // out = 0100
    START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
    StepmotorPortOUT = 0x02; // out = 0010
}

//-------------------------------------------------------------
//                Stepper counter-clockwise
//-------------------------------------------------------------
void Stepper_counter_clockwise(long speed_Hz){
    int speed_clk;
    // 1 step counter-clockwise of stepper
//    speed_clk = 131072/speed_Hz;
    speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
    StepmotorPortOUT = 0x08; // out = 0001
    START_TIMERA0(speed_clk); // (2^20/8)*(1/20[Hz]) = 6553
    StepmotorPortOUT = 0x01; // out = 1000
    START_TIMERA0(speed_clk); // (2^20/8)*(1/20[Hz]) = 6553
    StepmotorPortOUT = 0x02; // out = 0100
    START_TIMERA0(speed_clk); // (2^20/8)*(1/20[Hz]) = 6553
    StepmotorPortOUT = 0x04; // out = 0010
}


//-------------------------------------------------------------
//                Stepper Motor Calibration
//-------------------------------------------------------------
void calibrate(){
    int2str(counter_str, counter);
  //  sprintf(counter_str, "%d", counter);
    tx_index = 0;
    UCA0TXBUF = counter_str[tx_index++];
    IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
    __bis_SR_register(LPM0_bits + GIE); // Sleep
    curr_counter = 0;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
//-------------------------------------------------------------
//                Stepper Using JoyStick
//-------------------------------------------------------------
void StepperUsingJoyStick(){


    uint32_t counter_phi;
    uint32_t phi;
    uint32_t temp;
//    curr_counter = 0;
    while (counter != 0 && state==state0 && stateStepp==stateJSRotate){
        JoyStickADC_Steppermotor();
        if (!( Vr[1] > 400 && Vr[1] < 600 && Vr[0] > 400 && Vr[0] < 600)){
            Vrx = Vr[1] - 512;
            Vry = Vr[0] - 512;

            phi = atan2_fp(Vry, Vrx);
            temp = phi * counter;

            if (270 < phi) {
//            if (0 < Vrx && Vry < 0) {
                counter_phi = ((counter * 7) / 4) - (temp / 360);  // ((630-phi)/360)*counter;
                }
            else {
                counter_phi = ((counter * 3) / 4) - (temp / 360);  // ((270-phi)/360)*counter;
                }
            if ((int)(curr_counter - counter_phi) < 0) {
                Stepper_clockwise(600);
                curr_counter++;
            }
            else {
                Stepper_counter_clockwise(600);
                curr_counter--;
            }
        }
    }

}

