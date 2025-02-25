#include "../Bibliotecas/controle.h"
#include <string.h>

// Definições dos valores máximos para PM10, PM2.5, CO e IQAr
const uint maxPM10 = 260;   // Valor máximo para PM10
const uint maxPM25 = 140;   // Valor máximo para PM2.5
const uint maxCO = 17070;   // Valor máximo para CO
const uint maxIQAr = 210;   // Valor máximo para IQAr (qualidade do ar)

// Variáveis de controle
bool cor = true;            // Usada para alternar entre cores (ligar/desligar)
char iqa_str[20], co_str[20], pm10_str[20], pm25_str[20]; // Strings para armazenar os valores exibidos

// Função para centralizar o texto no display de 128x64 pixels
int centralizar_texto(const char *str) {
    int largura_texto = strlen(str) * 8;  // Cada caractere ocupa 8 pixels de largura
    return (128 - largura_texto) / 2;      // Calcula a posição central
}

// Função para mapear o valor de ADC para o valor máximo especificado
uint mapear_valor(uint16_t adc_value, uint maximo){
    return ((float) adc_value / 4095) * maximo - 5;  // Ajusta a leitura do ADC para o intervalo máximo
}

// Função que controla o funcionamento da sirene com duas frequências (grave e aguda)
void sirene(uint gpio, uint gpio2, uint freq_grave, uint freq_agudo, uint duration) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);  // Obtém o número do slice PWM associado ao pino
    uint clock = 125000000; // Frequência do clock do PWM (125 MHz)
    
    // Calcula o valor de wrap para cada frequência
    uint wrap_value_grave = clock / freq_grave; // Valor de wrap para a frequência grave
    uint wrap_value_agudo = clock / freq_agudo; // Valor de wrap para a frequência aguda

    absolute_time_t start_time = get_absolute_time();  // Obtém o tempo inicial
    while (absolute_time_diff_us(start_time, get_absolute_time()) < duration * 1000) {  // Loop por "duration" milissegundos
        // Toca a frequência grave 
        pwm_set_wrap(slice_num, wrap_value_grave);  // Define o valor de wrap para a frequência grave
        pwm_set_gpio_level(gpio, wrap_value_grave / 3); // Define o ciclo de trabalho do PWM para o pino do GPIO
        pwm_set_gpio_level(gpio2, wrap_value_grave / 3); // Define o ciclo de trabalho do PWM para o segundo pino
        sleep_ms(200); // Tempo de cada pulso (200 ms)

        // Toca a frequência aguda 
        pwm_set_wrap(slice_num, wrap_value_agudo);  // Define o valor de wrap para a frequência aguda
        pwm_set_gpio_level(gpio, wrap_value_agudo / 3); // Define o ciclo de trabalho do PWM para o pino do GPIO
        pwm_set_gpio_level(gpio2, wrap_value_grave / 3); // Define o ciclo de trabalho do PWM para o segundo pino
        sleep_ms(200); // Tempo de cada pulso (200 ms)
    }

    // Desliga o buzzer ao final da duração
    pwm_set_gpio_level(gpio, 0);
    pwm_set_gpio_level(gpio2, 0); // Desliga ambos os pinos
}

// Função para piscar o LED vermelho e emitir um som de alerta
void piscarVermelho(uint maximo, uint16_t adc_value, char *str) {
    // Enquanto o valor mapeado do ADC for maior que um limiar crítico
    while (mapear_valor(adc_value, maximo) > maximo - 70) {
        // Limpa o display
        ssd1306_fill(&ssd, false);
        
        // Desenha os retângulos coloridos de alerta
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        
        // Exibe as mensagens centralizadas no display
        if(str[0] == 'I'){
            ssd1306_draw_string(&ssd, "URGENTE!!", centralizar_texto("URGENTE!!"), 10);
            ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
            ssd1306_draw_string(&ssd, "PESSIMA", centralizar_texto("PESSIMA"), 30);
            ssd1306_draw_string(&ssd, str, centralizar_texto(str), 40);  // Exibe o valor do IQA
        }else{
            ssd1306_draw_string(&ssd, "URGENTE!!", centralizar_texto("URGENTE!!"), 10);
            ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
            ssd1306_draw_string(&ssd, "PESSIMA", centralizar_texto("PESSIMA"), 30);
            ssd1306_draw_string(&ssd, str, centralizar_texto(str), 40);  // Exibe a concentração dos poluentes 
        }
        
        // Ativa os LEDs e o buzzer
        ssd1306_send_data(&ssd);
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);
        sirene(buzzer1, buzzer2, 30, 500, 1000);
        sleep_ms(500);

        // Desliga os LEDs e o buzzer
        pwm_set_gpio_level(pin_red, 0);
        pwm_set_gpio_level(buzzer1, 0);
        pwm_set_gpio_level(buzzer2, 0);
        sleep_ms(500);

        // Lê novamente o valor do ADC
        adc_select_input(0);
        adc_value = adc_read();
    }
}

// Função para controlar o índice de qualidade do ar (IQAr)
void controlarIQAr(uint16_t adc_value) {
    uint IQAr = mapear_valor(adc_value, maxIQAr);  // Mapeia o valor do ADC
    printf("IQAr: %d\n", IQAr);

    snprintf(iqa_str, sizeof(iqa_str), "IQAr:%d", IQAr);  // Converte para string para exibição

    // Limpa o display
    ssd1306_fill(&ssd, false);

    // Lógica para verificar a qualidade do ar e exibir as mensagens correspondentes
    if (IQAr <= 40) {
        pwm_set_gpio_level(pin_green, wrap);  // Acende o LED verde
        pwm_set_gpio_level(pin_red, 0);      // Apaga o LED vermelho

        // Desenha os retângulos e exibe mensagens
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40);
    } else if (IQAr <= 80) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40);
    } else if (IQAr <= 200) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40);
    } else {
        piscarVermelho(maxIQAr, adc_value, iqa_str);  // Se o IQA for muito alto, pisca o vermelho
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

// Função para controlar a concentração de CO
void controlarCO(uint16_t adc_value){
    uint CO = mapear_valor(adc_value, maxCO);
    printf("CO: %d\n", CO);

    snprintf(co_str, sizeof(co_str), "CO:%d ug/m3", CO);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    // Exibe diferentes mensagens conforme o nível de CO
    if (CO <= 10000) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40);
    } else if (CO <= 13000) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40);
    } else if (CO <= 17000) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40);
    } else {
        piscarVermelho(maxCO, adc_value, co_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

// Função para controlar o nível de PM10
void controlarPM10(uint16_t adc_value) {
    uint PM10 = mapear_valor(adc_value, maxPM10);
    printf("PM10: %d\n", PM10);

    snprintf(pm10_str, sizeof(pm10_str), "PM10:%d ug/m3", PM10);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    // Exibe diferentes mensagens conforme o nível de PM10
    if (PM10 <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40);
    } else if (PM10 <= 100) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40);
    } else if (PM10 <= 250) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40);
    } else {
        piscarVermelho(maxPM10, adc_value, pm10_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

// Função para controlar o nível de PM2.5
void controlarPM25(uint16_t adc_value) {
    uint PM25 = mapear_valor(adc_value, maxPM25);
    printf("PM2.5: %d\n", PM25);

    snprintf(pm25_str, sizeof(pm25_str), "PM2.5:%d ug/m3", PM25);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    // Exibe diferentes mensagens conforme o nível de PM2.5
    if (PM25 <= 25) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40);
    } else if (PM25 <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40);
    } else if (PM25 <= 125) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);
        ssd1306_rect(&ssd, 0, 0, 127, 63, cor, !cor);
        ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40);
    } else {
        piscarVermelho(maxPM25, adc_value, pm25_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}
