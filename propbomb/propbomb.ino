#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <pitches.h>

//Begin Keypad definition
const byte numRows= 4;
const byte numCols= 4;
char keymap[numRows][numCols]= {  {'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '#', 'D'}  };
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols); //initializes an instance of the Keypad class
char key;
//End Keypad definition

//Begin LCD-i2c definition
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte arrowu[8] = {
  0b00000100,
  0b00001110,
  0b00010101,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000000,
};
 
byte arrowd[8] = {
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00010101,
  0b00001110,
  0b00000100,
  0b00000000,
};

byte arrowl[8] = {
  0b00000010,
  0b00000100,
  0b00001000,
  0b00011111,
  0b00001000,
  0b00000100,
  0b00000010,
  0b00000000,
};

byte arrowr[8] = {
  0b00001000,
  0b00000100,
  0b00000010,
  0b00011111,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00000000,
};

byte enter[8] = {
  0b00000001,
  0b00000001,
  0b00000001,
  0b00000101,
  0b00001001,
  0b00011111,
  0b00001000,
  0b00000100,
};

byte empty[8] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
};

byte bomb[8] = {
  0b00000010,
  0b00000100,
  0b00000100,
  0b00001110,
  0b00011111,
  0b00011111,
  0b00011111,
  0b00001110,
};
//End LCD-i2c definition

//Begin Game Mode definition
#define MODES_SIZE 3
byte gameMode = 0;
bool mode_setted = false;
String gameModes[MODES_SIZE]={"Defuse Code", "Defuse Wire", "Defuse Remote"};
//End Game Mode definition

//Begin Temporizer definition
#define TIMER_SIZE 6
byte timerPosition = 0;
bool timer_setted = false;
byte timerChange;
byte timeOver = 0;
long timeDelay = 1000000; // 1 second
char hourTen;
char hourUni;
char minTen;
char minUni;
char secTen;
char secUni;
//End Temporizer definition

//Begin Defuse Code definition
#define DEFUSE_CODE_SIZE 4
byte defuseCodePosition = 0;
bool defuseCode_setted = false;
String defuseCode = "";
//End Defuse Code definition

//Begin Wire Order definition
#define WIRE_ORDER_SIZE 3
byte wireOrderPosition = 0;
bool wireOrder_setted = false;
char wireOrder[WIRE_ORDER_SIZE];
char lastInput;
int wire1;
int wire2;
int wire3;
//End Wire Order definition

//Begin Game Variables
bool gameActive = true;
byte inputCodePosition = 0;
String inputCode = "";
byte asserts = 0;
//End Game Variables

void setup() {
  //Initialize LCD
  lcd.begin(16, 2);
  //Initialize lcd special characters
  lcd.createChar (1, arrowu);
  lcd.createChar (2, arrowd);
  lcd.createChar (3, arrowl);
  lcd.createChar (4, arrowr);
  lcd.createChar (5, enter);
  lcd.createChar (6, empty);
  lcd.createChar (7, bomb);

  //Game WELCOME
  showWelcomeMessage();

  //Game MODE Configuration
  lcd.clear();
  lcd.home ();
  lcd.print("GAME MODE");
  //Show First Game Option
  gameMode = showGameMode(0);
  
  while(!mode_setted){
    key = myKeypad.getKey();
    while (key == NO_KEY) key = myKeypad.getKey();
  
    switch (key) {
    case 'A':
      if(gameMode > 0){
        gameMode = gameMode - 1;
        gameMode = showGameMode(gameMode);
      };
      break;
    case 'B':
      if(gameMode < MODES_SIZE -1){
        gameMode = gameMode + 1;
        gameMode = showGameMode(gameMode);
      };
      break;
    case 'C':
      mode_setted = true;
      break;
    default:
      break;
    };
  };

  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("Mode selected");
  delay(750);
  lcd.setCursor(2,1);
  lcd.print(gameModes[gameMode]);
  delay(1500);
  lcd.clear();

  switch (gameMode) {
  case 0:
    timerSetUp();           //  Timer Configuration
    codeSetUp();            //  Defuse Code Configuration
    showGameStartMessage(); //  Game Start Message
  
    showTimer(4,0);
    lcd.setCursor(6,1);
    lcd.print("____");

    Timer1.initialize(timeDelay);
    Timer1.attachInterrupt(minusSecond);
    
    break;
  case 1:
    timerSetUp();           //Timer Configuration
    wireSetUp();            // Wire Order Set Up
    showGameStartMessage(); //  Game Start Message
    
    showTimer(4,0);
    lcd.setCursor(2,1);
    lcd.print("Wire Defuse");

    Timer1.initialize(timeDelay);
    Timer1.attachInterrupt(minusSecond);
    break;
  case 2:
    timerSetUp(); //Timer Configuration
    codeSetUp();  //Defuse Code Configuration
    showGameStartMessage(); //  Game Start Message
  
    showTimer(4,0);
    lcd.setCursor(2,1);
    lcd.print("Remote Defuse");

    Timer1.initialize(timeDelay);
    Timer1.attachInterrupt(minusSecond);
    break;
  };
}

void loop() {
  
  if(gameActive){
    switch(gameMode){
      case 0:
        if(timerChange == 1){
          showTimer(4,0);
          timerChange = 0;
        }
        
        if(timeOver == 1){
          Timer1.detachInterrupt();
          gameActive = false;
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("The Bomb has");
          lcd.setCursor(4,1);
          lcd.print("Exploded");
        }
        
        key = myKeypad.getKey();
        if(key != NO_KEY && gameActive){
          switch(key){
            case 'D':
              switch(inputCodePosition){
                case 0:
                  break;
                case 1:
                  lcd.setCursor(6,1);
                  lcd.print('_');
                  inputCodePosition = inputCodePosition - 1;
                  inputCode = inputCode.substring(0,inputCodePosition);
                  break;
                case 2:
                  lcd.setCursor(7,1);
                  lcd.print('_');
                  inputCodePosition = inputCodePosition - 1;
                  inputCode = inputCode.substring(0,inputCodePosition);
                  break;
                case 3:
                  lcd.setCursor(8,1);
                  lcd.print('_');
                  inputCodePosition = inputCodePosition - 1;
                  inputCode = inputCode.substring(0,inputCodePosition);
                  break;
                case 4:
                  lcd.setCursor(9,1);
                  lcd.print('_');
                  inputCodePosition = inputCodePosition - 1;
                  inputCode = inputCode.substring(0,inputCodePosition);
                  break;
              }
              break;
            case 'C':
              if (inputCodePosition == DEFUSE_CODE_SIZE) {
                if (inputCode.equals(defuseCode)){
                  gameActive = false;
                  Timer1.detachInterrupt();
                  lcd.clear();
                  lcd.setCursor(2,0);
                  lcd.print("Bomb has been");
                  lcd.setCursor(4,1);
                  lcd.print("Defused");
                } else {
                  timeDelay = timeDelay - timeDelay*20/100;
                  Timer1.setPeriod(timeDelay);
                  asserts = 0;
                  for (int i=0; i < DEFUSE_CODE_SIZE; i++){
                    if(inputCode[i] == defuseCode[i]){
                      asserts = asserts + 1;
                    }
                  }
                  inputCode = "";
                  inputCodePosition = 0;
                  lcd.setCursor(6,1);
                  lcd.print("____");
                  delay(250);
                  clearRow(1);
                  delay(250);
                  lcd.setCursor(6,1);
                  lcd.print("____");
                  lcd.setCursor(12,1);    
                  lcd.print(asserts);
                  lcd.print(" OK");
                };
              };
              break;
            default:
              if(isDigit(key) && inputCodePosition < DEFUSE_CODE_SIZE){
                inputCodePosition = inputCodePosition + 1;
                inputCode = inputCode + key;
                lcd.setCursor(inputCodePosition+5,1);
                lcd.print(key);
              }
              break;
          }
        }
        break;
      case 1:
        if(timerChange == 1){
          showTimer(4,0);
          timerChange = 0;
        }
        
        if(timeOver == 1){
          Timer1.detachInterrupt();
          gameActive = false;
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("The Bomb has");
          lcd.setCursor(4,1);
          lcd.print("Exploded");
        }

        if(((digitalRead(wire3)==0) && ((digitalRead(wire1)==1) || (digitalRead(wire2)==1))) || ((digitalRead(wire2)==0) && (digitalRead(wire1)==1))){
          Timer1.detachInterrupt();
          gameActive = false;
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("The Bomb has");
          lcd.setCursor(4,1);
          lcd.print("Exploded");
        }

        if((digitalRead(wire1)==0) && (digitalRead(wire2)==0) && (digitalRead(wire3)==0)){
          gameActive = false;
          Timer1.detachInterrupt();
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("Bomb has been");
          lcd.setCursor(4,1);
          lcd.print("Defused");
        }
        
        break;
      case 2:
        break;
      default:
        break;
    }
  }
}

void showWelcomeMessage(){
  lcd.home ();
  lcd.setCursor ( 2, 0);
  lcd.print("Bomb Game ");
  lcd.write((byte)7);
  lcd.write((byte)7);
  lcd.write((byte)7);
  lcd.setCursor ( 5, 1 );
  lcd.print("By Edu");
  delay (1500);
  for ( int i = 0; i < 15; i++ ) {
    lcd.scrollDisplayRight();
    delay (350);
  }
}

byte showGameMode(int modeIndex){
  clearRow(1);
  lcd.setCursor ( 0, 1 );
  lcd.write((byte)4); // right arrow
  lcd.print(gameModes[modeIndex]); // game option
  lcd.setCursor ( 14, 1 );
  /*
  lcd.write((byte)1); // up arrow
  lcd.write((byte)2); // down arrow
  */
  return modeIndex;
}

void clearRow(int rowNumber){
  lcd.setCursor(0,rowNumber);
  for ( int i = 0; i < 16; i++ ) {
    lcd.write((byte)6);
  }
}


void showTimer(byte x,byte y){
    lcd.setCursor(x,y);
    lcd.print(hourTen);
    lcd.print(hourUni);
    lcd.print(":");
    lcd.print(minTen);
    lcd.print(minUni);
    lcd.print(":");
    lcd.print(secTen);
    lcd.print(secUni);
}

void timerSetUp(){
  //Timer Configuration
  lcd.setCursor(0,0);
  lcd.print("Timer Set Up");
  clearRow(1);
  lcd.setCursor(0,1);
  lcd.print("__:__:__"); // 0 1 3 4 6 7
  while(!timer_setted){
    key = myKeypad.getKey();
    while (key == NO_KEY) key = myKeypad.getKey();
    switch (key) {
      case 'D':
        switch(timerPosition){
          case 0://HT
            break;
          case 1://HU
            lcd.setCursor(0,1);
            hourTen = '_';
            lcd.print(hourTen);
            timerPosition = 0;
            break;
          case 2://MT
            lcd.setCursor(1,1);
            hourUni = '_';
            lcd.print(hourUni);
            timerPosition = 1;
            break;
          case 3://MU
            lcd.setCursor(3,1);
            minTen = '_';
            lcd.print(minTen);
            timerPosition = 2;
            break;
          case 4://ST
            lcd.setCursor(4,1);
            minUni = '_';
            lcd.print(minUni);
            timerPosition = 3;
            break;
          case 5://SU
            lcd.setCursor(6,1);
            secTen = '_';
            lcd.print(secTen);
            timerPosition = 4;
            break;
          case 6://COMPLETED
            lcd.setCursor(7,1);
            secUni = '_';
            lcd.print(secUni);
            timerPosition = 5;
            break;
        };
        break;
      case 'C':
        if (timerPosition == TIMER_SIZE) timer_setted = true;
        break;
      default:
        if(isDigit(key)){
          switch (timerPosition) {
            case 0:
              lcd.setCursor(0,1);
              hourTen = key;
              lcd.print(hourTen);
              timerPosition = timerPosition + 1;
              break;
            case 1:
              lcd.setCursor(1,1);
              hourUni = key;
              lcd.print(hourUni);
              timerPosition = timerPosition + 1;
              break;
            case 2:
              lcd.setCursor(3,1);
              minTen = key;
              lcd.print(minTen);
              timerPosition = timerPosition + 1;
              break;
            case 3:
              lcd.setCursor(4,1);
              minUni = key;
              lcd.print(minUni);
              timerPosition = timerPosition + 1;
              break;
            case 4:
              lcd.setCursor(6,1);
              secTen = key;
              lcd.print(secTen);
              timerPosition = timerPosition + 1;
              break;
            case 5:
              lcd.setCursor(7,1);
              secUni = key;
              lcd.print(secUni);
              timerPosition = timerPosition + 1;
              break;
            default:
              break;
          };
        }
        break;
      };
  }


  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("Timer selected");
  delay(750);
  showTimer(4,1);
  delay(1500);
  lcd.clear();
}

void wireSetUp(){
  lcd.setCursor(0,0);
  lcd.print("Wire Set Up");
  clearRow(1);
  lcd.setCursor(0,1);
  lcd.print("[1|2|3]: ___");

  while(!wireOrder_setted){
    key = myKeypad.getKey();
    while (key == NO_KEY) key = myKeypad.getKey();

    switch(key){
      case 'D':
        switch(wireOrderPosition){
          case 1:
            lcd.setCursor(9,1);
            lcd.print('_');
            wireOrderPosition = wireOrderPosition - 1;
            lastInput = wireOrder[wireOrderPosition];
            wireOrder[wireOrderPosition] = '0';
            resetX(lastInput);
            break;
          case 2:
            lcd.setCursor(10,1);
            lcd.print('_');
            wireOrderPosition = wireOrderPosition - 1;
            lastInput = wireOrder[wireOrderPosition];
            wireOrder[wireOrderPosition] = '0';
            resetX(lastInput);
            break;
          case 3:
            lcd.setCursor(11,1);
            lcd.print('_');
            wireOrderPosition = wireOrderPosition - 1;
            lastInput = wireOrder[wireOrderPosition];
            wireOrder[wireOrderPosition] = '0';
            resetX(lastInput);
            break;
          default:
            break;
        }
        break;
      case 'C':
        if (wireOrderPosition == WIRE_ORDER_SIZE) wireOrder_setted = true;
        break;
      case '1':
        if((wireOrderPosition < WIRE_ORDER_SIZE) && notIn(key,wireOrder)){
          lcd.setCursor(9+wireOrderPosition,1);
          wireOrder[wireOrderPosition] = key;
          wireOrderPosition = wireOrderPosition + 1;
          lcd.print(key);
          lcd.setCursor(1,1);
          lcd.print('X');
        }
        break;
      case '2':
        if((wireOrderPosition < WIRE_ORDER_SIZE) &&  notIn(key,wireOrder)){
          lcd.setCursor(9+wireOrderPosition,1);
          wireOrder[wireOrderPosition] = key;
          wireOrderPosition = wireOrderPosition + 1;
          lcd.print(key);
          lcd.setCursor(3,1);
          lcd.print('X');
        }
        break;
      case '3':
        if((wireOrderPosition < WIRE_ORDER_SIZE) &&  notIn(key,wireOrder)){
          lcd.setCursor(9+wireOrderPosition,1);
          wireOrder[wireOrderPosition] = key;
          wireOrderPosition = wireOrderPosition + 1;
          lcd.print(key);
          lcd.setCursor(5,1);
          lcd.print('X');
        }
        break;
      default:
        break;
    }
  }

  wire1 = setWire(wireOrder[0]);
  
  wire2 = setWire(wireOrder[1]);
  
  wire3 = setWire(wireOrder[2]);

  lcd.clear();
  lcd.setCursor(3,0); 
  lcd.print("Wire Order");
  delay(750);
  lcd.setCursor(5,1);
  lcd.print(wireOrder[0]);
  lcd.print("-");
  lcd.print(wireOrder[1]);
  lcd.print("-");
  lcd.print(wireOrder[2]);
  delay(1500);
  lcd.clear();

}

void codeSetUp(){
  lcd.setCursor(0,0);
  lcd.print("Code Set Up");
  clearRow(1);
  lcd.setCursor(0,1);
  lcd.print("____");

  while(!defuseCode_setted){
    key = myKeypad.getKey();
    while (key == NO_KEY) key = myKeypad.getKey();
    switch(key){
      case 'D':
        switch(defuseCodePosition){
          case 0:
            break;
          case 1:
            lcd.setCursor(0,1);
            lcd.print('_');
            defuseCodePosition = defuseCodePosition - 1;
            defuseCode = defuseCode.substring(0,defuseCodePosition);
            break;
          case 2:
            lcd.setCursor(1,1);
            lcd.print('_');
            defuseCodePosition = defuseCodePosition - 1;
            defuseCode = defuseCode.substring(0,defuseCodePosition);
            break;
          case 3:
            lcd.setCursor(2,1);
            lcd.print('_');
            defuseCodePosition = defuseCodePosition - 1;
            defuseCode = defuseCode.substring(0,defuseCodePosition);
            break;
          case 4:
            lcd.setCursor(3,1);
            lcd.print('_');
            defuseCodePosition = defuseCodePosition - 1;
            defuseCode = defuseCode.substring(0,defuseCodePosition);
            break;
        }
        break;
      case 'C':
        if (defuseCodePosition == DEFUSE_CODE_SIZE) defuseCode_setted = true;
        break;
      default:
        if(isDigit(key) && defuseCodePosition < DEFUSE_CODE_SIZE){
          defuseCodePosition = defuseCodePosition + 1;
          defuseCode = defuseCode + key;
          lcd.setCursor(defuseCodePosition-1,1);
          lcd.print(key);
        }
        break;
    }
  }

  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("Code selected");
  delay(750);
  lcd.setCursor(6,1);
  lcd.print(defuseCode);
  delay(1500);
  lcd.clear();

}


void showGameStartMessage(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Bomb armed");
  delay(750);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Game Start In");
  lcd.setCursor(15,0);
  lcd.print("3");
  delay(1250);
  lcd.setCursor(15,0);
  lcd.print("2");
  delay(1250);
  lcd.setCursor(15,0);
  lcd.print("1");
  delay(1250);
  
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("GO!");
  delay(1250);
  lcd.clear();  
}

bool notIn(char elem, char vector[]){
  bool flag = true;
  for (int i = 0; i < sizeof(vector); i = i + 1) {
    if (vector[i] == elem) flag = false;
  }
  return flag;
}

void resetX(char input){
    switch (input){
      case '1':
        lcd.setCursor(1,1);
        lcd.print('1');
        break;
      case '2':
        lcd.setCursor(3,1);
        lcd.print('2');
        break;
      case '3':
        lcd.setCursor(5,1);
        lcd.print('3');
        break;
    }
}

int setWire(char wirePosition){
  int pin;
  switch(wirePosition){
    case '1':
      pin = 10;
      break;
    case '2':
      pin = 11;
      break;
    case '3':
      pin = 12;
      break;
  }
  return pin;
}

void minusSecond(){
  asm (
  "cpi %1,'0'         \n"
  "brne SUM1          \n"
  "cpi %2,'0'         \n"
  "brne STM1          \n"
  "cpi %3,'0'         \n"
  "brne MUM1          \n"
  "cpi %4,'0'         \n"
  "brne MTM1          \n"
  "cpi %5,'0'         \n"
  "brne HUM1          \n"
  "cpi %6,'0'         \n"
  "brne HTM1          \n"
  "ldi %0,1           \n"
  "rjmp END           \n"
  "SUM1:              \n"
    "DEC %1           \n"
    "RJMP END         \n"
  "STM1:              \n"
    "DEC %2           \n"
    "SUR: ldi %1, '9' \n"
    "RJMP END         \n"
  "MUM1:              \n"
    "DEC %3           \n"
    "STR: ldi %2, '5' \n"
    "RJMP SUR         \n"
  "MTM1:              \n"
    "DEC %4           \n"
    "MUR: ldi %3, '9' \n"
    "RJMP STR         \n"
  "HUM1:              \n"
    "DEC %5           \n"
    "MTR: ldi %4, '5' \n"
    "RJMP MUR         \n"
  "HTM1:              \n"
    "DEC %6           \n"
    "ldi %5, '9'      \n"
    "RJMP MTR         \n"
  "END:               \n"
  : "+r" (timeOver), "+r" (secUni), "+r" (secTen), "+r" (minUni), "+r" (minTen), "+r" (hourUni), "+r" (hourTen));

  timerChange = 1;
}


