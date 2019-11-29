//Michael Pierik
//Calculator using arduino and ElEGOO components

//1 x RGB LED
//1 x LCD 1602 (with pin header)
//1 x 4x4 Matrix Keypad
//1 x Rotary Encoder Module
//1 x 10K potentiometer (can be replaced with resistor for constant Contrast on LCD)
//3 x 220 Resistor
//1 x 1K Resistor
//1 x Arduino MEGA 2560

//Include Librarys
#include "SR04.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <ListLib.h>
//Define Pins

//RGB LED
#define RED 6   //RGB LED red value in slot 6
#define GREEN 9 //RGB LED green value in slot 6
#define BLUE 10 //RGB LED blue value in slot 6

//Rotery Encoder
#define outputA 5
#define outputB 4
#define outputSw 3

//PUSHBUTTON
#define SWITCH 11

//VARIABLE DECLERATION
int redValue;
int greenValue;
int blueValue;
int counter = 0;
int aState;
int bState;
int aLastState;
int positionLCD = 0;
int keyCounter = 0;
int toNumber;
int operatorType;
int switchIn = 0;
int RESwitchIn = 0;

bool hasMoved = false;
bool toggle = false;

double firstNumber;
double secondNumber;
double result;

List<int> Number1;
List<int> operatorPosition;
List<char> displayLCD;

//LCD Display
LiquidCrystal lcd(50, 49, 48, 47, 46, 45);  // Initiate LCD with Numbers for Pin slots

//Keypad
const byte ROWS = 4;     //Keypad has 4 rows
const byte COLUMNS = 4;  //Keypad has 4 columns
char hexKeys [ROWS][COLUMNS] = {  //KEYPAD Layout
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};
byte rowPins[ROWS] = {29, 28, 27, 26};        //connect to row pinouts of keypad
byte columnPins[COLUMNS] = {25, 24, 23, 22};  //connect to column pinouts of keypad

//Initialize new instance of Keypad
Keypad customKeypad = Keypad( makeKeymap(hexKeys), rowPins, columnPins, ROWS, COLUMNS); 

void setup() {
  pinMode(RED,OUTPUT);    //Sets RED as an output
  pinMode(GREEN, OUTPUT);   //Sets GREEN as an output
  pinMode(BLUE, OUTPUT);   //Sets BLUE as an output
  pinMode(outputA,INPUT);   //Sets outputA as an input
  pinMode(outputB,INPUT);   //Sets outputB as an input
  pinMode(outputSw, INPUT_PULLUP); 
  pinMode(SWITCH, INPUT_PULLUP);
  
  digitalWrite(RED, LOW);     
  digitalWrite(GREEN, HIGH);  //Sets Green as the default starting colour  
  digitalWrite(BLUE, LOW);
  
  Serial.begin(9600);
  lcd.begin(16, 2); //Initialises LCD telling the program the LCD is 16 * 2
  aLastState = digitalRead(outputA);  //Sets the last state of the Rotery Encoder to the Current one time
}

void loop() {
  //Toggle Switch simulation
  switchIn = digitalRead(SWITCH); //read input from pushbutton
  if(switchIn == 1){  //if the input is 1(PRESSED)
    Serial.println(switchIn); //TROUBLESHOOTING (print switch output)
    if(toggle == true){toggle = false;} //if the toogle is set to TRUE(state 1) then set it to FALSE
    else if(toggle == false){toggle = true;}// else if the toggle is set to FALSE then set it to TRUE
    do{switchIn = digitalRead(SWITCH);} while (switchIn == 1);  //Waste the rest of the push press past the first input of 1
  }
  if (toggle == true) {redValue = 255; blueValue = 0; greenValue = 0; analogWrite(RED, redValue); analogWrite(GREEN, greenValue);}
  if (toggle == false) {redValue = 0; blueValue = 0; greenValue = 255; analogWrite(RED, redValue); analogWrite(GREEN, greenValue);}
  char keyPress = customKeypad.getKey();  //Logging the users input on the keypad

  //Rotery Position
  
  aState = digitalRead(outputA);
  bState = digitalRead(outputB);

  //delay(0);
  if(aState != bState){ //if aState and bState are not equal
    counter = bState - aState;  //set the counter to the difference between bState and aState
    hasMoved = true;  //Tell the other statements that the cursor should move
  }

  if (aState == 1 && bState == 1 && hasMoved == true){  // if aState and bState are both 1 and if the cursor should move
    positionLCD += counter; // add the cursor number to the position
    Serial.print(counter); Serial.print("; "); Serial.println(positionLCD);
    hasMoved = false; //tell the other statements that the cursor should not move
  }
  
  if(positionLCD >= keyCounter){positionLCD = keyCounter;}  // if the position in the LCD is greater than or equal to the max position number then move the cursor to the max position number
  if(positionLCD < 0){positionLCD = 0;} //if the position in the LCD is less than the lowest position than set it to the lowest position
  if(positionLCD < keyCounter){lcd.blink();}  //if the position of the LCD is less than the max than make the cursor blink
  else{lcd.noBlink();}  // if the position of the LCD is at the max then stop blinking the cursor
  lcd.setCursor(positionLCD,0); // set the cursor to its new position

  // KEYPAD

  switch(keyPress) {  

    case '=':           //When = is Pressed
    lcd.setCursor(0,1); //Sets LCD Cursor to position 0 , line 1
    
    for(int i = 0; i < Number1.Count(); i++){         //Scan the arraylist for operators and mark them
      Serial.print(Number1[i]); Serial.print(" ");
      if (Number1[i] > 10){operatorPosition.Add(i);}   //operators are the only inputs greater than 10
    }
    Serial.println();
    for(int x = 0 ; x < operatorPosition.Count(); x++){ //Generates the numbers to use in math
      int i = 0;
      while(i < operatorPosition[x]){   //Generating first number using inputted didgets
        firstNumber = firstNumber * 10;
        firstNumber += Number1[i]; 
        i++;
      }
      if(i == operatorPosition[x]){ //Figuring out the operator
        switch (Number1[i]) {
          case 43 : Serial.println('+'); operatorType = 1; break; 
          case 45 : Serial.println('-'); operatorType = 2; break;
          case 42 : Serial.println('*'); operatorType = 3; break;
          case 47 : Serial.println('/'); operatorType = 4; break;
        }
        i++;
      }
      while(i > operatorPosition[x] && i < Number1.Count()){ //Generating second number using inputted didgets
         secondNumber = secondNumber * 10;
         secondNumber += Number1[i];
         i++;
         
      }
    }

    //MATH 
    switch(operatorType){
      case 1: result = firstNumber + secondNumber; break; //if + is the Operator
      case 2: result = firstNumber - secondNumber; break; //if - is the Operator
      case 3: result = firstNumber * secondNumber; break; //if * is the Operator
      case 4: result = firstNumber / secondNumber; break; //if / is the Operator
    }
    lcd.print("= "); lcd.print(result);   //Output the result to the LCD
    firstNumber = 0;
    secondNumber = 0;
    break;
    
    case 'C':           //When C is Pressed then clear everything and return to default values
    lcd.clear();
    lcd.setCursor(0,0);
    Number1.Clear();
    operatorPosition.Clear();
    displayLCD.Clear();
    firstNumber = 0;
    secondNumber = 0;
    keyCounter = 0;
    counter = 0;
    positionLCD = 0;
    break;

    //checking for operator
    case '+': toNumber = '+'; break;  // when + is pressed then tell arduino what to do
    case '-': toNumber = '-'; break;  // when - is pressed then tell arduino what to do
    case '*': toNumber = '*'; break;  // when * is pressed then tell arduino what to do
    case '/': toNumber = '/'; break;  // when / is pressed then tell arduino what to do

    //Typecasting char input to int
    case '0': toNumber = 0; break;
    case '1': toNumber = 1; break;
    case '2': toNumber = 2; break;
    case '3': toNumber = 3; break;
    case '4': toNumber = 4; break;
    case '5': toNumber = 5; break;
    case '6': toNumber = 6; break;
    case '7': toNumber = 7; break;
    case '8': toNumber = 8; break;
    case '9': toNumber = 9; break;

    
  }
  
  RESwitchIn = digitalRead(outputSw);
  if(RESwitchIn == 0 && displayLCD.Count() > 0){
    displayLCD.Remove(positionLCD);
    Number1.Remove(positionLCD);
    if (positionLCD < keyCounter){
      keyCounter --;
    }
    do{RESwitchIn = digitalRead(outputSw);} while (RESwitchIn == 0);
    lcd.clear();
    for(int i; i < displayLCD.Count(); i++){
      lcd.print(displayLCD[i]);
      Serial.print(displayLCD[i]);
    }
  }  

  if(keyPress){       //If any key is pressed
    positionLCD ++;
    if(keyPress != 'C'){
      keyCounter ++;
    }

    if(keyPress != 'C' && keyPress != '='){ //any excluding = and C
      lcd.clear();
      delay(100);
      if(positionLCD == keyCounter){
        Number1.Add(toNumber);  // add the number to the arraylist
        displayLCD.Add(keyPress);
      }
      if(toggle == false && positionLCD < keyCounter){  //IF it is in Insert mode then insert numbers (GREEN LIGHT)
        Number1.Insert( positionLCD - 1, toNumber);
        displayLCD.Insert(positionLCD - 1, keyPress);
      }
      if(toggle == true && positionLCD  < keyCounter){  //IF it is in Insert mode then insert numbers (RED LIGHT)
        Number1.Replace( positionLCD - 1, toNumber);
        displayLCD.Replace(positionLCD - 1, keyPress);
      }
      for(int i; i < displayLCD.Count(); i++){  //Display current situation
        lcd.print(displayLCD[i]);
        Serial.print(displayLCD[i]);
      }
    Serial.println();
    }
  }
}
