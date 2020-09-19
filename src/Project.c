/*----------------------------------------------------------------------------
 * Name:    Project.c
 * Purpose: Media Center
 * Note(s): 
 *----------------------------------------------------------------------------
 * Name: Rami Saad
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LPC17xx.H"                       
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "KBD.h"

#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbaudio.h"

#define __FI        1                           /* Font index 16x24 */
#define __USE_LCD   0			                /* Uncomment to use the LCD */

/* Joystick Positions */
#define UP          113
#define DOWN        89
#define LEFT        57
#define RIGHT       105
#define MIDDLE      121 
#define IN          120

/* Screens */
#define PH_GAL     11
#define AUD        12
#define SHUMPS     13
#define MENU			 14

/* Photos*/
#define MARIO_P      15
#define DS_P         16
#define DOTA_P       17

/*ITM Stimulus Port definitions for printf */
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

/* BitBand Mode */
#define BIT_ADDR1				(*((volatile unsigned long *)(0x233806f0)))
#define BIT_ADDR2				(*((volatile unsigned long *)(0x23380A88)))

/* Function mode */	
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))
volatile unsigned long * bit;

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

/* Audio Control */
extern  void SystemClockUpdate(void);
extern uint32_t SystemFrequency;  
uint8_t  Mute;                                  /* Mute State */
uint32_t Volume;                                /* Volume Level */

#if USB_DMA
uint32_t *InfoBuf = (uint32_t *)(DMA_BUF_ADR);
short *DataBuf = (short *)(DMA_BUF_ADR + 4*P_C);
#else
uint32_t InfoBuf[P_C];
short DataBuf[B_S];                             /* Data Buffer */
#endif

uint16_t  DataOut;                              /* Data Out Index */
uint16_t  DataIn;                               /* Data In Index */

uint8_t   DataRun;                              /* Data Stream Run State */
uint16_t  PotVal;                               /* Potenciometer Value */
uint32_t  VUM;                                  /* VU Meter */
uint32_t  Tick;                                 /* Time Tick */

/* Operation Flags */
uint16_t app; 
uint16_t sound_off = 0;
uint16_t resume = 0;

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
    if (DEMCR & TRCENA){
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
return(ch);
}

/******************************************************************************/

/* Import external variables from IRQ.c file */
extern uint8_t  clock_ms;

/* Photo Gallery Images */
extern unsigned char DOTA_2[]; 				          /* 225 x 225 */ 
extern unsigned char DS[];                      /* 225 x 225 */
extern unsigned char MARIO[];                   /* 240 x 240 */

/* Video Game Assets */ 
extern unsigned char SHIP[];                    /* 31 x 21 */
extern unsigned char BOULDER[];                 /* 31 x 31 */
extern unsigned char BOULDER_REMOVE[];          /* 31 x 31 */
extern unsigned char BULLET[];                  /* 18 x 6 */
extern unsigned char BULLET_REMOVE[];           /* 18 x 6 */

/* Video Game structs */ 
typedef struct{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
}Ship;

typedef struct{
	uint16_t x;
	uint16_t y;
	uint16_t st;
	uint16_t width;
	uint16_t height;
	uint16_t des;
}Bullet;

typedef struct{
	uint16_t x;
	uint16_t y;
	uint16_t st;
	uint16_t width;
	uint16_t height;
	uint16_t des;
}Boulder;
	
uint16_t new_game = 0;
uint16_t timer;

Bullet  bullets[10];
Boulder boulders[10];

/******************************************************************************/
/*************************** Audio Streaming Functions ************************/

/*get_potval: Get Potentiometer Value*/
void get_potval (void) {
  uint32_t val;

  LPC_ADC->CR |= 0x01000000;              	/* Start A/D Conversion */
  do {
    val = LPC_ADC->GDR;                   	/* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);        /* Wait for end of A/D Conversion */
  LPC_ADC->CR &= ~0x01000000;               /* Stop A/D Conversion */
  PotVal = ((val >> 8) & 0xF8) +            /* Extract Potenciometer Value */
           ((val >> 7) & 0x08);
}


/*
 * Timer Counter 0 Interrupt Service Routine
 *   executed each 31.25us (32kHz frequency)
 */

void TIMER0_IRQHandler(void) 
{
	uint32_t joystick = KBD_get();            /* Retrieve joystick state*/
	
  long  val;
  uint32_t cnt;

  if (DataRun) {                            /* Data Stream is running */
    val = DataBuf[DataOut];                 /* Get Audio Sample */
    cnt = (DataIn - DataOut) & (B_S - 1);   /* Buffer Data Count */
    if (cnt == (B_S - P_C*P_S)) {           /* Too much Data in Buffer */
      DataOut++;                            /* Skip one Sample */
    }
    if (cnt > (P_C*P_S)) {                  /* Still enough Data in Buffer */
      DataOut++;                            /* Update Data Out Index */
    }
    DataOut &= B_S - 1;                     /* Adjust Buffer Out Index */
    if (val < 0) VUM -= val;                /* Accumulate Neg Value */
    else         VUM += val;                /* Accumulate Pos Value */
    val  *= Volume;                         /* Apply Volume Level */
    val >>= 16;                             /* Adjust Value */
    val  += 0x8000;                         /* Add Bias */
    val  &= 0xFFFF;                         /* Mask Value */
  } else {
    val = 0x8000;                           /* DAC Middle Point */
  }

  if (Mute) {
    val = 0x8000;                           /* DAC Middle Point */
  }

  LPC_DAC->CR = val & 0xFFC0;               /* Set Speaker Output */

  if ((Tick++ & 0x03FF) == 0) {             /* On every 1024th Tick */
    get_potval();                           /* Get Potenciometer Value */
    if (VolCur == 0x8000) {                 /* Check for Minimum Level */
      Volume = 0;                           /* No Sound */
    } else {
      Volume = VolCur * PotVal;             /* Chained Volume Level */
    }
    val = VUM >> 20;                        /* Scale Accumulated Value */
    VUM = 0;                                /* Clear VUM */
    if (val > 7) val = 7;                   /* Limit Value */
  }

  LPC_TIM0->IR = 1;                         /* Clear Interrupt Flag */
	
	// joystick
	
	
	switch (joystick){
		case UP    :  app = MENU;
									sound_off = 1;
									resume = 1;
									USB_Suspend();
									NVIC_DisableIRQ(USB_IRQn);
									break;
	}
}

/* audio_sound: stream audio from PC to ARM Cortex*/
uint16_t audio_sound(void)
{	
		volatile uint32_t pclkdiv, pclk;

		if (resume == 1){
			USB_Resume();
		}
		/* SystemClockUpdate() updates the SystemFrequency variable */
		SystemClockUpdate();

		LPC_PINCON->PINSEL1 &=~((0x03<<18)|(0x03<<20));  
		
		/* P0.25, A0.0, function 01, P0.26 AOUT, function 10 */
		LPC_PINCON->PINSEL1 |= ((0x01<<18)|(0x02<<20));

		/* Enable CLOCK into ADC controller */
		LPC_SC->PCONP |= (1 << 12);

		LPC_ADC->CR = 0x00200E04;		            /* ADC: 10-bit AIN2 @ 4MHz */
		LPC_DAC->CR = 0x00008000;		            /* DAC Output set to Middle Point */

		/* By default, the PCLKSELx value is zero, thus, the PCLK for
		all the peripherals is 1/4 of the SystemFrequency. */
		/* Bit 2~3 is for TIMER0 */
		pclkdiv = (LPC_SC->PCLKSEL0 >> 2) & 0x03;
		switch ( pclkdiv )
		{ 
		case 0x00:
		default:
			pclk = SystemFrequency/4;
		break;
		case 0x01:
			pclk = SystemFrequency;
		break; 
		case 0x02:
			pclk = SystemFrequency/2;
		break; 
		case 0x03:
			pclk = SystemFrequency/8;
		break;
		}

		LPC_TIM0->MR0 = pclk/DATA_FREQ - 1;	    /* TC0 Match Value 0 */
		LPC_TIM0->MCR = 3;					            /* TCO Interrupt and Reset on MR0 */
		LPC_TIM0->TCR = 1;					            /* TC0 Enable */
		NVIC_EnableIRQ(TIMER0_IRQn);

		USB_Init();				                      /* USB Initialization */
		USB_Connect(TRUE);		                  /* USB Connect */
	
			
		/********* The main Function is an endless loop ***********/ 
	 while(sound_off == 0){
	 }
	 
	 return 0;
	 
}

/* delay: small delay function in kilo (1000) cpu cycles */
void delay(uint16_t kilo_cycles){
	uint16_t c, d;
   for (c = 1; c <= kilo_cycles; c++)
       for (d = 1; d <= 1000; d++)
       {}
}

/******************************************************************************/
/*************************** Main Menu Functions ******************************/

/* main_menu: set up main menu LCD Display */
void main_menu_LCD(){
	GLCD_Clear(White);                        /* Clear screen to white */

	// Menu Titles 
	GLCD_SetBackColor(Black);
  GLCD_SetTextColor(Yellow); 
  GLCD_DisplayString(0, 0, __FI, (unsigned char *)"   COE718 Project   ");
  GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, (unsigned char *)"    Media Center    ");
  
	// Menu choices
	GLCD_SetBackColor(White);
  GLCD_DisplayString(5, 0, __FI, (unsigned char *)"                    ");
	GLCD_SetTextColor(White); GLCD_SetBackColor(Black);
	GLCD_DisplayString(6, 4, __FI, (unsigned char *)"Photo Gallery");
	GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
	GLCD_DisplayString(7, 4, __FI, (unsigned char *)"audio Player");
	GLCD_DisplayString(8, 4, __FI, (unsigned char *)"Shoot 'em up");
}

/* mm_joystick_st: Joystick control to change between main menu choices */
uint16_t mm_joystick_st(uint16_t st){
	uint32_t joystick = KBD_get();            /*Retrieve joystick state*/
	
	
	switch (joystick){
		case DOWN  :  if (st == PH_GAL){return AUD;}
									if (st == AUD)   {return SHUMPS;}
									if (st == SHUMPS){return PH_GAL;}
										 
		case UP    :  if (st == PH_GAL){return SHUMPS;}
									if (st == AUD)   {return PH_GAL;}
									if (st == SHUMPS){return AUD;}
									
		case IN    :  if (st == PH_GAL){GLCD_Clear(White); app = PH_GAL;}
									if (st == AUD)   {GLCD_Clear(White); app = AUD;}
									if (st == SHUMPS){GLCD_Clear(White); app = SHUMPS;}
	}
	return st;
}

/* mm_change_LCD: Main Menu transition on update */
void mm_change_LCD(uint16_t st){
	if (st == PH_GAL){
		GLCD_SetBackColor(White);
		GLCD_DisplayString(5, 0, __FI, (unsigned char *)"                    ");
		GLCD_SetTextColor(White); GLCD_SetBackColor(Black);
		GLCD_DisplayString(6, 4, __FI, (unsigned char *)"Photo Gallery");
		GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
		GLCD_DisplayString(7, 4, __FI, (unsigned char *)"Audio Player");
		GLCD_DisplayString(8, 4, __FI, (unsigned char *)"Shoot 'em up");
		delay(1000);
		}
	
	if (st == AUD){
		GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
		GLCD_DisplayString(6, 4, __FI, (unsigned char *)"Photo Gallery");
		GLCD_SetTextColor(White); GLCD_SetBackColor(Black);
		GLCD_DisplayString(7, 4, __FI, (unsigned char *)"Audio Player");
		GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
		GLCD_DisplayString(8, 4, __FI, (unsigned char *)"Shoot 'em up");
		delay(1000);
		}
	
	if (st == SHUMPS){
		GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
		GLCD_DisplayString(6, 4, __FI, (unsigned char *)"Photo Gallery");
		GLCD_DisplayString(7, 4, __FI, (unsigned char *)"Audio Player");
		GLCD_SetTextColor(White); GLCD_SetBackColor(Black);
		GLCD_DisplayString(8, 4, __FI, (unsigned char *)"Shoot 'em up");
		GLCD_SetTextColor(Black); GLCD_SetBackColor(White);
		delay(1000);
		}
}

/******************************************************************************/
/************************* Photo Gallery Functions ****************************/

/* Change between Pictures*/
uint16_t ph_joystick_st(uint16_t st){
	uint32_t joystick = KBD_get();                                          /*Retrieve joystick state*/
	
	switch (joystick){
		case RIGHT :  if (st == MARIO_P){return DS_P;}
									if (st == DS_P)   {return DOTA_P;}
									if (st == DOTA_P) {return MARIO_P;}
										 
		case LEFT  :  if (st == MARIO_P){return DOTA_P;}
									if (st == DS_P)   {return MARIO_P;}
									if (st == DOTA_P) {return DS_P;}
									
		case UP    :  GLCD_Clear(White); app = MENU; main_menu_LCD();  
	}
	return st;
}

void ph_change_pic(uint16_t st){
	if (st == MARIO_P){
			GLCD_Clear(White);
			GLCD_Bitmap(0,0,240,240, MARIO);
		}
	
	if (st == DS_P){
			GLCD_Clear(White);
			GLCD_Bitmap(0,0,225,225, DS);
		}
	
	if (st == DOTA_P){
			GLCD_Clear(White);
			GLCD_Bitmap(0,0,225,225, DOTA_2);
		}
}

/******************************************************************************/
/************************* Video Game Functions *******************************/

/* create_bullet: reuses a previous bullet struct and reinitializes its parameters if any bullets have a state equal to 0 (only 10 bullets allowed at once) */
void create_bullet(Ship *Man){
	uint16_t i; 
	
	for(i = 0; i < 10; i++){
		if (bullets[i].st == 0){
			bullets[i].st = 1;
			bullets[i].x = Man->x;
			bullets[i].y = Man->y;
			bullets[i].height = 6;
			bullets[i].width = 18;
			bullets[i].des = 0;
			return;
		}
	}
}

/*get_input: update ship position and bullet creation based on joystick input */
void get_input(Ship *Man){
	
	uint32_t joystick = KBD_get();                                          /*Retrieve joystick state*/
	
	switch (joystick){
		case RIGHT : Man->x++;
								 break;
    
		case LEFT  : Man->x--;
								 break;
		
		case UP    : Man->y--;
								 break;
		
		case DOWN  : Man->y++;
								 break;
		
		case IN    : create_bullet(Man);
								 break;
	}
}

/*create_boulder: reuses a previous boulder struct and reinitializes its parameters if any boulders have a state equal to 0 (only 10 boulders allowed at once) */
void create_boulder(){
	uint16_t i;
	uint16_t y_cord_bould = rand() % (220 + 1 - 10) + 10;
	
	for(i = 0; i < 10; i++){
		if (boulders[i].st == 0){
			boulders[i].st = 1;
			boulders[i].x = 300;
			boulders[i].y = y_cord_bould;
			boulders[i].height = 31;
			boulders[i].width = 31;
			boulders[i].des = 0;
			return;
		}
	}
	
}

/*collision_detect: detect collisions between player ship and boulders, player bullet and boulders using  AABB Collision Detection Algorithm*/
void collision_detect(Ship *Man){
	uint16_t i, j;
	
	/* Ship-Boulder Detection*/
	for(i = 0; i < 10; i++){
		if(boulders[i].st == 1){
			if (((Man->x + Man->width) >= boulders[i].x) && (Man->x <= (boulders[i].x + boulders[i].width)) && ((Man->y + Man->height) >= boulders[i].y) && (Man->y <= (boulders[i].y + boulders[i].height))){
				GLCD_Clear(White);
				new_game = 0;
				mm_change_LCD(SHUMPS);
				app = MENU;
			}
		}		
	}

	/* Bullet-Boulder Detection*/
	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if(boulders[i].st == 1 && bullets[j].st == 1){
			if (((bullets[j].x + bullets[j].width) >= boulders[i].x) && (bullets[j].x <= (boulders[i].x + boulders[i].width)) && ((bullets[j].y + bullets[j].height) >= boulders[i].y) && (bullets[j].y <= (boulders[i].y + boulders[i].height))){
				boulders[i].des = 1;
				bullets[j].des = 1;
			}			
		}
	}
	}
}

/*update: process the gameplay loop and render appropriate Assets to the LED display*/
void update(Ship *Man){
	uint16_t i;
	timer++;
	
	collision_detect(Man);
	
	if (timer % 30 == 0){
		create_boulder();
	}
	
	for(i = 0; i < 10; i++){
		if (boulders[i].st == 1){
			boulders[i].x--;
			
			GLCD_Bitmap(boulders[i].x, boulders[i].y,31, 31, BOULDER);
			
			if ((boulders[i].x < 1) || (boulders[i].des == 1)){
				boulders[i].st = 0;
				boulders[i].des = 0;
				GLCD_Bitmap(boulders[i].x, boulders[i].y,31, 31, BOULDER_REMOVE);
			}
			
		}
	}
	
	GLCD_Bitmap(Man->x, Man->y,31,21, SHIP);
	
	for(i = 0; i < 10; i++){
		if (bullets[i].st == 1){
			bullets[i].x++;
			
			GLCD_Bitmap(bullets[i].x, bullets[i].y,18, 6, BULLET);
			if ((bullets[i].x > 300) || (bullets[i].des == 1)){
				bullets[i].st = 0;
				bullets[i].des = 0;
				GLCD_Bitmap(bullets[i].x, bullets[i].y,18, 6, BULLET_REMOVE);
			}
			
		}
	}
	
}

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/

int main (void) {

	/* Menu Initialization*/
	uint16_t mm_past_sel = PH_GAL;
	uint16_t mm_sel = PH_GAL;
	
	
	/* Gallery Initialization*/
	uint16_t past_pic = MARIO_P;
	uint16_t pic = MARIO_P;
	
	/* Game struct Declaration*/
	Ship player;
	
	app = MENU;																																		/* Start Program on Main Menu Screen*/
	LED_Init();                                                                   /* LED Initialization*/
  GLCD_Init();																																	/* Initialize graphical LCD*/ 
	main_menu_LCD();
	
	
	while (1){   		                                                              /* Loop forever*/
		
		/* Main Menu*/
		if (app == MENU){
			mm_past_sel = mm_sel;
			mm_sel = mm_joystick_st(mm_sel);
			
			if (mm_sel != mm_past_sel){
				mm_change_LCD(mm_sel);
			}	
		}
		
		/* Photo Gallery*/
		if (app == PH_GAL){
			past_pic = pic;
			pic = ph_joystick_st(pic);
			
			if (pic != past_pic){
				ph_change_pic(pic);
			}	
		}
		
		/* Audio Player */
		if (app == AUD){
			
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(Yellow); 
		GLCD_DisplayString(0, 0, __FI, (unsigned char *)"   COE718 Project   ");
		GLCD_SetTextColor(White);
		GLCD_DisplayString(1, 0, __FI, (unsigned char *)"    Audio Player    ");	
		
		audio_sound();
		}
		
		/* Video Game*/
		if (app == SHUMPS){
			
			if (new_game == 0){
				uint16_t i;
				player.x = 100;
				player.y = 100;
				player.height = 21;
				player.width = 31;
	
				for(i = 0; i < 10; i++){
					bullets[i].st = 0;
				}
				
				for(i = 0; i < 10; i++){
					boulders[i].st = 0;
				}
				
				new_game = 1;
			}
			
			get_input(&player);
			update(&player);
		}
		
	}
	
}

