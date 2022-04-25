//---------------------------------- MOTOR -----------------------------------------------------------------
//*                MSP432P401
//*             ------------------
//*         /|\|                  |
//*          | |                  |
//*          --|RST               |
//*            |                  |
//*            |                  |
//*Right wheel<--|P4.4        P2.4  |--> Output PWM
//*Left wheel<--|P4.5        P2.5  |--> Output PWM
//*            |                  |
//---------------------------------- MOTOR -----------------------------------------------------------------

#pragma once
#include "encoder.h"

//-------------------------------- MOTOR CONFIG -------------------------------------------------------------------
Timer_A_PWMConfig right_wheel =
{
        TIMER_A_CLOCKSOURCE_SMCLK,  //SubMain Clock
        TIMER_A_CLOCKSOURCE_DIVIDER_1, //Submain clock is 3MHz, 3MHz / 3 = 1MHz
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1, //Clears capture compare interrupt flag
        TIMER_A_OUTPUTMODE_RESET_SET, //set timer output mode even if its already running
        0
};

Timer_A_PWMConfig left_wheel =
{
        TIMER_A_CLOCKSOURCE_SMCLK,  //SubMain Clock
        TIMER_A_CLOCKSOURCE_DIVIDER_1, //Submain clock is 3MHz, 3MHz / 3 = 1MHz
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_2, //Clears capture compare interrupt flag
        TIMER_A_OUTPUTMODE_RESET_SET, //set timer output mode even if its already running
        0
};

//MOTOR FUNC
void Motorinit();   //Initialization of Motor. LEFT:PIN 4.0, PIN 4.2 ; RIGHT:PIN 4.4, PIN 4.5
void Movement(char direction);  //Controls movement of car via single character parameter
void NotchControl(char direction, int notches, int duty_cycle); //For more refined control using encoder notch detection
static void Delay(uint32_t loop);
static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}


void Motorinit() //initialize motor pins
{
    //Configure Right Wheel pins
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    //Set Right direction forward
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    //Configure Left Wheel pins
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    //Set Left direction forward
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    //Set PWM output Signal
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
    // Generate PWM for for timer A using PWM config
    Timer_A_generatePWM(TIMER_A0_BASE, &right_wheel);
    Timer_A_generatePWM(TIMER_A0_BASE, &left_wheel);
}

void Movement(char direction) //car direction
{

    switch(direction)
    {
        case 'f':
            //Set Right direction forward
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
            //Set Left direction forward
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
            //set duty cycle of right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 10000);
            //set duty cycle of left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 10000);
            break;
        case 'b':
            //Set Left direction reverse
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
            //Set Right direction reverse
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
            //set duty cycle of right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 10000);
            //set duty cycle of left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 10000);
            break;
        case 'l':
            // Left and right turns are initially set as 8000 Dutycycle to revv motors
            //and allow consistent performance when using lower duty cycles to increase turning accuracy
            //Set Right direction forward
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
            //revv motor
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 8000);
            //delay to allow motor to speed up just abit
            Delay(1000);
            //drop the cycle to allow accurate turning
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 4000);
            //move left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
            break;
        case 'r':
            //Set Left direction forward
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
            //set duty cycle of left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 8000);
            Delay(1000);
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 4000);
            //set duty cycle of right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
            break;
        case 's':
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
            break;
    }
}

void NotchControl(char direction, int notches, int duty_cycle) //control motor via no. of notches
{
    left_notches = 0;
    right_notches = 0;
    switch(direction)
    {
        case 'f': //forward
        case 'b': //backward
            //while wheel does not spin the notch amount
            while (notches > left_notches && notches > right_notches)
            {
                if (notches > left_notches)
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty_cycle);
                }
                else
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
                }

                if (notches > right_notches)
                 {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, duty_cycle);

                 }
                 else
                 {
                     Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
                 }
            }
            break;
        case 'l'://turn left
            Movement('f');
            //stop left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
            //move right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, duty_cycle);
            while (notches > right_notches)
            {
                if (notches > right_notches)
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, duty_cycle);
                }
                else
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
                }
            }
            break;
        case 'r'://turn right
            Movement('f');
            //stop right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
            //move left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty_cycle);
            while (notches > left_notches)
            {
                if (notches > left_notches)
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty_cycle);
                }
                else
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
                }
            }
            break;
        case 'z'://reverse right wheel
            Movement('b');
            //stop left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
            //move right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, duty_cycle);
            while (notches > right_notches)
            {
                if (notches > right_notches)
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, duty_cycle);
                }
                else
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
                }
            }
            break;
        case 'x'://reverse left wheel
            Movement('b');
            //stop right wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 0);
            //move left wheel
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty_cycle);
            while (notches > left_notches)
            {
                if (notches > left_notches)
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty_cycle);
                }
                else
                {
                    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, 0);
                }
            }
            break;
    }

    Movement('s');
}
