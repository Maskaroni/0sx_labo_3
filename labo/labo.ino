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

unsigned long startTime = 0;
unsigned long delayThis = 0;
const int numberOfScreens = 2;

int ledState;
int percent;
int offOn;
long x;
long y;
int go;
int leftRight;

void setup() {
  Serial.begin(115200);
  lcd.begin(115200);
  lcd.createChar(0, trenteNeuf);
  lcd.createChar(1, degre);
  pinMode(LED_PIN, OUTPUT);
  button.attachClick(changeScreen);
  Serial.println("Setup completed!");

  startTime = millis();
  start(startTime);
  Serial.println("The screen has displayed my name already!");
}

void loop() { 
  startTime = millis();
  button.tick();

  displayCarStats(startTime);
  headlights(startTime);
  directionVroom();
}


void start(unsigned long startTime) {
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

void displayCarStats(unsigned long startTime) {
  if ((startTime % 100) == 0) {
    Serial.print("etd:2486739,x:");
    Serial.print(x);
    Serial.print(",y:");
    Serial.print(y);
    Serial.print(",sys:");
    Serial.println(offOn);
  }
}

void headlights(unsigned long startTime) {
  unsigned static long timer;
  static bool timerIsDone;
  static int isOverFifty = 0;
  static int pastInput = 0;
  int isSameInput = false;
  percent = map((analogRead(PHOTORESISTOR_PIN)), 0, 1023, 0, 104);
  if (percent > 100) {
    percent = 100;
  }
  else if (percent < 0) {
    percent = 0;
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

void directionVroom() {
  y = analogRead(FORWARD_BACK_PIN);
  if (y > 514) {
    go = map(y, 514.1, 1023, 0, 120);     //Sans bouger, le joystick est à y = 514 ou 515
  }
  else if (y < 514) {
    go = map(y, 0, 513.9, -25, 0);
  }
  else {
    go = 0;
  }

  x = analogRead(LEFT_RIGHT_PIN);
  leftRight = map(x, 0, 1060, -90, 90);   //Sans bouger, le joystick est à x = 530
}

void changeScreen() {
  static int whichScreen = 0;
  whichScreen++;
  if (whichScreen > numberOfScreens) {
    whichScreen = 1;
  }
  
  screenOn(whichScreen);
}

void screenOn(int thisScreen) {
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
      else if (go < 0) {
        lcd.print("Recule");
      }
      else {
        lcd.print("Arrete");
      }
      lcd.setCursor(9, 0);
      if (go < 0) {                                          //J'ai affiché la vitesse toujours au positif
        go = -go;
      }
      lcd.print(go);
      lcd.print("km/h");

      lcd.setCursor(12, 2);
      if (leftRight > 0) {
        lcd.print("D");
      }
      else if (leftRight < 0) {
        lcd.print("G");
      }
      else {
        lcd.print("0");
        lcd.write(byte(1));
      }
      break;
    default:
      lcd.setCursor(7, 1);
      lcd.print("Problème!");
  }
}