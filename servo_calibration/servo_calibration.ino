#include <Adafruit_PWMServoDriver.h>
#include <TM1637Display.h>

#define FREQUENCY 50
#define inputPotPin A0
#define minPotPin A1
#define maxPotPin A2
#define factorPotPin A3
#define servoNum 0
#define CLK 2
#define DIO 3

// PCA9685 using the address 0x42 (default is 0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// default values
int inputPotValue = 300;
int tmpPwmValue = 0;
int minPulse = 0;
int maxPulse = 4096;
long factor = 100; 

// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

void setup() {
  Serial.begin(9600);

  display.setBrightness(5);
  display.clear();
  delay(1000); 
  
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  delay(10);
}

void readConfig() {
  
  // minPulse: 0, 100, 500, 2000
  int minPulsePot = analogRead(minPotPin);
  switch (minPulsePot) {
    case 0 ... 255:
      minPulse = 0;
      break;
    case 256 ... 511:
      minPulse = 100;
      break;
    case 512 ... 767:
      minPulse = 500;
      break;
    case 768 ... 1023:
      minPulse = 2000;
      break;
    default:
      break;
  }
  
  // maxPulse: 500, 1000, 2000, 4096
  int maxPulsePot = analogRead(maxPotPin);
  switch (maxPulsePot) {
    case 0 ... 255:
      maxPulse = 500;
      break;
    case 256 ... 511:
      maxPulse = 1000;
      break;
    case 512 ... 767:
      maxPulse = 2000;
      break;
    case 768 ... 1023:
      maxPulse = 4096;
      break;
    default:
      break;
  }

  // factor: 10, 50, 100, 500
  int factorPot = analogRead(factorPotPin);
  switch (factorPot) {
    case 0 ... 255:
      factor = 10;
      break;
    case 256 ... 511:
      factor = 50;
      break;
    case 512 ... 767:
      factor = 100;
      break;
    case 768 ... 1023:
      factor = 500;
      break;
    default:
      break;
  }

  Serial.print("Min Pulse = ");
  Serial.print(minPulse);
  Serial.print(", Max Pulse = ");
  Serial.print(maxPulse);
  Serial.print(", Factor = ");
  Serial.println(factor);
  
}

void loop() {
  inputPotValue = analogRead(inputPotPin);

  readConfig();

  // Input pot value will be between 0 and 1024

  // From the Adafruit_PWMServoDriver library:
  // "Depending on your servo make, the pulse width min and max may vary, you 
  // want these to be as small/large as possible without hitting the hard stop
  // for max range. You'll have to tweak them as necessary to match the servos you
  // have!"
  // Values are from 0 to 4096

  int pwmValue = map(inputPotValue, 0, 1023, minPulse, maxPulse);
  
  // rounding
  long rem = pwmValue % factor;
  pwmValue -= rem;
  pwmValue += factor;
  
  pwm.setPWM(servoNum, 0, pwmValue);

  // Check to make sure we're not unnecessarily displaying duplicate data
  if (pwmValue != tmpPwmValue)
  {
    Serial.print("Potentiometer Input = ");
    Serial.print(inputPotValue);
    Serial.print(", PWM = ");
    Serial.println(pwmValue);

    display.clear();
    delay(50);
    display.showNumberDec(pwmValue);
    delay(50);
  }

  tmpPwmValue = pwmValue;

  delay(1000); //15
}
