//SNAKE.C
//MAIN FUNCTIONS FOR SNAKE AND MOVEMENT HERE
#include "stdlib.h"
#include "stdio.h"
#include "my_init.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/lm3s1968.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "rit128x96x4.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "class-d.h"

#include "my_sounds.h"
#include "my_maze.h"
#include "snake.h"

//SYSTICK Interrupt

void SysTick_Handler(void) {	

	if(Game_Mode)
	{
		//Systick Effect while gaming
		LED_Blink();
		Check_Collision();
		Check_Eatable();	
		Update_Snake();		
		Update_Display();
		//Generate_Fruit();
	}
	else
	{
		//Systick to read GPIO while in Menu					
		Menu();		
		ClassDPlayADPCM(bgMusic, lengthofbgMusic);
	}



}

// GPIO Interrupt Handler
void GPIOPortG_Handler(void){
	  
	volatile unsigned long rawStatus = GPIOPinIntStatus(GPIO_PORTG_BASE,false);
 	
	GPIOPinIntClear(GPIO_PORTG_BASE,BUTTON_UP|BUTTON_DOWN|BUTTON_LEFT|BUTTON_RIGHT|BUTTON_SELECT);
 	 	
 	if (rawStatus & BUTTON_UP) {
		if(Current_Snake_Direction != DIRECTION_DOWN)
			Current_Snake_Direction = DIRECTION_UP;
 	}
 	
 	if (rawStatus & BUTTON_DOWN) {
		if(Current_Snake_Direction != DIRECTION_UP)
			Current_Snake_Direction = DIRECTION_DOWN;
 	}
 	
 	if (rawStatus & BUTTON_LEFT) {
		if(Current_Snake_Direction != DIRECTION_RIGHT)
			Current_Snake_Direction = DIRECTION_LEFT;
 	}
 	
 	if (rawStatus & BUTTON_RIGHT) {
		if(Current_Snake_Direction != DIRECTION_LEFT)
			Current_Snake_Direction = DIRECTION_RIGHT;
 	}
 	
	
	if (rawStatus & BUTTON_SELECT) {

		if(Pause){
			SysTickDisable();
			Pause = 0;
		}
		else {
			SysTickEnable();
			Pause = 1;
		}
 	}

}

void Check_Collision(void){

		int Map_Collision_Check , Snake_Collision_Check , n; 
		Snake_Collision_Check = 0;
				
		Map_Collision_Check = Frame_Display[My_Snake[0].LocationY*64 +My_Snake[0].LocationX/2]
			+ Frame_Display[(My_Snake[0].LocationY+1)*64 + My_Snake[0].LocationX/2]
			+ Frame_Display[My_Snake[0].LocationY*64 + (My_Snake[0].LocationX+1)/2]
			+ Frame_Display[(My_Snake[0].LocationY-1)*64 +My_Snake[0].LocationX/2]
			+ Frame_Display[My_Snake[0].LocationY*64 + (My_Snake[0].LocationX-1)/2]
			+ Frame_Display[(My_Snake[0].LocationY+1)*64 + (My_Snake[0].LocationX+1)/2]
			+ Frame_Display[(My_Snake[0].LocationY-1)*64 + (My_Snake[0].LocationX-1)/2];				 
		
		for (n = 1 ; n < Snake_Tail_Index ; n++){
			if( (My_Snake[0].LocationY == My_Snake[n].LocationY) && (My_Snake[0].LocationX == My_Snake[n].LocationX))
				Snake_Collision_Check = 1;
		}

	if(	Map_Collision_Check || Snake_Collision_Check )
		SysTickDisable();
}

void Check_Eatable(void){	
		//Check if snake head ate the fruit		
	if(
		((My_Snake[0].LocationY == FruitY) || (My_Snake[0].LocationY + 1 == FruitY) || (My_Snake[0].LocationY - 1 == FruitY)) && 
		((My_Snake[0].LocationX == FruitX) || (My_Snake[0].LocationX + 1 == FruitX) || (My_Snake[0].LocationX - 1 == FruitX))
		)		
	{						
		Score = Score + Speed;	
		Update_Score();

		Generate_Fruit();

		My_Snake[Snake_Tail_Index].Direction = My_Snake[Snake_Tail_Index-1].Direction;
		My_Snake[Snake_Tail_Index].LocationY = My_Snake[Snake_Tail_Index-1].LocationY;		
		My_Snake[Snake_Tail_Index].LocationX = My_Snake[Snake_Tail_Index-1].LocationX;

		if(My_Snake[Snake_Tail_Index].Direction == DIRECTION_UP)
			My_Snake[Snake_Tail_Index].LocationY = My_Snake[Snake_Tail_Index-1].LocationY + 1;
		if(My_Snake[Snake_Tail_Index].Direction == DIRECTION_DOWN)
			My_Snake[Snake_Tail_Index].LocationY = My_Snake[Snake_Tail_Index-1].LocationY - 1;
		if(My_Snake[Snake_Tail_Index].Direction == DIRECTION_LEFT)
			My_Snake[Snake_Tail_Index].LocationX = My_Snake[Snake_Tail_Index-1].LocationX + 1;
		if(My_Snake[Snake_Tail_Index].Direction == DIRECTION_RIGHT)
			My_Snake[Snake_Tail_Index].LocationX = My_Snake[Snake_Tail_Index-1].LocationX - 1;			
		Snake_Tail_Index = Snake_Tail_Index + 1;
		
		ClassDStop();
		ClassDPlayADPCM(eat_sound, lengthofeat_sound);			
	}
}

void Update_Snake(void){
	
	int n;
	
	//Delete Tail of snake
	const unsigned char AntiBlob[] = {0x00 , 0x00, 0x00, 0x00};
	RIT128x96x4ImageDraw(AntiBlob,My_Snake[Snake_Tail_Index-1].LocationX,My_Snake[Snake_Tail_Index-1].LocationY,2,2);
	
	
	//Loop to update snake structures current position and next directions
	for (n = 0 ; n < Snake_Tail_Index; n++){
		
		if(My_Snake[n].Direction == DIRECTION_UP)
			My_Snake[n].LocationY = My_Snake[n].LocationY - 1;
		if(My_Snake[n].Direction == DIRECTION_DOWN)
			My_Snake[n].LocationY = My_Snake[n].LocationY + 1;
		if(My_Snake[n].Direction == DIRECTION_LEFT)
			My_Snake[n].LocationX = My_Snake[n].LocationX - 1;
		if(My_Snake[n].Direction == DIRECTION_RIGHT)
			My_Snake[n].LocationX = My_Snake[n].LocationX + 1;		

	}
	
	for (n=Snake_Tail_Index ; n > 0; n--){
		
		My_Snake[n].Direction = My_Snake[n-1].Direction;
	}
	
	My_Snake[0].Direction = Current_Snake_Direction;

		  
}

void Generate_Fruit(void){
	
	int randX,randY,check,i;
   	const unsigned char Fruit[] = {0xFF , 0xFF, 0xFF, 0xFF};
	const unsigned char AntiFruit[] = {0x00 , 0x00, 0x00, 0x00};
	check = 1;
do{
	randX = 5 + rand() % ( Xres-10 );
	randY = 3 + rand() % ( Yres-18 );

	if(
		(Frame_Display[(randY * 64) + (randX / 2)] > 0)||	
		(Frame_Display[((randY+1) * 64) + (randX / 2)] > 0)||
		(Frame_Display[(randY * 64) + ((randX+1) / 2)] > 0)||
		(Frame_Display[((randY-1) * 64) + (randX / 2)] > 0)||
		(Frame_Display[((randY+1) * 64) + ((randX+1) / 2)] > 0)||	
		(Frame_Display[((randY-1)* 64) + ((randX-1) / 2)] > 0)
		)
	{
	  for(i = 0 ; i< Snake_Tail_Index ; i++)
	  {
		if(
		(My_Snake[i].LocationX - randX > -2 && My_Snake[i].LocationY - randY > -2) &&
		(My_Snake[i].LocationX - randX < +2 && My_Snake[i].LocationY - randY < +2)		
		)
			{
			 	check = 1;
			}		
	  }
	   
	}
	 else
			check = 0;
}while(check);

	RIT128x96x4ImageDraw(AntiFruit,FruitX,FruitY,2,2); 	
	FruitX = randX;
	FruitY = randY;
	RIT128x96x4ImageDraw(Fruit,FruitX,FruitY,2,2);
}

void Update_Display(void){
	
		//Update the Display to the new head and tail of the snake.
	int n;
	const unsigned char Blob[] = {0xFF , 0xFF, 0xFF, 0xFF};  

	for (n = 0 ; n < Snake_Tail_Index; n++){
		
		RIT128x96x4ImageDraw(Blob,My_Snake[n].LocationX,My_Snake[n].LocationY,2,2);	
	
	}
	
}




//Switch ON/Off LED functions here
void LED_On(void){
	
	GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0xFF);
	
}

void LED_Off(void){
	
	GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00);
	
}

void LED_Blink(void){
	
	LED_On();
	SysCtlDelay(100000);
	LED_Off();	
		
}



void Diplay_Maze_Init(void){

	int x;
	int y;
	int i;

	//CLEAR DISPLAY
	for(y = 0; y < Yres ; y++){
		
		for(x = 0; x < Xres; x++){
				
			Map_Display[y][x] = 0x00;
		}
	}
	
	switch(Maze)
	{

	case 1: 
			//MAKE BORDER		
			for(y = 0; y < Yres ; y++){				
					Map_Display[y][0] = 0xFF;
					Map_Display[y][Xres-1] = 0xFF;
			}
		
			for(x = 0; x < Xres ; x++){				
					Map_Display[0][x] = 0xFF;
					Map_Display[Yres-10][x] = 0xFF;
			}

			//
		    // Loop over the 94 scan lines of the display.
		    //
		    for(y = 0; y < Yres; y++)
		    {
		        //
		        // Loop over the 128 columns of the display.
		        //
		        for(x = 0; x < Xres ; x += 2)
		        	{
		                //
		                // Display these two columns of the maze.
		                //
		                Frame_Display[(y * 64) + (x / 2)] = ((Map_Display[y][x] ? 0xf0 : 0x00) |
															(Map_Display[y][x + 1] ? 0x0f : 0x00));
		            }
		     }


			break;
	
	case 2:
			//Load from header		
			for(i = 0; i < Yres*Xres/2 ; i++){					
					Frame_Display[i] = Maze_2[i];
			}
			break;

	case 3:
			//Load from header		
			for(i = 0; i < Yres*Xres/2 ; i++){					
					Frame_Display[i] = Maze_3[i];
			}
			break;
	default: 
			//MAKE BORDER		
			for(y = 0; y < Yres ; y++){				
					Map_Display[y][0] = 0xFF;
					Map_Display[y][Xres-1] = 0xFF;
			}
		
			for(x = 0; x < Xres ; x++){				
					Map_Display[0][x] = 0xFF;
					Map_Display[Yres-10][x] = 0xFF;
			}

			//
		    // Loop over the 94 scan lines of the display.
		    //
		    for(y = 0; y < Yres; y++)
		    {
		        //
		        // Loop over the 128 columns of the display.
		        //
		        for(x = 0; x < Xres ; x += 2)
		        	{
		                //
		                // Display these two columns of the maze.
		                //
		                Frame_Display[(y * 64) + (x / 2)] = ((Map_Display[y][x] ? 0xf0 : 0x00) |
															(Map_Display[y][x + 1] ? 0x0f : 0x00));
		            }
		     }
			break;
	}
    
   	RIT128x96x4ImageDraw(Frame_Display,0,0,Xres,Yres);

}

void Update_Score(void){
	char buffer[20];
	RIT128x96x4StringDraw("Score :",5,Yres-8,15);	
	sprintf(buffer, "%d",Score);
	RIT128x96x4StringDraw(buffer,50,Yres-8,15);	
}

void Menu(){
	
	char buffer[20];
	
	RIT128x96x4StringDraw("Press Select to Play",5,Yres-18,15);
		
	RIT128x96x4StringDraw("Up/Down for Vol U/D:",5,Yres-38,15);
	sprintf(buffer, "%d",Volume);
	RIT128x96x4StringDraw(buffer,100,Yres-28,15);	

	RIT128x96x4StringDraw("Left to cycle speeds",5,Yres-58,15);
	sprintf(buffer, "%d",Speed);
	RIT128x96x4StringDraw(buffer,100,Yres-48,15);

	RIT128x96x4StringDraw("Right to cycle mazes",5,Yres-78,15);
	sprintf(buffer, "%d",Maze);
	RIT128x96x4StringDraw(buffer,100,Yres-68,15);
	

	if (GPIOPinRead(GPIO_PORTG_BASE,(BUTTON_UP)) == 0) {
		Volume = Volume + 1;		
		if (Volume > 10)
			Volume = 10;

		ClassDVolumeSet(Volume * 20);

	}

 	if (GPIOPinRead(GPIO_PORTG_BASE,(BUTTON_DOWN)) == 0) {
		Volume = Volume - 1;
		 if(Volume < 0)
		 	Volume = 0;
		
		ClassDVolumeSet(Volume * 20);
	}
 	
 	if (GPIOPinRead(GPIO_PORTG_BASE,(BUTTON_LEFT)) == 0) {
		Speed = Speed + 1;
		if (Speed > 5)
			Speed = 1;					  	
	}
 	
 	if (GPIOPinRead(GPIO_PORTG_BASE,(BUTTON_RIGHT)) == 0) {
		Maze = Maze + 1;
		if(Maze > 3)
			Maze = 1;		
	}
 	
	
	if (GPIOPinRead(GPIO_PORTG_BASE,(BUTTON_SELECT)) == 0) {
		SysTickIntDisable();
		SysTickDisable();
		Game_Mode = 1;
 	}

	if(Game_Mode == 1)
	{	
		Diplay_Maze_Init();
		Snake_Init();		
		GPIOInt_Init();		
		SysTickPeriodSet(SysCtlClockGet()/(Speed*20));	
		SysTickEnable();
		SysTickIntEnable();
		
	}
}
