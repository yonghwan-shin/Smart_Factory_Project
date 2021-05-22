#define echoPin 32
#define trigPin 33
#include "WiFi.h"

int IRpin = 34;
int sensor_val;
float dist;


long duration;
int distance;

void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("WiFi Setup done");

  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  }
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
      char ssid[] = "JE_InP_Fac" ;
      char pass[] = "142536nm@@";
      String string_ssid = ssid;
      if (WiFi.SSID(i).equals(ssid) == 0) {
        WiFi.begin(ssid, pass);

        while (WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.println("loading..");
        }
        Serial.print("Connected to network. My IP address is: ");
        Serial.println(WiFi.localIP());
        break;
      }
    }
  }
  Serial.println("");
  delay(5000);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.0343 / 2; // Speed of sound wave divided by 2 (go and back)
  Serial.print("Sonar Distance: ");
  Serial.print(distance);
  Serial.print(" cm");




  sensor_val = map(analogRead(IRpin), 0, 4096, 0, 3300);
  dist = (24.61 / (sensor_val - 0.1696)) * 1000;
  Serial.print("  laser ");
  Serial.print(dist);
  Serial.println(" cm");
}
