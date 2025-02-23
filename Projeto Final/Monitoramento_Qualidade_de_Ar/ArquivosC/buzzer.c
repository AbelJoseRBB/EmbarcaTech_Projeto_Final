#include "../Bibliotecas/buzzer.h"

void sirene(uint gpio, uint gpio2, uint freq_grave, uint freq_agudo, uint duration) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint clock = 125000000; // Frequência do clock do PWM (125 MHz)
    uint wrap_value_grave = clock / freq_grave; // Valor de wrap para a frequência grave
    uint wrap_value_agudo = clock / freq_agudo; // Valor de wrap para a frequência "aguda"

    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < duration * 1000) {
        // Toca a frequência grave 
        pwm_set_wrap(slice_num, wrap_value_grave);
        pwm_set_gpio_level(gpio, wrap_value_grave/3); // Ciclo de trabalho de 50%
        pwm_set_gpio_level(gpio2, wrap_value_grave/3); // Ciclo de trabalho de 50%
        sleep_ms(200); // Tempo de cada pulso

        // Toca a frequência "aguda" 
        pwm_set_wrap(slice_num, wrap_value_agudo);
        pwm_set_gpio_level(gpio, wrap_value_agudo/3); // Ciclo de trabalho de 50%
        pwm_set_gpio_level(gpio2, wrap_value_grave/3); // Ciclo de trabalho de 50%
        sleep_ms(200); // Tempo de cada pulso
    }

    // Desliga o buzzer
    pwm_set_gpio_level(gpio, 0);
    pwm_set_gpio_level(gpio2, 0); // Ciclo de trabalho de 50%
}