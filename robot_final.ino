#include <AFMotor.h>

//Potentiometer Analog Pins
int potPin1 = 7;
int potPin2 = 9;
int potPin3 = 11;
int potPin4 = 13;
int potPin5 = 15;

//Potentiometer Analog Values
int potVal1 = 0;
int potVal2 = 0;
int potVal3 = 0;
int potVal4 = 0;
int potVal5 = 0;

const int ledPin = 53;
const int motorSpeed = 255;

int movementStage1 = 0;
int movementStage2 = 0;
int calibrationStage = 0;
int turnCnt = 0;

int pot1Min = 210;
int pot1Max = 230;
int pot3Min = 25;
int pot3Max = 40;
int pot4Min = 126;
int pot4Max = 140;
int pot5Min = 175;
int pot5Max = 200; //Previously 228

int pot1Cal = 225;
int pot3Cal = 35;
int pot4Cal = 140;
int pot5Cal = 175;

int homingSequence = 0;
int playAreaSequence = 0;

int pot3Home = 35;
int pot4Home = 166;
int pot5Home = 135;

int pot3Help = 35;
// int pot4Help = 166;

int pot3Mid = 17;
int pot4Mid = 138;
int pot5Mid = 160;

int pot3Play = 2;
int pot4Play = 127;
int pot5Play = 200;

int angle = 0;
int done = 0;

AF_DCMotor M1(1, MOTOR12_1KHZ);
AF_DCMotor M3(2, MOTOR12_1KHZ);
AF_DCMotor M4(3, MOTOR34_1KHZ);
AF_DCMotor M5(4, MOTOR34_1KHZ);

void setup() {
    Serial.begin(19200);
    pinMode(ledPin, OUTPUT);

    M1.setSpeed(motorSpeed);
    M3.setSpeed(motorSpeed);
    M4.setSpeed(motorSpeed);
    M5.setSpeed(motorSpeed);

    potVal1 = analogRead(potPin1);
    potVal3 = analogRead(potPin3);
    potVal4 = analogRead(potPin4);
    potVal5 = analogRead(potPin5);
    potVal1 = map(potVal1, 0, 1023, 0, 270);
    potVal3 = map(potVal3, 0, 1023, 0, 270);
    potVal4 = map(potVal4, 0, 1023, 0, 270);
    potVal5 = map(potVal5, 0, 1023, 0, 270);

    homingSequence++;

}

void homing_sequence();
void playArea_sequence();

void loop() {
    homing_sequence();

    if (Serial.available() > 0){
        if (Serial.read() == 'A'){
            angle = Serial.parseInt();
            if (Serial.read() == 'D'){
                done = Serial.parseInt();
            }
        }
    }
    while (Serial.available() > 0){
        Serial.read();
    }
    if (done == 1){
        M1.run(RELEASE);
        M3.run(RELEASE);
        M4.run(RELEASE);
        M5.run(RELEASE);
        exit(0);
    }

    playArea_sequence(angle + 5);

    potVal1 = analogRead(potPin1);
    potVal3 = analogRead(potPin3);
    potVal4 = analogRead(potPin4);
    potVal5 = analogRead(potPin5);
    potVal1 = map(potVal1, 0, 1023, 0, 270);
    potVal3 = map(potVal3, 0, 1023, 0, 270);
    potVal4 = map(potVal4, 0, 1023, 0, 270);
    potVal5 = map(potVal5, 0, 1023, 0, 270);

    // Serial.print("M3 Home Value: ");
    // Serial.print(potVal3);
    // Serial.print(", ");
    // Serial.print("M4 Home Value: ");
    // Serial.print(potVal4);
    // Serial.print(", ");
    // Serial.print("M5HomeValue: ");
    // Serial.println(potVal5);

}

void homing_sequence(){
    // Homing Sequence
    // Go to top of Dice Tower
    switch (homingSequence){
        case 1:
            playAreaSequence = 0;
            if (potVal4 < pot4Home){
                M4.run(FORWARD);
            }
            else{
                M4.run(RELEASE);
                homingSequence++;
            }
        break;
        case 2:
            if (potVal5 > pot5Mid){
                M5.run(FORWARD);
            }
            else{
                M5.run(RELEASE);
                homingSequence++;
            }
        break;
        case 3:
            if (potVal3 < pot3Home){
                M3.run(BACKWARD);
            }
            else{
                M3.run(RELEASE);
                homingSequence++;
            }
        break;
        case 4:
            if (potVal5 > pot5Home){
                delay(500);
                M5.run(FORWARD);
            }
            else{
                M5.run(RELEASE);
                homingSequence++;
            }
        break;
        case 5:
            if (potVal1 < pot1Max){
                M1.run(BACKWARD);
            }
            else{
                M1.run(RELEASE);
                homingSequence++;
            }
        break;
        case 6:
            M1.run(RELEASE);
            M3.run(RELEASE);
            M4.run(RELEASE);
            M5.run(RELEASE);
            playAreaSequence++;
        break;
        default:
            break;
    }
}

void playArea_sequence(int diceAngle){
    // Play Area Sequence
    // Go to Play Area
    switch (playAreaSequence){
        case 1:
            homingSequence = 0;
            if (potVal5 < pot5Mid){
                M5.run(BACKWARD);
            }
            else{
                M5.run(RELEASE);
                playAreaSequence++;
            }
        break;
        case 2:
            if (potVal3 > pot3Mid){
                M3.run(FORWARD);
            }
            else{
                M3.run(RELEASE);
                playAreaSequence++;
            }
        break;
        case 3:
            if (potVal5 < (pot5Mid + diceAngle) && potVal3 <= pot3Mid){
                delay(500);
                M5.run(BACKWARD);
            }
            else{
                M5.run(RELEASE);
                playAreaSequence++;
            }
        break;
        case 4:
            if (potVal4 > pot4Play){
                M4.run(BACKWARD);
            }
            else{
                M4.run(RELEASE);
                playAreaSequence++;
            }
        break;
        case 5:
            if (potVal1 > pot1Min){
                M1.run(FORWARD);
            }
            else{
                M1.run(RELEASE);
                playAreaSequence++;
            }
        break;
        case 6:
            M1.run(RELEASE);
            M3.run(RELEASE);
            M4.run(RELEASE);
            M5.run(RELEASE);
            homingSequence++;
        break;
        default:
            break;
    }
}