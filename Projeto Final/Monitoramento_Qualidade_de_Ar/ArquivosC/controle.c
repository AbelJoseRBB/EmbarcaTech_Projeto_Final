#include "../Bibliotecas/controle.h"
#include "../Bibliotecas/buzzer.h"
#include <string.h>

const uint maxPM10 = 250;
const uint maxPM25 = 125;
const uint maxSO2 = 800;
const uint maxCO = 17000;
const uint maxO3 = 200;
const uint maxIQAr = 210;
ssd1306_t ssd;
char iqa_str[20];

int centralizar_texto(const char *str) {
    // Cada caractere tem 8 pixels de largura
    int largura_texto = strlen(str) * 8;
    return (128 - largura_texto) / 2; // Centraliza no display de 128 pixels
}

void controlarIQAr(uint16_t adc_value) {
    uint IQAr = mapear_valor(adc_value, maxIQAr);
    printf("IQAr: %d\n", IQAr);

    snprintf(iqa_str, sizeof(iqa_str), "IQAr: %d ug/m3", IQAr);

    // Limpa o display
    ssd1306_fill(&ssd, false);

    if (IQAr <= 50) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, 0);

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "SEM PROBLEMAS", centralizar_texto("SEM PROBLEMAS"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "OTIMA", centralizar_texto("OTIMA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40); // Exibe o valor do IQA
    } else if (IQAr <= 100) {
        pwm_set_gpio_level(pin_green, wrap);
        pwm_set_gpio_level(pin_red, wrap);

        // Exibe as mensagens centralizadas
        ssd1306_draw_string(&ssd, "ATENCAO!", centralizar_texto("ATENCAO!"), 10);
        ssd1306_draw_string(&ssd, "QUALIDADE", centralizar_texto("QUALIDADE"), 20);
        ssd1306_draw_string(&ssd, "MODERADA", centralizar_texto("MODERADA"), 30);
        ssd1306_draw_string(&ssd, iqa_str, centralizar_texto(iqa_str), 40); // Exibe o valor do IQA
    } else if (IQAr <= 200) {
        pwm_set_gpio_level(pin_green, 0);
        pwm_set_gpio_level(pin_red, wrap);

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

void piscarVermelho(uint maximo, uint16_t adc_value, char *str) {
    while (mapear_valor(adc_value, maximo) > maximo - 10) {
        // Limpa o display
        ssd1306_fill(&ssd, false);

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

uint mapear_valor(uint16_t adc_value, uint maximo){
    return ((float) adc_value / 4095) * maximo - 5 ;
}