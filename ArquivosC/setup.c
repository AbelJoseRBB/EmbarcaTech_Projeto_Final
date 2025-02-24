#include "../Bibliotecas/setup.h"

ssd1306_t ssd;  // Instancia o display SSD1306

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

// Função para configurar um botão com interrupção
void button_setup(uint gpio, gpio_irq_callback_t callback){
    gpio_init(gpio);  // Inicializa o GPIO
    gpio_set_dir(gpio, GPIO_IN);  // Define o pino como entrada
    gpio_pull_up(gpio);  // Ativa o resistor pull-up interno
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL, true, callback);  // Habilita a interrupção na borda de queda e define o callback
}

// Função para configurar o PWM em um pino
void pwm_setup(uint gpio){
    gpio_set_function(gpio, GPIO_FUNC_PWM);  // Define o pino como função PWM
    uint slice_num = pwm_gpio_to_slice_num(gpio);  // Obtém o número do slice PWM
    pwm_set_wrap(slice_num, wrap);  // Define o valor de wrap para o PWM
    pwm_set_enabled(slice_num, true);  // Habilita o PWM no slice
}

// Função para configurar o ADC em um pino
void adc_setup(uint gpio){
    adc_init();  // Inicializa o ADC
    adc_gpio_init(gpio);  // Inicializa o pino específico para o ADC
}

// Função para configurar o I2C no pino GPIO
void i2c_setup(uint gpio){
    i2c_init(i2c_port, 400 * 1000);  // Configura o I2C com a velocidade de 400 kHz
    gpio_set_function(gpio, GPIO_FUNC_I2C);  // Define o pino como função I2C
    gpio_pull_up(gpio);  // Ativa o resistor pull-up interno
}

// Função para configurar o display SSD1306
void display_setup(ssd1306_t *ssd){ 
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ADDR, i2c_port);  // Inicializa o display SSD1306 com as configurações fornecidas
    ssd1306_config(ssd);  // Configura o display
    ssd1306_fill(ssd, false); // Limpa o display
    ssd1306_send_data(ssd);   // Envia os dados para o display
}
