  bool ativado = false;
void setup() {   
  pinMode(2, OUTPUT);
  pinMode (16, OUTPUT);
  pinMode (4, OUTPUT);
  digitalWrite(2, HIGH);

 // put your setup code here, to run once:

}

void loop() {
  semaforo();

  
  }




int bt = 0;
void botao(){
  bt = digitalRead(11); 
  if ( bt == HIGH){
     
     ativado= !ativado;
  Serial.println(bt) ;
    

}
  
  
  }

void semaforo(){
unsigned long Tatual = 0;
 
  if (digitalRead(2) == HIGH){
     delay(500);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(16, HIGH);
      
    
  }
  
   else if (digitalRead(16) == HIGH){
  delay(500);
    digitalWrite(16, LOW);
    digitalWrite(2, LOW);
    digitalWrite(4, HIGH);
  }
  
  else if (digitalRead(4) == HIGH){
  delay(500);
    digitalWrite(16, LOW);
    digitalWrite(4, LOW);
    digitalWrite(2, HIGH);
  }


}
