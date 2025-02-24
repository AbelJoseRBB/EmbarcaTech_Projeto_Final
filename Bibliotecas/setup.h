#ifndef SETUP_H
#define SETUP_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "../Bibliotecas/ssd1306.h"
#include "../Bibliotecas/font.h"
#include "../Bibliotecas/setup.h"

// Definições dos pinos utilizados para os botões e joystick
#define buttonB 6  // Botão B conectado ao pino GPIO 6
#define buttonA 5  // Botão A conectado ao pino GPIO 5
#define joyButton 22  // Botão do joystick conectado ao pino GPIO 22
#define wrap 4096  // Valor de wrap para o PWM (máximo de 4096)

// Definições dos pinos utilizados para LEDs e buzzer
#define pin_red 13    // Pino do LED vermelho
#define pin_green 11  // Pino do LED verde
#define buzzer1 21    // Pino do primeiro buzzer
#define buzzer2 10    // Pino do segundo buzzer

#define joyY 26  // Pino de entrada do eixo Y do joystick (GPIO 26)

#define i2c_port i2c1  // Define o barramento I2C a ser utilizado (i2c1)
#define ADDR 0x3c  // Endereço do display SSD1306 (0x3C)
#define i2c_sda 14  // Pino SDA do I2C conectado ao GPIO 14
#define i2c_scl 15  // Pino SCL do I2C conectado ao GPIO 15

// Função para controlar a sirene com duas frequências (grave e aguda)
void sirene(uint gpio, uint gpio2, uint freq_grave, uint freq_agudo, uint duration);

// Função para configurar um botão com interrupção
void button_setup(uint gpio, gpio_irq_callback_t callback);

// Função para configurar o PWM em um pino
void pwm_setup(uint gpio);

// Função para configurar o ADC em um pino
void adc_setup(uint gpio);

// Função para configurar o I2C no pino GPIO
void i2c_setup(uint gpio);

// Função para configurar o display SSD1306
void display_setup(ssd1306_t *ssd);

#endif
