#include <LiquidCrystal_I2C.h>
#include <Keypad.h> //by Mark Stanley
#include "modules/SecurityKey.cpp"
#include "modules/Tonix.cpp"
#include "modules/storage.cpp"

SecurityKey sec;
Tonix tones;
localStorage ls;

int RED = 12;
int GREEN = 13;
int BUZZER = 9;

int resetDelay = 5000; // Delay before the Lock resets from open
unsigned long prevTime;

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

bool passCorrect = false;
bool toggleHold = false;
bool inSetupMode = false;
int setupStage = 0;
String tempNew = "";

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'o','0','c'}
};
byte rowPins[ROWS] = {5, 4, 3, 2};
byte colPins[COLS] = {8, 7, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  String storedPass = ls.getItem(0, 11);
  sec.setPassKey(storedPass.toInt() == 0 ? "1234" : storedPass); //default passcode or load the saved one
  
  lcd.init();
  lcd.backlight();
  tones.init(BUZZER);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Pass: ");
  
  keypad.setHoldTime(1000);
}

void loop() {
  handleKeypad();
  updateLEDs();
  resetLockIfNeeded();
}


//Handle State changes to the buttons
void handleKeypad() {
  char key = keypad.getKey();
  tones.playOn();
  int state = keypad.getState();

  if (key != NO_KEY) {
    toggleHold = false;
    processKeyInput(key);
  }

  // Handle long press of 'c' to enter setup mode
  if (state == HOLD && !toggleHold && keypad.findInList('c') >= 0) {
    toggleHold = true;
    if (passCorrect) {
      setupPasscode();
    }
  }
}


//helper function for input process
void processKeyInput(char key) {
  if (key == 'o' && !inSetupMode) {
    validatePassword();
  } else if (key == 'c') {
    clearInput();
  } else if (key == 'o' && inSetupMode) {
    processSetupMode();
  } else {
    sec.inputKeys(key);
    lcd.setCursor(0, 0);
    if (inSetupMode) {
      screenForSetup();
    } else {
      lcd.print("Enter Pass: ");
    }
    lcd.setCursor(0, 1);
    lcd.print(sec.keysEntered);
    tone(BUZZER, 500, 80);
  }
}


//helper function for validating our passcode
void validatePassword() {
  passCorrect = sec.validateKeys();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(passCorrect ? "Accepted!" : "Wrong Code!");
  tones.play(passCorrect ? 0 : 1);
  prevTime = millis();
  sec.clearInput();
}



void clearInput() {
  sec.clearInput();
  lcd.clear();
  lcd.setCursor(0, 0);
  if (inSetupMode) {
    screenForSetup();
  } else {
    lcd.print("Enter Pass: ");
  }
  tones.play(2);
}


//Handles the logic for saving and setup for new pass key
void processSetupMode() {
  if (sec.keysEntered.length() < 4) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Min is 4 Digits");
  } else if (setupStage == 0) {
    setupStage = 1;
    tempNew = sec.keysEntered;
    sec.clearInput();
    lcd.clear();
    screenForSetup();
  } else if (setupStage == 1) {
    if (tempNew == sec.keysEntered) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Passkey Saved!");
      prevTime = millis();
      sec.setPassKey(tempNew);
      ls.setItem(0, 11, tempNew);
      tones.play(0);
      inSetupMode = false;
      setupStage = 0;
      sec.clearInput();
    } else {
      setupStage = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Not matched");
      sec.clearInput();
    }
  }
}


//LED Helper function
void updateLEDs() {
  digitalWrite(GREEN, passCorrect ? HIGH : LOW);
  digitalWrite(RED, passCorrect ? LOW : HIGH);
}


//Automatic Reset to LockMode
void resetLockIfNeeded() {
  if (passCorrect && millis() - prevTime >= resetDelay) {
      
      if(inSetupMode){ //don't lock during setup mode
          return;
      }
      
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Pass: ");
    lcd.setCursor(0, 1);
    lcd.print(sec.keysEntered);
    passCorrect = false;
  }
}



void setupPasscode() {
  if (!passCorrect) return;
  
  if(inSetupMode){ //toggle off if on setup
      inSetupMode = false;
      setupStage = 0;
        lcd.setCursor(0, 0);
        lcd.print("Enter Pass:   ");
      return;
  }
  inSetupMode = true;
  lcd.setCursor(0, 0);
  lcd.print("Enter New Pass: ");
}

void screenForSetup() {
  lcd.print(setupStage == 0 ? "Enter New Pass:  " : "ReEnter New Pass: ");
}
