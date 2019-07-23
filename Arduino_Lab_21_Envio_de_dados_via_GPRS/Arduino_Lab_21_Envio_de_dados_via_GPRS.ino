/***************************************************************************
    Arduino Lab 21 - Envio de dados para um servidor remoto via GPRS
                 
****************************************************************************
  -- IDE do Arduino Versão 1.8.9
  -- Autor: Eduardo Avelar
  -- Blog: easytromlabs.com
  -- email: contato@easytromlabs.com

  -- Julho, 2019
****************************************************************************/
#include <String.h>
#include <SoftwareSerial.h>
#include "DHT.h"

SoftwareSerial A6board (8, 9);  // RX e TX

#define DEBUG                 // Define o debud de dados pela porta serial 
#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 3                 // Pino do microcontrolador ligado ao reset do modulo
#define voltPin A7            // Pino que recebe o valor de tensão da bateria
#define LED_PIN 13            // Led de sinalização de final de transmissão

#define DHTPIN 2              // Pino digital ao qual está conectado o DHT
#define DHTTYPE DHT22         // DHT 22  (AM2302), AM2321
#define SERIALTIMEOUT 3000

// Variáveis para Globais
int contador = 0;
char end_c[2];
float voltage;
float denominator;
float umidade;
float temperatura;
int resistor1 = 4700;
int resistor2 = 4700;

DHT dht(DHTPIN, DHTTYPE);
int signalPower = 0;
int avaregaPower = 0;

// Protótipo das funções
bool A6begin();
String A6read();
void reset();
int getSignalStrength();
bool sendToRemoteServerGSM(float hum, float temp, float volt, int pot);
String returnStringFromModemComand(String ATcommand, int responseDelay);
byte A6waitFor(String response1, String response2, int timeOut);
byte A6command(String command, String response1, String response2, int timeOut, int repetitions);

//--------------------------------------------------------------------------------------
// Função para configuração
//--------------------------------------------------------------------------------------
void setup() {
  A6board.begin(9600);                  // the GPRS baud rate
  Serial.begin(9600);                   // Serial baud rate
  pinMode(RST, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  dht.begin();
  reset();

#ifdef DEBUG
  Serial.println("Leitura de um sensor de umidade e temperatura e envio para um servidor");
#endif
  //  R2 / (R1 + R2)
  denominator = (float)resistor2 / (resistor1 + resistor2);
}

//--------------------------------------------------------------------------------------
// Função principal, looping infinito
//--------------------------------------------------------------------------------------
void loop() {

  signalPower = getSignalStrength();
  avaregaPower += signalPower;
  umidade = dht.readHumidity();            // Realiza a leitura de himidade
  temperatura = dht.readTemperature();      // Realiza a leitura da temperatura em °C
  voltage = analogRead(voltPin);            // Realiza a aquisição da tensão da bateria
  voltage = (voltage / 1024) * 5.0;         // Converte o valor para um range de tensão de 0 a 5 Vdc
  voltage = voltage / denominator;          // converte o valor para o range de entrada - Máximo 10V para o divosor de tensão

  if (isnan(umidade) || isnan(temperatura)) {
#ifdef DEBUG
    Serial.println("Falha na leitura com o sensor!");
#endif
    return;
  }
  // Em telecomunicação, a sigla RSSI (Received Signal Strength Indicator)
  // é a medida da potencia presente em um sinal de rádio recebido
  // Para transformar em dmb, basta: dbm = (2 * RSSI) - 113 para redes 2G
#ifdef DEBUG
  Serial.print("Qualidade do sinal: ");
  Serial.print(signalPower);
  Serial.println(" RSSI");
  Serial.print("Bateria: ");
  Serial.print(voltage);
  Serial.println(" V");
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" *C ");
  Serial.print("Valor do contador: ");
  Serial.println(contador);
  Serial.println();
#endif
  contador ++;
  delay(10000);

  if (contador == 3) {                               // Aguarda 10 ciclos para enviar os dados para o servidor
    avaregaPower = avaregaPower / 3;                 // Realiza uma média aritmética com o valor da potencia
#ifdef DEBUG
    Serial.print("Potencia média: ");
    Serial.println(avaregaPower);
    Serial.println("Enviando para o servidor...");
#endif
    sendToRemoteServerGSM(umidade, temperatura, voltage, avaregaPower); // Envia para o servidor
    contador = 0;
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }
}

//--------------------------------------------------------------------------------------
// Função para iniciar o modem com os comandos corretos
//--------------------------------------------------------------------------------------
bool A6begin() {
  A6board.println("AT+CREG?");            // Solicita o Status da rede
  byte hi = A6waitFor("1,", "5,", 1500);  // Possíveis Respostas-> 1: Registrado na rede local, de origem ; 5: Registrado em modo roaming
  while ( hi != OK) {                     // Prende o firmware  modem até que ele se registre a uma rede
    A6board.println("AT+CREG?");
    hi = A6waitFor("1,", "5,", 1500);
  }
  if (A6command("ATE0", "OK", "yy", 5000, 2) == OK) {           // Desabilita o modo ECHO do modem
    if (A6command("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) {    // Habilita a indicaão completa de erros do modem
      return OK; 
    }
    else {
      return NOTOK;
    }
  }
}

//--------------------------------------------------------------------------------------
// Envia dados para um servidor remoto
//--------------------------------------------------------------------------------------
bool sendToRemoteServerGSM(float hum, float temp, float volt, int pot) {
  String host = "iot-minasgerais.000webhostapp.com";
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // VERIFICA O STATUS DA CONEXÃO IP
  A6command("AT+CGATT?", "OK", "yy", 20000, 2);                             // VERIFICA SE ESTAMOS CONECTADOS A REDE
  A6command("AT+CGATT=1", "OK", "yy", 20000, 2);                            // SE NÃO ESTIVER CONECTADO A REDE, CONECTA
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // VERIFICA O STATUS DA CONEXÃO IP
  A6command("AT+CGDCONT=1,\"IP\",\"timbrasil.br\"", "OK", "yy", 20000, 2);  // CONECTA A REDE GPRS DA TIM BRASIL COM O APN timbrasil.br
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // STABELECE O LINK PDP - PACKEGE DATA PROTOCOL
  A6command("AT+CGACT=1,1", "OK", "yy", 10000, 2);                          // ATIVA O CONTEXTO DO PDP (PACKET DATA PROTOCOL)
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // VERIFICA NOVAMENTE O STATUS DA CONEXAO
  A6command("AT+CIFSR", "OK", "yy", 20000, 2);                              // SOLICITA O IP LOCAL
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // VERIFICA O STATUS DA REDE NOVAMENTE
  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",80", "CONNECT OK", "yy", 25000, 2);   // INICIA A CONEXAO DO TIPO TCP, NA PORTA 80, E ESPERA A RESPOSTA "CONNECT OK"
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // SOLICITA O STATUS DA REDE NOVAMENTE
  A6command("AT+CIPSEND", ">", "yy", 10000, 1);                             // ABRE A PORTA PARA ENVIAR OS DADOS ATRAVEZ DE UMA CONEXAO TCP

  delay(500);

  // Formata a URL, no realizando uma requisição GET, do protocolo HTTP (Tamanho máximo de 255 caracters)
  A6board.print("GET /salvar.php");       // Envia uma solicitação do tipo GET no protocolo HTTP para a página salvar.php
  A6board.print("?hum=");                 // Formata a URL, parametro hum
  A6board.print(hum);                     // Carrega o valor da umidade
  A6board.print("&temp=");                // Formata a URL, parametro temp
  A6board.print(temp);                    // Carrega o valor da temperatura
  A6board.print("&volt=");                // Formata a URL, parametro volt
  A6board.print(volt);                    // Carrega o valor da tensão da bateria de alimentação do GPRS
  A6board.print("&pot=");                 // Formata a URL, parametro pot
  A6board.print(pot);                     // Carrega o valor da potencia do sinal do rádio do GPRS
  A6board.print(" HTTP/1.1");             // Protocolo utilizado para transferencia dos dados
  A6board.print("\r\n");                  // Carrier return e new line
  A6board.print("HOST: ");                // Formatação da url, parametro HOST
  A6board.print(host);                    // Carrega a string com o endereço do HOST
  A6board.print("\r\n");                  // Carrier Return e new line
  A6board.print("\r\n");                  // Carrier Return e new line novamente. Necessário para o HTTP

#ifdef DEBUG
  Serial.print("GET /salvar.php");
  Serial.print("?hum=");
  Serial.print(hum);
  Serial.print("&temp=");
  Serial.print(temp);
  Serial.print("&volt=");
  Serial.print(volt);
  Serial.print("&pot=");
  Serial.print(pot);
  Serial.print(" HTTP/1.1");
  Serial.print("\r\n");
  Serial.print("HOST: ");
  Serial.print(host);
  Serial.print("\r\n");
  Serial.print("\r\n");
#endif

  A6command(end_c, "HTTP/1.1", "yy", 30000, 1);
  unsigned long   entry = millis();
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // SOLICITA O STATUS DA CONEXAO NOVAMENTE
  A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1);                           // FECHA A CONEXAO TCP IP
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);                          // SOLICITA O STATUS DA CONEXAO NOVAMENTE
  delay(100);
#ifdef DEBUG
  Serial.println("-------------------------FIM------------------------------");
#endif
}

//--------------------------------------------------------------------------------------
// Gera um reset no módlo GPRS
//--------------------------------------------------------------------------------------
void reset() {
#ifdef DEBUG
  Serial.println("Start");
#endif
  digitalWrite(RST, HIGH);
  delay(5000);
  digitalWrite(RST, LOW);
  delay(500);

  if (A6begin() != OK) {
#ifdef DEBUB
    Serial.println("Erro!");
    Serial.println("Reiniciando novamente...");
#endif
    reset();
  }
}

//--------------------------------------------------------------------------------------
// Função para capturar as respostas esperadas pelo modem
//--------------------------------------------------------------------------------------
byte A6waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();     // Armazena o valor da variável millis para saber quanto tempo estará dentro da função, relizar o timout
  int count = 0;                      // Variável para o contador
  String reply = A6read();            // Realiza a leitura da resposta do modem
  byte retVal = 99;                   // Variável para o retorno
  do {
    reply = A6read();                 // Realiza uma primeira leitura
#ifdef DEBUG
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
#endif
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut ); // Tenta localizar as strings de resposta utilizando indexOf()
  if ((millis() - entry) >= timeOut) {   // Confere se o tempo foi excedido
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;  // Tenta localizar novamente se as strings passadas como parametro estão presentes
    else retVal = NOTOK;
  }
  return retVal;
}

//------------------------------------------------------------------------------------------------
// Função para enviar os comandos AT para o modem e aguarda as respostas passadas como parametro
//------------------------------------------------------------------------------------------------
byte A6command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) {
    A6board.println(command);
#ifdef DEBUG
    Serial.print("Command: ");
    Serial.println(command);
#endif
    if ( A6waitFor(response1, response2, timeOut) == OK ) {
      returnValue = OK;
    } else {
      returnValue = NOTOK;
    }
    count++;
  }
  return returnValue;
}

//--------------------------------------------------------------------------------------
// Função que retorna uma String de resposta do modem
//--------------------------------------------------------------------------------------
String A6read() {
  String reply = "";
  if (A6board.available())  {         // Se algum dado estiver disponível na serial, 
    reply = A6board.readString();     // Realiza a leitura do dado disponível na serial
  }
  return reply;                       // Retorna a String de resposta
}

//--------------------------------------------------------------------------------------
// Função que solicita o valor da potência do sinal de recepção do modem
//--------------------------------------------------------------------------------------
int getSignalStrength() {
  
  String modemResponse = returnStringFromModemComand("AT+CSQ", 1000);   // Envia o comando solicitando a qualidade do sinal
  char res_to_split[modemResponse.length()];                            // Variável que carrega o valor do tamanho da resposta do modem, para formar um buffer
  modemResponse.toCharArray(res_to_split, modemResponse.length());      // Copia os caracteres da String dentro do buffer "res_to_split" do tamanho "modemResponse.length()"

  // strstr() -> Função que lê a string e agaurda a ocorrencia da string entre ""
  if ((strstr(res_to_split, "ERROR") == NULL) | (strstr(res_to_split, "99") == NULL)) { // Confere se algumas das Strings enrtre " " está presente 
  
    char *ptr_token;                                    // Cria um apontador (ponteiro) do tipo Char
    ptr_token = strtok(res_to_split, ":");              // Essa função retorna o endereço do primeiro delimitador ("Token" :) passado pelo segundo parametro, da string "res_to_split"
    ptr_token = strtok(NULL, ":");                      // Passamos o valor NULL para a função strtok() para que ela continue trabalhando com a string anterior
    ptr_token = strtok(ptr_token, ",");                 // Essa função retorna o endereço do primeiro delimitador ("Token" ,) passado pelo segundo parametro
    String final_result = ptr_token;                    // Armazena em "final_result" o conteudo apontado por "ptr_token"
    final_result.trim();                                // Elimina qualquer "espaço" em branco dentro da string
    int intResult = final_result.toInt();               // Realiza um Type Cast de String para int, com o valor da potencia do sinal
    return intResult;
  } else {
    return (NULL);
  }
}

//--------------------------------------------------------------------------------------
// Função que retorna uma string em função do comando enviado
//--------------------------------------------------------------------------------------
String returnStringFromModemComand(String ATcommand, int responseDelay) {
  
  A6board.println(ATcommand);           // Envia o comando AT 
  delay(responseDelay);                 // Aguarda o tempo passado como parametro
  String result = "";                   // Cria uma String para armazenar o resultado
  while (A6board.available()) {         // Enquanto houver algum dado disponivel na serial
    char c = A6board.read();            // Armazena os dados lidos
    result += c;                        // Monta uma String com os dados de resposta
  }
  return result;                        // Retorna a resposta do modem.
}
