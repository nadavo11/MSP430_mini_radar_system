#ifndef _bspGPIO_H_
#define _bspGPIO_H_
////UPDATE14;55
#define LEDsArrayPort     		P2OUT
#define LEDsArrayPortDir  		P2DIR
#define LEDsArrayPortSel  		P2SEL

#define PBsArrayPort			P1IN 
#define PBsArrayIntPending		P1IFG 
#define PBsArrayIntEnable		P1IE
#define PBsArrayIntEdgeSel		P1IES
#define PBsArrayPortSel   		P1SEL 
#define PBsArrayPortDir   		P1DIR 



extern void InitGPIO(void);

#endif



