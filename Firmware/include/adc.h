/**
 * @file adc.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Header file for ADC module on the ATmega328PB
 * @date 2024-03-04
 *
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <avr/io.h>

void adc_init(void);

uint8_t adc_read(void);

#endif
