#include <ti/devices/msp432p4xx/driverlib/driverlib.h>



#include "encoder.h"
#include "motor.h"
#include "uart.h"
#include "ultrasonic.h"

//------------------------------- MAIN --------------------------------------------------------------------------------
int main(void)
{

    /* Halting WDT  */
    WDT_A_holdTimer();

    // INITIALIZE SENSORS
    Motorinit();
    Encoderinit();
    UARTinit();
    Initalise_HCSR04();
    Interrupt_enableMaster();
    uint8_t a;
    while(1)
    {
        a = UART_receiveData(EUSCI_A2_BASE);//recieve Uart byte
        uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);//get interrupt status
        if((getCentreDistance() > MIN_DISTANCE))//if obstacle is too near car (30 cm)
        {
            Movement(a);//move car based on rpi input
            UART_clearInterruptFlag(EUSCI_A2_BASE, status);///clear interrupt flag
        }
        else{
            DetectObstacle();//else perform obstacle avoidance
        }
    }
}
