#include <OneWire.h>  
#include <DallasTemperature.h>
#define p100 2
const int LDR = A0;
OneWire oneWire(p100); 
DallasTemperature sensors(&oneWire); 
String LDR_AND_P100_Volume;
const int INTERRUPCAO_SENSOR = 1; //interrupt = 0 equivale ao pino digital 2
const int PINO_SENSOR = 4;
//definicao da variavel de contagem de voltas
unsigned long contador = 0;

//definicao do fator de calibracao para conversao do valor lido
const float FATOR_CALIBRACAO = 4.5;

//definicao das variaveis de fluxo e volume
float fluxo = 0;
float volume = 0;
float volume_total = 0;

//definicao da variavel de intervalo de tempo
unsigned long tempo_antes = 0;

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  pinMode(PINO_SENSOR, INPUT_PULLUP);
  pinMode(LDR, INPUT);
}

   
    
    // Use strtok para dividir a string com base no delimitador "-"
    

void loop() { 
  delay(80);
  int valorAnalogico_LDR = analogRead(LDR);
  float luminosidade = map(valorAnalogico_LDR, 0, 1023, 0, 100);
  sensors.requestTemperatures();
   LDR_AND_P100_Volume = String(luminosidade)+"-"+String(sensors.getTempCByIndex(0))+"-"+String(volume_agua());
  Serial.println(LDR_AND_P100_Volume);
if (luminosidade>85) {
   digitalWrite(7, LOW);// turn relay ON
}
else{
  digitalWrite(7, HIGH);// turn relay OFF
 delay(1000);// keep it OFF for 5 seconds
 }
}

float volume_agua(){
//executa a contagem de pulsos uma vez por segundo


    //desabilita a interrupcao para realizar a conversao do valor de pulsos
    detachInterrupt(INTERRUPCAO_SENSOR);

    //conversao do valor de pulsos para L/min
    fluxo = ((1000.0 / (millis() - tempo_antes)) * contador) / FATOR_CALIBRACAO;


    //calculo do volume em L passado pelo sensor
    volume = fluxo / 60;

    //armazenamento do volume
    volume_total += volume;

    //exibicao do valor de volume
    Serial.print("Volume: ");
    Serial.print(volume_total);
    Serial.println(" L");
    Serial.println();
   
    //reinicializacao do contador de pulsos
    contador = 0;

    //atualizacao da variavel tempo_antes
    tempo_antes = millis();

    //contagem de pulsos do sensor
    attachInterrupt(INTERRUPCAO_SENSOR, contador_pulso, FALLING);
    return volume_total;
    
  
}
void contador_pulso() {
  
  contador++;
  
}
