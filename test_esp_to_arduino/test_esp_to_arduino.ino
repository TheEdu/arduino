#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char incomingByte = 0;
String ip_address = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  //while (true) if ( Serial.available() ) {  lcd.write( Serial.read() );  }
  
  Serial.print("get_ip");
}

void loop() {
  // send data only when you receive data:
  
  while (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    ip_address.concat(incomingByte);
  }

  
  if(ip_address.indexOf(":response") != -1){
    lcd.clear();
    lcd.print(ip_address.substring(0,ip_address.indexOf(":response")));
    ip_address = "";
  }

}
