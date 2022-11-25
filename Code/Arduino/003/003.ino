#include <Servo.h>

Servo myservo;    // lid servo

// serial vars:
int var1;
int var2;
int var3;
int var4;
int var5;
int var6;
int check1;
int check2;

int sw1;
int sw2;
int sw3;
int sw4;

int sw1a;
int sw2a;
int sw3a;
int sw4a;

int location = 0;

int driveFlag1 = 0;
int driveFlag2 = 0;
int driveFlag3 = 0;
int driveFlag4 = 0;

unsigned long currentMillis;
long previousMillis = 0;    // set up timers
long interval = 10;        // time constant for timer
long previousDriveMillis = 0;    // set up timers

#define encoder0PinA 2      // encoder 0
#define encoder0PinB 3
#define encoder1PinA 20      // encoder 1
#define encoder1PinB 21

volatile long encoder0Pos = 0;    // encoder 0      // left wheel
volatile long encoder1Pos = 0;    // encoder 1      // right wheel

int encoder0Demand;   // left wheel
int encoder1Demand;   // right wheel

int wheel0;
int wheel0a;
int wheel1;
int wheel1a;

void setup() {
  
  Serial.begin(115200);      // Debug
  Serial1.begin(115200);      // serial comms to Nano
  Serial2.begin(115200);      // serial comms to Nano 

  pinMode(encoder0PinA, INPUT_PULLUP);    // encoder pins 0
  pinMode(encoder0PinB, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);

  pinMode(encoder1PinA, INPUT_PULLUP);    // encoder pins 1
  pinMode(encoder1PinB, INPUT_PULLUP);
  attachInterrupt(2, doEncoderC, CHANGE);
  attachInterrupt(3, doEncoderD, CHANGE);

  pinMode(22, INPUT_PULLUP);    // switches
  pinMode(24, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);

  pinMode(4, OUTPUT);     // motors PWM
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  myservo.attach(36);   // lid servo
  myservo.write(140);   // closed  (20 - open)
}

 
void loop() {

    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {  // start timed event
  
        previousMillis = currentMillis;

        if (Serial1.available() > 1){
            check1 = Serial1.parseInt();
                if (check1 == -20) {               //wait for the check value to come around before reading the rest of the data
                    var1 = Serial1.parseInt();
                    var2 = Serial1.parseInt();
                    var3 = Serial1.parseInt();                    
                }
                else if (check1 == -40) {
                    //Serial.println("no image detection");
                }
        }

        if (check1 == -20) {

            //Serial.print(var1);
            //Serial.print(" , ");
            //Serial.println(var2);
        }


        if (Serial2.available() > 1){
            check2 = Serial2.parseInt();
                if (check2 == -20) {               //wait for the check value to come around before reading the rest of the data
                    var4 = Serial2.parseInt();  
            
                }
        }

        sw1a = digitalRead(22);    // go to the table
        sw2a = digitalRead(24);    // go to the counter
        sw3a = digitalRead(26);    // nothing switch
        sw4a = digitalRead(28);    // go home

        if (sw1a == 0 || var4 == 4) {
          sw1 = 0;
        }

        else if (sw2a == 0 || var4 == 2) {
          sw2 = 0;
        }

        else if (sw4a == 0 || var4 == 1) {
          sw4 = 0;
        }

        else {
          sw1 = 1;
          sw2 = 1;
          sw4 = 1;
        }

        // ***** go to the table ***

        if (sw1 == 0 && driveFlag1 == 0 && location == 0) {      // drive from home
          driveFlag1 = 1;
          wheel0 = 180;
          wheel1 = 180;
          previousDriveMillis =  currentMillis;                 
        }

        else if (driveFlag1 == 1 && var1 > 430 ) {  // drive until it sees the target
          driveFlag1 = 2;
          wheel0 = 0;
          wheel1 = 0; 
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag1 == 2 && currentMillis - previousDriveMillis >= 500) {  // turn
          driveFlag1 = 3;
          wheel0 = 100;
          wheel1 = -100;
          encoder0Pos = 0;     
          encoder1Pos = 0;
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag1 == 3 && encoder0Pos > 8000) {   // until it's turned 90'
          driveFlag1 = 4;
          wheel0 = 0;
          wheel1 = 0; 
          encoder0Pos = 0;     
          encoder1Pos = 0;  
          previousDriveMillis = currentMillis;           
        }

        else if (driveFlag1 == 4 && currentMillis - previousDriveMillis >= 500) {  // wait and then drive forward
          driveFlag1 = 5;
          wheel0 = 180;
          wheel1 = 180;  
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;      
        }

        else if (driveFlag1 == 5 && encoder0Pos > 32000) {  // until it's gone to the table, then stop
          driveFlag1 = 6;
          wheel0 = 0;
          wheel1 = 0;  
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag1 == 6 && currentMillis - previousDriveMillis >= 500) {  // wait 
          driveFlag1 = 0;
          myservo.write(20);   // open lid
          encoder0Pos = 0;     
          encoder1Pos = 0;
          location = 1; // it is at the table 
          previousDriveMillis = currentMillis;            
        }

        // *******************************
        // *** get home from the table ***
        //********************************

        if (sw4 == 0 && location == 1) {
          myservo.write(140);   // close lid
          driveFlag2 = 1;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 1 && currentMillis - previousDriveMillis >= 1000) {   // turn
          wheel0 = -100;
          wheel1 = 100;
          driveFlag2 = 2;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }
        else if (driveFlag2 == 2 && encoder0Pos < -15000) {     // until it's turned 180'
          wheel0 = 0;
          wheel1 = 0;
          driveFlag2 = 3;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 3 && currentMillis - previousDriveMillis >= 500) {     // wait, then drive forward
          wheel0 = 180;
          wheel1 = 180;
          driveFlag2 = 4;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 4 && encoder0Pos > 32000) {     // wait, then drive forward
          wheel0 = 0;
          wheel1 = 0;
          driveFlag2 = 5;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 5 && currentMillis - previousDriveMillis >= 500) {     // wait, then turn left
          wheel0 = -40;
          wheel1 = 40;
          driveFlag2 = 6;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 6 && var2 > 620 && var2 < 640) {     // until it sees the home target
          wheel0 = 0;
          wheel1 = 0;
          driveFlag2 = 7;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 7 && currentMillis - previousDriveMillis >= 500) {     // wait, and the drive forward
          wheel0 = 100;
          wheel1 = 100;
          driveFlag2 = 8;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 8 && currentMillis - previousDriveMillis >= 500) {     // wait, and the drive forward
          wheel0 = 100;
          wheel1 = 100;
          driveFlag2 = 10;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 10 && var1 > 430) {     // until the target is bigger
          wheel0 = 0;
          wheel1 = 0;
          driveFlag2 = 11;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 11 && currentMillis - previousDriveMillis >= 500) {     // wait, then turn
          wheel0 = 100;
          wheel1 = -100;
          driveFlag2 = 12;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 12 && encoder1Pos < -8000) {      // until it's turned 90'
          wheel0 = 100;
          wheel1 = -100;
          driveFlag2 = 13;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 13) {     // keep turning, but slower
          wheel0 = 40;
          wheel1 = -40;
          driveFlag2 = 14;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag2 == 14 && var2 > 650 && var2 < 670) {     // until the other target is in the middle
          wheel0 = 0;
          wheel1 = 0;
          driveFlag2 = 0;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          location = 0; // you are home
          previousDriveMillis = currentMillis;
        }

        // ************************
        // *** drive to counter ***
        // ************************

        if (sw2 == 0 && driveFlag3 == 0 && location == 0) {      // drive from home
          driveFlag3 = 1;
          wheel0 = 180;
          wheel1 = 180;
          previousDriveMillis =  currentMillis;                 
        }

        else if (driveFlag3 == 1 && var1 > 395 ) {  // drive until it sees the target
          driveFlag3 = 2;
          wheel0 = 0;
          wheel1 = 0; 
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag3 == 2 && currentMillis - previousDriveMillis >= 500) {  // turn
          driveFlag3 = 3;
          wheel0 = -100;
          wheel1 = 100;
          encoder0Pos = 0;     
          encoder1Pos = 0;
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag3 == 3 && encoder0Pos < -5000) {   // until it's turned 90'
          driveFlag3 = 4;
          wheel0 = 0;
          wheel1 = 0; 
          encoder0Pos = 0;     
          encoder1Pos = 0;  
          previousDriveMillis = currentMillis;           
        }

        else if (driveFlag3 == 4 && currentMillis - previousDriveMillis >= 500) {  // wait and then drive forward
          driveFlag3 = 5;
          wheel0 = 180;
          wheel1 = 180;  
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;      
        }

        else if (driveFlag3 == 5 && encoder0Pos > 17000) {  // until it's gone to the counter, then stop
          driveFlag3 = 6;
          wheel0 = 0;
          wheel1 = 0;  
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;            
        }

        else if (driveFlag3 == 6 && currentMillis - previousDriveMillis >= 500) {  // wait, the open the lid
          driveFlag3 = 0;
          myservo.write(20);   // open lid 
          encoder0Pos = 0;     
          encoder1Pos = 0;
          location = 2;   // you are at the counter 
          previousDriveMillis = currentMillis;            
        }

        // ********************************
        // *** go home from the counter ***
        // ********************************

        if (sw4 == 0 && location == 2) {
          myservo.write(140);   // close lid
          driveFlag4 = 1;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 1 && currentMillis - previousDriveMillis >= 1000) {   // wait then turn left
          wheel0 = 100;
          wheel1 = -100;
          driveFlag4 = 2;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }
        else if (driveFlag4 == 2 && encoder0Pos > 15000) {     // stop
          wheel0 = 0;
          wheel1 = 0;
          driveFlag4 = 3;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 3 && currentMillis - previousDriveMillis >= 500) {     // wait, then drive forward
          wheel0 = 100;
          wheel1 = 100;
          driveFlag4 = 4;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 4 && encoder0Pos > 19000) {     // until you get there
          wheel0 = 0;
          wheel1 = 0;
          driveFlag4 = 6;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 6 && currentMillis - previousDriveMillis >= 500) {    // wait, then turn right
          wheel0 = 40;
          wheel1 = -40;
          driveFlag4 = 7;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 7 && var2 > 630 && var2 < 650) {    // until you see the home target, then stop
          wheel0 = 0;
          wheel1 = 0;
          driveFlag4 = 8;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 8 && currentMillis - previousDriveMillis >= 500) {    // wait, then drive forward
          wheel0 = 100;
          wheel1 = 100;
          driveFlag4 = 9;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 9 && var1 > 430) {    // until the target is big enough
          wheel0 = 0;
          wheel1 = 0;
          driveFlag4 = 10;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 10 && currentMillis - previousDriveMillis >= 500) {    // wait, then turn
          wheel0 = -100;
          wheel1 = 100;
          driveFlag4 = 11;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 11 && encoder0Pos < -8000) {   // keep turning, but slower
          wheel0 = -40;
          wheel1 = 40;
          driveFlag4 = 12;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          previousDriveMillis = currentMillis;
        }

        else if (driveFlag4 == 12 && var2 > 630 && var2 < 650) {   // until you see the other target
          wheel0 = 0;
          wheel1 = 0;
          driveFlag4 = 0;
          encoder0Pos = 0;     
          encoder1Pos = 0; 
          location = 0;
          previousDriveMillis = currentMillis;
        }    

       
        
        // ******* drive wheels ********

        // *** wheel1 ***

        if (wheel0 > 0) {
          analogWrite(5, wheel0);
          analogWrite(4, 0);
        }
        else if (wheel0 < 0) {
          wheel0a = abs(wheel0);
          analogWrite(4, wheel0a);
          analogWrite(5, 0);
        }
        else {
          analogWrite(5, 0);
          analogWrite(4, 0);
        }



        // *** wheel2 ***

        if (wheel1 > 0) {
          analogWrite(6, wheel1);
          analogWrite(7, 0);
        }
        else if (wheel1 < 0) {
          wheel1a = abs(wheel1);
          analogWrite(7, wheel1a);
          analogWrite(6, 0);
        }
        else {
          analogWrite(6, 0);
          analogWrite(7, 0);
        } 

    } // end of timed loop
  

} // end of main loop
