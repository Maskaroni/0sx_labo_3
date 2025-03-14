#include <LiquidCrystal.h>
#include <LCD_I2C.h>
#include <OneButton.h>

LCD_I2C lcd(0x27, 16, 2);
byte trenteNeuf[8] = {B11111, B10100, B10100, B01000, B00000, B01010, B10101, B10101};
byte degre[8] = {B01100, B10010, B10010, B01100, B00000, B00000, B00000, B00000};
const int BTN_PIN = 2;
OneButton button(BTN_PIN, true);
const int LED_PIN = 8;
const float PHOTORESISTOR_PIN = A0;
const float FORWARD_BACK_PIN = A1;
const float LEFT_RIGHT_PIN = A2;

const int maxBack = -25;
const int maxFront = 120;
const int maxLeft = -90;
const int maxRight = 90;
const int zero = 0;
const int maxYAndPhotoresistor = 1023;
const int middleY = 514;
const int maxX = 1060;

unsigned long startTime = 0;
unsigned long delayThis = 0;
const int numberOfScreens = 2;
int whichScreen = 1;

int ledState;
int percent;
int offOn;
long x;
long y;
int go;
int leftRight;

void setup() {                                                                                  //Setup
  Serial.begin(115200);
  lcd.begin(115200);
  lcd.backlight();
  lcd.createChar(0, trenteNeuf);
  lcd.createChar(1, degre);
  pinMode(LED_PIN, OUTPUT);
  button.attachClick(changeScreen);
  Serial.println("Setup completed!");

  startTime = millis();
  start(startTime);
  Serial.println("The screen has displayed my name already!");
}

void loop() {                                                                                    //Loop
  startTime = millis();
  button.tick();

  displayCarStats(startTime);
  headlights(startTime);
  directionVroom();
}


void start(unsigned long startTime) {                                                            //Start
  unsigned long currentTimeHere;
  delayThis = 3000;

  lcd.print("Lacerte");
  lcd.setCursor(0, 2);
  lcd.write(byte(0));
  lcd.setCursor(14, 2);
  lcd.print("39");

  do{
    currentTimeHere = millis();
  }while ((currentTimeHere - startTime) <= delayThis);

  screenOn(1);
}

void displayCarStats(unsigned long startTime) {                                                  //DisplayCartStats
  if ((startTime % 100) == zero) {
    Serial.print("etd:2486739,x:");
    Serial.print(x);
    Serial.print(",y:");
    Serial.print(y);
    Serial.print(",sys:");
    Serial.println(offOn);

    screenOn(whichScreen);
  }
}

void headlights(unsigned long startTime) {                                                       //Headlights
  unsigned static long timer;
  static bool timerIsDone;
  static int isOverFifty = 0;
  static int pastInput = 0;
  int isSameInput = false;
  percent = map((analogRead(PHOTORESISTOR_PIN)), zero, maxYAndPhotoresistor, zero, 104);
  if (percent > 100) {
    percent = 100;
  }
  else if (percent < zero) {
    percent = zero;
  }

  delayThis = 5000;

  isOverFifty = (percent > 50) ? 1 : 0;
  if (isOverFifty == pastInput) {
    if (!timerIsDone) {
      timer = startTime + delayThis;
      timerIsDone = true;
    }
    if (startTime >= timer) {
      isSameInput = true;
      timerIsDone = false;
    }
  }
  else {timerIsDone = false;}


  if (isSameInput == true) {
    ledState = (percent > 50) ? LOW : HIGH;
    offOn = (percent > 50) ? 0 : 1;
  }

  pastInput = isOverFifty;
  digitalWrite(LED_PIN, ledState);
}

void directionVroom() {                                                                          //DirectionVroom
  y = analogRead(FORWARD_BACK_PIN);
  if (y > middleY) {
    go = map(y, 514.1, maxYAndPhotoresistor, zero, maxFront);     //Sans bouger, le joystick est à y = 514 ou 515
  }
  else if (y < middleY) {
    go = map(y, zero, 513.9, maxBack, zero);
  }
  else {
    go = zero;
  }

  x = analogRead(LEFT_RIGHT_PIN);
  leftRight = map(x, zero, maxX, maxLeft, maxRight);   //Sans bouger, le joystick est à x = 530
}

void changeScreen() {                                                                            //ChangeScreen
  whichScreen++;
  if (whichScreen > numberOfScreens) {
    whichScreen = 1;
  }
  
  screenOn(whichScreen);
}

void screenOn(int thisScreen) {                                                                  //ScreenOn
  lcd.clear();

  switch(thisScreen) {
    case 1:
      lcd.print("Pct lum: ");
      lcd.print(percent);
      lcd.print("%");

      lcd.setCursor(0, 2);
      lcd.print("Phares: ");
      if (offOn == 1) {
        lcd.print("ON");
      }
      else {
        lcd.print("OFF");
      }
      break;
    case 2:
      if (go > 0) {
        lcd.print("Avance");
      }
      else if (go < zero) {
        lcd.print("Recule");
      }
      else {
        lcd.print("Arrete");
      }
      lcd.setCursor(9, 0);
      if (go < zero) {                                          //J'ai affiché la vitesse toujours au positif
        go = -go;
      }
      lcd.print(go);
      lcd.print("km/h");

      lcd.setCursor(12, 2);
      if (leftRight > zero) {
        lcd.print("D");
      }
      else if (leftRight < zero) {
        lcd.print("G");
      }
      else {
        lcd.print("0");
        lcd.write(byte(1));
      }
      break;
    default:
      lcd.setCursor(4, 0);
      lcd.print("Probleme!");
  }
}