#include "WiFiEsp.h" //INCLUSÃO DA BIBLIOTECA
#include "SoftwareSerial.h"//INCLUSÃO DA BIBLIOTECA
#include <Servo.h>

Servo esc;
int spd = 0;

SoftwareSerial Serial1(6, 7); //PINOS QUE EMULAM A SERIAL, ONDE O PINO 6 É O RX E O PINO 7 É O TX

char ssid[] = "2.4G_ClaroVirtua 150"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "18001600";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO

int status = WL_IDLE_STATUS; //STATUS TEMPORÁRIO ATRIBUÍDO QUANDO O WIFI É INICIALIZADO E PERMANECE ATIVO
//ATÉ QUE O NÚMERO DE TENTATIVAS EXPIRE (RESULTANDO EM WL_NO_SHIELD) OU QUE UMA CONEXÃO SEJA ESTABELECIDA
//(RESULTANDO EM WL_CONNECTED)

WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80

RingBuffer buf(8); //BUFFER PARA AUMENTAR A VELOCIDADE E REDUZIR A ALOCAÇÃO DE MEMÓRIA

int statusLed = LOW; //VARIÁVEL QUE ARMAZENA O ESTADO ATUAL DO LED (LIGADO / DESLIGADO)

void setup() {
  esc.attach(9);

  Serial.begin(9600); //INICIALIZA A SERIAL
  Serial1.begin(9600); //INICIALIZA A SERIAL PARA O ESP8266
  WiFi.init(&Serial1); //INICIALIZA A COMUNICAÇÃO SERIAL COM O ESP8266
  WiFi.config(IPAddress(192, 168, 0, 25)); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR

  //INÍCIO - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
  }
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }
  server.begin();
  //FIM - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
}

void loop() {
  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE

  if (client) { //SE CLIENTE TENTAR SE CONECTAR, FAZ
    buf.init(); //INICIALIZA O BUFFER
    while (client.connected()) { //ENQUANTO O CLIENTE ESTIVER CONECTADO, FAZ
      if (client.available()) { //SE EXISTIR REQUISIÇÃO DO CLIENTE, FAZ
        char c = client.read(); //LÊ A REQUISIÇÃO DO CLIENTE
        buf.push(c); //BUFFER ARMAZENA A REQUISIÇÃO

        //IDENTIFICA O FIM DA REQUISIÇÃO HTTP E ENVIA UMA RESPOSTA
        if (buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }
        if (buf.endsWith("GET /H")) { //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "H", FAZ
          digitalWrite(LED_BUILTIN, HIGH); //ACENDE O LED
          statusLed = 1; //VARIÁVEL RECEBE VALOR 1(SIGNIFICA QUE O LED ESTÁ ACESO)
        }
        else { //SENÃO, FAZ
          if (buf.endsWith("GET /L")) { //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "L", FAZ
            digitalWrite(LED_BUILTIN, LOW); //APAGA O LED
            statusLed = 0; //VARIÁVEL RECEBE VALOR 0(SIGNIFICA QUE O LED ESTÁ APAGADO)
          }
        }
      }
    }
    client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
  }
}

//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
void sendHttpResponse(WiFiEspClient client) {
  client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
  client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
  client.println("");
  client.println("<!DOCTYPE HTML>"); //INFORMA AO NAVEGADOR A ESPECIFICAÇÃO DO HTML
  client.println("<html>"); //ABRE A TAG "html"
  client.println("<head>"); //ABRE A TAG "head"
  client.println("<h1 = 'escControl'/>"); //DEFINIÇÃO DO ICONE DA PÁGINA
  client.println("<link rel='stylesheet' type='text/css' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/webpagecss.css' />"); //REFERENCIA AO ARQUIVO CSS (FOLHAS DE ESTILO)
  client.println("<title>escControl</title>"); //ESCREVE O TEXTO NA PÁGINA
  client.println("</head>"); //FECHA A TAG "head"

  //AS LINHAS ABAIXO CRIAM A PÁGINA HTML
  client.println("<body>"); //ABRE A TAG "body"

  if (statusLed == HIGH) { //SE VARIÁVEL FOR IGUAL A HIGH (1), FAZ
    client.println("<p style='line-height:0'><font color='green'>LIGADO</font></p>"); //ESCREVE "LIGADO" NA PÁGINA
    client.println("<a href=\"/L\">APAGAR</a>"); //COMANDO PARA APAGAR O LED (PASSA O PARÂMETRO /L)
  } else { //SENÃO, FAZ
    if (statusLed == LOW) { //SE VARIÁVEL FOR IGUAL A LOW (0), FAZ
      client.println("<p style='line-height:0'><font color='red'>DESLIGADO</font></p>"); //ESCREVE "DESLIGADO" NA PÁGINA
      client.println("<a href=\"/H\">ACENDER</a>"); //COMANDO PARA ACENDER O LED (PASSA O PARÂMETRO /H)
    }
  }
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("</body>"); //FECHA A TAG "body"
  client.println("</html>"); //FECHA A TAG "html"
  delay(1); //INTERVALO DE 1 MILISSEGUNDO
}
