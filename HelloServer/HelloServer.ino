#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
#include <Arduino.h>
#ifndef STASSID
#define DHT_PIN 2       // Defina o pino ao qual o sensor KY-015 está conectado
#define DHT_TYPE DHT11  // Dependendo do modelo do sensor, pode ser DHT11, DHT21 ou DHT22

#endif


DHT dht(DHT_PIN, DHT_TYPE);
const int pinSensorUmidadeSolo = A0;
const char* ssid = "iPhone de Daniel";
const char* password = "20212022@";
char *token;

ESP8266WebServer server(80);

const int led = 13;
const int capacitor= A0;
 
   int temperature = dht.readTemperature();  // Lê a temperatura em graus Celsius
  float umidade = dht.readHumidity();
   int valorAnalogico = analogRead(pinSensorUmidadeSolo);
    float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);


void handleRoot() {
  int temperature = dht.readTemperature();  // Lê a temperatura em graus Celsius
  float umidade = dht.readHumidity();
   int valorAnalogico = analogRead(pinSensorUmidadeSolo);
    float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);
  String umidadedsolo = ""+String(100-umidadeSolo)+"%";
String texto = "Temperatura: "+ String(temperature)+ "C\nUmidade: "+String(umidade)+"\nUmidade do solo: "+ umidadedsolo+ "\niluminosidade "+iluminosidade()+"\ntemperatura do solo "+p100();
  if (isnan(temperature)) {
    Serial.println("Falha ao ler a temperatura!");
  } else {
    
    Serial.println("lendo");
    digitalWrite(led, 1);
    server.send(200, "text/plain", texto);

    
    digitalWrite(led, 0);
    server.sendHeader("Access-Control-Allow-Origin", "*");
  }
}


void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(15, OUTPUT);
  dht.begin();
  pinMode(capacitor, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
      server.sendHeader("Access-Control-Allow-Origin", "*");
  });

  server.on("/gif", []() {
    static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
      server.sendHeader("Access-Control-Allow-Origin", "*");
  });

  server.onNotFound(handleNotFound);

  /////////////////////////////////////////////////////////
  // Hook examples

  server.addHook([](const String& method, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction contentType) {
    (void)method;       // GET, PUT, ...
    (void)url;          // example: /root/myfile.html
    (void)client;       // the webserver tcp client connection
    (void)contentType;  // contentType(".html") => "text/html"
    Serial.printf("A useless web hook has passed\n");
    Serial.printf("(this hook is in 0x%08x area (401x=IRAM 402x=FLASH))\n", esp_get_program_counter());
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient*, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/fail")) {
      Serial.printf("An always failing web hook has been triggered\n");
      return ESP8266WebServer::CLIENT_MUST_STOP;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/dump")) {
      Serial.printf("The dumper web hook is on the run\n");

      // Here the request is not interpreted, so we cannot for sure
      // swallow the exact amount matching the full request+content,
      // hence the tcp connection cannot be handled anymore by the
      // webserver.
#ifdef STREAMSEND_API
      // we are lucky
      client->sendAll(Serial, 500);
#else
      auto last = millis();
      while ((millis() - last) < 500) {
        char buf[32];
        size_t len = client->read((uint8_t*)buf, sizeof(buf));
        if (len > 0) {
          Serial.printf("(<%d> chars)", (int)len);
          Serial.write(buf, len);
          last = millis();
        }
      }
#endif
      // Two choices: return MUST STOP and webserver will close it
      //                       (we already have the example with '/fail' hook)
      // or                  IS GIVEN and webserver will forget it
      // trying with IS GIVEN and storing it on a dumb WiFiClient.
      // check the client connection: it should not immediately be closed
      // (make another '/dump' one to close the first)
      Serial.printf("\nTelling server to forget this connection\n");
      static WiFiClient forgetme = *client;  // stop previous one if present and transfer client refcounter
      return ESP8266WebServer::CLIENT_IS_GIVEN;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  // Hook examples
  /////////////////////////////////////////////////////////

  server.begin();
  Serial.println("HTTP server started");
    
}

void plantaIdeal(){
  int valorAnalogico = analogRead(pinSensorUmidadeSolo);
   float temp = map(valorAnalogico, 0, 1023, 0, 100);
  if (temp<40) {
digitalWrite(15, HIGH);

  }
  else {
digitalWrite(15, LOW);


  }
 
  
  
  delay(1000);
}
String input; // Variável global para armazenar a leitura da porta serial

String readSerialLine() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim(); // Remove espaços em branco extras do início e do fim
    return line;
  }
  return ""; // Retorna uma string vazia se nada estiver disponível
}

int iluminosidade() {
  if (input.length() > 0) {
    // Usa indexOf para encontrar a posição do "-"
    int pos = input.indexOf('-');

    if (pos != -1) {
      // Obtém a parte da string antes do "-"
      String parte1 = input.substring(0, pos);

      // Converte a parte antes do "-" em um número inteiro
      int numeroRecebido = parte1.toInt();

      // Agora você pode usar o número inteiro conforme necessário
      return numeroRecebido;
    }
  }
  return 0;
}

float p100() {
  if (input.length() > 0) {
    // Usa indexOf para encontrar a posição do "-"
    int pos = input.indexOf('-');

    if (pos != -1 && pos < input.length() - 1) {
      // Obtém a parte da string após o "-"
      String parte2 = input.substring(pos + 1);

      // Converte a parte após o "-" em um número de ponto flutuante (float)
      float valor = parte2.toFloat();

      // Agora você pode usar o valor de ponto flutuante conforme necessário
      return valor;
    }
  }
  return 0.0; // Valor padrão em caso de erro
}



void loop(void) {
  plantaIdeal();
  server.handleClient();
  MDNS.update();
  
  // Leia a linha da porta serial e armazene na variável input
  input = readSerialLine();
  
  // Chame os métodos iluminosidade e p100 para processar a mesma linha
  int valorIluminosidade = iluminosidade();
  float valorP100 = p100();
  
  // Use os valores conforme necessário
}
