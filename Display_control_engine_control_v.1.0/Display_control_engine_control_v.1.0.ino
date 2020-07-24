
//Версия с контролем двигателей

//#include <math.h>
#include <GyverButton.h> // button control
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // dislpay control
#include <ServoSmooth.h> // servo control
//#include <vector>
LiquidCrystal_I2C lcd(0x27, 16, 2);     //  setting 16x2 display






#define AMOUNT 4

#define BTN1 10
#define BTN2 11
#define BTN3 12
#define BTN4 13




GButton butt1(BTN1);
GButton butt2(BTN2);
GButton butt3(BTN3);
GButton butt4(BTN4);

ServoSmooth syr[AMOUNT]; //4 Servos

// LCD Display logic
const int numofScreens = 6;
int currentScreen = 0;
int currentServo = 0;
String screens[numofScreens][1] = {
  {"Servo"},
  {"Positions"},
  {"hours"},
  {"minutes"},
  {"volume ul"},
  {"mode"}
};

long last_time[4] = {0, 0, 0, 0}; // for timer


boolean zapuskData[4] = {0, 0, 0, 0}; // array of states: 1 = work 0 = rest

int endPositions[4] = {600, 600, 600, 600}; // Array for comparison

int parameters[numofScreens][4] = {
  {0, 1, 2, 3},             // Servos 0
  {2350, 2350, 2350, 2350}, // Positions 1
  {6, 6, 6, 6},             // Hours 2
  {0, 0, 0, 0},             // Minutes 3
  {400, 400, 400, 400},     // Volume 4
  {0, 0, 0, 0}              // Mode 5
};

long times[4] = {0, 0, 0, 0}; // Reaction time in seconds
int increment = 5; // for write.Microseconds
float volCoefs[4] = {4.43, 4.27, 4.19, 4.34}; // volCoef = d(positions)/d(volume)
float reaction_times[4]={0,0,0,0};


void setup() {
  lcd.begin();
  Serial.begin(9600);
  butt1.setTickMode(AUTO); // button requests
  butt2.setTickMode(AUTO);
  butt3.setTickMode(AUTO);
  butt4.setTickMode(AUTO);

  butt1.setTimeout(1000);  // time for long push
  butt2.setTimeout(1000);
  butt3.setTimeout(1000);
  butt4.setTimeout(1000);

  syr[0].attach(2); // defining servo pins
  syr[1].attach(3);
  syr[2].attach(4);
  syr[3].attach(5);
  printScreen();
  printSerial();
  for (byte i = 0; i < AMOUNT; i++) {
    syr[i].writeMicroseconds(parameters[1][i]); // set all Servos for max angle
    changeEndPositions(i);
  }
  for ( byte i = 0; i < AMOUNT; i++) {
    times[i] = parameters[2][i] * 60 * 60 + parameters[3][i] * 60;
    //Serial.println(times[i]);
  }
}

void loop() {
  combination();
 // printSerial();
  
}


// Serial output
void printSerial() {
  Serial.println(screens[currentScreen][0]);
  Serial.println(parameters[currentScreen][currentServo]);
}


// LCD output
void printScreen() {
  lcd.clear();
  lcd.setCursor(10, 0);
  lcd.print("Servo");
  lcd.print(currentServo);
  lcd.setCursor(0, 0);
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0, 1);
  lcd.print(parameters[currentScreen][currentServo]);
  lcd.setCursor(15, 1);
  lcd.print(zapuskData[currentServo]);
}


// updating array of reaction times
void checktime(int i) {
  times[i] = parameters[2][i] * 60 * 60 + parameters[3][i] * 60; // перевод времени в секунды
  //Serial.println(times[i]);

}


// button for change setting screen (Servos, Hours, etc.)
void changeScreen() {
  if (butt4.isClick() && currentScreen < numofScreens - 1) {
    currentScreen++;
    printSerial();
    printScreen();
  }
  if (butt1.isClick() && currentScreen > 0) {
    currentScreen--;
    printSerial();
    printScreen();
  }
}


// Select Servo
void changeServo() {
  if (currentScreen == 0 && butt3.isClick() && currentServo < 3) {
    currentServo++;
    printSerial();
    printScreen();
  }
  if (currentScreen == 0 && butt2.isClick() && currentServo > 0) {
    currentServo--;
    printSerial();
    printScreen();
  }
}


// Set up positions
void changePositions () {
  if (currentScreen == 1 && butt3.isClick() ) {
    parameters[currentScreen][currentServo] += 5;
    syr[currentServo].writeMicroseconds(parameters[currentScreen][currentServo]);
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 1 && butt3.isHold() ) { // Faster mode
    parameters[currentScreen][currentServo] += 5;
    syr[currentServo].writeMicroseconds(parameters[currentScreen][currentServo]);
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 1 && butt2.isClick() && parameters[currentScreen][currentServo] > 600) {
    parameters[currentScreen][currentServo] -= 5;
    syr[currentServo].writeMicroseconds(parameters[currentScreen][currentServo]);
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 1 && butt2.isHold() && parameters[currentScreen][currentServo] > 600) {  // Faster mode
    parameters[currentScreen][currentServo] -= 5;
    syr[currentServo].writeMicroseconds(parameters[currentScreen][currentServo]);
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
}


// Set up Hours of reaction time
void changeHours () {
  if (currentScreen == 2 && butt3.isClick() ) {
    parameters[currentScreen][currentServo]++;
    printSerial();
    printScreen();
    checktime(currentServo);
  }
  if (currentScreen == 2 && butt2.isClick() && parameters[currentScreen][currentServo] > 0) {
    parameters[currentScreen][currentServo]--;
    printSerial();
    printScreen();
    checktime(currentServo);
  }
}


// Set up Minutes of reaction time
void changeMinutes () {
  if (currentScreen == 3 && butt3.isClick() ) {
    parameters[currentScreen][currentServo]++;
    printSerial();
    printScreen();
    checktime(currentServo);
  }
  if (currentScreen == 3 && butt2.isClick() && parameters[currentScreen][currentServo] > 0) {
    parameters[currentScreen][currentServo]--;
    printSerial();
    printScreen();
    checktime(currentServo);
  }
}


// Set up volume of solution to inject
void changeVolume () {
  if (currentScreen == 4 && butt3.isClick() ) {
    parameters[currentScreen][currentServo]++;
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 4 && butt3.isHold() ) {
    parameters[currentScreen][currentServo]++;
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 4 && butt2.isClick() && parameters[currentScreen][currentServo] > 0) {
    parameters[currentScreen][currentServo]--;
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
  if (currentScreen == 4 && butt2.isHold() && parameters[currentScreen][currentServo] > 0) {
    parameters[currentScreen][currentServo]--;
    printSerial();
    printScreen();
    changeEndPositions (currentServo);
  }
}


// Set up Mode of injection (linear, exponential etc. Used in addMode() function)
void changeMode () {
  if (currentScreen == 5 && butt3.isClick() && parameters[currentScreen][currentServo] < 3) {
    parameters[currentScreen][currentServo]++;
    printSerial();
    printScreen();
  }
  if (currentScreen == 5 && butt2.isClick() && parameters[currentScreen][currentServo] > 0) {
    parameters[currentScreen][currentServo]--;
    printSerial();
    printScreen();
  }
}




// function of all functions to insert into void loop()
void combination() {
  changeScreen();
  changeServo();
  changePositions();
  changeHours();
  changeMinutes();
  changeVolume();
  changeMode();
  zapusk();
  adding();
}
