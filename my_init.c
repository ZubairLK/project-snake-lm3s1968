//INIT FUNCTIONS

#include "my_init.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/lm3s1968.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "rit128x96x4.h"

#include "class-d.h"
#include "snake.h"
#include "my_sounds.h"
//Init Clocks
//Init Int
//Init Systick
//Init OLED
//Init Port G


void Clock_Init(void){
	
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	
}

void PortG_Init(void){
	
	//Port Enable
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOG);
	
	//Gating
	SysCtlPeripheralClockGating(true);
	
	//LED
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
	
	// Set pins 3, 4, 5, 6 as input
	GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIODirModeSet(GPIO_PORTG_BASE,GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_DIR_MODE_IN);
	
	//Pull-up
	GPIOPadConfigSet(GPIO_PORTG_BASE,
	GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6|GPIO_PIN_7,
	GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD_WPU);
	
}

void SysTickInt_Init(void) {
	
	SysTickPeriodSet(SysCtlClockGet()/5);
	SysTickIntEnable();
	SysTickEnable();
	
}

void GPIOInt_Init(void){
	
	GPIOIntTypeSet(GPIO_PORTG_BASE,BUTTON_UP|BUTTON_DOWN|BUTTON_LEFT|BUTTON_RIGHT|BUTTON_SELECT,GPIO_FALLING_EDGE);
	GPIOPinIntClear(GPIO_PORTG_BASE,BUTTON_UP|BUTTON_DOWN|BUTTON_LEFT|BUTTON_RIGHT|BUTTON_SELECT);
	GPIOPinIntEnable(GPIO_PORTG_BASE,BUTTON_UP|BUTTON_DOWN|BUTTON_LEFT|BUTTON_RIGHT|BUTTON_SELECT);
	IntPrioritySet(INT_GPIOG, 0x05);
	IntEnable(INT_GPIOG);
	//IntMasterEnable();
	
}

void OLED_Init(void){
	RIT128x96x4Init(1000000);
}

void Snake_Init(void){

	int n;
	int x_init = 50;
	Snake_Tail_Index = 20;
	Current_Snake_Direction = DIRECTION_RIGHT;
	for (n = 0 ; n < Snake_Tail_Index ; n++){

		My_Snake[n].Direction = DIRECTION_RIGHT;
		My_Snake[n].LocationX = x_init--;
		My_Snake[n].LocationY = 50;
	}
	
	//First rand would always show at a wierd place. So twice.
	FruitX = 75;	//Init to avoid clearing corner of maze.
	FruitY = 75;	
	Generate_Fruit();	
	Generate_Fruit();		

}

void Sound_Init(void){

	if(REVISION_IS_A2)
    {
        SysCtlLDOSet(SYSCTL_LDO_2_75V);
    }
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	ClassDInit(SysCtlClockGet());
	while(ClassDBusy())
    {
    }

//	ClassDPlayADPCM(g_pucIntro, sizeof(g_pucIntro));

}
