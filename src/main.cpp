#include <Arduino.h>
#include <ADC.h>

#undef SAME_ADC

#define CURRENT_IN	A10
#ifdef SAME_ADC
#define VREF_IN    	A11
#else
#define VREF_IN    	A1
#endif


ADC adc;

volatile float vrefValue;
volatile float currentValue;
volatile uint8_t adc0Run;
volatile uint8_t adc1Run;

void adc0isr(void)
{
	int adcval = (uint16_t)adc.adc0->readSingle();

	if(adc0Run == 1)
	{
		vrefValue = adcval;
		adc.adc0->startSingleRead(VREF_IN);
	}
}

void adc1isr(void)
{
	int adcval = (uint16_t)adc.adc1->readSingle();

	if(adc1Run == 1)
	{
		currentValue = adcval;
		adc.adc1->startSingleRead(CURRENT_IN);
#ifdef SAME_ADC
		adc1Run = 2;
		return;
#endif		
	}
#ifdef SAME_ADC
	if(adc1Run == 2)
	{
		vrefValue = adcval;//((1-ADC_FILTER_10)*(float)adcval) + (ADC_FILTER_10 * vrefValue);
		adc.adc1->startSingleRead(CURRENT_IN);
		adc1Run = 1;
		return;
	}
#endif	
}

void setup() 
{
	delay(1000);
	Serial.println("Teensy 3.6 ADC Test");

	pinMode(VREF_IN, INPUT_DISABLE);
	pinMode(CURRENT_IN, INPUT_DISABLE);

	adc.adc0->singleMode();
	adc.adc0->setAveraging(8); // set number of averages
	adc.adc0->setResolution(12); // set bits of resolution
	adc.adc0->setReference(ADC_REFERENCE::REF_3V3);
	adc.adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
	adc.adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);
	adc.adc0->enableInterrupts(adc0isr);

	adc.adc1->singleMode();
	adc.adc1->setAveraging(8); // set number of averages
	adc.adc1->setResolution(12); // set bits of resolution
	adc.adc1->setReference(ADC_REFERENCE::REF_3V3);
	adc.adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
	adc.adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);
	adc.adc1->enableInterrupts(adc1isr);

	vrefValue = 0.0;
	currentValue = 0.0;

	adc0Run = 1;
	adc1Run = 1;

	//adc.adc0->calibrate();
	//adc.adc1->calibrate();
#ifndef SAME_ADC	
	adc.adc0->startSingleRead(VREF_IN);
#endif	
	adc.adc1->startSingleRead(CURRENT_IN);
}

void loop() 
{
	delay(50);
	Serial.print(vrefValue);
	Serial.print(' ');
	Serial.print(currentValue);
	Serial.print(' ');
	Serial.println(vrefValue - currentValue);
}

