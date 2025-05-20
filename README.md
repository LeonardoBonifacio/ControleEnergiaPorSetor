# Controle de Energia por Setor - BitDogLab (RP2040 + Wi-Fi)

Este projeto implementa um sistema de **controle de energia por setor** em um ambiente fabril, utilizando a placa **BitDogLab com Raspberry Pi Pico W**. Através de uma interface web acessada via Wi-Fi, o usuário pode **ligar ou desligar setores específicos da fábrica**, visualizando o estado em tempo real. O sistema também conta com alertas visuais e sonoros quando muitos setores estão ligados simultaneamente.

---

## 🛠 Funcionalidades

- Interface web responsiva via Wi-Fi para controle remoto dos setores.
- Controle individual de até **9 setores** com botões Ligar/Desligar.
- Controle em grupo de 3 em 3 Setores por botões
- Controle de desligamento de todos os setores por um unico Botão
- Indicação no display OLED SSD1306 do número de setores ligados e alertas.
- Alerta visual (LEDs WS2812 + LEDs da BitDogLab) e sonoro (buzzer PWM) ao atingir limites críticos de energia.
- Interrupção no botão BOOTSEL para entrar em modo BOOTSEL facilmente.
- Utiliza o chip wifi cy43439, PIO para WS2812, I2C para display OLED, e PWM para buzzer.

---

## 🔧 Hardware Utilizado

- BitDogLab (Raspberry Pi Pico W + periféricos)
- Display OLED SSD1306 via I2C (SDA: GP14, SCL: GP15)
- Matriz de LEDs WS2812 controlada via PIO
- Buzzer passivo no GPIO 21 com PWM
- Botão BOOTSEL no GPIO 6 (para modo bootloader)

---

## 🌐 Interface Web

A interface web é acessível ao conectar a placa à rede Wi-Fi. Ela exibe:

- Estado atual de cada setor (ligado/desligado)
- Botões para controle individual de cada setor
- Estilo adaptado para dispositivos móveis


## 📶 Configuração Wi-Fi

Edite as linhas abaixo no `main.c` com o nome e senha da sua rede:

```c
#define WIFI_SSID     "SEU_SSID"
#define WIFI_PASSWORD "SUA_SENHA"
```

## 🧪 Compilação e Gravação
Pré-requisitos
- SDK do Raspberry Pi Pico
- CMake
- Compilador arm-gnu
- Biblioteca lwIP configurada para Pico W

##  ⚠️ Alertas e Regras de Segurança
Quando mais de 6 setores estão ativos, um alerta visual (LEDs acesos) e sonoro (buzzer) é ativado.
Ao ligar todos os 9 setores, o sistema desliga todos automaticamente como forma de proteção.
