#ifndef SSD_H
#define SSD_H

#include <stdlib.h>  
#include "pico/stdlib.h"  
#include "hardware/i2c.h"  

// Definindo as dimensões do display OLED
#define WIDTH 128  // Largura do display (em pixels)
#define HEIGHT 64  // Altura do display (em pixels)

// Enum com os comandos de controle do display SSD1306
typedef enum {
  SET_CONTRAST = 0x81,              // Comando para ajustar o contraste
  SET_ENTIRE_ON = 0xA4,             // Comando para acionar o modo "toda tela ligada"
  SET_NORM_INV = 0xA6,              // Comando para inverter ou normalizar os pixels
  SET_DISP = 0xAE,                  // Comando para ligar/desligar o display
  SET_MEM_ADDR = 0x20,              // Comando para definir o modo de endereçamento de memória
  SET_COL_ADDR = 0x21,              // Comando para definir a coluna a ser acessada
  SET_PAGE_ADDR = 0x22,             // Comando para definir a página a ser acessada
  SET_DISP_START_LINE = 0x40,       // Comando para definir a linha inicial de exibição
  SET_SEG_REMAP = 0xA0,             // Comando para inverter o mapeamento de segmentos
  SET_MUX_RATIO = 0xA8,             // Comando para definir a relação de multiplexação
  SET_COM_OUT_DIR = 0xC0,           // Comando para definir a direção da linha COM
  SET_DISP_OFFSET = 0xD3,           // Comando para definir o deslocamento de exibição
  SET_COM_PIN_CFG = 0xDA,           // Comando para definir a configuração do pino COM
  SET_DISP_CLK_DIV = 0xD5,          // Comando para configurar o divisor de relógio
  SET_PRECHARGE = 0xD9,             // Comando para definir o tempo de pré-carga
  SET_VCOM_DESEL = 0xDB,            // Comando para definir a resistência do VCOM
  SET_CHARGE_PUMP = 0x8D            // Comando para ativar/desativar a bomba de carga
} ssd1306_command_t;

// Estrutura que define o modelo do display SSD1306
typedef struct {
  uint8_t width, height, pages, address;  // Tamanho do display e endereço I2C
  i2c_inst_t *i2c_port;                    // Instância do barramento I2C
  bool external_vcc;                       // Indicador de se o display usa VCC externo
  uint8_t *ram_buffer;                     // Buffer de dados da RAM do display
  size_t bufsize;                          // Tamanho do buffer de dados
  uint8_t port_buffer[2];                  // Buffer auxiliar para comunicação I2C
} ssd1306_t;

// Funções que são usadas para inicializar, configurar e enviar dados para o display

// Inicializa a estrutura do display com configurações básicas
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);

// Configura o display com os comandos de inicialização
void ssd1306_config(ssd1306_t *ssd);

// Envia um comando para o display através da interface I2C
void ssd1306_command(ssd1306_t *ssd, uint8_t command);

// Envia os dados do buffer para o display
void ssd1306_send_data(ssd1306_t *ssd);

// Funções para desenhar elementos no display

// Desenha um pixel no display na posição (x, y) com o valor especificado
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value);

// Preenche toda a tela com pixels de valor 0 ou 1 (preenchido ou apagado)
void ssd1306_fill(ssd1306_t *ssd, bool value);

// Desenha um retângulo com as bordas no display, podendo preencher a área interna
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill);

// Desenha uma linha reta entre os pontos (x0, y0) e (x1, y1)
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value);

// Desenha uma linha horizontal entre os pontos (x0, x1) em uma linha y constante
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value);

// Desenha uma linha vertical entre os pontos (y0, y1) em uma coluna x constante
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value);

// Função para desenhar um caractere no display a partir de uma matriz de fontes
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y);

// Função para desenhar uma string (sequência de caracteres) no display
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y);

#endif 