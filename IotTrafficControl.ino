#include <SoftwareSerial.h>

const int redLed[] = {2, 5, 8, 11};
const int yellowLed[] = {3, 6, 9, 12};
const int greenLed[] = {4, A5, 10, 13};

const int irSensor[] = {A0, A1, A2, A3};
const int buzzerPin = A4;

SoftwareSerial espSerial(6, 7);  // RX, TX

const int interval = 2500;  // Interval for each light state in milliseconds

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(redLed[i], OUTPUT);
    pinMode(yellowLed[i], OUTPUT);
    pinMode(greenLed[i], OUTPUT);
    pinMode(irSensor[i], INPUT);
  }

  pinMode(buzzerPin, OUTPUT);

  // Turn off all LEDs initially
  turnOffAllLights();
}





void loop() {
  int sensor[4];
  bool highTraffic = false;
  int highTrafficLane = -1;

  // Read sensors
  for (int i = 0; i < 4; i++) {
    sensor[i] = digitalRead(irSensor[i]);
    if (sensor[i] == LOW) {
      highTraffic = true;
      highTrafficLane = i;
    }
  }





  if (highTraffic) {
    handleHighTraffic(highTrafficLane, sensor);
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn off the buzzer when density drops
    normalTrafficLightSequence();
  }
}




void handleHighTraffic(int lane, int sensor[]) {
  digitalWrite(buzzerPin, HIGH);  // Turn on the buzzer

  // Set all lanes to red except the high traffic lane to green
  setTrafficLights(lane);

  // Send traffic data to ESP8266
  String data = String(sensor[0]) + "," + String(sensor[1]) + "," + String(sensor[2]) + "," + String(sensor[3]);
  espSerial.println(data);

  Serial.println("Data sent to ESP8266: " + data);

  delay(1000);  // Delay for 1 second
}

void setTrafficLights(int lane) {
  turnOffAllLights();

  Serial.print("Setting traffic lights. High traffic lane: ");
  Serial.println(lane);

  for (int i = 0; i < 4; i++) {
    if (i == lane) {
      digitalWrite(greenLed[i], HIGH);
      Serial.print("Green LED on lane ");
      Serial.print(i);
      Serial.println(" is ON");
    } else {
      digitalWrite(redLed[i], HIGH);
      Serial.print("Red LED on lane ");
      Serial.print(i);
      Serial.println(" is ON");
    }
  }
}

void turnOffAllLights() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(redLed[i], LOW);
    digitalWrite(yellowLed[i], LOW);
    digitalWrite(greenLed[i], LOW);
  }
  Serial.println("All lights turned off");
}

void normalTrafficLightSequence() {
  int lanePairs[4][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};

  for (int i = 0; i < 4; i++) {
    int lane1 = lanePairs[i][0];
    int lane2 = lanePairs[i][1];


    // Set all lanes to red initially
    for (int j = 0; j < 4; j++) {
      digitalWrite(redLed[j], HIGH);
      digitalWrite(yellowLed[j], LOW);
      digitalWrite(greenLed[j], LOW);
    }

    // Set current pair of lanes to green
    digitalWrite(redLed[lane1], LOW);     // Turn off red
    digitalWrite(redLed[lane2], LOW);     // Turn off red
    digitalWrite(greenLed[lane1], HIGH);  // Turn on green
    digitalWrite(greenLed[lane2], HIGH);  // Turn on green
    delay(interval);                      // Green light duration

    digitalWrite(greenLed[lane1], LOW);    // Turn off green
    digitalWrite(greenLed[lane2], LOW);    // Turn off green
    digitalWrite(yellowLed[lane1], HIGH);  // Turn on yellow
    digitalWrite(yellowLed[lane2], HIGH);  // Turn on yellow
    delay(interval / 2);                   // Yellow light duration (shorter)

    digitalWrite(yellowLed[lane1], LOW);  // Turn off yellow
    digitalWrite(yellowLed[lane2], LOW);  // Turn off yellow
  }
}
