
#include <TimerOne.h>

byte timeOver = 0;
char hourTen = '1';
char hourUni = '0';
char minTen  = '0';
char minUni  = '1';
char secTen  = '1';
char secUni  = '2';


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(plusSecond);
  
  Serial.print(hourTen);
  Serial.print(hourUni);
  Serial.print(":");
  Serial.print(minTen);
  Serial.print(minUni);
  Serial.print(":");
  Serial.print(secTen);
  Serial.println(secUni);

 
}

void loop() {
  // put your main code here, to run repeatedly: 

}

void plusSecond(){
  asm (
  "INICIO: \n"
  "cpi %1,'0' \n"
  "brne SUM1  \n"
  "cpi %2,'0' \n"
  "brne STM1  \n"
  "cpi %3,'0' \n"
  "brne MUM1  \n"
  "cpi %4,'0' \n"
  "brne MTM1  \n"
  "cpi %5,'0' \n"
  "brne HUM1  \n"
  "cpi %6,'0' \n"
  "brne HTM1  \n"
  "ldi %0,1   \n"
  "rjmp END   \n"
  "SUM1:      \n"
    "DEC %1   \n"
    "RJMP END \n"
  "STM1: \n"
    "DEC %2        \n"
    "SUR: ldi %1, '9' \n"
    "RJMP END         \n"
  "MUM1: \n"
    "DEC %3        \n"
    "STR: ldi %2, '5' \n"
    "RJMP SUR         \n"
  "MTM1: \n"
    "DEC %4        \n"
    "MUR: ldi %3, '9' \n"
    "RJMP STR         \n"
  "HUM1: \n"
    "DEC %5        \n"
    "MTR: ldi %4, '5' \n"
    "RJMP MUR         \n"
  "HTM1: \n"
    "DEC %6           \n"
    "ldi %5, '9'      \n"
    "RJMP MTR         \n"
  "END:               \n"
  : "+r" (timeOver) , "+r" (secUni)  , "+r" (secTen), "+r" (minUni)  , "+r" (minTen), "+r" (hourUni)  , "+r" (hourTen));
  
  Serial.print(hourTen);
  Serial.print(hourUni);
  Serial.print(":");
  Serial.print(minTen);
  Serial.print(minUni);
  Serial.print(":");
  Serial.print(secTen);
  Serial.println(secUni);
}
