#include <Keypad.h>

//Begin Keypad definition
const byte numRows= 4;
const byte numCols= 4;
char keymap[numRows][numCols]= {  {'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '#', 'D'}  };
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols); //initializes an instance of the Keypad class
char keypressed;
//End Keypad definition

//Begin Temporizer definition
const byte tempoInputs = 6;
byte tempoInputsCounter = 0;
char hourTen;
char hourUni;
char minTen;
char minUni;
char secTen;
char secUni;
//End Temporizer definition

//Begin Defuse Code definition
const byte defuseCodeLength = 4;
byte defuseCodeLengthCounter = 0;
String defuseCode = "";
String testCode = "";
//End Defuse Code definition



void setup() {
  Serial.begin(9600);
  Serial.print("Enter the Defuse_Code and press (A) to save the code or (B) to delete the most recent character:");
  while (defuseCodeLengthCounter < (defuseCodeLength + 1)){
     //while(Serial.available() == 0); //Dont work. I dont know why
     keypressed = myKeypad.getKey();
     if (keypressed != NO_KEY){
        if(isDigit(keypressed) && defuseCodeLengthCounter < defuseCodeLength){
         asm("mov r0,%0 \n\t"  // copy a to r0
             "inc r0    \n\t"  // increment   
             "mov %0,r0 \n\t"  // copy r0 back to a
             :  "+r" (defuseCodeLengthCounter));
          //defuseCodeLengthCounter = defuseCodeLengthCounter + 1;
          defuseCode = defuseCode + keypressed;
          Serial.print(keypressed);
        } else if ((keypressed == 'B')&&(defuseCodeLengthCounter > 0)){
          defuseCodeLengthCounter = defuseCodeLengthCounter - 1;
          defuseCode = defuseCode.substring(0,defuseCodeLengthCounter);
          Serial.print("\ncode: " + defuseCode);
        } else if ((keypressed == 'A')&&(defuseCodeLengthCounter == defuseCodeLength)){
          defuseCodeLengthCounter = defuseCodeLengthCounter + 1;
        }
     }
  }
  Serial.print("\nThe Defuse Code is " + defuseCode + "\n");
}

void loop() {
  keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY){
    if(isDigit(keypressed)){
      testCode = testCode + keypressed;
      Serial.print(keypressed);
      if (testCode.equals(defuseCode)){
        Serial.println("\nOK");
        testCode = "";
      } else if (testCode.length() == defuseCodeLength){
        Serial.println("\nERROR");
        testCode = "";
      }
    } else if ((keypressed == 'B')&&(testCode.length() > 0)){
        testCode = testCode.substring(0,testCode.length()-1);
        Serial.print("\ncode: " + testCode);
    }
  }
}
