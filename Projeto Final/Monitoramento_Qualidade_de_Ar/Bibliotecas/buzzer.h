#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

void sirene(uint gpio, uint gpio2, uint freq_grave, uint freq_agudo, uint duration);

#endif