//--------------------------------------------- ULTRA SONIC -------------------------------------------------------------
//*                MSP432P401
//*             ------------------
//*         /|\|                  |
//*          | |                  |
//*Left Echo-->|P4.7              |
//*Left Trig-->|P4.6              |
//*Center echo-->|P5.4            |
//*Center Trig--> |P5.5           |
//*RightEcho-->|P6.5              |
//Right Trig--> |P6.4             |
//--------------------------------------------- ULTRA SONIC -------------------------------------------------------------
#pragma once

#include "motor.h"

//------------------------------- GLOBAL VARIABLES --------------------------------------------------------------------
#define MIN_DISTANCE 30.0f
#define TICKPERIOD 1000
#define TURNSPEED 5000

volatile uint32_t LeftUltraIntTimes = 0;
volatile uint32_t RightUltraIntTimes = 0;
volatile uint32_t CentreUltraIntTimes = 0;
volatile uint32_t left_notches = 0;
volatile uint32_t right_notches = 0;


//ultrasonic
void Initalise_HCSR04(void);
static uint32_t getCentreTime(void);
float getCentreDistance(void);

void DetectObstacle(void);
void LeftRightCheck(void);
static uint32_t getLeftTime(void);
float getLeftDistance(void);
static uint32_t getRightTime(void);
float getRightDistance(void);



void Initalise_HCSR04(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
            TICKPERIOD,                             // 1000 tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    /* Configure middle ultra sonic */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN5);//Trig
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);//Trig

     /* Configure left ultra sonic */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);//Trig
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);//Trig

    /* Configure right ultra sonic */
   GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);//Trig
   GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);//Trig


    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P6, GPIO_PIN4);//Middle Echo
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P4, GPIO_PIN6);//Left Echo
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P6, GPIO_PIN5);//Right Echo

    /* Configuring Timer_A1 for Up Mode, Middle*/
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    /* Configuring Timer_A2 for Up Mode, Left */
    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    /* Configuring Timer_A3 for Up Mode, Right */
    Timer_A_configureUpMode(TIMER_A3_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableInterrupt(INT_TA1_0);
    Interrupt_enableInterrupt(INT_TA2_0);
    Interrupt_enableInterrupt(INT_TA3_0);

    Timer_A_clearTimer(TIMER_A1_BASE);
    Timer_A_clearTimer(TIMER_A2_BASE);
    Timer_A_clearTimer(TIMER_A3_BASE);
}

void TA1_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    CentreUltraIntTimes++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

void TA2_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    LeftUltraIntTimes++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

void TA3_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    RightUltraIntTimes++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}


static uint32_t getCentreTime(void)
{
    uint32_t pulsetime = 0;
    pulsetime = CentreUltraIntTimes * TICKPERIOD;
    pulsetime += Timer_A_getCounterValue(TIMER_A1_BASE);
    Timer_A_clearTimer(TIMER_A1_BASE);

    Delay(3000);
    return pulsetime;
}

float getCentreDistance(void)
{
    uint32_t pulseduration = 0;
    float calculateddistance = 0;
    /* Generate 10us pulse*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);
    /* Wait for positive-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN4) == 0);
    CentreUltraIntTimes=0;
    Timer_A_clearTimer(TIMER_A1_BASE);
    /* Start Timer */
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    /* Detects negative-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN4) == 1);
    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A1_BASE);
    /* Obtain Pulse Width in microseconds */
    pulseduration = getCentreTime();
    calculateddistance = (float)pulseduration / 58.0f;

    return calculateddistance;
}


void DetectObstacle(void)
{
    Movement('s');
    //pause for dramatic effect
    Delay(100000);

    LeftRightCheck();
}

void LeftRightCheck()
{
    float l = getLeftDistance();
    float r = getRightDistance();
    if(r>l)
    {
        NotchControl('r', 20, TURNSPEED);
        Delay(3000);
        NotchControl('f', 20, TURNSPEED);
        Delay(3000);
        NotchControl('l', 20, TURNSPEED);
    }else
    {
        NotchControl('l', 20, TURNSPEED);
        NotchControl('f', 20, TURNSPEED);
        NotchControl('r', 20, TURNSPEED);
    }
}

static uint32_t getLeftTime(void)
{

    uint32_t pulsetime = 0;
    pulsetime = LeftUltraIntTimes * TICKPERIOD;
    pulsetime += Timer_A_getCounterValue(TIMER_A2_BASE);
    Timer_A_clearTimer(TIMER_A2_BASE);

    Delay(3000);
    return pulsetime;
}

float getLeftDistance(void)
{
    uint32_t pulseduration = 0;
    float calculateddistance = 0;
    /* Generate 10us pulse*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    /* Wait for positive-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6) == 0);
    LeftUltraIntTimes=0;
    Timer_A_clearTimer(TIMER_A2_BASE);
    /* Start Timer */
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    /* Detects negative-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6) == 1);
    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A2_BASE);
    /* Obtain Pulse Width in microseconds */
    pulseduration = getLeftTime();
    calculateddistance = (float)pulseduration / 58.0f;

    return calculateddistance;
}

static uint32_t getRightTime(void)
{

    uint32_t pulsetime = 0;
    pulsetime = RightUltraIntTimes * TICKPERIOD;
    pulsetime += Timer_A_getCounterValue(TIMER_A3_BASE);
    Timer_A_clearTimer(TIMER_A3_BASE);

    Delay(3000);
    return pulsetime;
}

float getRightDistance(void)
{
    uint32_t pulseduration = 0;
    float calculateddistance = 0;
    /* Generate 10us pulse*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    /* Wait for positive-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0);
    RightUltraIntTimes=0;
    Timer_A_clearTimer(TIMER_A3_BASE);
    /* Start Timer */
    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);
    /* Detects negative-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 1);
    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A3_BASE);
    /* Obtain Pulse Width in microseconds */
    pulseduration = getRightTime();
    calculateddistance = (float)pulseduration / 58.0f;

    return calculateddistance;
}
