#include <msp430.h> 
#include <driverlib.h>

/**
 * main.c
 */

void USCI_A0_ISR(void);     //Our interrupt service routine
void config_USCI_A0(void);      //Function to configure the UART
void config_IO(void);           //GPIO configuring function

volatile unsigned char IN1 = 0x00;     //holds data in UART receive buffer
volatile unsigned char IN2 = 0x00;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config_IO();
	while(1);

	return 0;
}


/*
 * Just as before, the parameters and functions required
 * to configure the UART are in the driverlib files.
 *
 * Open eusci_a_uart.c and its .h file
 */


void config_USCI_A0(void)
{
    // Configure USCI_A0 for UART mode at 9600
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_ACLK;
    param.clockPrescalar = 3;           //Go to page 566 of family user guide for table
    param.secondModReg = 0x92;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    while(!EUSCI_A_UART_init(EUSCI_A0_BASE, &param))        //wait for OK sign from eUSCI_A0 that config was successful
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}


//UART_A0 ISR for modem
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR_modem(void)

#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  IN1 = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);        // in #include <driverlib.h>
  IN2 = UCA0RXBUF;      // in #include <msp430.h>

  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA0IFG&UCTXIFG));        //UCTXIFG flag tells us if the UART is ready for next data

      if(IN1 == 'r')
      {
          GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);       //red LED
      }
      else if(IN1 == 'g')
      {
          GPIO_toggleOutputOnPin(GPIO_PORT_P9,GPIO_PIN7);       //green LED
      }

      __no_operation();     //no debug
      break;

    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}


void config_IO(void)
{
    GPIO_setAsPeripheralModuleFunctionInputPin(
                GPIO_PORT_PJ,
                GPIO_PIN4 + GPIO_PIN5,
                GPIO_PRIMARY_MODULE_FUNCTION
            );
        GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
        GPIO_setAsOutputPin(GPIO_PORT_P9,GPIO_PIN7);
        PMM_unlockLPM5();       //GPIOs locked on startup. Unlock it.
}


/*

//UART_A1 ISR for modem

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR_modem(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  unsigned char RXdata = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));

      //Your code here

      __no_operation();
      break;

    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

*/
