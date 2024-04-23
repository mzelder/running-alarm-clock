#include "soc/rtc_cntl_reg.h"


int right1 = 14;
int right2 = 27;

int left1 = 26;
int left2 = 25;

void forward() {
  analogWrite(left1, 190);
  analogWrite(left2, 0);
  
  analogWrite(right1, 255);
  analogWrite(right2, 0);

}
void backward() {
  analogWrite(left1, 0);
  analogWrite(left2, 190);
  
  analogWrite(right1, 0);
  analogWrite(right2, 255);
}

void turnRight() {
  analogWrite(left1, 190);  // Left motor forward
  analogWrite(left2, 0);
  
  analogWrite(right1, 0);
  analogWrite(right2, 255);  // Right motor backward
}

void turnLeft() {
  analogWrite(left1, 0);
  analogWrite(left2, 255);  // Left motor backward
  
  analogWrite(right1, 255);  // Right motor forward
  analogWrite(right2, 0);
}

void turn180Degrees() {
  // Assuming turnRight() rotates the robot right
  turnRight();
  delay(3000);  // Adjust this delay to match the time it takes to rotate 180 degrees
}

void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
}

void loop() {
  //Going forwards
  forward();
  delay(1000); 

  backward();  // Rotate 180 degrees
  delay(1000);  // Wait for a second before repeating loop
}
