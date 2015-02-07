/*
 * main.c
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_adc.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/adc.h"


#define ADC_SSFIFO              (ADC_O_SSFIFO0 - ADC_O_SSMUX0)
#define ADC_SSFSTAT             (ADC_O_SSFSTAT0 - ADC_O_SSMUX0)

main(void){
    volatile uint32_t pui32ADC0Value[1], valore, conteggio = 0;

    /// clock a 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    /// non servon se si legge in sesnore di temperatura
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    /// seleziona la sequenza 3 con trigger avviato dal processore (pag. 802 U.G.)
    /// scrive 0 nei bit da 15 a 12
    HWREG(ADC0_BASE + ADC_O_EMUX) = ADC_EMUX_EM3_PROCESSOR;
    ///  imposta la priorota' massima per il sequencer 3 scrivendo 0 nei bit 13 e 12
    HWREG(ADC0_BASE + ADC_O_SSPRI) = (ADC_SSPRI_SS3_M & 0);
    //ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);



    HWREG(ADC0_BASE + 0x000000A0) = 0;
    HWREG(ADC0_BASE + 0x000000B4) = 0;
    /// ADCSSCTL3
    HWREG(ADC0_BASE + 0x000000A4) =  (ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END) >> 4;
    /// ADCSSOP3 invia i dati alla fifo e non al comparatore
    HWREG(ADC0_BASE + 0x000000B0) = 0;
    //ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);

    //HWREG(ui32Base + ADC_O_ACTSS) |= 1 << ui32SequenceNum;
    /// abilita il sequencer numero 3
    HWREG(ADC0_BASE) |= 1 << 3;
    //ADCSequenceEnable(ADC0_BASE, 3);

    //HWREG(ui32Base + ADC_O_ISC) = 1 << ui32SequenceNum;
    /// resetta l'interruzione del sequencer 3
    HWREG(ADC0_BASE + ADC_O_ISC) = 1 << 3;
    //ADCIntClear(ADC0_BASE, 3);

    while(1)
    {
    	//HWREG(ui32Base + ADC_O_PSSI) |= ((ui32SequenceNum & 0xffff0000) |
        //(1 << (ui32SequenceNum & 0xf)));
    	/// avvia la conversione del sequencer3
    	HWREG(ADC0_BASE + ADC_O_ISC) = 1 << 3;
        //ADCProcessorTrigger(ADC0_BASE, 3);

    	//ui32Temp = (HWREG(ui32Base + ADC_O_RIS) &
        //(0x10000 | (1 << ui32SequenceNum)));
        while(!ADCIntStatus(ADC0_BASE, 3, false));
        /// resetta l'interruzione del sequencer 3
        HWREG(ADC0_BASE + ADC_O_ISC) = 1 << 3;
        //ADCIntClear(ADC0_BASE, 3);
       /* while(!(HWREG(ui32Base + ADC_SSFSTAT) & ADC_SSFSTAT0_EMPTY) &&
              (ui32Count < 8))
        {
            //
            // Read the FIFO and copy it to the destination.
            //
            *pui32Buffer++ = HWREG(ui32Base + ADC_SSFIFO);*/
        conteggio = 0;
        while(!(HWREG(ADC0_BASE + 0xAC) & ADC_SSFSTAT0_EMPTY) && (conteggio < 8)){
        	valore = HWREG(ADC0_BASE + 0xA8);
        	conteggio++;
        }
        //ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

        //System_printf("AIN0 = %4d\r", pui32ADC0Value[0]);

        SysCtlDelay(SysCtlClockGet() / 120);
    }
}
