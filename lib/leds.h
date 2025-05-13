#include "hardware/pio.h"        // Biblioteca para controle do Bloco Pio em uso

// Gpios que representam os setores da fabrica(setor 1 a 9)
#define GPIO_LED_SETOR_1 16
#define GPIO_LED_SETOR_2 17
#define GPIO_LED_SETOR_3 28
#define GPIO_LED_SETOR_4 18
#define GPIO_LED_SETOR_5 19
#define GPIO_LED_SETOR_6 20
#define GPIO_LED_SETOR_7 4
#define GPIO_LED_SETOR_8 9
#define GPIO_LED_SETOR_9 8

extern int gpios_setores[9];
#define GPIO_LED_VERDE_BITDOGLAB 11
#define GPIO_LED_AZUL_BITDOGLAB 12
#define GPIO_LED_VERMELHO_BITDOGLAB 13


// Definição do número de LEDs e pinos.
#define LED_COUNT 25
#define MATRIZ_LED_PIN 7


static volatile int quantidadeSetoresLigados = 0;
static volatile int  ultimoSetorLigado = 0;

// Buffers para armazenar quais LEDs estão ligados formando o número do último setor ligado
extern bool setor_1[LED_COUNT];
extern bool setor_2[LED_COUNT];
extern bool setor_3[LED_COUNT];
extern bool setor_4[LED_COUNT];
extern bool setor_5[LED_COUNT];
extern bool setor_6[LED_COUNT];
extern bool setor_7[LED_COUNT];
extern bool setor_8[LED_COUNT];
extern bool setor_9[LED_COUNT];
extern bool matriz_preenchida[LED_COUNT];


static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool desenho[]);
void mostraUltimoSetorLigado(int ultimoSetor);