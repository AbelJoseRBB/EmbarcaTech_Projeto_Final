#ifndef CONTROLE_H
#define CONTROLE_H

#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#include "../Bibliotecas/ssd1306.h"
#include "../Bibliotecas/font.h"

#define pin_red 13
#define pin_green 11
#define buzzer1 21
#define buzzer2 10
#define wrap 4096

extern const uint maxPM10;
extern const uint maxPM25;
extern const uint maxSO2;
extern const uint maxCO;
extern const uint maxO3;
extern const uint maxIQAr;
extern ssd1306_t ssd;

void controlarIQAr(uint16_t adc_value);
void piscarVermelho(uint maximo, uint16_t adc_value, char *str);
uint mapear_valor(uint16_t adc_value, uint maximo);

#endif