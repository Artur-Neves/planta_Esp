#include <OneWire.h>  
#include <DallasTemperature.h>
#define p100 2
const int LDR = A0;
OneWire oneWire(p100); 
DallasTemperature sensors(&oneWire); 
String LDR_AND_P100;

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);

  pinMode(LDR, INPUT);
}

   
    
    // Use strtok para dividir a string com base no delimitador "-"
    

void loop() { 
  delay(500);
  int valorAnalogico_LDR = analogRead(LDR);
  float luminosidade = map(valorAnalogico_LDR, 0, 1023, 0, 100);
  sensors.requestTemperatures();
   LDR_AND_P100 = String(luminosidade)+"-"+String(sensors.getTempCByIndex(0));
  Serial.println(LDR_AND_P100);
if (luminosidade>85) {
   digitalWrite(7, LOW);// turn relay ON
}
else{
  digitalWrite(7, HIGH);// turn relay OFF
 delay(1000);// keep it OFF for 5 seconds
 }
}
