#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "msp430xG46x.h"
#include "stdio.h"




void sysConfig(){
    GPIOconfig();
    TimerB_Config();
    lcd_init();



}

void set_angel(int phi){
    TACCR2=phi;
}

/*______________________________________
 *                                      *
 *          LCD API                     *
 *______________________________________*/

//
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

void LDR_measurement(int t){
        ADC_config();
        ADC_start();
        enable_interrupts();
        enterLPM(mode0);
        ADC_stop();
        long_delay();
        lcd_reset();
}

void trigger_ultrasonic(){
    delay_us(del60ms);
    delay2();
    P5OUT ^= 0x02;
    delay2();
    P5OUT ^= 0x02;
    long_delay();
}

void print_measurments(int LLDR , int RLDR){
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


//int LDR_measure(){
//
//
//    //make ADC conversion
//    ADC_config();
//    ADC_start();
//
//    enable_interrupts();
//    enterLPM(mode0);
//    disable_interrupts();
//    ADC12CTL0 &= ~ADC12ON;                    // Disable ADC10 interrupt
//
//    return ADC12MEM;
//
//}

//void scan_lights(int t){
//    int angle = 170;
//    while (state == state2){
//        // increment servo motor angle
//        angle = (angle + 1)%180;
//        servo_angle(angle);
//
//        //scan lights
//        ultrasonic_config();
//        int distance = LDR_measure();
//
//        //Display results
//        lcd_reset();
//        lcd_puts("LDR[mv]:");
//        lcd_print_voltage(distance * 3.4);       // Display results
//        break;
//    }
//    //stopTimerA0();
//}
