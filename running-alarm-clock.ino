#include <WiFi.h>
#include <WebServer.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <time.h>
#include <WiFiUdp.h>

// Motor control pins
#define left1 14
#define left2 27
#define right1 26
#define right2 25
#define front_trig 4
#define front_echo 16 
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

int alarmHour = -1;
int alarmMinute = -1;
bool alarmSet = false;

WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 7200;

void setup() {
  Serial.begin(115200);
  softwareSerial.begin(9600);

  // Initialize DFPlayer
  if (player.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini OK");
    player.volume(30);
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

  // Connect to Wi-Fi
  WiFi.begin("iPhone", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Configure time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Define web server routes
  server.on("/", handleRoot);
  server.on("/current_time", handleCurrentTime);
  server.on("/set_alarm", handleSetAlarm);
  server.onNotFound(handleNotFound);
  
  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Server responses
  server.handleClient();  // Handle web server requests



  if (alarmSet) {
    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }

    // Check if it's time to trigger the alarm
    if (timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) {
      Serial.println("Alarm triggered!");
      player.play(1);  // Play the first track
      while (alarmSet == true) {
        // Moving robot
        int frontDistance = get_distance(front_trig, front_echo);
        int rightDistance = get_distance(right_trig, right_echo);
        int leftDistance = get_distance(left_trig, left_echo);
        Serial.println(frontDistance);
        Serial.println(rightDistance);
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
        // if (button == "pressed") {
        //alarmSet = false; 
        //}  
      }
    }
  }
}

// Web server handlers
void handleRoot() {
  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char currentTime[9];
  sprintf(currentTime, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  String html = "<html><head><style>";
  html += "body { font-family: 'Arial', sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; background-color: #0b3d91; color: #ffffff; }";
  html += "h1 { font-size: 48px; }";
  html += ".time-display { font-size: 96px; margin-bottom: 40px; }";
  html += ".alarm-setup { display: flex; align-items: center; }";
  html += ".time-picker { display: flex; flex-direction: column; align-items: center; margin: 0 10px; }";
  html += ".time-picker input { font-size: 48px; width: 100px; text-align: center; border: 2px solid #ffffff; background-color: #0b3d91; color: #ffffff; margin-bottom: 10px; }";
  html += "form { display: flex; flex-direction: column; align-items: center; }";
  html += "input[type=submit] { font-size: 24px; padding: 10px 20px; margin-top: 20px; background-color: #ffffff; border: none; color: #0b3d91; cursor: pointer; }";
  html += "</style></head><body>";
  html += "<div class='time-display' id='current-time'>" + String(currentTime) + "</div>"; // Display current time
  
  // Display the set alarm time if an alarm is set
  if (alarmSet) {
    char alarmTime[6];
    sprintf(alarmTime, "%02d:%02d", alarmHour, alarmMinute);
    html += "<div class='alarm-display'>Alarm set for: " + String(alarmTime) + "</div>";
  }

  html += "<form action='/set_alarm' method='GET'>";
  html += "<div class='alarm-setup'>";
  html += "<div class='time-picker'>";
  html += "<input type='text' id='hour' name='hour' pattern='[0-2][0-9]' placeholder='HH'>";
  html += "</div>";
  html += "<span>:</span>";
  html += "<div class='time-picker'>";
  html += "<input type='text' id='minute' name='minute' pattern='[0-5][0-9]' placeholder='MM'>";
  html += "</div>";
  html += "</div>";
  html += "<input type='submit' value='SET ALARM'>";
  html += "</form>";
  html += "<script>";
  html += "function updateTime() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/current_time', true);";
  html += "  xhr.onload = function() {";
  html += "    if (xhr.status === 200) {";
  html += "      document.getElementById('current-time').innerHTML = xhr.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhr.send();";
  html += "}";
  html += "setInterval(updateTime, 1000);"; // Update time every second
  html += "</script></body></html>";
  server.send(200, "text/html", html);
}

void handleCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    server.send(500, "text/plain", "Failed to obtain time");
    return;
  }
  char currentTime[9];
  sprintf(currentTime, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  server.send(200, "text/plain", currentTime);
}

void handleSetAlarm() {
  if (server.hasArg("hour") && server.hasArg("minute")) {
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();
    if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60) {
      alarmHour = hour;
      alarmMinute = minute;
      alarmSet = true;
      server.sendHeader("Location", "/");
      server.send(303);
    } else {
      server.send(400, "text/html", "Invalid time values");
    }
  } else {
    server.send(400, "text/html", "Invalid parameters");
  }
}

void handleNotFound() {
  server.send(404, "text/html", "Not Found");
}

// Calculate distance
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

// Robot move functions
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
