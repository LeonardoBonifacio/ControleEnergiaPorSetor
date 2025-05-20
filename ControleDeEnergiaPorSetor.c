// Controle de energia por setor em Fábrica
#include <stdio.h>              // Biblioteca padrão do C
#include "pico/bootrom.h"       // Para facilitar o desenvolvimento e entrar no modo bootsel pelo botão b da bitdoglab
#include "pico/cyw43_arch.h"    // Biblioteca para arquitetura Wi-Fi da Pico com CYW43  
#include "lwip/pbuf.h"          // Lightweight IP stack - manipulação de buffers de pacotes de rede
#include "lwip/tcp.h"           // Lightweight IP stack - fornece funções e estruturas para trabalhar com o protocolo TCP
#include "lwip/netif.h"         // Lightweight IP stack - fornece funções e estruturas para trabalhar com interfaces de rede (netif)
#include "ws2812.pio.h"         // Arquivo .h gerado pelo Pio em Assembly
#include "hardware/i2c.h"       // Biblioteca para conexão I2C com SSD1306
#include "lib/ssd1306.h"        // Bilioteca com funções utilitárias do display oled
#include "lib/font.h"           // Fonte usada para desenhar no Display oled
#include "lib/leds.h"           //  Lib para comandar todos os leds usados
#include "hardware/pwm.h"       // Pwm para sinal sonoro do buzzer ao passar de 7 setores energizados
#include "pico/stdlib.h"        // Definições padroes para pico
#include "hardware/clocks.h"    // Usado no calculo do pwm do buzzer



// Credenciais WIFI - Coloque as credencias da sua Rede
#define WIFI_SSID "LEONARDO"
#define WIFI_PASSWORD "00695470PI"


// Variáveis para conexão i2c e do display oled
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
ssd1306_t ssd; // Inicializa a estrutura do display


#define BUZZER_PIN_A 21
uint32_t wrap = 12500;// Configura o wrap a 12500 para configuração do buzzer



#define BOTAOBOOTSEL 6 // Auto Explicativo
// Interrupção acionado pelo Botão B da BitDogLab para entrar em modo BootSel
void gpio_irq_handler(uint gpio, uint32_t events){
  reset_usb_boot(0, 0);
}

// Configurar pwm no pino do buzzer
void configure_buzzer(int pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Define o pino 21 para função pwm
    uint slice_num = pwm_gpio_to_slice_num(pin); // Pega o slice correspondente a este pino
    pwm_config config = pwm_get_default_config(); // Peega um conjunto de valores padrões para a configuração do pwm
    pwm_set_wrap(slice_num, wrap - 1); // Define o wrap no slice correspondente
    uint32_t clock_hz = clock_get_hz(clk_sys); // Pega o clock do sistema que é 125Mhz
    uint32_t clkdiv = clock_hz / (440 * wrap); //  Calcula o divisor de clock
    pwm_set_clkdiv(slice_num, clkdiv); // Define o divisor de clock no slice correspondente
    pwm_init(slice_num, &config, true); // Inicializa o pwm naquele slice
    pwm_set_gpio_level(pin, 0); // Define o duty cycle pra 0
}


// Somente inicializa gpios usadas para os leds e botão(da interrupção)
void InicializaGpios(void){
    gpio_init(BOTAOBOOTSEL);
    gpio_set_dir(BOTAOBOOTSEL, GPIO_IN);
    gpio_pull_up(BOTAOBOOTSEL);
    gpio_set_irq_enabled_with_callback(BOTAOBOOTSEL, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    for (int i = 0; i < 9; i++){
        gpio_init(gpios_setores[i]);
        gpio_set_dir(gpios_setores[i], GPIO_OUT);
    }
    
    gpio_init(GPIO_LED_AZUL_BITDOGLAB);
    gpio_set_dir(GPIO_LED_AZUL_BITDOGLAB,GPIO_OUT);
    gpio_init(GPIO_LED_VERDE_BITDOGLAB);
    gpio_set_dir(GPIO_LED_VERDE_BITDOGLAB,GPIO_OUT);
    gpio_init(GPIO_LED_VERMELHO_BITDOGLAB);
    gpio_set_dir(GPIO_LED_VERMELHO_BITDOGLAB,GPIO_OUT);

}

// Inicialização i2c , usando a 400Khz
void inicializaDisplay_I2C(void){
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_fill(&ssd, false); // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(&ssd); // Envia os dados
}

// Bloco Pio e state machine usadas na matriz de leds
void configura_Inicializa_Pio(void){
    PIO pio = pio0;// Seleciona o bloco pio que será usado
    int sm = 0; // Define qual state machine será usada
    uint offset = pio_add_program(pio, &ws2812_program);// Carrega o programa PIO para controlar os WS2812 na memória do PIO.
    ws2812_program_init(pio, sm, offset, MATRIZ_LED_PIN, 800000, false); //Inicializa a State Machine para executar o programa PIO carregado.
}

// Tratamento do request do usuário para ligar ou desligar setores da fábrica, acionadas ao apertar o botão de cada setor(ligar/desligar)
void user_request(char **request){

    if (strstr(*request, "GET /setor_1_ligado") != NULL && gpio_get(GPIO_LED_SETOR_1) == 0){
        gpio_put(GPIO_LED_SETOR_1,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 1;
    }else if (strstr(*request, "GET /setor_1_desligado") != NULL && gpio_get(GPIO_LED_SETOR_1) == 1){
        gpio_put(GPIO_LED_SETOR_1,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_2_ligado") != NULL && gpio_get(GPIO_LED_SETOR_2) == 0){
        gpio_put(GPIO_LED_SETOR_2,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 2;
    }else if (strstr(*request, "GET /setor_2_desligado") != NULL && gpio_get(GPIO_LED_SETOR_2) == 1){
        gpio_put(GPIO_LED_SETOR_2,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_3_ligado") != NULL && gpio_get(GPIO_LED_SETOR_3) == 0){
        gpio_put(GPIO_LED_SETOR_3,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 3;
    }else if (strstr(*request, "GET /setor_3_desligado") != NULL && gpio_get(GPIO_LED_SETOR_3) == 1){
        gpio_put(GPIO_LED_SETOR_3,false);
        quantidadeSetoresLigados--;
    }else if(strstr(*request,"GET /setor_4_ligado")!= NULL && gpio_get(GPIO_LED_SETOR_4) == 0){
        gpio_put(GPIO_LED_SETOR_4,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 4;
    }else if(strstr(*request,"GET /setor_4_desligado")!= NULL && gpio_get(GPIO_LED_SETOR_4) == 1){
        gpio_put(GPIO_LED_SETOR_4,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_5_ligado") != NULL && gpio_get(GPIO_LED_SETOR_5) == 0){
        gpio_put(GPIO_LED_SETOR_5,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 5;
    }else if (strstr(*request, "GET /setor_5_desligado") != NULL && gpio_get(GPIO_LED_SETOR_5) == 1){
        gpio_put(GPIO_LED_SETOR_5,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_6_ligado") != NULL && gpio_get(GPIO_LED_SETOR_6) == 0){
        gpio_put(GPIO_LED_SETOR_6,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 6;
    }else if (strstr(*request, "GET /setor_6_desligado") != NULL && gpio_get(GPIO_LED_SETOR_6) == 1){
        gpio_put(GPIO_LED_SETOR_6,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_7_ligado") != NULL && gpio_get(GPIO_LED_SETOR_7) == 0){
        gpio_put(GPIO_LED_SETOR_7,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 7;
    }else if (strstr(*request, "GET /setor_7_desligado") != NULL && gpio_get(GPIO_LED_SETOR_7) == 1){
        gpio_put(GPIO_LED_SETOR_7,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_8_ligado") != NULL && gpio_get(GPIO_LED_SETOR_8) == 0){
        gpio_put(GPIO_LED_SETOR_8,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 8;
    }else if (strstr(*request, "GET /setor_8_desligado") != NULL && gpio_get(GPIO_LED_SETOR_8) == 1){
        gpio_put(GPIO_LED_SETOR_8,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /setor_9_ligado") != NULL && gpio_get(GPIO_LED_SETOR_9) == 0){
        gpio_put(GPIO_LED_SETOR_9,true);
        quantidadeSetoresLigados++;
        ultimoSetorLigado = 9;
    }else if (strstr(*request, "GET /setor_9_desligado") != NULL && gpio_get(GPIO_LED_SETOR_9) == 1){
        gpio_put(GPIO_LED_SETOR_9,false);
        quantidadeSetoresLigados--;
    }else if (strstr(*request, "GET /todos_desligado")){
        for (int i = 0; i < 9; i++){
            gpio_put(gpios_setores[i],0);
            quantidadeSetoresLigados = 0;
            ultimoSetorLigado = 0;
        }
    }else if (strstr(*request, "GET /ligar_primeiros_3")){
        if (gpio_get(gpios_setores[0]) && gpio_get(gpios_setores[1]) && gpios_setores[2]){
            return; // Caso ja estejam as 3 ligadas a lógica é pulada
        }else{
            for (int i = 0; i < 3; i++)gpio_put(gpios_setores[i],1);
            quantidadeSetoresLigados += 3;
            ultimoSetorLigado = 3;
        }
        
    }else if (strstr(*request, "GET /ligar_meio_3")){
        if (gpio_get(gpios_setores[3]) && gpio_get(gpios_setores[4]) && gpio_get(gpios_setores[5])){
            return; //  Caso ja estejam as 3 ligadas a lógica é pulada
        }else{
            for (int i = 3; i < 6; i++)gpio_put(gpios_setores[i],1);
            quantidadeSetoresLigados += 3;
            ultimoSetorLigado = 6;
        }
    }else if (strstr(*request, "GET /ligar_ultimos_3")){
        if (gpio_get(gpios_setores[6]) && gpio_get(gpios_setores[7]) && gpio_get(gpios_setores[8])){
            return; // Caso ja estejam as 3 ligadas a lógica é pulada
        }else{
            for (int i = 6; i < 9; i++)gpio_put(gpios_setores[i],1);
            quantidadeSetoresLigados += 3;
            ultimoSetorLigado = 9;
        }
    }

};


// Função de callback para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){
    if (!p){
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    // Alocação do request na memória dinámica
    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Tratamento de request - Controle dos LEDs
    user_request(&request);
    

    // Cria a resposta HTML com menos espaço ocupado
    char *html = (char *)malloc(3500);  // Menor alocação necessária
    if (!html) return ERR_MEM;

    /// (Dentro do snprintf inicial)
    snprintf(html, 3500,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<style>"
        "body{background:#222;color:#eee;font-family:sans-serif;text-align:center;padding:20px}"
        "h1{font-size:2em;margin-bottom:0.5em}"
        ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:15px}"
        ".s{padding:15px;border-radius:8px;transition:background .3s;}"
        ".s.on {background:#2a4;}"       // verde suave
        ".s.off{background:#444;opacity:0.5;}" // cinza esmaecido
        "button{margin-top:8px;padding:8px;width:100%;border:none;border-radius:5px;cursor:pointer}"
        ".on button.off, .off button.on{display:inline-block;}"
        ".on button.on, .off button.off{display:none;}" // só mostra o botão relevante
    "</style>"
    "</head><body>"
    "<h1>Controle de Energia</h1>"
    "<div class='grid'>"
    );

    // Dentro do loop, para cada setor i:
    for (int i = 0; i < 9; i++) {
        bool ligado = gpio_get(gpios_setores[i]);
        char setor[350];
        snprintf(setor, sizeof(setor),
        "<div class='s %s'>"
            "<strong>Setor %d</strong><br>"
            "<form action='./setor_%d_ligado'><button class='on'>Ligar</button></form>"
            "<form action='./setor_%d_desligado'><button class='off'>Desligar</button></form>"
        "</div>",
        ligado ? "on" : "off",
        i+1,
        i+1,
        i+1
        );
        strncat(html, setor, 3500 - strlen(html) - 1);
    }


    // Adicionei as duas novas divs com os forms necessários para os 4 novos botões
    strncat(html,
    "</div>" // fecha .grid
    "<br><div style='margin-top:30px;'>"
        "<form action='./ligar_primeiros_3' style='display:inline-block;margin:5px;'>"
            "<button>Ligar Setores 1 a 3</button>"
        "</form>"
        "<form action='./ligar_meio_3' style='display:inline-block;margin:5px;'>"
            "<button>Ligar Setores 4 a 6</button>"
        "</form>"
        "<form action='./ligar_ultimos_3' style='display:inline-block;margin:5px;'>"
            "<button>Ligar Setores 7 a 9</button>"
        "</form>"
        "<div style='margin-top:20px;text-align:center;'>"
            "<form action='./todos_desligado' style='display:inline-block;'>"
                "<button style='padding:10px 20px;'>Desligar Todos</button>"
            "</form>"
        "</div>"
    "</body></html>",
    3500 - strlen(html) - 1);


    // Envia para o cliente
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    // Limpeza
    free(request);
    free(html);
    pbuf_free(p);

    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err){
    tcp_recv(newpcb, tcp_server_recv); // Para a nova conexão a função de callback sera tcp_server_recv
    return ERR_OK;
}


void checaQuantidadeSetoresLigados(){
    if (quantidadeSetoresLigados < 7){
        mostraUltimoSetorLigado(ultimoSetorLigado);
        gpio_put(GPIO_LED_AZUL_BITDOGLAB,0);
        gpio_put(GPIO_LED_VERDE_BITDOGLAB,0);
        gpio_put(GPIO_LED_VERMELHO_BITDOGLAB,0);
        pwm_set_gpio_level(BUZZER_PIN_A, 0);
        ssd1306_show_numbers_of_sectors_on(&ssd,quantidadeSetoresLigados);
    }
    else if (quantidadeSetoresLigados < 9){
        ultimoSetorLigado = 0;
        set_one_led(255,255,255,matriz_preenchida);
        gpio_put(GPIO_LED_AZUL_BITDOGLAB,1);
        gpio_put(GPIO_LED_VERDE_BITDOGLAB,1);
        gpio_put(GPIO_LED_VERMELHO_BITDOGLAB,1);
        pwm_set_gpio_level(BUZZER_PIN_A, wrap/2);
        ssd1306_draw_alert_message(&ssd);
    }
    else{
        set_one_led(0,0,0,matriz_preenchida);
        gpio_put(GPIO_LED_AZUL_BITDOGLAB,0);
        gpio_put(GPIO_LED_VERDE_BITDOGLAB,0);
        gpio_put(GPIO_LED_VERMELHO_BITDOGLAB,0);
        for (int i = 0; i < 9; i++){
            gpio_put(gpios_setores[i], 0);
        }        
        quantidadeSetoresLigados = 0;
        ultimoSetorLigado = 0;
        pwm_set_gpio_level(BUZZER_PIN_A, 0);
        ssd1306_fill(&ssd,false);
    }
    ssd1306_send_data(&ssd);
}

int main(){
    //Inicializa todos os tipos de bibliotecas stdio padrão presentes que estão ligados ao binário.
    stdio_init_all();

    // Função básica para inicializar Gpios Utilizadas
    InicializaGpios();

    // Inicializa display oled e conexão i2c
    inicializaDisplay_I2C();

    // Inicializa variáveis e programa PIO
    configura_Inicializa_Pio();

    //Configura o pwm no pino do Buzzer
    configure_buzzer(BUZZER_PIN_A);


    // Estes sleeps no main são so pra poder ver melhor no monitor serial as mensagens printadas
    sleep_ms(3000);
    //Inicializa a arquitetura do cyw43
    while (cyw43_arch_init()){
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(1000);
        return -1;
    }

     // Ativa o Wi-Fi no modo Station, de modo a que possam ser feitas ligações a outros pontos de acesso Wi-Fi.
    cyw43_arch_enable_sta_mode();

     // Conectar à rede WiFI - fazer um loop até que esteja conectado
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)){
         printf("Falha ao conectar");
         sleep_ms(1000);
         return -1;
    }
    printf("Conectado ao Wifi!\n\n");
    sleep_ms(1000);
 
    // Caso seja a interface de rede padrão - imprimir o IP do dispositivo.
    if (netif_default){
        printf("Ip do Dispositivo: %s\n\n",ipaddr_ntoa(&netif_default->ip_addr));
        sleep_ms(1000);
    }
 
    // Configura o servidor TCP - cria novos PCBs TCP. É o primeiro passo para estabelecer uma conexão TCP.
    struct tcp_pcb *server = tcp_new();
    if (!server){
        printf("Falha ao criar servidor TCP\n\n");
        sleep_ms(1000);
        return -1;
    }
 
    //vincula um PCB (Protocol Control Block) TCP a um endereço IP e porta específicos.
    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK){
        printf("Falha ao associar servidor TCP à porta 80\n");
        sleep_ms(1000);
        return -1;
    }
 
    // Coloca um PCB (Protocol Control Block) TCP em modo de escuta, permitindo que ele aceite conexões de entrada.
    server = tcp_listen(server);
 
    // Define uma função de callback para aceitar conexões TCP de entrada. É um passo importante na configuração de servidores TCP.
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n\n");
    sleep_ms(1000);
 
    while (true){
        /* 
        * Efetuar o processamento exigido pelo cyw43_driver ou pela stack TCP/IP.
        * Este método deve ser chamado periodicamente a partir do ciclo principal 
        * quando se utiliza um estilo de sondagem pico_cyw43_arch 
        */
       cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo e para tratamento de pacotes
       checaQuantidadeSetoresLigados();
       sleep_ms(100);      // Reduz o uso da CPU

    }
    
    //Desligar a arquitetura CYW43.
    cyw43_arch_deinit();
    return 0;
}
