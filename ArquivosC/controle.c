#include "../Bibliotecas/controle.h"
#include "../Bibliotecas/buzzer.h"
#include <string.h>

const uint maxPM10 = 260;
const uint maxPM25 = 140;
const uint maxCO = 17070;
const uint maxIQAr = 210;
bool cor = true;
ssd1306_t ssd;
char iqa_str[20], co_str[20], pm10_str[20], pm25_str[20], o3_str[20], so2_str[20];

int centralizar_texto(const char *str) {
    // Cada caractere tem 8 pixels de largura
    int largura_texto = strlen(str) * 8;
    return (128 - largura_texto) / 2; // Centraliza no display de 128 pixels
}

uint mapear_valor(uint16_t adc_value, uint maximo){
    return ((float) adc_value / 4095) * maximo - 5 ;
}

void piscarVermelho(uint maximo, uint16_t adc_value, char *str) {
    while (mapear_valor(adc_value, maximo) > maximo - 70) {
        // Limpa o display

        ssd1306_fill(&ssd, false);
        
        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      
        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "URGENTE!!", centralizar_texto("URGENTE!!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "PESSIMA", centralizar_texto("PESSIMA"), 30);
        ssd1306_draw_string(&ssd, str, centralizar_texto(str), 40); // Exibe o valor do IQA
        ssd1306_send_data(&ssd);

        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);
        sirene(buzzer1, buzzer2, 30, 500, 1000);
        sleep_ms(500);

        pwm_set_gpio_level(pin_red, 0);
        pwm_set_gpio_level(buzzer1, 0);
        pwm_set_gpio_level(buzzer2, 0);
        sleep_ms(500);

        adc_select_input(0);
        adc_value = adc_read();
    }
}

void controlarIQAr(uint16_t adc_value) {
    uint IQAr = mapear_valor(adc_value, maxIQAr);
    printf("IQAr: %d\n", IQAr);

    snprintf(iqa_str, sizeof(iqa_str), "IQAr:%d", IQAr);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    if (IQAr <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      
        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40); // Exibe o valor do IQA
    } else if (IQAr <= 100) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40); // Exibe o valor do IQA
    } else if (IQAr <= 200) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40); // Exibe o valor do IQA
    } else {
        piscarVermelho(maxIQAr, adc_value, iqa_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

void controlarCO(uint16_t adc_value){
    uint CO = mapear_valor(adc_value, maxCO);
    printf("CO: %d\n", CO);

    snprintf(co_str, sizeof(co_str), "CO:%d ug/m3", CO);
 
    // Limpa o display
    ssd1306_fill(&ssd, false);

    if (CO <= 10000) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40); // Exibe o valor do IQA

    } else if (CO <= 13000) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40); // Exibe o valor do IQA
    } else if (CO <= 17000) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, co_str, centralizar_texto(co_str), 40); // Exibe o valor do IQA
    } else {
        piscarVermelho(maxCO, adc_value, co_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

void controlarPM10(uint16_t adc_value){
    uint PM10 = mapear_valor(adc_value, maxPM10);
    printf("PM10: %d\n", PM10);

    snprintf(pm10_str, sizeof(pm10_str), "PM10:%d ug/m3", PM10);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    if (PM10 <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40); // Exibe o valor do IQA
    } else if (PM10 <= 100) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);


        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40); // Exibe o valor do IQA
    } else if (PM10 <= 250) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);


        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, pm10_str, centralizar_texto(pm10_str), 40); // Exibe o valor do IQA
    } else {
        piscarVermelho(maxPM10, adc_value, pm10_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

void controlarPM25(uint16_t adc_value){
    uint PM25 = mapear_valor(adc_value, maxPM25);
    printf("PM2,5: %d\n", PM25);

    snprintf(pm25_str, sizeof(pm25_str), "PM25:%d ug/m3", PM25);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    if (PM25 <= 25) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40); // Exibe o valor do IQA
    } else if (PM25 <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40); // Exibe o valor do IQA
    } else if (PM25 <= 125) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);

        ssd1306_rect(&ssd, 0,0, 127, 63, cor,!cor);
        ssd1306_rect(&ssd, 2,2, 123, 59, cor,!cor);
      

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ALERTA!", centralizar_texto("ALERTA!"), 10);
        ssd1306_draw_string(&ssd, "CONCENTRACAO", centralizar_texto("CONCENTRACAO"), 20);
        ssd1306_draw_string(&ssd, "MUITO RUIM", centralizar_texto("MUITO RUIM"), 30);
        ssd1306_draw_string(&ssd, pm25_str, centralizar_texto(pm25_str), 40); // Exibe o valor do IQA
    } else {
        piscarVermelho(maxPM25, adc_value, pm25_str);
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}