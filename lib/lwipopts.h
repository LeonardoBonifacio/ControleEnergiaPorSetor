#ifndef LWIPOPTS_H
#define LWIPOPTS_H

// Configuração mínima para lwIP
#define NO_SYS 1 //  Não estou usando sistema operacional(FreeRtos)
#define LWIP_SOCKET 0 // Dessativa a api de Socket
#define LWIP_NETCONN 0 //  Desativa a api Netconn
// Habilitam o suporte aos protocolos TCP,UDP,RAW
#define LWIP_TCP 1
#define LWIP_UDP 1
#define LWIP_RAW 1

#define MEM_ALIGNMENT 4 // Alinhamento de memória 4 bytes para arquitetura do rp2040
#define MEM_SIZE  (22*1024) // Tamanho total da heap do lwIP -> usado para buffers, conexões, alocações dinâmicas
#define MEMP_NUM_PBUF 16    // Quantidade de packet buffers disponíveis    
#define PBUF_POOL_SIZE 16               // Ajuste conforme necessário
// Número máximo de conexões (pcb) simultâneas
#define MEMP_NUM_UDP_PCB 4             
#define MEMP_NUM_TCP_PCB 4

#define MEMP_NUM_TCP_SEG 24 // Quantidade de segmentos tcp disponivéis para empacotamento
#define LWIP_IPV4 1 //  Ativa suporte a IPV4
#define LWIP_ICMP 1 // Ativa suporte a ICMP, necessário para pings por exemplo
#define LWIP_DHCP 1  // Obtenção automática de ip da rede
#define LWIP_AUTOIP 1 //  Geração de ip automática na ausência de DHCP
#define LWIP_DNS 1   // Permite resolver domínios
#define LWIP_HTTPD 1 // Ativa o servidor web embutido do lwIP
#define LWIP_HTTPD_SSI              1  // Permite usar comandos tipo <!--#echo var="uptime" -->  no HTML
#define LWIP_HTTPD_SUPPORT_POST     0  // Habilita suporte a POST, se necessário
#define LWIP_HTTPD_DYNAMIC_HEADERS 1 // Permite que os cabeçalhos http sejam gerados dinamicamente em runtime
#define HTTPD_USE_CUSTOM_FSDATA 0  // 0 usa html como strings normais em C , 1 usaria um sistema de arquivos gerados por ferramenta
#define LWIP_HTTPD_CGI 0           // Desative CGI para economizar memória, chamadas de funções C associadas a caminhos url
#define LWIP_NETIF_HOSTNAME 1    // Permite definir um nome de host para o dispositivo 
#define TCP_MSS    (1500 /*mtu*/ - 20 /*iphdr*/ - 20 /*tcphhr*/) // Define o tamamnho máximo de segmetos dos dados em TCP
#define TCP_SND_BUF (6*TCP_MSS) // Tamanho do buffer de envio
#define TCP_WND  (2*TCP_MSS)  // tamanho do buffer da janela de recepção

#endif /* LWIPOPTS_H */
