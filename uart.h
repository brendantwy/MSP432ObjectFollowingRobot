//----------------------------------------------- UART -----------------------------------------------------------------
//*                MSP432P401
//*             ------------------
//*         /|\|                  |
//*          | |                  |
//*      RXD-->|P3.2              |
//*      TXD-->|P3.3              |
//*            |                  |
//*            |                  |
//*            |                  |
//----------------------------------------------- UART -----------------------------------------------------------------
#pragma once

//------------------------------- UART CONFIG ----------------------------------------------------------------------
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        19,                                             // BRDIV = 19
        8,                                              // UCxBRF = 8
        85,                                              // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                        // NO Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                          // 8 bit data length
};

void UARTinit(void);
void EUSCIA2_IRQHandler(void);
void uPrintf(unsigned char * TxArray);

void UARTinit(void)
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts (Rx) */
//    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
//    Interrupt_enableInterrupt(INT_EUSCIA2);
}

/* EUSCI A2 UART ISR */
void EUSCIA2_IRQHandler(void)
{

    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    uint8_t a = UART_receiveData(EUSCI_A2_BASE);

    Movement(a);

    UART_clearInterruptFlag(EUSCI_A2_BASE, status);
}

void uPrintf(unsigned char * TxArray)
{
    unsigned short i = 0;
    {
        UART_transmitData(EUSCI_A2_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}
