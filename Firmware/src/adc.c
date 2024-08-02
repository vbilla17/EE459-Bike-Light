/**
 * @file adc.c
 * @brief Implementation file for ADC module on the ATmega328PB.
 * @date 2024-03-04
 * 
 * @author Vishal Billa (vbilla@usc.edu)
 */

#include "adc.h"

void adc_init(void) {
    // Enable ADC
    ADCSRA |= (1 << ADEN);

    // Set prescaler to 64
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);

    // Left adjust ADC result
    ADMUX |= (1 << ADLAR);

    // Hardcode MUX to read ADC7
    ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0);

    // Set reference voltage to internal reference
    ADMUX |= (1 << REFS0);
}

uint8_t adc_read(void) {
    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));

    // Return ADC result
    return ADCH;
}
