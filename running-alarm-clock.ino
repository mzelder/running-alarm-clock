#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define left1 14
#define left2 27
#define right1 26
#define right2 25
#define front_trig 33
#define front_echo 32 
#define right_trig 17
#define right_echo 18
#define left_trig 19
#define left_echo 21

// Define software serial pins for DFPlayer Mini
static const uint8_t PIN_MP3_TX = 22; 
static const uint8_t PIN_MP3_RX = 23; 

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

int cm;
int distance;
long TIME;
int counter = 0;

void setup() {
  Serial.begin(115200);
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");
    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Play the first MP3 file on the SD card
    player.play(1);
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);

  pinMode(front_trig, OUTPUT);
  pinMode(front_echo, INPUT);
  
  pinMode(right_trig, OUTPUT);
  pinMode(right_echo, INPUT);
  
  pinMode(left_trig, OUTPUT);
  pinMode(left_echo, INPUT);
}

void loop() {
  int frontDistance = get_distance(front_trig, front_echo);
  int rightDistance = get_distance(right_trig, right_echo);
  int leftDistance = get_distance(left_trig, left_echo);

  Serial.print("Front Distance: ");
  Serial.println(frontDistance);

  
  Serial.print("Right Distance: ");
  Serial.println(rightDistance);
  
  Serial.print("Left Distance: ");
  Serial.println(leftDistance);

  if (frontDistance < 40) {
    // Obstacle detected in front
    stop(100);
    backward();
    delay(300);
    stop(100);
    
    int rightDistance = get_distance(right_trig, right_echo);
    int leftDistance = get_distance(left_trig, left_echo);
    if (leftDistance > rightDistance) {
      left();
    } else {
      right();
    }
  } else {
    // No obstacle detected, move forward
    forward();
  }
}

// CALCULATE DISTANCE
int get_distance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  long duration = pulseIn(echo, HIGH, 30000); 
  if (duration == 0) {
    return 999; 
  }
  int distance = duration / 58.2;
  return distance;
}

// ROBOT MOVE FUNCTIONS
void forward() {
  analogWrite(right1, 255);
  analogWrite(right2, 0);
  analogWrite(left1, 200);
  analogWrite(left2, 0);
}

void backward() {
  analogWrite(right1, 0);
  analogWrite(right2, 255);
  analogWrite(left1, 0);
  analogWrite(left2, 200);
}

void stop(int TIME) {
  digitalWrite(right1, LOW);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(left2, LOW);
  delay(TIME);
}

void left() {
  digitalWrite(right1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW); 
  digitalWrite(left2, LOW);
  analogWrite(right1, 255);
  analogWrite(right2, LOW);
  analogWrite(left1, LOW); 
  analogWrite(left2, LOW);
  delay(300);
  stop(100);
}

void right() {
  digitalWrite(right1, LOW);
  digitalWrite(right2, LOW);
  digitalWrite(left1, HIGH);
  digitalWrite(left2, LOW);
  analogWrite(right1, LOW);
  analogWrite(right2, LOW);
  analogWrite(left1, 230); 
  analogWrite(left2, LOW);
  delay(300);
  stop(100);
}

void turn180() {
  digitalWrite(right1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW); 
  digitalWrite(left2, LOW);
  analogWrite(right1, 255);
  analogWrite(right2, LOW);
  analogWrite(left1, LOW); 
  analogWrite(left2, LOW);
  delay(700);
  stop(100);
}
