#include "Bibliotecas/ssd1306.h"
#include "Bibliotecas/font.h"

// Função de inicialização do display SSD1306
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
  ssd->width = width;                    // Largura do display
  ssd->height = height;                  // Altura do display
  ssd->pages = height / 8U;              // Número de páginas (como cada página tem 8 linhas)
  ssd->address = address;                // Endereço I2C do display
  ssd->i2c_port = i2c;                   // Instância I2C
  ssd->bufsize = ssd->pages * ssd->width + 1;  // Tamanho do buffer de dados
  ssd->ram_buffer = calloc(ssd->bufsize, sizeof(uint8_t));  // Aloca o buffer de RAM para o display
  ssd->ram_buffer[0] = 0x40;            // Inicia o buffer de dados
  ssd->port_buffer[0] = 0x80;           // Inicializa o buffer para comandos
}

// Função de configuração do display SSD1306
void ssd1306_config(ssd1306_t *ssd) {
  // Envia os comandos de configuração para o display SSD1306
  ssd1306_command(ssd, SET_DISP | 0x00);  // Desliga o display
  ssd1306_command(ssd, SET_MEM_ADDR);     // Configura o modo de endereçamento
  ssd1306_command(ssd, 0x01);             // Define o modo de endereçamento horizontal
  ssd1306_command(ssd, SET_DISP_START_LINE | 0x00); // Define a linha inicial do display
  ssd1306_command(ssd, SET_SEG_REMAP | 0x01);  // Inverte a direção do mapeamento de segmentos
  ssd1306_command(ssd, SET_MUX_RATIO);         // Define o número de linhas de multiplexação
  ssd1306_command(ssd, HEIGHT - 1);            // Define a altura do display
  ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08); // Configura a direção das linhas COM
  ssd1306_command(ssd, SET_DISP_OFFSET);       // Define o deslocamento de exibição
  ssd1306_command(ssd, 0x00);                  // Nenhum deslocamento
  ssd1306_command(ssd, SET_COM_PIN_CFG);       // Configura os pinos COM
  ssd1306_command(ssd, 0x12);                  // Define os pinos COM
  ssd1306_command(ssd, SET_DISP_CLK_DIV);      // Define o divisor de frequência do display
  ssd1306_command(ssd, 0x80);                  // Frequência
  ssd1306_command(ssd, SET_PRECHARGE);         // Define o valor de pré-carregamento
  ssd1306_command(ssd, 0xF1);                  // Configura o pré-carregamento
  ssd1306_command(ssd, SET_VCOM_DESEL);        // Configura o desequilíbrio de VCOM
  ssd1306_command(ssd, 0x30);                  // Valor do desequilíbrio de VCOM
  ssd1306_command(ssd, SET_CONTRAST);          // Define o contraste do display
  ssd1306_command(ssd, 0xFF);                  // Valor do contraste
  ssd1306_command(ssd, SET_ENTIRE_ON);         // Exibe todos os pixels ligados
  ssd1306_command(ssd, SET_NORM_INV);          // Define o modo de inversão normal
  ssd1306_command(ssd, SET_CHARGE_PUMP);       // Configura a bomba de carga
  ssd1306_command(ssd, 0x14);                  // Ativa a bomba de carga
  ssd1306_command(ssd, SET_DISP | 0x01);       // Liga o display
}

// Função para enviar um comando para o display
void ssd1306_command(ssd1306_t *ssd, uint8_t command) {
  ssd->port_buffer[1] = command;             // Coloca o comando no buffer
  i2c_write_blocking(                        // Envia o comando via I2C
    ssd->i2c_port,
    ssd->address,
    ssd->port_buffer,
    2,
    false
  );
}

// Função para enviar dados para o display
void ssd1306_send_data(ssd1306_t *ssd) {
  // Define os endereços de coluna e página para onde os dados serão enviados
  ssd1306_command(ssd, SET_COL_ADDR);      
  ssd1306_command(ssd, 0);                 
  ssd1306_command(ssd, ssd->width - 1);    
  ssd1306_command(ssd, SET_PAGE_ADDR);     
  ssd1306_command(ssd, 0);                 
  ssd1306_command(ssd, ssd->pages - 1);    
  // Envia os dados do buffer para o display
  i2c_write_blocking(
    ssd->i2c_port,
    ssd->address,
    ssd->ram_buffer,
    ssd->bufsize,
    false
  );
}

// Função para desenhar um pixel no display
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value) {
  uint16_t index = (y >> 3) + (x << 3) + 1;  // Calcula o índice do pixel no buffer
  uint8_t pixel = (y & 0b111);                // Calcula a posição do pixel dentro do byte
  if (value)
    ssd->ram_buffer[index] |= (1 << pixel);   // Define o pixel como ligado
  else
    ssd->ram_buffer[index] &= ~(1 << pixel);  // Define o pixel como desligado
}

// Função para preencher todo o display com uma cor (ligado/desligado)
void ssd1306_fill(ssd1306_t *ssd, bool value) {
    for (uint8_t y = 0; y < ssd->height; ++y) {  // Itera por todas as linhas do display
        for (uint8_t x = 0; x < ssd->width; ++x) {  // Itera por todas as colunas
            ssd1306_pixel(ssd, x, y, value);  // Define o pixel de acordo com o valor
        }
    }
}

// Função para desenhar um retângulo no display
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill) {
  // Desenha as bordas do retângulo
  for (uint8_t x = left; x < left + width; ++x) {
    ssd1306_pixel(ssd, x, top, value);
    ssd1306_pixel(ssd, x, top + height - 1, value);
  }
  for (uint8_t y = top; y < top + height; ++y) {
    ssd1306_pixel(ssd, left, y, value);
    ssd1306_pixel(ssd, left + width - 1, y, value);
  }

  // Preenche o retângulo, se necessário
  if (fill) {
    for (uint8_t x = left + 1; x < left + width - 1; ++x) {
      for (uint8_t y = top + 1; y < top + height - 1; ++y) {
        ssd1306_pixel(ssd, x, y, value);
      }
    }
  }
}

// Função para desenhar uma linha entre dois pontos
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        ssd1306_pixel(ssd, x0, y0, value);  // Desenha o pixel atual

        if (x0 == x1 && y0 == y1) break;   // Termina quando atinge o ponto final

        int e2 = err * 2;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Função para desenhar uma linha horizontal
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value) {
  for (uint8_t x = x0; x <= x1; ++x)
    ssd1306_pixel(ssd, x, y, value);  // Desenha cada pixel da linha
}

// Função para desenhar uma linha vertical
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value) {
  for (uint8_t y = y0; y <= y1; ++y)
    ssd1306_pixel(ssd, x, y, value);  // Desenha cada pixel da linha
}

// Função para desenhar um caractere no display
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y) {
    uint8_t index = (uint8_t)c;  // Obtém o índice do caractere
    if(index == 135) {
      index = 127;
    } else {
      if(index >= 129){
        index = 0;
      }
    }
    
    const uint8_t *char_data = font[index];  // Obtém os dados do caractere a partir da fonte

    // Desenha cada linha do caractere
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t line = char_data[i];  // Linha atual do caractere
        for (uint8_t j = 0; j < 8; j++) {
            if (line & 0x01) {  // Verifica o bit mais à direita
                ssd1306_pixel(ssd, x + j, y + i, 1);  // Desenha o pixel
            } else {
                ssd1306_pixel(ssd, x + j, y + i, 0);  // Apaga o pixel
            }
            line >>= 1;  // Desloca para o próximo bit
        }
    }
}

// Função para desenhar uma string no display
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y)
{
  while (*str) {
    ssd1306_draw_char(ssd, *str++, x, y);  // Desenha cada caractere da string
    x += 8;  // Move para a próxima posição horizontal
    if (x + 8 >= ssd->width) {  // Se a linha estiver cheia, vai para a próxima linha
      x = 0;
      y += 8;
    }
    if (y + 8 >= ssd->height) {  // Se não houver espaço vertical, sai
      break;
    }
  }
}