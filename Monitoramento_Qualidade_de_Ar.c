#include <stdio.h>
#include "pico/bootrom.h"
#include "Bibliotecas/setup.h"
#include "Bibliotecas/controle.h"

// Variáveis globais
uint16_t adc_value;  // Armazena o valor lido do ADC
volatile bool on_off = false;  // Estado ligado/desligado do sistema
volatile char op;  // Armazena a opção escolhida no menu

// Funções declaradas
void setup();
void off();
void button_isr(uint gpio, uint32_t events);
void select();

// Enum para as opções do menu
typedef enum {
    OP_GERAL = '1',  // Monitoramento geral
    OP_CO = '2',  // Monitoramento de CO
    OP_PM10 = '3',  // Monitoramento de PM10
    OP_PM25 = '4'  // Monitoramento de PM2.5
} MenuOpcao;

int main()
{
    stdio_init_all();  // Inicializa a entrada e saída padrão
    setup();  // Configura o hardware

    // Loop principal do programa
    while (true) {
        if (on_off) {  // Se o sistema estiver ligado
            // Exibe as informações no display SSD1306
            ssd1306_draw_string(&ssd, "BEM VINDO!", centralizar_texto("BEM VINDO!"), 0);
            ssd1306_draw_string(&ssd, "=====MENU=====", centralizar_texto("=====MENU====="), 12);
            ssd1306_draw_string(&ssd, "1.Geral 2.CO", centralizar_texto("1.Geral  2.CO   "), 25);
            ssd1306_draw_string(&ssd, "3.PM10  4.PM2.5", centralizar_texto("3.PM10  4.PM2.5 "), 45);
            ssd1306_draw_string(&ssd, "===============", centralizar_texto("==============="), 55);
            ssd1306_send_data(&ssd);  // Atualiza o display com as informações

            select();  // Chama a função para selecionar a opção do menu
        } else {  // Se o sistema estiver desligado
            off();  // Chama a função para desligar o sistema
        }
        sleep_ms(100);  // Aguarda 100ms antes de repetir o loop
    }
}

// Função para configurar o hardware
void setup(){
    // Inicializa os PWM para controle de LEDs e buzzer
    pwm_setup(pin_green);    
    pwm_setup(pin_red);
    pwm_setup(buzzer1);
    pwm_setup(buzzer2);
    
    // Inicializa os botões para interrupções
    button_setup(buttonA, button_isr);
    button_setup(buttonB, button_isr);
    button_setup(joyButton, button_isr);
    
    // Inicializa o ADC para o joystick
    adc_setup(joyY);
    
    // Inicializa a comunicação I2C para o display SSD1306
    i2c_setup(i2c_scl);
    i2c_setup(i2c_sda);

    // Configura o display SSD1306
    display_setup(&ssd);
}

// Função para desligar o sistema
void off(){
    pwm_set_gpio_level(pin_red, 0);  // Desliga o LED vermelho
    pwm_set_gpio_level(pin_green, 0);  // Desliga o LED verde
    pwm_set_gpio_level(buzzer1, 0);  // Desliga o buzzer 1
    pwm_set_gpio_level(buzzer2, 0);  // Desliga o buzzer 2
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_send_data(&ssd);  // Atualiza o display
    op = '\0';  // Zera a opção selecionada
}

// Função de interrupção para os botões
void button_isr(uint gpio, uint32_t events){
    static absolute_time_t last_press = 0;  // Armazena o tempo da última pressão
    absolute_time_t now = get_absolute_time();  // Pega o tempo atual
    
    // Verifica se passou tempo suficiente entre as pressões para evitar múltiplos registros
    if(absolute_time_diff_us(last_press, now) > 200000){
        if(!gpio_get(buttonA)){  // Se o botão A for pressionado
            on_off = !on_off;  // Alterna o estado de ligado/desligado
        } else { 
            if(!gpio_get(buttonB)){  // Se o botão B for pressionado
                reset_usb_boot(0, 0);  // Reinicia o sistema
            } else { 
                if(!gpio_get(joyButton)){  // Se o botão do joystick for pressionado
                    op = '\0';  // Reseta a opção de menu
                    pwm_set_gpio_level(pin_red, 0);  // Desliga o LED vermelho
                    pwm_set_gpio_level(pin_green, 0);  // Desliga o LED verde
                }
            }
        }
    }   
    last_press = now;  // Atualiza o tempo da última pressão
}

// Função para selecionar a opção do menu
void select() {
    // Verifica se há entrada via USB
    if (stdio_usb_connected()) {
        int ch = getchar_timeout_us(0);  // Tenta ler um caractere sem bloquear
        if (ch != PICO_ERROR_TIMEOUT) {  // Se houver uma entrada válida
            op = (char)ch;  // Armazena o caractere lido
        }
    }

    // Executa a ação correspondente à opção escolhida
    switch (op) {
        case OP_GERAL:
            printf("Monitorando Qualidade do Ar\n");
            while (op != '\0' && on_off) {  // Continua enquanto a opção for válida e o sistema estiver ligado
                adc_select_input(0);  // Seleciona o canal do ADC
                adc_value = adc_read();  // Lê o valor do ADC
                controlarIQAr(adc_value);  // Processa o valor para monitoramento da qualidade do ar
                sleep_ms(500);  // Aguarda 500ms

                // Verifica se há nova entrada via USB
                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);
                    if (ch != PICO_ERROR_TIMEOUT) {
                        op = (char)ch;  // Atualiza a opção escolhida
                    }
                }
            }
            ssd1306_fill(&ssd, false);  // Limpa o display
            break;

        case OP_CO:
            printf("Monitorando Concentração de CO\n");
            while (op != '\0' && on_off) {
                adc_select_input(0);
                adc_value = adc_read();
                controlarCO(adc_value);  // Processa o valor para monitoramento de CO
                sleep_ms(500);

                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);
                    if (ch != PICO_ERROR_TIMEOUT) {
                        op = (char)ch;
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        case OP_PM10:
            printf("Monitorando Concentração de PM10\n");
            while (op != '\0' && on_off) {
                adc_select_input(0);
                adc_value = adc_read();
                controlarPM10(adc_value);  // Processa o valor para monitoramento de PM10
                sleep_ms(500);

                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);
                    if (ch != PICO_ERROR_TIMEOUT) {
                        op = (char)ch;
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;

        case OP_PM25:
            printf("Monitorando Concentração de PM2.5\n");
            while (op != '\0' && on_off) {
                adc_select_input(0);
                adc_value = adc_read();
                controlarPM25(adc_value);  // Processa o valor para monitoramento de PM2.5
                sleep_ms(500);

                if (stdio_usb_connected()) {
                    int ch = getchar_timeout_us(0);
                    if (ch != PICO_ERROR_TIMEOUT) {
                        op = (char)ch;
                    }
                }
            }
            ssd1306_fill(&ssd, false);
            break;
    }
}
