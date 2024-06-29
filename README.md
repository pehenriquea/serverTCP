# Sistema de Envio de Mensagens Curtas (Mini-X)

## Visão Geral
O projeto consiste na implementação de um sistema de envio de mensagens curtas (um mini-X) que funciona em um modelo multi-servidor. Do ponto de vista didático, o objetivo é dominar a programação orientada a eventos utilizando a primitiva `select` e a temporização por sinais.

Esse projeto faz parte do Trabalho Prático 2 de Redes de Computadores 1, lecionada pelo professor Ricardo Carlini Sperandio. Integrantes do Grupo: Amanda Veiga de Moura, Isabela Ferreira Scarabelli, e Pedro Henrique de Almeida Santos.

## Estrutura do Projeto
- `server.c`: Implementação do servidor que gerencia a troca de mensagens entre os clientes.
- `send_client.c`: Implementação do cliente responsável por enviar mensagens ao servidor.
- `display_client.c`: Implementação do cliente responsável por exibir as mensagens recebidas do servidor.
- `Makefile`: Arquivo de automação de compilação que contém as instruções para compilar todos os componentes do projeto.

## Compilação
Para compilar todos os programas, abra três terminais diferentes e execute em cada um deles os seguintes comandos:

- Terminal para o Servidor:
    ```sh
    gcc server.c -o server -lws2_32
    ```
- Terminal para o Cliente de Envio:
    ```sh
    gcc -o send_client send_client.c -lws2_32
    ```
- Terminal para o Cliente de Exibição:
    ```sh
    gcc -o display_client display_client.c -lws2_32
    ```

## Execução do Projeto

### Servidor
Para iniciar o servidor, execute no terminal referente a ele:
```sh
.\server
```
### Cliente de Envio
Para iniciar um cliente de envio, execute no terminal referente a ele:
```sh
.\send_client <ID> <IP do Servidor> <Porta do Servidor>
```
### Cliente de Exibição
Para iniciar um cliente de exibição, execute no terminal referente a ele:
```sh
.\display_client <ID> <IP do Servidor> <Porta do Servidor>
```

## Detalhes importantes
### Identificadores (ID)
- Clientes de exibição são identificados por inteiros positivos entre 1 e 1000.
- Clientes de envio de mensagens são identificados por inteiros entre 1001 e 1999.
- O servidor deve garantir que não haja duplicação de identificadores entre clientes conectados.

### IP e Porta de Servidor
- O IP do servidor deve ser fornecido no formato xxx.xxx.xxx.xxx.
- A porta do servidor deve ser um número inteiro correspondente à porta na qual o servidor está escutando.

## Protocolo de comunicação
O protocolo de comunicação utilizado é o TCP. As mensagens possuem um formato definido com quatro campos binários (inteiros de dois bytes):
```sh
typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[141];
} msg_t;
```

### Tipos de mensagens:
- OI (0): Mensagem de identificação inicial do cliente.
- TCHAU (1): Mensagem de desconexão do cliente.
- MSG (2): Mensagem de texto enviada do cliente para o servidor e do servidor para o cliente.

### Detalhes de Implementação
- O servidor gerencia múltiplas conexões simultaneamente utilizando a primitiva select.

## Considerações Finais
O projeto Mini-X implementa um sistema de envio de mensagens curtas com a finalidade de exercitar o uso de select e temporização por sinais em um contexto de rede. As funcionalidades básicas incluem a conexão de clientes, envio e recebimento de mensagens e a desconexão de clientes.

