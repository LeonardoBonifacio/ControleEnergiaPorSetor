#include "hardware/pio.h"        // Biblioteca para controle do Bloco Pio em uso
#include "leds.h"


// Array para guardar numeros da gpios, para ficar mais facil de manusear
int gpios_setores[9] = {GPIO_LED_SETOR_1,
                        GPIO_LED_SETOR_2,
                        GPIO_LED_SETOR_3,
                        GPIO_LED_SETOR_4,
                        GPIO_LED_SETOR_5,
                        GPIO_LED_SETOR_6,
                        GPIO_LED_SETOR_7,
                        GPIO_LED_SETOR_8,
                        GPIO_LED_SETOR_9};

// Buffers para armazenar quais LEDs estão ligados formando o número do último setor ligado
bool  setor_1[LED_COUNT] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 1, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
};

bool  setor_2[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool  setor_3[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};


bool  setor_4[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 0, 0, 1
};


bool  setor_5[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};


bool  setor_6[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};

bool  setor_7[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool setor_8[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool setor_9[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool matriz_preenchida[LED_COUNT] = {
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1
};

// Para mandar um valor grb de 32bits(mas so 24 sendo usados) para a maquina de estado 0 do bloco 0 do PIO
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// cria um valor grb de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool desenho[])
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (desenho[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void mostraUltimoSetorLigado(int ultimoSetor){
    switch (ultimoSetor){
        case 1:
            set_one_led(255,0,0,setor_1);
            break;
        case 2:
            set_one_led(255,0,0,setor_2);
            break;
        case 3:
            set_one_led(255,0,0,setor_3);
            break;
        case 4:
            set_one_led(255,255,0,setor_4);
            break;
        case 5:
            set_one_led(255,255,0,setor_5);
            break;
        case 6:
            set_one_led(255,255,0,setor_6);
            break;
        case 7:
            set_one_led(0,255,0,setor_7);
            break;
        case 8:
            set_one_led(0,255,0,setor_8);
            break;
        case 9:
            set_one_led(0,255,0,setor_9);
            break;
    }
}