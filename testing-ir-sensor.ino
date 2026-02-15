#define IR_PIN 12   // D0 connected here

void setup() {
  pinMode(IR_PIN, INPUT_PULLUP);   // REQUIRED for LM393
  Serial.begin(9600);
  Serial.println("LM393 IR Sensor Test");
  Serial.println("WHITE = 0 (LED ON), BLACK = 1 (LED OFF)");
}

void loop() {
  int irValue = digitalRead(IR_PIN);

  Serial.print("IR raw value: ");
  Serial.print(irValue);

  if (irValue == LOW) {
    Serial.println("  -> WHITE detected");
  } else {
    Serial.println("  -> BLACK detected");
  }

  delay(300);
}
