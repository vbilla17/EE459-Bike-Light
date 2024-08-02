/**
 * @file adc.h
 * @brief Header file for ADC module on the ATmega328PB.
 * @date 2024-03-04
 * 
 * @details This library provides functionality to read the ADC value from the ATmega328PB.
 * 
 * @author Vishal Billa (vbilla@usc.edu)
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief Initialize the ADC module.
 */
void adc_init(void);

/**
 * @brief Read the ADC value.
 * 
 * @return uint8_t ADC value.
 */
uint8_t adc_read(void);

#endif // ADC_H
