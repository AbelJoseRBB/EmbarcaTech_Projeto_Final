#ifndef CONTROLE_H
#define CONTROLE_H

#include "../Bibliotecas/setup.h"

// Definições dos valores máximos para PM10, PM25, CO e IQAr
extern const uint maxPM10;    // Valor máximo para PM10
extern const uint maxPM25;    // Valor máximo para PM2.5
extern const uint maxCO;      // Valor máximo para CO
extern const uint maxIQAr;    // Valor máximo para a qualidade do ar (IQAr)

// Declaração do objeto SSD1306 para o display
extern ssd1306_t ssd;

// Função para centralizar um texto no display de 128x64 pixels
int centralizar_texto(const char *str);

// Função para mapear o valor do ADC para um valor proporcional ao máximo especificado
uint mapear_valor(uint16_t adc_value, uint maximo);

// Função para controlar a sirene com duas frequências (grave e aguda)
void sirene(uint gpio, uint gpio2, uint freq_grave, uint freq_agudo, uint duration);

// Função para acionar a sirene e piscar o LED vermelho quando os valores estão críticos
void piscarVermelho(uint maximo, uint16_t adc_value, char *str);

// Funções específicas para controlar e monitorar os diferentes tipos de sensores
void controlarIQAr(uint16_t adc_value);  // Monitoramento da qualidade do ar
void controlarCO(uint16_t adc_value);    // Monitoramento da concentração de CO
void controlarPM10(uint16_t adc_value);  // Monitoramento da concentração de PM10
void controlarPM25(uint16_t adc_value);  // Monitoramento da concentração de PM2.5

#endif
