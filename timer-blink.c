#include <msp430.h>
#include <driverlib.h>

void timer0_A3_setup(void);
void timer0_A3_ISR(void);
void timer0_B7_setup(void);
void timer0_B7_ISR(void);


/*
 * So how did I know which driverlib code and functions to use?
 * I simply went to the driverlib examples for my chip and checked the .c and .h files
 */

void main(void)
{

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	//WDT_A_hold(WDT_A_BASE);

	/*
	 *  Check CSTL1 register in user guide for the right settings in clock system.
	 *  Driverlib parameters are in the the cs.h file
	 */

	CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_0);       //Set DCO for 1MHz
	CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	GPIO_setAsPeripheralModuleFunctionInputPin(
	        GPIO_PORT_PJ,
	        GPIO_PIN4 + GPIO_PIN5,
	        GPIO_PRIMARY_MODULE_FUNCTION
	    );
	GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P9,GPIO_PIN7);
	PMM_unlockLPM5();       //GPIOs locked on startup. Unlock it.
	timer0_A3_setup();
	timer0_B7_setup();


	//Enter LPM0, enable interrupts
    __bis_SR_register(LPM0_bits + GIE);

    //For debugger
    __no_operation();

	while(1);
}


void timer0_A3_setup(void)
{
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);

    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_ACLK; //Sourced from 32.768kHz clock
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = false;
    Timer_A_initContinuousMode(TIMER_A0_BASE, &param);

    Timer_A_startCounter(TIMER_A0_BASE,
        TIMER_A_CONTINUOUS_MODE
        );
}


void timer0_B7_setup(void)
{
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);

    Timer_B_initContinuousModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;           //Sourced from DCO at 8MHz
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = false;
    Timer_B_initContinuousMode(TIMER_B0_BASE, &param);

    Timer_B_startCounter(TIMER_B0_BASE,
        TIMER_B_CONTINUOUS_MODE
        );
}



//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A1_VECTOR)))
#endif
void timer0_A3_ISR(void)
{
    switch ( __even_in_range(TA0IV,14) ){
            case  0: break;                          //No interrupt
            case  2: break;                          //CCR1 not used
            case  4: break;                          //CCR2 not used
            case  6: break;                          //CCR3 not used
            case  8: break;                          //CCR4 not used
            case 10: break;                          //CCR5 not used
            case 12: break;                          //CCR6 not used
            case 14:                                 //Timer overflow interrupt
                GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
                break;
            default: break;
        }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_B1_VECTOR)))
#endif
void timer0_B7_ISR(void)
{
    switch(__even_in_range(TB0IV,14)){
        case  0: break;                          //No interrupt
       case  2: break;                          //CCR1 not used
       case  4: break;                          //CCR2 not used
       case  6: break;                          //CCR3 not used
       case  8: break;                          //CCR4 not used
       case 10: break;                          //CCR5 not used
       case 12: break;                          //CCR6 not used
       case 14:                                 //Timer overflow
           GPIO_toggleOutputOnPin(GPIO_PORT_P9,GPIO_PIN7);
           break;
       default: break;
    }
}
