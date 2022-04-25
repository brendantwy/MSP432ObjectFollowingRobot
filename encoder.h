/*
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P3.7  |<--- Right 
 *            |                  |
 *            |            P3.6  |<--Left Encoder
 *            |                  |
 *            |                  |

Port 3 interrupt is configured to increment a counter for both left and right encoders, to count the number of notches detected per side.
*/

#pragma once

void Encoderinit();
void PORT3_IRQHandler(void);
//--------------------------------- ENCODER ----------------------------------------------------------------
volatile extern uint32_t left_notches;
volatile extern uint32_t right_notches;

void Encoderinit()
{
    //Left encoder
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P3,GPIO_PIN7);
    //Right encoder
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN6);
    Interrupt_enableInterrupt(INT_PORT3);
}

void PORT3_IRQHandler(void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    if(status & GPIO_PIN7){
        left_notches++;
//        printf("left_notches: %d \n\n", left_notches);
//        fflush(stdout);
    }
    if(status & GPIO_PIN6){
        right_notches++;
//        printf("right_notches: %d \n\n", right_notches);
//        fflush(stdout);
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
}
