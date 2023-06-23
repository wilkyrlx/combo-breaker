#include "Arduino.h"
#include "Keypad.h"

// ======================== SETUP ==============================================
// Define stepper motor connections and steps per revolution:
int dirPin = 4;
int stepPin = 3;
int stepsPerRevolution = 5;

int resetSwitchPin = 2;

// Setup keypad variables
// The order from left to right is COL2, ROW1, COL1, ROW4, COL3, ROW3, ROW2
// therefore, col2 is 13, row1 is 12, col1 is 11, row4 is 10 etc
const byte ROWS = 4;  // four rows
const byte COLS = 3;  // three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
byte rowPins[ROWS] = {12, 5, 6,
                      10};  // connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 13,
                      9};  // connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Global variable
int firstNum;
int secondNum[10];
int thirdNum[2];

// ======================== FUNCTIONS ==========================================
void setup() {
    Serial.begin(9600);
    Serial.println("Serial monitor connection established");
    pinMode(stepPin, OUTPUT);        // stepper pin - controls # of steps
    pinMode(dirPin, OUTPUT);         // stepper pin - controls direction
    pinMode(resetSwitchPin, INPUT);  // reset switch pin
}

void loop() {
    char keyCode[7] = getKeyCode();
    calculateCombos(keyCode);
    printAnswers();
}

char* getKeyCode() {
    char keyBuffer[7];
    int keyBufferIndex = 0;

    while (true) {
        char c = keypad.getKey();

        if (c != NO_KEY && c != '#' && c != '*') {
            Serial.println(c);
            keyBuffer[keyBufferIndex] = c;
            keyBufferIndex++;
        }

        // try to enter code
        if (c == '#') {
            if (keyBufferIndex == 6) {
                Serial.println("Sending full code");
                Serial.println(keyBuffer);
                keyBufferIndex = 0;
                return keyBuffer;
            } else {
                Serial.println("Key buffer not full");
            }
        }

        // reset code
        if (c == '*') {
            Serial.println("Resetting key buffer");
            keyBufferIndex = 0;
        }
    }

    // should never happen
    return [];
}

void calculateCombos(char keyCode[]) {
    int firstLockedPosition = parseCharToInt(keyCode[0], keyCode[1], '0');
    int secondLockedPosition = parseCharToInt(keyCode[2], keyCode[3], '0');
    float resistantLocation =
        parseCharToInt(keyCode[4], keyCode[5], keyCode[6]);

    // Calculate first number
    // TODO: verify we want to return floor of this
    firstNum = resistantLocation + 5.5f;
    int moduloPlace = firstNum % 4;
    int minusModuloAnswer = (firstNum - 2) % 10;

    // Calculate second numbers
    for (int i = 0; i < 10; i++) {
        secondNum[i] = minusModuloAnswer + (4 * i);
    }

    // Calculate third numbers
    for (int i = 0; i < 4; i++) {
        potThird[i] = (firstLockedPosition % 10) + (i * 10);
    }
    for (int i = 4; i < 8; i++) {
        potThird[i] = (secondLockedPosition % 10) + ((i - 4) * 10);
    }
    for (int i = 0; i < 8; i++) {
        if ((potThird[i]) % 4 == moduloPlace) {
            thirdNum[i] = potThird[i];
        }
    }
}

float parseCharToInt(char first, char second, char decimalPlace) {
    int tensPlace = first - '0';
    int onesPlace = second - '0';
    float decimalPlace = (decimalPlace - '0' == 0) ? 0 : 0.5;
    return (tensPlace * 10) + onesPlace + decimalPlace;
}

void printAnswers() {
    Serial.println("First Number:");
    Serial.println(firstNum);
    Serial.println("----------------------");
    Serial.println("Second Numbers:");
    for (int i = 0; i < 10; i++) {
        Serial.print(secondNum[i] + " ");
    }
    Serial.println();
    Serial.println("----------------------");
    Serial.println("Third Numbers:");
    Serial.println(third[0] + " " + third[1]);
}

void tryAnswers() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 2; j++) {
            int potentialSecond = secondNum[i];
            int potentialThird = thirdNum[j];
            // Removes two possibilities due to manufacturing practices, +/- 2
            if (abs(potentialSecond - potentialThird) != 2) {
                enterCombo(firstNum, potentialSecond, potentialThird);
            }
        }
    }
}

// tries out a combo of three numbers
void enterCombo(int pinA, int pinB, int pinC) {
    Serial.println("Combo: " + pinA + " " + pinB + " " + pinC);
    delay(100);
    spin(0, 40 - pinA);
    spin(1, (40 + (40 - pinA) + pinB));
    spin(0, pinB + 40 - pinC);
    delay(3000);
    spin(0, pinC + 80);
}

void spin(int dir, int ticks) {
    // 0 is left/counterclockwise
    // 1 is right/clockwise
    if (dir == 0) {
        digitalWrite(dirPin, HIGH);
        Serial.println("HIGH");
    } else {
        digitalWrite(dirPin, LOW);
        Serial.println("LOW");
    }

    for (int i = 0; i < stepsPerRevolution * ticks; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
    }

    delay(1000);
}
