int IRpin = 34; // Put sensor_value pin to 32
int sensor_val;
float dist;
#define echoPin 32 // attach pin 32 Arduino to pin Echo of HC-SR04
#define trigPin 33 //attach pin 33 Arduino to pin Trig of HC-SR04


// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement




void setup() {
  // put your setup code here, to run once:
pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
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
