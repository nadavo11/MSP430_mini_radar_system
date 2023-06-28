#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "msp430xG46x.h"
////UPDATE14;55
#include "stdio.h"

int tones[7] = {1000,1250,1500,17500,2000,2250,2500};
int count = 0;
int tone = 0;
int c;
/*______________________________________
 *                                      *
 *          LCD API                     *
 *______________________________________*/


void lcd_reset(){
    lcd_clear();
    lcd_home();
}

/*__________________________________________________________
 *                                                          *
 *          STATE 1 : SCAN OBJECTS                          *
 *__________________________________________________________*
 * move servo motor and scan objects
 * */

int objects[180];
int distance;
void  servo_angle(int angle){
    //set servo angle
    int PWM_period = (int)((angle/180)*0xFFF0);
    servo_PWM(PWM_period);
}

void scan_objects(int t){
    int angle = 170;
    while (state == state1){
        // increment servo motor angle
        angle = (angle + 1)%180;
        servo_angle(angle);

        //scan objects
        ultrasonic_config();
        int distance = ultrasonic_measure();
        lcd_print_num(distance);
        break;
    }
    //stopTimerA0();
}



/*__________________________________________________________
 *                                                          *
 *          STATE 2 : SCAN lights                           *
 *__________________________________________________________*
 * move servo motor and scan objects
 * */
int LDR_measure(){


    //make ADC conversion
    ADC_config();
    ADC_start();

    enable_interrupts();
    enterLPM(mode0);
    disable_interrupts();
    ADC10CTL0 &= ~ADC10ON;                    // Disable ADC10 interrupt

    return ADC10MEM;

}

void scan_lights(int t){
    int angle = 170;
    while (state == state2){
        // increment servo motor angle
        angle = (angle + 1)%180;
        servo_angle(angle);

        //scan lights
        ultrasonic_config();
        int distance = LDR_measure();

        //Display results
        lcd_reset();
        lcd_puts("LDR[mv]:");
        lcd_print_voltage(distance * 3.4);       // Display results
        break;
    }
    //stopTimerA0();
}
