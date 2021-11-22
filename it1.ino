#include <Servo.h>
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
#include <LiquidCrystal_I2C.h>
//#include <ezButton.h>
// initialize the library with port, columns, and rows
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
int downState = 0;

//sets up the days and hours need for days
long dayLimits[7] = {0,0,0,0,0,0,0};

//int to track the day
int day = 1;

//basic Strings
String dayStd = "Day: ";
String dayAsStr = String(day);
String f = dayStd + dayAsStr;

String hours = "Hours: ";
String hourVar = String(dayLimits[day - 1]);
String bottom = hours + hourVar;

//time variables
long startTime = millis();
long dayStartTime = 0;
long dayLength = 30;
long intervalStartTime = 0;
long intervalTime = 0;
int tick = 1000;
bool doneForToday = false;
bool movingOn = false;

//temperature values
int maxTemp = 20;
int sunTime = 0;

int sensePin = A0;  //This is the Arduino Pin that will read the sensor output
int sensorInput;    //The variable we will use to store the sensor input
double temp;        //The variable we will use to store temperature in degrees. 


//photoResistor setup
const int pResistor = A1;
int pValue;
int lightStd = 10; //fall morning sunlight reading w/ 1K resistor: 840


//setup Servos
Servo serv1;
Servo serv2;
int pos1 = serv1.read();
int pos2 = serv2.read();
bool isOpen = true;


//limit switch 
int limitSwitch = 2;  // create ezButton object that attach to pin 7;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void setup() {
  //servo setup
  serv1.attach(7);
  serv2.attach(6);
  
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
  
  //set up limitswitch
  pinMode(limitSwitch, INPUT);
  
  //Serial setup for testing
  Serial.begin(9600);
  
  //photoresistor setup
  pinMode(pResistor, INPUT);// Set pResistor - A0 pin as an input (optional)
}


void loop() {
  while(true && !movingOn){
    Serial.println("in configuration loop");
    getTemp();
    readPSensor();
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
        tempOut(maxTemp);
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
      tempOut(maxTemp);
      }else{
        outLCD();
      }

    }
    
    //check up button
    if(upState == HIGH){
      if(day != 0){
        dayLimits[day - 1] ++;
        if(dayLimits[day - 1] > 24)dayLimits[day - 1] =24;
        outLCD();
      }else{
        maxTemp++;
        tempOut(maxTemp);
      }
      
    }
    
    
    //check down button
    if(downState == HIGH){
      if(day != 0){
        dayLimits[day - 1] --;
        if(dayLimits[day - 1] < 0)dayLimits[day - 1] =0;
    
        outLCD();
      }else{
        maxTemp--;
          tempOut(maxTemp);
      }
      
    }
    if (moveOn()) {
      //translate hour values to ms (to be more compatible with clocks)
      for (int d=0; d<7; d++) {
        Serial.println("day limit "+String(d)+" was " +String(dayLimits[d]) +"");
        dayLimits[d] *= 3600000;
      }
      day = 0;
      movingOn = true;
      break; //get out of while(true)
             //I know it's a bitch way to do this logic but otherwise I'd multiply time by 3600 every day
    }
    delay(200);
  }
  if(day < 7) {
    progLCD();
    Serial.println("in main loop");
    day++;
    Serial.println("day = " + String(day) + "");
    open();
    doWork(day);
  }
}

//////////////////////////////////////////////////////////
//Now start the part where we control the motors and shade
//////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////


//////////////////////////////////
//Functions start here going down--------------------------------------
//////////////////////////////////


void doWork(int day){
  dayStartTime = millis();
  Serial.println("beginning doWork()");
  Serial.println("day start time is " + String(dayStartTime) + "");
  while((millis() - dayStartTime)/1000 < dayLength) {
    intervalStartTime = millis();
    readPSensor();
    if (isOpen) {
      Serial.println("is open");
      if(isSunny()) {
        sunTime+=(intervalTime + tick);
        Serial.println("is sunny, incrementing sunTime by " + String(intervalTime + tick) + "");
      }
      if(sunTime > dayLimits[day]) {
        Serial.println("sunTime (" +String(sunTime)+ ") is over limit (" + String(dayLimits[day]) +").");
        doneForToday = true;
        close();
      }
      if(getTemp() > maxTemp) {
        Serial.println("temp is over maxTemp.");
        close();
      }
    }else{
      Serial.println("is closed");
      if (!doneForToday) {
        if(getTemp() <= maxTemp) {
          Serial.println("temp is under max.");
          open();
        }
      }
    }
    intervalTime = millis() - intervalStartTime;
    Serial.println("time this interval: " + String(intervalTime) + "ms");
    delay(tick);
  }
  Serial.println("done with day " + String(day) + "");  
}

//function that prints out the updated version for LCD
void outLCD(){
  lcd.clear();
  
    dayAsStr = String(day);
  f = dayStd + dayAsStr;
    //print the new str
    lcd.print(f);
  
  
    lcd.setCursor(0,1);
    hourVar = String(dayLimits[day - 1]);
  bottom = hours + hourVar;
  
    lcd.print(bottom);
}

//for temp
void tempOut(int temp){
  lcd.clear();
  
    
    String temper = "Max Temperature:";
    //print the new str
    lcd.print(temper);
  
  
    lcd.setCursor(0,1);
    String tempStr = String(temp);
    String tempStd = " `C";
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

//function that returns how long the system has been in its current state either on or off
int timeElapsed(int start){
  return millis() - start;
  
}

//open function with Servos
void open(){
  Serial.println("opening");
  while(!digitalRead(limitSwitch)){
    Serial.println("running servos positive");
    serv1.write(0);
    serv2.write(0);
  }
  serv1.write(90);
  serv2.write(90);
  delay(500);
  isOpen = true;
  digitalWrite(LED_BUILTIN, HIGH);
}

//close for servos
void close(){
  Serial.println("closing");
  while(!digitalRead(limitSwitch)){
    Serial.println("running servos negative");
    serv1.write(180);
    serv2.write(180);
  }
  serv1.write(90);
  serv2.write(90);
  delay(500);
  isOpen = false;
  digitalWrite(LED_BUILTIN, LOW);
}

//function that tells us what day it is 1-7
int getDay(){
  int currentTime = (millis()- startTime)/1000;
    day = (currentTime / dayLength) + 1;
    return day;
}

/**
 * @brief method to actually open or close the system
 * 
 * 
 * @param pDay passes the previous day that was used, should just be the instance variable previous day
 */

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

double getTemp(){
 
  sensorInput = analogRead(sensePin);    //read the analog sensor and store it
  temp = (double)sensorInput / 1024;       //find percentage of input reading
  temp = temp * 5;                 //multiply by 5V to get voltage
  temp = temp - 0.5;               //Subtract the offset 
  temp = temp * 100;               //Convert to degrees 
 
  Serial.print("Current Temperature: ");
  Serial.println(temp);
  return temp;
}

int readPSensor(){
  pValue = analogRead(pResistor);
  Serial.print("light reading: ");
  Serial.println(pValue);
  return pValue;
}
  
//checks sunlight
bool isSunny(){
  if(pValue < lightStd){
    Serial.println("No Sun");
    return false;
  }else{
    return true;
  }
}
