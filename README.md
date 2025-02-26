# Sistema de Monitoramento da Qualidade do Ar
Projeto para monitoramento da qualidade do ar, utilizando o Raspberry Pi Pico W e um display OLED SSD1306. Sensores (simulados por um joystick) capturam dados que são processados para determinar os níveis de poluentes no ar, como CO, PM10 e PM2.5. O sistema fornece alertas visuais e sonoros em casos críticos.

# Vídeo Explicativo
A seguir um vídeo explicativo do projeto.

https://drive.google.com/file/d/10AthzcxbARXmoBIOvCiYJZkTimYx8_gf/view?usp=sharing 

# Instruções  
Para executar o código, é necessário possuir um Raspberry Pi Pico W e conectar corretamente os periféricos ao hardware.  

## **Funcionalidades**  

### **Monitoramento da Qualidade do Ar**  
- Exibição das medições da qualidade do ar no display OLED.  
- Simulação da recepção de dados por meio do joystick.  

1. **Alertas Visuais e Sonoros**  
- O LED RGB indica diferentes condições da qualidade do ar.  
- O buzzer emite sinais sonoros em casos de alerta crítico.  

2. **Controle por Botões e Joystick**  
- **Botão A**: Liga ou desliga o sistema de monitoramento.  
- **Botão B**: Reinicia o sistema.  
- **Botão do Joystick**: Retorna ao menu principal.  
- **Eixo Y do Joystick**: Simula a variação dos dados monitorados.  

3. **Comunicação Serial**  
- O sistema envia os dados monitorados para o computador via USB.  
- O usuário pode interagir com o sistema por meio da comunicação serial.  

# Composição do Código
## Arquivo Principal: `Monitoramento_Qualidade_de_Ar.c`
- **main():** Loop principal do programa.
- **setup():** Configura GPIOs, ADC, PWM, I2C e display.
- **select():** Gerencia a seleção do menu e monitoramento.
- **off():** Desliga o sistema e limpa as saídas.
- **button_isr():** Interrupção para lidar com eventos dos botões.

## Biblioteca: `controle.h` e `controle.c`
- **centralizar_texto():** Centraliza textos no display.
- **mapear_valor():** Mapeia leituras do ADC para valores utilizáveis.
- **sirene():** Controla os sinais sonoros do buzzer.
- **piscarVermelho():** Pisca LED vermelho em casos críticos.
- **controlarIQAr(), controlarCO(), controlarPM10(), controlarPM25():** Funções de monitoramento de poluentes.

## Biblioteca: `setup.h` e `setup.c`
- **button_setup():** Configura botões com interrupções.
- **pwm_setup():** Configura PWM para LEDs e buzzer.
- **adc_setup():** Inicializa o ADC para leitura do joystick.
- **i2c_setup():** Configura interface I2C para o display.
- **display_setup():** Inicializa e configura o display OLED SSD1306.

## Biblioteca: `ssd1306.h` e `ssd1306.c`
- **ssd1306_init():** Inicializa o display OLED.
- **ssd1306_config():** Envia comandos de configuração.
- **ssd1306_draw_string():** Exibe texto no display.
- **ssd1306_send_data():** Atualiza a exibição do display.

# Como Executar
1. **Compilar e Carregar o Código:**
   - Utilize a IDE Visual Studio Code com extensão Pico SDK.
   - Compile o código e faça o upload para o Raspberry Pi Pico W.

2. **Interação com o Sistema:**
   - Utilize os botões para ligar/desligar o monitoramento.
   - Navegue pelo menu via USB e joystick.
   - Observe os alertas visuais no LED RGB.
   - O buzzer será acionado em caso de níveis críticos.

# Desenvolvedor
**Nome:** Abel José Rocha Barros Bezerra
