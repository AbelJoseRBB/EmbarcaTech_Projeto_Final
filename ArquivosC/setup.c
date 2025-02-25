#include "../Bibliotecas/setup.h"

ssd1306_t ssd;  // Instancia o display SSD1306

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
