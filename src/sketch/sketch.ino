// ----------------------------------------------------------------------
// Project: Kaprekar Calculator
// Description: Computes the Kaprekar constant or cycle, using a keypad and LCD.
// Developer: GueuleDeL0up
// Date: 2025-04-05
// Version: 1.0
// ----------------------------------------------------------------------

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Computes the Kaprekar sequence or cycle for a given number
int* computeKaprekarSequence(int startNum, int numberBase, int delayTime, int& sequenceLength) {
  const int MAX_SEQUENCE_LENGTH = 100;
  static int sequence[MAX_SEQUENCE_LENGTH];
  char numStr[10];

  sprintf(numStr, "%d", startNum);
  int numDigits = strlen(numStr);

  int seenValues[MAX_SEQUENCE_LENGTH];
  int sequenceCount = 0;

  while (true) {
    sprintf(numStr, "%0*d", numDigits, startNum);

    char asc[10], desc[10];
    strcpy(asc, numStr);
    strcpy(desc, numStr);

    for (int i = 0; i < numDigits - 1; i++) {
      for (int j = i + 1; j < numDigits; j++) {
        if (asc[i] > asc[j]) {
          char temp = asc[i];
          asc[i] = asc[j];
          asc[j] = temp;
        }
        if (desc[i] < desc[j]) {
          char temp = desc[i];
          desc[i] = desc[j];
          desc[j] = temp;
        }
      }
    }

    int diff = soustractionBase(String(desc), String(asc), numberBase).toInt();
    
    lcdDisplay(0, 1, String(desc) + " - " + String(asc));
    lcdDisplay(0, 2, "= " + String(diff));
    delay(delayTime);
    Serial.println(String(desc) + " - " + String(asc) + " = " + String(diff));

    for (int i = 0; i < sequenceCount; i++) {
      if (seenValues[i] == diff) {
        sequenceLength = 0;
        for (int j = i; j < sequenceCount; j++) {
          sequence[sequenceLength++] = seenValues[j];
        }
        return sequence;
      }
    }

    if (sequenceCount < MAX_SEQUENCE_LENGTH) {
      seenValues[sequenceCount] = diff;
      sequence[sequenceCount++] = diff;
    }

    startNum = diff;
  }
}

// soustractionBase
String soustractionBase(String A, String B, int baseN) {
  int len = A.length();
  String resultat = "";
  int emprunt = 0;

  for (int i = len - 1; i >= 0; i--) {
    int chiffreA = A[i] - '0';
    int chiffreB = B[i] - '0';

    int diff = chiffreA - chiffreB - emprunt;

    if (diff < 0) {
      diff += baseN;
      emprunt = 1;
    } else {
      emprunt = 0;
    }

    resultat = char(diff + '0') + resultat;
  }

  return resultat;
}

// displayProcessingKaprekar
void displayProcessingKaprekar() {
  lcdDisplay(4, 1, "Processing");
  lcdDisplay(6, 2, "Kaprekar");
  for (int i = 0; i < 3; i++) {
    lcdDisplay(6, 2, "Kaprekar   ");
    delay(250);
    lcdDisplay(6, 2, "Kaprekar.");
    delay(250);
    lcdDisplay(6, 2, "Kaprekar..");
    delay(250);
    lcdDisplay(6, 2, "Kaprekar...");
    delay(250);
  }
}

// Checks if all digits in a number are identical
bool hasAllIdenticalDigits(const String& numStr) {
  char firstDigit = numStr[0];
  for (int i = 1; i < numStr.length(); i++) {
    if (numStr[i] != firstDigit) {
      return false;
    }
  }
  return true;
}

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;
char keyMap[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 52, 50, 48, 46 };
byte colPins[COLS] = { 44, 42, 40, 38 };
Keypad keypad = Keypad(makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);

// Captures input from keypad until '*' is pressed
String getKeypadInput(int maxChar) {
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        return input;
      } else if (key == '#' && input.length() > 0) {
        input.remove(input.length() - 1);
        lcdDisplay(0, 2, "                    ");
        lcdDisplay(0, 2, input);
      } else if (maxChar <= input.length()) {
        continue;
      } else if (key >= '0' && key <= '9') {
        input += key;
        lcdDisplay(0, 2, "                    ");
        lcdDisplay(0, 2, input);
      }
    }
    delay(10);
  }
}

// Waits for '*' key to continue
void waitForUserConfirmation() {
  while (true) {
    if (keypad.getKey() == '*') break;
    delay(10);
  }
}

// Checks if a number is valid in a specific base
bool isValidInBase(String number, int base) {
  for (int i = 0; i < number.length(); i++) {
    int digit = number.charAt(i) - '0';
    if (digit >= base) return false;
  }
  return true;
}

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Print message on LCD
void lcdDisplay(int col, int row, const String& message) {
  lcd.setCursor(col, row);
  lcd.print(message);
}

// Clear LCD
void lcdClear() {
  lcd.clear();
  lcd.backlight();
}


/* =========================================== */

// setup
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcdClear();
  lcdDisplay(0, 0, "Hello! This is a");
  lcdDisplay(0, 1, "Kaprekar calculator!");
  lcdDisplay(0, 3, "[*] to continue...");
  waitForUserConfirmation();
}

// loop
void loop() {
  Serial.println();
  Serial.println("--- Start ---");

  // Base input
  lcdClear();
  lcdDisplay(0, 0, "Enter a base:");
  lcdDisplay(0, 1, "Between [2-10]");
  lcdDisplay(0, 3, "[*] to validate...");
  String numberBase;
  while (true) {
    numberBase = getKeypadInput(2);
    int baseVal = numberBase.toInt();
    if (baseVal < 2 || baseVal > 10) {
      lcdDisplay(0, 2, numberBase + " not in [2-10]");
    } else {
      break;
    }
  }

  // Number input
  lcdClear();
  lcdDisplay(0, 0, "Enter a number:");
  lcdDisplay(0, 1, "10 <= number");
  lcdDisplay(0, 3, "[*] to validate...");
  String userNumber;
  while (true) {
    userNumber = getKeypadInput(8);
    if (!isValidInBase(userNumber, numberBase.toInt())) {
      lcdDisplay(0, 2, userNumber + " not in base " + numberBase);
    } else if (userNumber.toInt() < 10) {
      lcdDisplay(0, 2, userNumber + " < 10");
    } else if (hasAllIdenticalDigits(userNumber)) {
      lcdDisplay(0, 2, userNumber + " is identical");
    } else {
      break;
    }
  }

  // Delay time input
  lcdClear();
  lcdDisplay(0, 0, "Enter a delay:");
  lcdDisplay(0, 1, "In [ms]");
  lcdDisplay(0, 3, "[*] to validate...");
  int delayTime;
  delayTime = getKeypadInput(8).toInt();

  // Kaprekar animation
  lcdClear();
  displayProcessingKaprekar();

  
  // Kaprekar logic
  lcdClear();
  lcdDisplay(0, 0, "Result (" + userNumber + ")" + numberBase + ":");
  lcdDisplay(0, 3, "[*] to restart...");
  int sequenceLength = 0;
  int* sequence = computeKaprekarSequence(userNumber.toInt(), numberBase.toInt(), delayTime, sequenceLength);

  // Display result
  lcdClear();
  lcdDisplay(0, 0, "Result (" + userNumber + ")" + numberBase + ":");
  lcdDisplay(0, 3, "[*] to restart...");
  String resultStr = "";
  for (int i = 0; i < sequenceLength; i++) {
    resultStr += String(sequence[i]);
    if (i < sequenceLength - 1) resultStr += "-";
  }

  if (40 < resultStr.length()) {
    lcdDisplay(0, 1, "Result too long,");
    lcdDisplay(0, 2, "Check Serial Monitor");
    Serial.println("Result (" + userNumber + ")" + numberBase + ": " + resultStr);
  } else {
    for (int i = 0; i < resultStr.length(); i += 40) {
      lcd.setCursor(0, 1);
      lcd.print(resultStr.substring(i, i + 20));
      lcd.setCursor(0, 2);
      lcd.print(resultStr.substring(i + 20, i + 40));
    }
  }

  Serial.println("--- End ---");

  // Wait for restart
  waitForUserConfirmation();
}
