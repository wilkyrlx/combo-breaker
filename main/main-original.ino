#include "Arduino.h"
#include "Keypad.h"

// ======================== SETUP ==============================================
// Define stepper motor connections and steps per revolution:
int dirPin = 4;
int stepPin = 3;
int stepsPerRevolution = 5;

// decoder variables
int comboA;
int comboB;
float comboC;

// keypad variables
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {12, 5, 6, 10}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 13, 9};    // connect to the column pinouts of the keypad
// The order from left to right is COL2, ROW1, COL1, ROW4, COL3, ROW3, ROW2
// therefore, col2 is 13, row1 is 12, col1 is 11, row4 is 10 etc

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String keysPressed;
String fullKeycode;
//--------------------------------------------------------------

//--------------------------------------------------------------
// switch variables --------------------------------------------
int resetSwitchPin = 2; // the lever which determines if a lock is being cracked. When it is switched on, a new combo can be entered
bool memory[5];
//--------------------------------------------------------------
// stepper variables --------------------------------------------
int a;   // first position
int b;   // second position
float c; // resistant location
//---------
int cINT;      // c*10
int first;     // potential # for 1st
int second[8]; // potential # for 2nd
int third[2];  // potential # for 3rd
int potThird[8];
int moduloPlace;
int i = 0;
int iB = 0;
int iThird = 0;
int iSecond = 0;
int minusModulo;
int mmAnswer;
int thirdA;
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial monitor connection established");
    // stepper
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    // reset switch
    pinMode(resetSwitchPin, INPUT);
}

void loop()
{
    // state of the reset switch

    memory[4] = memory[3];
    memory[3] = memory[2];
    memory[2] = memory[1];
    memory[1] = memory[0];
    memory[0] = getSwitchState();
    // keypad stuff
    if (memory[0] == true && memory[1] == true && memory[2] == true && memory[3] == true && memory[4] == true)
    {
        char c = getKeyPressed();
        if (c != NO_KEY)
        {
            fullKeycode += c;
            Serial.println(fullKeycode);
            if (c == '#')
            {
                decodeCombo(fullKeycode);
            }
        }
    }
    else
    {
        fullKeycode = "";
        Serial.println("keycode cleared, resetting...");
    }
}
//-------------------------------------------------------------
void tryAnswers()
{
    for (int i = 0; i < 10; i++)
    {
        enterCombo(first, second[i], third[0]);
        Serial.print(first);
        Serial.print(second[i]);
        Serial.println(third[0]);
    }
    for (int i = 0; i < 10; i++)
    {
        enterCombo(first, second[i], third[1]);
        Serial.print(first);
        Serial.print(second[i]);
        Serial.println(third[1]);
    }
}
//--------------------------------------------------------------

void calculateCombo()
{
    cINT = c * 10;
    first = c + 5.5f;

    moduloPlace = first % 4;
    minusModulo = first - 2;
    while (i != 4)
    {
        potThird[i] = (a % 10) + (i * 10);
        i++;
    }

    while (i != 8 && i > 3)
    {
        potThird[i] = (b % 10) + ((i - 4) * 10);
        i++;
    }
    i = 0;
    while (i != 8)
    {
        if ((potThird[i]) % 4 == moduloPlace)
        {
            third[iThird] = potThird[i];
            thirdA = thirdA + potThird[i];
            iThird++;
        }
        i++;
    }
    mmAnswer = minusModulo % 10;
    while (iSecond != 10)
    {
        second[iSecond] = mmAnswer + (4 * iSecond);
        iSecond++;
    }

    SendAnswers();
}

void SendAnswers()
{
    Serial.println("First Number:");
    if (cINT % 10 == 5)
    {
        first = c + 5.5f;
        Serial.println(c + 5.5f);
    }
    else
    {
        first = c + 5;
        Serial.println(c + 5);
    }
    Serial.println("----------------------");
    iSecond = 0;
    Serial.println("Second Numbers:");
    while (iSecond != 10)
    {
        Serial.println(second[iSecond]);
        iSecond++;
    }
    Serial.println("----------------------");
    Serial.println("Third Numbers:");
    Serial.println(third[0]);
    Serial.println(third[1]);

    //----
    tryAnswers(); // send answers to be entered by the stepper motor
}

//-------------------------------------
//-------------------------------------
//------------Blackbox-----------------
//-------------------------------------
//-------------------------------------
char getKeyPressed() // get key pressed for keypad
{
    char key = keypad.getKey();

    if (key != NO_KEY)
    {
        Serial.println(key);
    }
    return key;
}

bool getSwitchState() // reset switch
{
    int switchState = digitalRead(resetSwitchPin);
    if (switchState == LOW)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------

void decodeCombo(String raw) // turn combo into something machine readable, send it to the decoder
{
    Serial.println(raw + "raw");
    //-------------------combo a
    if (raw[0] == "0")
    {
        comboA = (raw[1]) - '0';
    }
    else
    {
        comboA = (10 * ((raw[0]) - '0')) + ((raw[1]) - '0');
    }
    //-------------------combo b
    if (raw[2] == "0")
    {
        comboB = (raw[3]) - '0';
    }
    else
    {
        comboB = (10 * ((raw[2]) - '0')) + ((raw[3]) - '0');
    }
    //-------------------combo c
    if (raw[4] == "0")
    {
        comboC = (raw[5]) - '0';
    }
    else
    {
        comboC = (10 * ((raw[4]) - '0')) + ((raw[5]) - '0');
    }
    if (raw[6] == '*')
    {
        comboC = comboC + 0.5f;
    }

    if (comboA != 0 || comboB != 0 || comboC != 0)
    {
        a = comboA;
        b = comboB;
        c = comboC;
        Serial.println(comboC);
        calculateCombo();
    }
}
//---------------------------------------------------------------------------
// stepper code
//---------------------------------------------------------------------------
void spin(int dir, int ticks)
{
    // 0 is left/counterclockwise
    // 1 is right/clockwise
    if (dir == 0)
    {
        digitalWrite(dirPin, HIGH);
        Serial.println("HIGH");
    }
    else
    {
        digitalWrite(dirPin, LOW);
        Serial.println("LOW");
    }

    for (int i = 0; i < stepsPerRevolution * ticks; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
    }

    delay(1000);
}
void enterCombo(int ax, int bx, int cx)
{
    // tries out a combo of three numbers
    delay(100);
    spin(0, 40 - ax);
    spin(1, (40 + (40 - ax) + bx));
    spin(0, bx + 40 - cx);
    delay(3000);
    spin(0, cx + 80);
}
//------------------------------
