#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Atakarim";
const char* password = "123123123";
const String apiKey = "C8Z5TSHHQRQ2EEDK";
const String server = "api.thingspeak.com";

void setup() {
  Serial.begin(9600); // Set the same baud rate as Arduino's SoftwareSerial
  delay(100);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // Check if data is available from the Arduino
  if (Serial.available() > 0) {
    // Read the data from the Arduino
    String arduinoData = Serial.readStringUntil('\n');
    Serial.println("Data received from Arduino: " + arduinoData);

    // Split the data into individual sensor readings
    int sensorValues[4]; // Assuming 4 sensors
    int currentIndex = 0;
    int commaIndex = arduinoData.indexOf(',');

    for (int i = 0; i < 4; i++) {
      if (commaIndex != -1) {
        sensorValues[i] = arduinoData.substring(0, commaIndex).toInt();
        arduinoData = arduinoData.substring(commaIndex + 1);
        commaIndex = arduinoData.indexOf(',');
      } else {
        sensorValues[i] = arduinoData.toInt();
      }
    }

    // Print sensor values for debugging
    Serial.print("Sensor values: ");
    for (int i = 0; i < 4; i++) {
      Serial.print(sensorValues[i]);
      if (i < 3) Serial.print(", ");
    }
    Serial.println();

    // Check if high density is detected (assuming any sensor value is LOW)
    bool highDensityDetected = false;
    for (int i = 0; i < 4; i++) {
      if (sensorValues[i] == LOW) {
        highDensityDetected = true;
        break;
      }
    }

    if (highDensityDetected) {
      Serial.println("High traffic detected");
      // Construct the URL for ThingSpeak API
      String url = "/update?api_key=" + apiKey + "&field1=" + String(sensorValues[0]) + "&field2=" + String(sensorValues[1]) + "&field3=" + String(sensorValues[2]) + "&field4=" + String(sensorValues[3]);

      // Create a WiFiClient object
      WiFiClient client;

      // Create an HTTP client object and make a HTTP POST request to ThingSpeak
      HTTPClient http;
      if (http.begin(client, server, 80, url)) {
        int httpCode = http.POST("");

        // Check for successful upload
        if (httpCode > 0) {
          Serial.print("HTTP POST request to ThingSpeak successful, response code: ");
          Serial.println(httpCode);
        } else {
          Serial.print("HTTP POST request to ThingSpeak failed, error code: ");
          Serial.println(httpCode);
        }

        // Close connection
      } else {
        Serial.println("Failed to connect to ThingSpeak");
      }
    } else {
      Serial.println("No high traffic detected");
    }
  }
}
