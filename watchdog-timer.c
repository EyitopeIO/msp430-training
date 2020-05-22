#include <msp430.h>
#include <driverlib.h>


/**
 * main.c for watchdog timer
 */

unsigned int count = 0x00;
unsigned int state = 0;

void dog_barks(void);       //Our interrupt service routine caused by watchdog

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	//WDT_A_hold(WDT_A_BASE);


	GPIO_setAsPeripheralModuleFunctionInputPin(
	            GPIO_PORT_PJ,
	            GPIO_PIN4 + GPIO_PIN5,
	            GPIO_PRIMARY_MODULE_FUNCTION
	        );

    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);    //our LEDs
    GPIO_setAsOutputPin(GPIO_PORT_P9,GPIO_PIN7);

    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P9,GPIO_PIN7);

    PMM_unlockLPM5();       //GPIOs locked on startup

    WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_ACLK, WDT_A_CLOCKDIVIDER_32K);
    WDT_A_start(WDT_A_BASE);

/*
    WDT_A_initIntervalTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_ACLK, WDT_A_CLOCKDIVIDER_32K);
    WDT_A_start(WDT_A_BASE);
*/

    //Enable Watchdog Interupt
    SFR_clearInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);
    SFR_enableInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);

    __bis_SR_register(GIE);     //Enable global interrupt
    __no_operation();       //Don't use debugger

    while(1);

}

#pragma vector=WDT_VECTOR
__interrupt void dog_barks(void)
{
    switch(state)
    {
    case 0:
        GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
        state = 1;
        break;
    case 1:
        GPIO_toggleOutputOnPin(GPIO_PORT_P9,GPIO_PIN7);
        state = 0;
        break;
    }
}
