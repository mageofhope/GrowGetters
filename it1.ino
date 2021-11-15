#include <LiquidCrystal.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2);

//initialize the button variables
int leftBtn = 10;
int rightBtn = 9;
int upBtn = 13;
int downBtn = 8;

//state variables for the buttons
int leftState = 0;
int rightState = 0;
int upState = 0;
int downState =0;

//sets up the days and hours need for days
int days[7] = {0,0,0,0,0,0,0};

//int to track the day
int day = 1;

//basic Strings
String dayStd = "Day: ";
String dayAsStr = String(day);
String f = dayStd + dayAsStr;
String hours = "Hours: ";
String hourVar = String(days[day - 1]);
String bottom = hours + hourVar;

//time variables
long startTime = millis();
long dayLength = 24;

//led variables
//this is to test the time functions and opening and closing time
int greenLED = 7;
int redLED = 6;
    
//temperature values
int temp = 0;
int timeOpen = 0;
int curOpen = 0;
int previousDay = 0;
bool nextStage = false;
float sTime =0;


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void setup() {
  
  
  // set up the LCD's number of columns and rows:
  lcd.begin();
  // Print a message to the LCD.
  lcd.print(f);
  //lcd.write(" Day: " + "day");
  //set cursor to bottom line;
  lcd.setCursor(0, 1);
  lcd.print(bottom);
  
  //Set up the buttons
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(leftBtn, INPUT);
  pinMode(rightBtn, INPUT);
  pinMode(upBtn, INPUT);
  pinMode(downBtn, INPUT);
  
  //Serial setup for testing
  Serial.begin(9600);
  
  
  //time setup
  
  //LED setup
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  //setup the simulation representative
  close();
  
  
  
  
}


void loop() {
  Serial.print(moveOn());
  Serial.println(digitalRead(redLED));
  if(!moveOn()){
  
    //redundancy is good
    if(day > 7)day = 1;
    if(day < 0) day = 7;
    
    //check state of the buttons
    readStates();
    
    
    
    //check left button
    if(leftState == HIGH){
      //lcd.clear();
      //set the cursor to the top line
      lcd.setCursor(0,0);
      
      day--;
      if(day == 0){
        tempOut(temp);
      }else if(day < 0) {
        day = 7;
        //print out what we want
        outLCD();
      }else{
        outLCD();
      }
      
      
      
    }
    
    
    //check right button
    if(rightState == HIGH){
      //lcd.clear();
      lcd.setCursor(0,0);
      
      day++;
      if(day > 7){
          day = 0;
          //reset the str variables for printing
      }
      if(day == 0){
      tempOut(temp);
      }else{
        outLCD();
      }
      
      
    

    }
    
    
    //check up button
    if(upState == HIGH){
  
      if(day != 0){
        days[day - 1] ++;
        if(days[day - 1] > 24)days[day - 1] =24;
        outLCD();
      }else{
        temp++;
        tempOut(temp);
      }
      
    }
    
    
    //check down button
    if(downState == HIGH){
      if(day != 0){
        days[day - 1] --;
        if(days[day - 1] < 0)days[day - 1] =0;
    
        outLCD();
      }else{
        temp--;
          tempOut(temp);
      }
      
    }
    delay(250);
  }else{
    progLCD();
    Serial.println("Second loop");
      if(shouldBeOpen(previousDay)){
        Serial.println("Were in the inside");
        doWork();
      }else{
      Serial.println("We didn't get inside");
      }
  }
  
  //delay at the end of loop
  
  
  //timeChecker();
  
  
  //////////////////////////////////////////////////////////
  //Now start the part where we control the motors and shade
  //////////////////////////////////////////////////////////
  
}



//////////////////////////////////////////////////////////////


//////////////////////////////////
//Functions start here going down--------------------------------------
//////////////////////////////////


//function that prints out the updated version for LCD
void outLCD(){
  lcd.clear();
  
    dayAsStr = String(day);
  f = dayStd + dayAsStr;
    //print the new str
    lcd.print(f);
  
  
    lcd.setCursor(0,1);
    hourVar = String(days[day - 1]);
  bottom = hours + hourVar;
  
    lcd.print(bottom);
}

//for temp
void tempOut(int temp){
  lcd.clear();
  
    
    String temper = "Max Temperature:";
    
  String out = temper;
    //print the new str
    lcd.print(temper);
  
  
    lcd.setCursor(0,1);
    String tempStr = String(temp);
    String tempStd = " F";
  bottom = tempStr + tempStd;
  
    lcd.print(bottom);
}

//function that reads the states of the buttons and updates them.
void readStates(){
  leftState = digitalRead(leftBtn);
    rightState = digitalRead(rightBtn);
    upState = digitalRead(upBtn);
    downState = digitalRead(downBtn);
}

//function that checks time and determines if we still need sunlight. Takes timeOpen as a parameter
void timeChecker(bool state, int openTime){
  long currentTime = millis();
  long totalElapsed = (currentTime - startTime)/1000;
}

//returns whether or not the system should be open
bool shouldBeOpen(int pDay){
  if(previousDay > 7)return true;
  
  int currentDay = getDay();
  if(currentDay != pDay){
    timeOpen = 0;
    previousDay = currentDay;
  }
  
  
  Serial.print("Current Day: ");
  Serial.println(currentDay);
  
  
  Serial.print("opent time: ");
  Serial.print(timeOpen);
  Serial.println("//////");

  Serial.print("day time: ");
  Serial.print(days[currentDay-1]);
  Serial.println("//////");
  if(timeOpen <= days[currentDay-1]){
    return true;
  }
  close();
  return false;
}

//function that returns how long the system has been in its current state either on or off
int timeElapsed(int start){
  return millis() - start;
  
}

 
//function that simulates an open orientation with the 
//shadecloth retracted, using LEDs
void open(){
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
}

//function that simulates a closed orientation with the
//shadecloth over the plants, using LEDs
void close(){
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  Serial.println("Close was run");
}

//checks whether or not the current state is open
bool isOpen(){
  digitalRead(greenLED);
}

//function that tells us what day it is 1-7
int getDay(){
  int currentTime = (millis()- startTime)/1000;
    int day = (currentTime / dayLength) + 1;
    return day;
}

/**
 * @brief method to actually open or close the system
 * 
 * 
 * @param pDay passes the previous day that was used, should just be the instance variable previous day
 */
void doWork(){
   
    // float sTime = millis();
    // curOpen = sTime;
    
    if(timeOpen <= days[getDay()] &&!isOpen()){
      open();
      sTime = millis();
      curOpen = sTime;
      Serial.println("It was closed");
    }else if (isOpen && timeOpen <= days[getDay()-1]){
      Serial.println("fuck babby fuck");
      //timeOpen += (millis() - curOpen)/1000;
      timeOpen++;
    }else if(timeOpen > days[getDay()-1]){ 
        Serial.print(timeOpen);
        close(); //just put this to avoid error, not logical yet, need to check
        //timeOpen += (millis() - curOpen)/1000;
        Serial.print("and I liked it");
    }else{
      Serial.print("This isn't working");
    }
    delay(900);


}


/**
 * @brief This is the bool that determines if the programming is done
 * 
 * @return true 
 * @return false 
 */

bool moveOn(){
    if (upState == HIGH && rightState == HIGH){
      /* code */
      return true;
    }
    return false;
    
}

//new output for function
void progLCD(){
  lcd.clear();
  
  f = "Done";
    //print the new str
    lcd.print(f);
  
  
    lcd.setCursor(0,1);
    
  bottom = "Running...";
    lcd.print(bottom);
}
