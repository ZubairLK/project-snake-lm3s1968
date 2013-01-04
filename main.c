
#include <lm3s1968.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/lm3s1968.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"

#include "rit128x96x4.h"
#include "my_init.h"
#include "snake.h"


// Global Variables
unsigned char Map_Display[Yres][Xres]; //2D map of the display
unsigned char Frame_Display[Xres*Yres/2];//Linear array input required by Display Lib
struct Snake My_Snake[3072]; //Upper limit of snake length for a very pro player
int Current_Snake_Direction; //Current direction of the head of snake
int Snake_Tail_Index;//Tail index for location of tail in My_Snake
int FruitX;//X coordinate of Current fruit location
int FruitY;//Y coordinate of Current fruit location
int Pause;//Game in Pause/Unpause mode.
int Score = 0;//Score counter
int Speed = 1;//Speed of snake. Related to divider of Systick
int Volume = 5;//Volume of the sound
int Maze = 1;//Maze selection. 1,2,3 possible.
int Game_Mode = 0;//Game/Menu mode selector.

int main(void) {
	
	Clock_Init();
	PortG_Init();
	Sound_Init();	
	OLED_Init();	
	LED_Blink();
	SysTickInt_Init();	
	IntMasterEnable();

	

	while(1) { 
		// SLEEP 
		}
	
}
