void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT);
  pinMode(11, INPUT);  
  pinMode(12, INPUT);  

}

void loop() {
  Serial.print("pin10: ");
  if(digitalRead(10) == 1){
    Serial.println("Hola Mundo!");
  }
  Serial.print("pin11: ");
  Serial.println(digitalRead(11));
  Serial.print("pin12: ");
  Serial.println(digitalRead(12));
  delay(1000);
}
