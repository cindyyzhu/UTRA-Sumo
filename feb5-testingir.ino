int sensorPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop() {
  Serial.println(digitalRead(sensorPin));
  delay(200);
}
