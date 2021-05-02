int IRpin = 32; // Put sensor_value pin to 32
int sensor_val;
float dist;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor_val = map(analogRead(IRpin), 0, 4096, 0, 3300);
  dist = (24.61 / (sensor_val - 0.1696)) * 1000;
  Serial.print(dist);
  Serial.println(" cm");
  delay(100);
}
