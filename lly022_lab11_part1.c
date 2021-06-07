/* Author: Linda Ly
 * Lab Section: 23
 * Assignment: Lab #11  Exercise #1
 * Exercise Description: My custom version of the classic DDR game
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://youtu.be/Ty_wH0wumBA
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer variables and functions for atmega1284p microcontroller
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerISR()
{
    TimerFlag = 1;
}

void TimerOff()
{
    TCCR1B = 0x00;
}

void TimerOn()
{
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;

}

ISR(TIMER1_COMPA_vect)
{
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M)
{
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

typedef struct task{
        // Tasks should have members that include: state, period,
        //a measurement of elapsed time, and a function pointer.
        signed char state;              //Task's current state
        unsigned long period;           //Task period
        unsigned long elapsedTime;      //Time elapsed since last task tick
        int (*TickFct)(int);            //Task tick function
} task;

//global variables
unsigned char Mode = 0x00; //assign one for easy mode, two for hard mode
unsigned char easyMode = 0x00;
unsigned char hardMode = 0x00;
unsigned char gameDone = 0x00;
unsigned char keepScore = 0x00;
unsigned char numericalScore = 0x00;
double accPercentage = 0.00;
unsigned char incScore = 0x00;
unsigned char j = 0x00;
unsigned char k = 0x00;

//fourty elements for easy mode
unsigned char arrEasyModeC[40] =
{0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF};

unsigned char arrEasyModeD[40] =
{0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF,
 0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF};

//one hundred elements for hard mode
unsigned char arrHardModeC[100] =
{0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
 0xFF, 0x80, 0x01, 0xFF, 0x01, 0xFF, 0x80, 0xFF, 0xFF, 0xFF};

unsigned char arrHardModeD[100] =
{0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF,
 0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF,
 0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF,
 0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF,
 0xFE, 0xEF, 0xFE, 0xE0, 0xE0, 0xE0, 0xEF, 0xFE, 0xEF, 0xE0,
 0xFE, 0xE0, 0xE0, 0xEF, 0xE0, 0xFE, 0xE0, 0xEF, 0xFE, 0xEF};

//compute and return player's accuracy after game is done
double ComputeAccuracy(unsigned char numericalScore){
        double accPercentage = 0.00;
        if(Mode == 0x01){ //easy
                accPercentage = numericalScore/40.00;
        }
        else if(Mode = 0x02){ //hard
                accPercentage = numericalScore/100.00;
        }
        else{
                accPercentage = 0.00;
        }
        return accPercentage;
}

//based on global variable accPercentage, value that goes into PORTC is determined, shows score to player
unsigned char DetermineC(double accPercentage){
        unsigned char tempC = 0x00;
        if(0 <= accPercentage && accPercentage <= 0.25){
            tempC = 0xFF;
         }
         else if(0.26 <= accPercentage && accPercentage <= 0.50){
            tempC = 0xFF;
         }
         else if(0.51 <= accPercentage && accPercentage <= 0.75){
            tempC = 0xFF;
         }
         else if(0.76 <= accPercentage && accPercentage <= 0.99){
            tempC = 0xFF;
         }
         else{
            tempC = 0xFF;
         }
         return tempC;
}

//based on global variable accPercentage, value that goes into PORTD is determined, shows score to player
unsigned char DetermineD(double accPercentage){
        unsigned char tempD = 0x00;
        if(0 <= accPercentage && accPercentage <= 0.25){
            tempD = 0xEF;
         }
         else if(0.26 <= accPercentage && accPercentage <= 0.50){
            tempD = 0xE7;
         }
         else if(0.51 <= accPercentage && accPercentage <= 0.75){
            tempD = 0xE3;
         }
         else if(0.76 <= accPercentage && accPercentage <= 0.99){
            tempD = 0xE1;
         }
         else{
            tempD = 0xE0;
         }
         return tempD;
}

//starter SM - start menu, choose difficulty mode, flashing of score displayed, go back to start menu so player can play again
enum SM1_STATES { SM1_SMStart, SM1_Init, SM1_StartMenu, SM1_SelectEasy, SM1_SelectHard, SM1_GameOver, SM1_CompAcc, SM1_Wait, SM1_WaitA0, SM1_WaitA1, SM1_On1, SM1_Off1, SM1_On2, SM1_Off2, SM1_On3, SM1_Off3};
int StarterSMFct(int state) {
   unsigned char A0 = ~PINA & 0x01;
   unsigned char A1 = ~PINA & 0x02;
   unsigned char A2 = ~PINA & 0x04;
   unsigned char A3 = ~PINA & 0x08;
   switch(state) {
      case SM1_SMStart:
         if (1) {
            state = SM1_Init;
         }
         break;
      case SM1_Init:
         if (!A0 && !A1 && !A2 && !A3) {
            state = SM1_Init;
         }
         else if (A0 || A1 || A2 || A3) {
            state = SM1_StartMenu;
         }
         else {
            state = SM1_Init;
         }
         break;
      case SM1_StartMenu:
         if (!A0 && !A1) {
            state = SM1_StartMenu;
         }
         else if (A0) {
            state = SM1_WaitA0;
         }
         else if (A1) {
            state = SM1_WaitA1;
         }
         else {
            state = SM1_StartMenu;
         }
         break;
      case SM1_SelectEasy:
         if (!gameDone) {
            state = SM1_SelectEasy;
         }
         else if (gameDone) {
            state = SM1_GameOver;
         }
         else {
            state = SM1_SelectEasy;
         }
         break;
      case SM1_SelectHard:
         if (!gameDone) {
            state = SM1_SelectHard;
         }
         else if (gameDone) {
            state = SM1_GameOver;
         }
         else {
            state = SM1_SelectHard;
         }
         break;
      case SM1_GameOver:
         if (1) {
            state = SM1_CompAcc;
         }
         break;
      case SM1_CompAcc:
         if (1) {
            state = SM1_On1;
            k = 0x00;
         }
         break;
      case SM1_Wait:
         if (!A0 && !A1 && !A2 && !A3) {
            state = SM1_Wait;
         }
         else if (A0 || A1 || A2 || A3) {
            state = SM1_StartMenu;
         }
         else {
            state = SM1_Wait;
         }
         break;
      case SM1_WaitA0:
         if (A0) {
            state = SM1_WaitA0;
         }
         else if (!A0) {
            state = SM1_SelectEasy;
         }
         else {
            state = SM1_WaitA0;
         }
         break;
      case SM1_WaitA1:
         if (A1) {
            state = SM1_WaitA1;
         }
         else if (!A1) {
            state = SM1_SelectHard;
         }
         else {
            state = SM1_WaitA1;
         }
         break;
      case SM1_On1:
         if (k < 0X04) {
            state = SM1_On1;
         }
         else if (k >= 0X04) {
            state = SM1_Off1;
            k = 0x00;
         }
         else {
            state = SM1_On1;
         }
         break;
      case SM1_Off1:
         if (k < 0x04) {
            state = SM1_Off1;
         }
         else if (k >= 0x04) {
            state = SM1_On2;
            k = 0x00;
         }
         else {
            state = SM1_Off1;
         }
         break;
      case SM1_On2:
         if (k < 0x04) {
            state = SM1_On2;
         }
         else if (k >= 0x04) {
            state = SM1_Off2;
            k = 0x00;
         }
         else {
            state = SM1_On2;
         }
         break;
      case SM1_Off2:
         if (k < 0x04) {
            state = SM1_Off2;
         }
         else if (k >= 0x04) {
            state = SM1_On3;
            k = 0x00;
         }
         else {
            state = SM1_Off2;
         }
         break;
      case SM1_On3:
         if (k < 0x04) {
            state = SM1_On3;
         }
         else if (k >= 0x04) {
            state = SM1_Off3;
            k = 0x00;
         }
         else {
            state = SM1_On3;
         }
         break;
      case SM1_Off3:
         if (k < 0x04) {
            state = SM1_Off3;
         }
         else if (k >= 0x04) {
            state = SM1_Wait;
            k = 0x00;
         }
         else {
            state = SM1_Off3;
         }
         break;
      default:
         state = SM1_Init;
         break;
   }
   switch(state) {
      case SM1_SMStart:

         break;
      case SM1_Init:
         PORTC = 0x00;
         PORTD = 0xFF;

         break;
      case SM1_StartMenu:
         PORTC = 0x10;
         PORTD = 0xF9;
         easyMode = 0x00;
         hardMode = 0x00;
         Mode = 0x00;
         gameDone = 0x00;
         numericalScore = 0x00;
         break;
      case SM1_SelectEasy:
         easyMode = 0x01;
         Mode = 0x01;
         break;
      case SM1_SelectHard:
         hardMode = 0x01;
         Mode = 0x02;
         break;
      case SM1_GameOver:
         PORTC = 0xFF;
         PORTD = 0xFB;
         break;
      case SM1_CompAcc:
         accPercentage = ComputeAccuracy(numericalScore);
         PORTC = 0x00;
         PORTD = 0xFF;
         break;
      case SM1_Wait:
         PORTC = DetermineC(accPercentage);
         PORTD = DetermineD(accPercentage);
         break;
      case SM1_WaitA0:

         break;
      case SM1_WaitA1:

         break;
      case SM1_On1:
         if(k < 0x04){
            k++;
         }
         PORTC = DetermineC(accPercentage);
         PORTD = DetermineD(accPercentage);
         break;
      case SM1_Off1:
         if(k < 0x04){
            k++;
         }
         PORTC = 0x00;
         PORTD = 0xFF;
         break;
      case SM1_On2:
         if(k < 0x04){
            k++;
         }
         PORTC = DetermineC(accPercentage);
         PORTD = DetermineD(accPercentage);
         break;
      case SM1_Off2:
         if(k < 0x04){
            k++;
         }
         PORTC = 0x00;
         PORTD = 0xFF;
         break;
      case SM1_On3:
         if(k < 0x04){
            k++;
         }
         PORTC = DetermineC(accPercentage);
         PORTD = DetermineD(accPercentage);
         break;
      case SM1_Off3:
         if(k < 0x04){
            k++;
         }
         PORTC = 0x00;
         PORTD = 0xFF;
         break;
   }
   return state;
}

//SM for easy mode option
enum SM2_STATES { SM2_SMStart, SM2_WaitEasy, SM2_FiveCount, SM2_FourCount, SM2_ThreeCount, SM2_TwoCount, SM2_OneCount, SM2_KeepScore, SM2_game, SM2_Done};
int EasyModeFct(int state) {
   static unsigned char i = 0x00;
   switch(state) {
      case SM2_SMStart:
         if (1) {
            state = SM2_WaitEasy;
         }
         break;
      case SM2_WaitEasy:
         if (!easyMode) {
            state = SM2_WaitEasy;
         }
         else if (easyMode) {
            state = SM2_FiveCount;
         }
         else {
            state = SM2_WaitEasy;
         }
         break;
      case SM2_FiveCount:
         if (i < 0x02) {
            state = SM2_FiveCount;
         }
         else if (i >= 0x02) {
            state = SM2_FourCount;
            i = 0x00;
         }
         else {
            state = SM2_FiveCount;
         }
         break;
      case SM2_FourCount:
         if (i < 0x02) {
            state = SM2_FourCount;
         }
         else if (i >= 0x02) {
            state = SM2_ThreeCount;
            i = 0x00;
         }
         else {
            state = SM2_FourCount;
         }
         break;
      case SM2_ThreeCount:
         if (i < 0x02) {
            state = SM2_ThreeCount;
         }
         else if (i >= 0x02) {
            state = SM2_TwoCount;
            i = 0x00;
         }
         else {
            state = SM2_ThreeCount;
         }
         break;
      case SM2_TwoCount:
         if (i < 0x02) {
            state = SM2_TwoCount;
         }
         else if (i >= 0x02) {
            state = SM2_OneCount;
            i = 0x00;
         }
         else {
            state = SM2_TwoCount;
         }
         break;
      case SM2_OneCount:
         if (i < 0x02) {
            state = SM2_OneCount;
         }
         else if (i >= 0x02) {
            state = SM2_KeepScore;
            i = 0x00;
         }
         else {
            state = SM2_OneCount;
         }
         break;
      case SM2_KeepScore:
         if (1) {
            state = SM2_game;
         }
         break;
      case SM2_game:
         if (j < 0x28) {
            state = SM2_game;
         }
         else if (j >= 0x28) {
            state = SM2_Done;
         }
         else {
            state = SM2_game;
         }
         break;
      case SM2_Done:
         if (1) {
            state = SM2_WaitEasy;
            easyMode = 0x00;
         }
         break;
      default:
         state = SM2_WaitEasy;
         break;
   }
   switch(state) {
      case SM2_SMStart:

         break;
      case SM2_WaitEasy:
         i = 0x00;
         break;
      case SM2_FiveCount:
         if(i < 0x02){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFE;
         break;
      case SM2_FourCount:
         if(i < 0x02){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFD;
         break;
      case SM2_ThreeCount:
         if(i < 0x02){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFB;
         break;
      case SM2_TwoCount:
         if(i < 0x02){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xF7;
         break;
      case SM2_OneCount:
         if(i < 0x02){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xEF;
         break;
      case SM2_KeepScore:
         keepScore = 0x01;
         break;
      case SM2_game:
         PORTC = arrEasyModeC[j];
         PORTD = arrEasyModeD[j];
         if(j < 0x28){
            j++;
         }
         break;
      case SM2_Done:
         gameDone = 0x01;
         keepScore = 0x00;
         j = 0x00;
         break;
   }
   return state;
}

//SM for hard mode
enum SM4_STATES { SM4_SMStart, SM4_WaitHard, SM4_FiveCount, SM4_FourCount, SM4_ThreeCount, SM4_TwoCount, SM4_OneCount, SM4_KeepScore, SM4_game, SM4_Done};
int HardModeFct(int state) {
   static unsigned char i = 0x00;
   switch(state) {
      case SM4_SMStart:
         if (1) {
            state = SM4_WaitHard;
         }
         break;
      case SM4_WaitHard:
         if (!hardMode) {
            state = SM4_WaitHard;
         }
         else if (hardMode) {
            state = SM4_FiveCount;
         }
         else {
            state = SM4_WaitHard;
         }
         break;
      case SM4_FiveCount:
         if (i < 0x04) {
            state = SM4_FiveCount;
         }
         else if (i >= 0x04) {
            state = SM4_FourCount;
            i = 0x00;
         }
         else {
            state = SM4_FiveCount;
         }
         break;
      case SM4_FourCount:
         if (i < 0x04) {
            state = SM4_FourCount;
         }
         else if (i >= 0x04) {
            state = SM4_ThreeCount;
            i = 0x00;
         }
         else {
            state = SM4_FourCount;
         }
         break;
      case SM4_ThreeCount:
         if (i < 0x04) {
            state = SM4_ThreeCount;
         }
         else if (i >= 0x04) {
            state = SM4_TwoCount;
            i = 0x00;
         }
         else {
            state = SM4_ThreeCount;
         }
         break;
      case SM4_TwoCount:
         if (i < 0x04) {
            state = SM4_TwoCount;
         }
         else if (i >= 0x04) {
            state = SM4_OneCount;
            i = 0x00;
         }
         else {
            state = SM4_TwoCount;
         }
         break;
      case SM4_OneCount:
         if (i < 0x04) {
            state = SM4_OneCount;
         }
         else if (i >= 0x04) {
            state = SM4_KeepScore;
            i = 0x00;
         }
         else {
            state = SM4_OneCount;
         }
         break;
      case SM4_KeepScore:
         if (1) {
            state = SM4_game;
         }
         break;
      case SM4_game:
         if (j < 0x64) {
            state = SM4_game;
         }
         else if (j >= 0x64) {
            state = SM4_Done;
         }
         else {
            state = SM4_game;
         }
         break;
      case SM4_Done:
         if (1) {
            state = SM4_WaitHard;
            hardMode = 0x00;
         }
         break;
      default:
         state = SM4_WaitHard;
         break;
   }
   switch(state) {
      case SM4_SMStart:

         break;
      case SM4_WaitHard:
         i = 0x00;
         break;
      case SM4_FiveCount:
         if(i < 0x04){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFE;
         break;
      case SM4_FourCount:
         if(i < 0x04){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFD;
         break;
      case SM4_ThreeCount:
         if(i < 0x04){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xFB;
         break;
      case SM4_TwoCount:
         if(i < 0x04){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xF7;
         break;
      case SM4_OneCount:
         if(i < 0x04){
            i++;
         }
         PORTC = 0xFF;
         PORTD = 0xEF;
         break;
      case SM4_KeepScore:
         keepScore = 0x01;
         break;
      case SM4_game:
         PORTC = arrHardModeC[j];
         PORTD = arrHardModeD[j];
         if(j < 0x64){
            j++;
         }
         break;
      case SM4_Done:
         gameDone = 0x01;
         keepScore = 0x00;
         j = 0x00;
         break;
   }
   return state;
}

//SM keeps track of current score
enum SM3_STATES { SM3_SMStart, SM3_WaitKeep, SM3_Check, SM3_WaitA0, SM3_WaitA1, SM3_WaitA2, SM3_WaitA3, SM3_RelA0, SM3_RelA1, SM3_RelA2, SM3_RelA3};
int KeepScoreFct(int state) {
   unsigned char A0 = ~PINA & 0x01;
   unsigned char A1 = ~PINA & 0x02;
   unsigned char A2 = ~PINA & 0x04;
   unsigned char A3 = ~PINA & 0x08;
   switch(state) {
      case SM3_SMStart:
         if (1) {
            state = SM3_WaitKeep;
         }
         break;
      case SM3_WaitKeep:
         if (!keepScore) {
            state = SM3_WaitKeep;
         }
         else if (keepScore) {
            state = SM3_Check;
         }
         else {
            state = SM3_WaitKeep;
         }
         break;
      case SM3_Check:
         if (!A0 && !A1 && !A2 && !A3) {
            state = SM3_Check;
         }
         else if (A0 && keepScore) {
            state = SM3_WaitA0;
         }
         else if (A1 && keepScore) {
            state = SM3_WaitA1;
         }
         else if (A2 && keepScore) {
            state = SM3_WaitA2;
         }
         else if (A3 && keepScore) {
            state = SM3_WaitA3;
         }
         else {
            state = SM3_Check;
         }
         break;
      case SM3_WaitA0:
         if (A0) {
            state = SM3_WaitA0;
         }
         else if (!A0) {
            state = SM3_RelA0;
         }
         else {
            state = SM3_WaitA0;
         }
         break;
      case SM3_WaitA1:
         if (A1) {
            state = SM3_WaitA1;
         }
         else if (!A1) {
            state = SM3_RelA1;
         }
         else {
           state = SM3_WaitA1;
         }
         break;
      case SM3_WaitA2:
         if (A2) {
            state = SM3_WaitA2;
         }
         else if (!A2) {
            state = SM3_RelA2;
         }
         else {
            state = SM3_WaitA2;
         }
         break;
      case SM3_WaitA3:
         if (A3) {
            state = SM3_WaitA3;
         }
         else if (!A3) {
            state = SM3_RelA3;
         }
         else {
            state = SM3_WaitA3;
         }
         break;
      case SM3_RelA0:
         if (1) {
            state = SM3_Check;
         }
         break;
      case SM3_RelA1:
         if (1) {
            state = SM3_Check;
         }
         break;
      case SM3_RelA2:
         if (1) {
            state = SM3_Check;
         }
         break;
      case SM3_RelA3:
         if (1) {
            state = SM3_Check;
         }
         break;
      default:
         state = SM3_WaitKeep;
         break;
   }
   switch(state) {
      case SM3_SMStart:

         break;
      case SM3_WaitKeep:
         break;
      case SM3_Check:
         incScore = 0x00;
         break;
      case SM3_WaitA0:
         if(easyMode && !hardMode){
            if(arrEasyModeC[j] == 0xFF && arrEasyModeD[j] == 0xFE){ //checking if button pressed matches pattern
               incScore = 0x01;
            }
         }
         if(hardMode && !easyMode){
            if(arrHardModeC[j] == 0xFF && arrHardModeD[j] == 0xFE){
               incScore = 0x01;
            }
         }
         break;
      case SM3_WaitA1:
         if(easyMode && !hardMode){
            if(arrEasyModeC[j] == 0xFF && arrEasyModeD[j] == 0xEF){
               incScore = 0x01;
            }
         }
         if(hardMode && !easyMode){
            if(arrHardModeC[j] == 0xFF && arrHardModeD[j] == 0xEF){
               incScore = 0x01;
            }
         }
         break;
      case SM3_WaitA2:
         if(easyMode && !hardMode){
            if(arrEasyModeC[j] == 0x80 && arrEasyModeD[j] == 0xE0){
               incScore = 0x01;
            }
         }
         if(hardMode && !easyMode){
            if(arrHardModeC[j] == 0x80 && arrHardModeD[j] == 0xE0){
               incScore = 0x01;
            }
         }
         break;
      case SM3_WaitA3:
         if(easyMode && !hardMode){
            if(arrEasyModeC[j] == 0x01 && arrEasyModeD[j] == 0xE0){
               incScore = 0x01;
            }
         }
         if(hardMode && !easyMode){
            if(arrHardModeC[j] == 0x01 && arrHardModeD[j] == 0xE0){
               incScore = 0x01;
             }
         }
         break;
      case SM3_RelA0:
         if(incScore = 0x01){
            numericalScore++;
         }
         break;
      case SM3_RelA1:
         if(incScore = 0x01){
            numericalScore++;
         }
         break;
      case SM3_RelA2:
         if(incScore = 0x01){
            numericalScore++;
         }
         break;
      case SM3_RelA3:
         if(incScore = 0x01){
            numericalScore++;
         }
         break;
   }
   return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; //input
    DDRC = 0xFF; PORTC = 0x00; //output
    DDRD = 0xFF; PORTD = 0x00; //output
    static task task1;
    static task task2;
    static task task3;
    static task task4;
    task *tasks[] = {&task1, &task2, &task3, &task4};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    task1.state = SM1_SMStart;
    task1.period = 250;
    task1.elapsedTime = task1.period;
    task1.TickFct = &StarterSMFct;

    task2.state = SM2_SMStart;
    task2.period = 500; //easy mode period
    task2.elapsedTime = task2.period;
    task2.TickFct = &EasyModeFct;

    task3.state = SM3_SMStart;
    task3.period = 10;
    task3.elapsedTime = task3.period;
    task3.TickFct = &KeepScoreFct;

    task4.state = SM4_SMStart;
    task4.period = 250; //hard mode period
    task4.elapsedTime = task4.period;
    task4.TickFct = &HardModeFct;

    TimerSet(10);
    TimerOn();

    unsigned short i;
    while (1) {
        for(i = 0; i < numTasks; i++){ //task scheduler
                if(tasks[i]->elapsedTime == tasks[i]->period){
                        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                        tasks[i]->elapsedTime = 0;
                }
                tasks[i]->elapsedTime += 10;
            }

            while(!TimerFlag) {}
            TimerFlag = 0;
    }
    return 1;
}
