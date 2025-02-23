#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "Bibliotecas/buzzer.h"
#include "Bibliotecas/controle.h"

#define joyX 27
#define joyY 26

#define buttonB 6
#define button 5

#define i2c_port i2c1
#define ADDR 0x3c
#define i2c_sda 14
#define i2c_scl 15

uint16_t adc_value;
volatile bool on_off = false;


void setup();
void pwm_setup(uint gpio);
void button_isr(uint gpio, uint32_t events);
void off();

int main()
{
    stdio_init_all();
    setup();    

    while (true) {

            if(on_off){
                adc_select_input(0);
                adc_value = adc_read();
                printf("Valor ADC: %d\n", adc_value);
                controlarIQAr(adc_value);
            }else{
                off();
            }
        sleep_ms(500);
    }
}

void pwm_setup(uint gpio){
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
}

void setup(){
    
    pwm_setup(pin_green);    
    pwm_setup(pin_red);
    pwm_setup(buzzer1);
    pwm_setup(buzzer2);
    
    // Inicializa e configura o callback dos botões
    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);
    gpio_set_irq_enabled_with_callback(button, GPIO_IRQ_EDGE_FALL, true, button_isr);
    
    gpio_init(buttonB);
    gpio_set_dir(buttonB, GPIO_IN);
    gpio_pull_up(buttonB);
    gpio_set_irq_enabled_with_callback(buttonB, GPIO_IRQ_EDGE_FALL, true, button_isr);
    
    // Inicializa o ADC para o joystick
    adc_init();
    adc_gpio_init(joyX);
    adc_gpio_init(joyY);
    
    // Inicializa o I2C para o display SSD1306
    i2c_init(i2c_port, 400 * 1000); // Configura a comunicação I2C com velocidade de 400kHz
    gpio_set_function(i2c_sda, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_sda);
    gpio_set_function(i2c_scl, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_scl);
    
    // Inicializa o display SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDR, i2c_port);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd);   // Envia os dados para o display
}

void off(){
    pwm_set_gpio_level(pin_red, 0);
    pwm_set_gpio_level(pin_green, 0);
    pwm_set_gpio_level(buzzer1, 0);
    pwm_set_gpio_level(buzzer2, 0);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void button_isr(uint gpio, uint32_t events){
    
    static absolute_time_t last_press = 0;
    absolute_time_t now = get_absolute_time();
    
    if(absolute_time_diff_us(last_press, now) > 200000){
        if(!gpio_get(button))   on_off = !on_off;
            else if(!gpio_get(buttonB)) reset_usb_boot(0,0);
    }   
    last_press = now;
}





