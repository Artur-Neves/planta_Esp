#include <NewPing.h>
#define TRIGGER_PIN  A1  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A0  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int red =2;
int yellow =3;
NewPing distancia (TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
void setup() {
 pinMode(red, OUTPUT);
 pinMode(yellow, OUTPUT);
 Serial.begin(9600);
}

void loop() {
  delay(500);
  Serial.print("Distância: ");
  Serial.print(distancia.ping_cm());
  Serial.println("cm");
 acender(distancia.ping_cm());
}

void acender(int distancia){
  if (distancia>10 && distancia<20)
{
  digitalWrite(yellow, HIGH);
   digitalWrite(red,LOW);
}   else if  (distancia<10){
  digitalWrite (red, HIGH);
   digitalWrite(yellow, LOW);
}
else {
  digitalWrite(yellow, LOW);
  digitalWrite(red,LOW);
}

}
