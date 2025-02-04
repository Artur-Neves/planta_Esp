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
const char* ssid = "Artur";
const char* password = "12345678";
char *token;

ESP8266WebServer server(80);

const int led = 13;
const int capacitor= A0;
 
   int temperature = dht.readTemperature();  // Lê a temperatura em graus Celsius
  float umidade = dht.readHumidity();
   int valorAnalogico = analogRead(pinSensorUmidadeSolo);
    float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);
int umidadeParaRegar;

void handleRoot() {
  int temperature = dht.readTemperature();  // Lê a temperatura em graus Celsius
  float umidade = dht.readHumidity();
   int valorAnalogico = analogRead(pinSensorUmidadeSolo);
    float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);
  String umidadedsolo = ""+String(100-umidadeSolo);
  
String texto =  String(temperature)+ "-"+String(umidade)+"-"+ umidadedsolo+ "-"+iluminosidade()+"-"+p100()+"-"+String(Volume_Agua());
  if (isnan(temperature)) {
    Serial.println("Falha ao ler a temperatura!");
  } else {
    
    //Serial.println("lendo");
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
  temperature = dht.readTemperature();  
  int valorAnalogico = analogRead(pinSensorUmidadeSolo);
   float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);
   // 60 umidade quando a temperatura for 
   // abaixo de 60 independente da temperatura
   // apartir de uma temperatura que seria mais ou menos 40 graus ele rega ate a planta tiver usn 75% de umidade 
   // apartir de 35 graus ambiente vai aumentar a temperatura da planta ate uns 75
   //
   int temperature = dht.readTemperature(); 


  umidadeParaRegar=40;

if(temperature>35) {
  umidadeParaRegar=25;
}

else if(p100().toInt()!= 0 && p100().toInt()>40){
  
umidadeParaRegar=  25;

}
<<<<<<< HEAD
if (umidadeSolo>umidadeParaRegar) {
Serial.println("ligado");
  }
  else if((umidadeSolo+5)<umidadeParaRegar) {
=======
if (umidadeSolo<umidadeParaRegar) {
Serial.println("ligado");
  }
  else if((umidadeSolo-5)>umidadeParaRegar) {
>>>>>>> 07f91f0ba96d81e3172f3dabcc3baeea83abb03a
Serial.println("desligado");
  }  delay(1000);}
String input; // Variável global para armazenar a leitura da porta serial

String iluminosidade() {
  int pos1 = input.indexOf('-');
  if (pos1 != -1) {
    return input.substring(0, pos1);
  }
  return "";
}

String p100() {
  int pos1 = input.indexOf('-');
  int pos2 = input.indexOf('-', pos1 + 1);
  if (pos2 != -1) {
    return input.substring(pos1 + 1, pos2);
  }
  return "";
}

String Volume_Agua() {
  int pos2 = input.indexOf('-');
  int pos3 = input.lastIndexOf('-');
  if (pos3 != -1) {
    return input.substring( pos3+1);
  }
  return "";
}
String readSerialLine() {
  String line = "";

  if (Serial.available()) {
    line = Serial.readStringUntil('\n');
    line.trim(); // Remove espaços em branco extras do início e do fim
  }
  return line;
}

void loop(void) {
  delay(500);
  plantaIdeal();
  MDNS.update();
  
  // Leia a linha da porta serial e armazene na variável input
  input = readSerialLine();
  
  // Chame os métodos iluminosidade, p100 e Volume_Agua uma vez para processar a mesma linha de entrada
  String valorIluminosidade = iluminosidade();
  String valorP100 = p100();
  String valorVolumeAgua = Volume_Agua();

  // Depuração: Verifique os valores retornados
  //Serial.println("Iluminosidade: " + valorIluminosidade);
  //Serial.println("P100: " + valorP100);
  //Serial.println("Volume_Agua: " + valorVolumeAgua);
  
  // Use os valores conforme necessário
  if (valorP100 != "") {
    server.handleClient();
  }
}
