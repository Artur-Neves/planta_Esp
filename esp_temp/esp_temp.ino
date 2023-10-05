
#include <DHT.h>
#include <Arduino.h>

#define DHT_PIN 2       // Defina o pino ao qual o sensor KY-015 está conectado
#define DHT_TYPE DHT11  // Dependendo do modelo do sensor, pode ser DHT11, DHT21 ou DHT22

DHT dht(DHT_PIN, DHT_TYPE);

const int pinSensorUmidadeSolo = A0;

void setup() {
  pinMode(15, OUTPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  digitalWrite(15, HIGH);
  float temperature = dht.readTemperature();  // Lê a temperatura em graus Celsius
  float umidade = dht.readHumidity(); // Lê a umidade 

  if (isnan(temperature)) {
    Serial.println("Falha ao ler a Temperatura e a Umidade!");
  } else {
    Serial.print("Temperatura do ambiente: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Umidade do ambiente:");
    Serial.print(umidade);
    Serial.println("%");
  }

  // Realiza a leitura do valor analógico do sensor
  int valorAnalogico = analogRead(pinSensorUmidadeSolo);

  // Mapeia o valor lido para a porcentagem de umidade do solo (0 a 100%)
  float umidadeSolo = map(valorAnalogico, 0, 1023, 0, 100);

  // Exibe o valor da umidade do solo no monitor serial
  Serial.print("Umidade do solo: ");
  Serial.print(100 - umidadeSolo);
  Serial.println("%");

  delay(3000);  // Aguarda 2 segundos antes de ler a temperatura novamente
}