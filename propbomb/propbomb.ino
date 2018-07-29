#include <Keypad.h>
#include <Wire.h>
#include  <LiquidCrystal_I2C.h>

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
//End LCD-i2c definition

//Begin Game Configuration
#define OPTIONS_NUMBER 3
String gameOptions[OPTIONS_NUMBER]={"Defuse Code", "Defuse Wire", "Defuse Extra"};
byte gameOptionSelected;
byte gameOption = 0;
//End Game Configuration

void setup() {
  Serial.begin(9600);
  //Initialize LCD
  lcd.begin(16, 2);
  //Initialize lcd special characters
  lcd.createChar (1, arrowu);
  lcd.createChar (2, arrowd);
  lcd.createChar (3, arrowl);
  lcd.createChar (4, arrowr);
  lcd.createChar (5, enter);
  lcd.createChar (6, empty);

  //Game WELCOME
  showWelcomeMessage();

  //Game MODE Configuration
  lcd.clear();
  lcd.home ();
  lcd.print("GAME MODE");
  //Show First Game Option
  gameOption = showGameOption(0);
  
  bool option_selected = false;
  while(!option_selected){
    key = myKeypad.getKey();
    while (key == NO_KEY) key = myKeypad.getKey();
  
    switch (key) {
    case 'A':
      if(gameOption > 0){
        gameOption = gameOption - 1;
        gameOption = showGameOption(gameOption);
      };
      break;
    case 'B':
      if(gameOption < OPTIONS_NUMBER -1){
        gameOption = gameOption + 1;
        gameOption = showGameOption(gameOption);
      };
      break;
    case 'C':
      gameOptionSelected = gameOption;
      option_selected = true;
      break;
    default:
      break;
    };
  };

  lcd.clear();
  lcd.home();
  lcd.print(gameOptions[gameOption]);

  switch (gameOption) {
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  };
}

void loop() {
}

void showWelcomeMessage(){
  lcd.home ();
  lcd.setCursor ( 2, 0);
  lcd.print("PropBOMB Game");
  lcd.setCursor ( 3, 1 );
  lcd.print("By Edu");
  delay (1500);
  for ( int i = 0; i < 15; i++ ) {
    lcd.scrollDisplayRight();
    delay (350);
  }
}

byte showGameOption(int optionIndex){
  clearRow(1);
  lcd.setCursor ( 0, 1 );
  lcd.write((byte)4); // right arrow
  lcd.print(gameOptions[optionIndex]); // game option
  lcd.setCursor ( 14, 1 );
  lcd.write((byte)1); // up arrow
  lcd.write((byte)2); // down arrow
  return optionIndex;
}

void clearRow(int rowNumber){
  lcd.setCursor(0,rowNumber);
  for ( int i = 0; i < 16; i++ ) {
    lcd.write((byte)6);
  }
}
