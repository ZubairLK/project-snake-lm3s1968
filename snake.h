
#ifndef __SNAKE_H__
#define __SNAKE_H__

//Defines
//Direction
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4
//Buttons
#define BUTTON_UP GPIO_PIN_3
#define BUTTON_DOWN GPIO_PIN_4
#define BUTTON_LEFT GPIO_PIN_5
#define BUTTON_RIGHT GPIO_PIN_6
#define BUTTON_SELECT GPIO_PIN_7

//Display		  
#define Xres	128
#define Yres	96
// Global Variables
extern unsigned char Map_Display[Yres][Xres];
extern unsigned char Frame_Display[Xres*Yres/2];
extern struct Snake My_Snake[3072]; //Upper limit of snake length for a very pro player :p
extern int Current_Snake_Direction;
extern int Snake_Head_Index;
extern int Snake_Tail_Index;
extern int FruitX;
extern int FruitY;
extern int Pause;
extern int Score;
extern int Speed;
extern int Maze;
extern int Volume;
extern int Game_Mode;


struct Snake
{
	int Direction;
	int LocationX;
	int LocationY;
};



// Function Prototypes
void GPIOPortG_Handler(void);
void Check_Collision(void);
void Check_Eatable(void);
void Update_Snake(void);
void Generate_Fruit(void);
void Update_Display(void);
void LED_On(void);
void LED_Off(void);
void LED_Blink(void);
void SysTick_Handler(void);
void Diplay_Maze_Init(void);
void Update_Score(void);
void Menu(void);

#endif // __SNAKE_H__
