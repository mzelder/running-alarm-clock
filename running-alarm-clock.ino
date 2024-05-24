#DEFINE right1 14
#DEFINE right2 27

#DEFINE left1 26
#DEFINE left2 25

#DEFINE front_trig 33
#DEFINE front_echo 32 

#DEFINE back_trig 4
#DEFINE back_echo 16

#DEFINE right_trig 17
#DEFINE right_echo 18

#DEFINE left_trig 19
#DEFINE left_echo 21

int cm;
int distance;
long TIME;

int counter = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);

  pinMode(front_trig, OUTPUT);
  pinMode(front_echo, INPUT);
  
  pinMode(back_trig, OUTPUT);
  pinMode(back_echo, INPUT);


}

void loop() {
  distance = get_distance();
  Serial.println(distance);
  
  forward();
  delay(1000);
  stop(1000);
}

// CALCULATE DISTANCE
int get_distance() {
  const int numReadings = 15;  // Number of measurements to average
  int readings[numReadings];  // Array to store the distance readings
  int total = 0;              // Total of all distance measurements
  int average = 0;            // Average distance
  
  for (int i = 0; i < numReadings; i++) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long time = pulseIn(echo, HIGH);
    readings[i] = time / 58;  // Convert time to distance and store in the array
    
    total += readings[i];     // Add the reading to the total
    delay(10);                // Short delay between measurements to stabilize sensor
  }
  
  average = total / numReadings;  // Calculate the average distance
  return average;                 // Return the average distance
}

// ROBOT MOVE FUNCTIONS
// right motor is slower than the left 
// so the left motor need to be slowed down 
// left -> 200 , right -> 255

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
  analogWrite(right1, 0);
  analogWrite(right2, 0);
  analogWrite(left1, 0);
  analogWrite(left2, 0);
  delay(TIME);
}

void left() {
  analogWrite(right1, 255);
  analogWrite(right2, 0);
  analogWrite(left1, 0); 
  analogWrite(left2, 0);
  delay(300);
}

void right() {
  analogWrite(right1, 0);
  analogWrite(right2, 0);
  analogWrite(left1, 200);
  analogWrite(left2, 0);
  delay(300);
}

void turn180() {
  analogWrite(right1, 255);
  analogWrite(right2, 0);
  analogWrite(left1, 0); 
  analogWrite(left2, 0);
  delay(700);
}

// CHOOSE WHERE GOING
void decideTurn() {
  if (random(2) == 0) {  // Randomly choose 0 or 1
    left();
  } else {
    right();
  }
}

