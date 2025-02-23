#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "Bibliotecas/buzzer.h"
#include "Bibliotecas/controle.h"

#define joyY 26

#define buttonB 6
#define button 5
#define joyButton 22

#define i2c_port i2c1
#define ADDR 0x3c
#define i2c_sda 14
#define i2c_scl 15

uint16_t adc_value;
volatile bool on_off = false;
volatile char op;

void setup();
void pwm_setup(uint gpio);
void button_isr(uint gpio, uint32_t events);
void select();


void off(){
    pwm_set_gpio_level(pin_red, 0);
    pwm_set_gpio_level(pin_green, 0);
    pwm_set_gpio_level(buzzer1, 0);
    pwm_set_gpio_level(buzzer2, 0);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    op = '5';
}

int main()
{
    stdio_init_all();
    setup();    

    

    while (true) {
            if(on_off){
                ssd1306_draw_string(&ssd, "BEM VINDO!", centralizar_texto("BEM VINDO!"), 0);
                ssd1306_draw_string(&ssd, "=====MENU=====", centralizar_texto("=====MENU====="), 12);
                ssd1306_draw_string(&ssd, "1.Geral 2.CO", centralizar_texto("1.Geral  2.CO   "), 25);
                ssd1306_draw_string(&ssd, "3.PM10  4.PM2.5", centralizar_texto("3.PM10  4.PM2.5 "), 45);
                ssd1306_draw_string(&ssd, "===============", centralizar_texto("==============="), 55);
                ssd1306_send_data(&ssd);

                select();
            }else{
                off();
            }
        sleep_ms(100);
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

    gpio_init(joyButton);
    gpio_set_dir(joyButton, GPIO_IN);
    gpio_pull_up(joyButton);
    gpio_set_irq_enabled_with_callback(joyButton, GPIO_IRQ_EDGE_FALL, true, button_isr);
    
    // Inicializa o ADC para o joystick
    adc_init();
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

void button_isr(uint gpio, uint32_t events){
    
    static absolute_time_t last_press = 0;
    absolute_time_t now = get_absolute_time();
    
    if(absolute_time_diff_us(last_press, now) > 200000){
        if(!gpio_get(button)){
            on_off = !on_off;
        }else{ 
            if(!gpio_get(buttonB)){
                reset_usb_boot(0,0);
            }else{ 
                if(!gpio_get(joyButton)){
                    op = '\0';
                    pwm_set_gpio_level(pin_red, 0);
                    pwm_set_gpio_level(pin_green, 0);
                }
            }
        }
    }   
    last_press = now;
}

void select() {
    // Verifica se há uma nova entrada do usuário antes de entrar no switch
    if (stdio_usb_connected()) {
        int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
        if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada disponível
            op = (char)ch;  // Armazena o caractere lido
        }
    }

    // Executa a ação correspondente à opção selecionada
    switch (op) {
        case '1':
            printf("Monitorando Qualidade do Ar\n");
            while (op != '\0' && on_off) {  // Sai se op for '\0' ou on_off for false
                adc_select_input(0);
                adc_value = adc_read();
                controlarIQAr(adc_value);
                sleep_ms(500);

                // Verifica se há uma nova entrada do usuário
                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
                    if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada disponível
                        op = (char)ch;  // Armazena o caractere lido
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        case '2':
            printf("Monitorando Concentração de CO\n");
            while (op != '\0' && on_off) {  // Sai se op for '\0' ou on_off for false
                adc_select_input(0);
                adc_value = adc_read();
                controlarCO(adc_value);
                sleep_ms(500);

                // Verifica se há uma nova entrada do usuário
                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
                    if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada disponível
                        op = (char)ch;  // Armazena o caractere lido
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        case '3':
            printf("Monitorando Concentração de PM10\n");
            while (op != '\0' && on_off) {  // Sai se op for '\0' ou on_off for false
                adc_select_input(0);
                adc_value = adc_read();
                controlarPM10(adc_value);
                sleep_ms(500);

                // Verifica se há uma nova entrada do usuário
                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
                    if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada disponível
                        op = (char)ch;  // Armazena o caractere lido
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        case '4':
            printf("Monitorando Concentração de PM2.5\n");
            while (op != '\0' && on_off) {  // Sai se op for '\0' ou on_off for false
                adc_select_input(0);
                adc_value = adc_read();
                controlarPM25(adc_value);
                sleep_ms(500);

                // Verifica se há uma nova entrada do usuário
                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
                    if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada disponível
                        op = (char)ch;  // Armazena o caractere lido
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        default:
            // Nenhuma opção válida selecionada
            break;
    }
}




