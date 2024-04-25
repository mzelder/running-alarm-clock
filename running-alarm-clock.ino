int right1 = 14;
int right2 = 27;

int left1 = 26;
int left2 = 25;

int trig = 33;
int echo = 32;
int cm;
int distance;
long TIME;

int counter = 0;


void setup() {
  Serial.begin(115200);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
}

void loop() {
  distance = get_distance();
  Serial.println(distance);
  
  if (counter == 3) {
    turn180();
    counter = 0;
  }
  else if (distance <= 50) {
    stop(500);
    decideTurn();
    counter++;
  } else {
    forward();
  }
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

