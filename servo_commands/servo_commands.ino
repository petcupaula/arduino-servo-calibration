/* Example of reading servo number and angle from Serial 
   Format for moving servo number 0 to 120 degrees: <0,120>

   The code does not handle situations where the formatting is wrong.
   For example, if no integers provided, it will convert the empty string to int anyway, 
   and will default to servo 0 / angle 0. 

   Input parsing based on the example here: https://forum.arduino.cc/index.php?topic=396450.0

   Voltage supplied for servos used with this sketch: 6V
*/

#include <Adafruit_PWMServoDriver.h>

#define FREQUENCY 50

// Set the 'minimum' and 'maximum' pulse length count (out of 4096)
#define MG996R_SERVOMIN  140
#define MG996R_SERVOMAX  450
#define JX6221MG_SERVOMIN  120
#define JX6221MG_SERVOMAX  450

// variables for parsing data
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars]; // temporary array for use when parsing
int integerServo = 0;
int integerDegrees = 0;
boolean newData = false;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x42);

// hand_thumb
int servo_hand_thumb = 0;
int degree_hand_thumb_rest = 180;
int pulse_hand_thumb_min = JX6221MG_SERVOMIN;
int pulse_hand_thumb_max = JX6221MG_SERVOMAX;

// hand_index
int servo_hand_index = 1;
int degree_hand_index_rest = 180;
int pulse_hand_index_min = JX6221MG_SERVOMIN;
int pulse_hand_index_max = JX6221MG_SERVOMAX;

// hand_middle
int servo_hand_middle = 2;
int degree_hand_middle_rest = 160;
int pulse_hand_middle_min = MG996R_SERVOMIN;
int pulse_hand_middle_max = MG996R_SERVOMAX;

// hand_ring
int servo_hand_ring = 3;
int degree_hand_ring_rest = 0;
int pulse_hand_ring_min = MG996R_SERVOMIN;
int pulse_hand_ring_max = MG996R_SERVOMAX;

// hand_pinky
int servo_hand_pinky = 4;
int degree_hand_pinky_rest = 0;
int pulse_hand_pinky_min = MG996R_SERVOMIN;
int pulse_hand_pinky_max = MG996R_SERVOMAX;

// wrist_abduction/adduction
int servo_wrist_abd = 5;
int degree_wrist_abd_rest = 90;
int pulse_wrist_abd_min = JX6221MG_SERVOMIN;
int pulse_wrist_abd_max = JX6221MG_SERVOMAX;

// wrist_flexion/extension
int servo_wrist_flex = 6;
int degree_wrist_flex_rest = 60;
int pulse_wrist_flex_min = JX6221MG_SERVOMIN;
int pulse_wrist_flex_max = JX6221MG_SERVOMAX;


void setup() {
  Serial.begin(9600);
  Serial.println("This demo expects 2 integers");
  Serial.println("Example formatting for moving servo number 0 to 120 degrees: <0,120>");
  Serial.println();

  pwm.begin();
  pwm.setPWMFreq(FREQUENCY); 
  delay(10);
}

void loop() {
  
  receiveWithStartEndMarkers();
  
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;

    moveServoTo(integerServo, integerDegrees, pulse_hand_thumb_min, pulse_hand_thumb_max);
  }
}


void moveServoTo(int servonum, int degree, int servomin, int servomax) {
  int pulselen = map(degree, 0, 180, servomin, servomax);
  Serial.println(pulselen);
  pwm.setPWM(servonum, 0, pulselen);
}

void receiveWithStartEndMarkers() {
  static boolean receiveInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (receiveInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        receiveInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      receiveInProgress = true;
    }
  }
}

void parseData() {      
  // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars,",");      // get the first part - the string
  integerServo = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");    // this continues where the previous call left off
  integerDegrees = atoi(strtokIndx);     // convert this part to an integer

}

void showParsedData() {
  Serial.print("Servo ");
  Serial.print(integerServo);
  Serial.print(", Position ");
  Serial.println(integerDegrees);
}
