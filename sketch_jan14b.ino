// ===== MOTOR DRIVER (TB6612FNG) =====
#define STBY 8
#define PWMA 6
#define AIN1 2
#define AIN2 3
#define BIN1 4
#define BIN2 7
#define PWMB 5

// ===== SENSORS =====
#define TRIG_PIN 9
#define ECHO_PIN 10
#define IR_PIN 12   // MH-Sensor D0

// ===== CONSTANTS =====
#define ATTACK_DISTANCE 36.5   // cm
#define IR_CONFIRM_TIME 25     // ms

void setup() {
  // Motor pins (kept, but motors disabled)
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW); // DISABLE motors for safety

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("Sensor debug started...");
}

// ===== ULTRASONIC =====
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1; // no echo
  return duration * 0.034 / 2;
}

// ===== IR CONFIRMATION =====
bool dangerBelow() {
  unsigned long start = millis();
  while (millis() - start < IR_CONFIRM_TIME) {
    if (digitalRead(IR_PIN) == HIGH) {
      return false;
    }
  }
  return true;
}

void loop() {
  long distance = getDistance();
  int irState = digitalRead(IR_PIN);

  // ---- PRINT RAW SENSOR DATA ----
  Serial.print("Ultrasonic distance: ");
  if (distance == -1) {
    Serial.print("No echo");
  } else {
    Serial.print(distance);
    Serial.print(" cm");
  }

  Serial.print(" | IR state: ");
  Serial.print(irState == HIGH ? "HIGH (safe)" : "LOW (triggered)");

  // ---- INTERPRETATION ----
  if (irState == LOW && dangerBelow()) {
    Serial.print(" --> IR DANGER DETECTED");
  }
  else if (distance > 0 && distance <= ATTACK_DISTANCE) {
    Serial.print(" --> OBJECT IN RANGE");
  }
  else {
    Serial.print(" --> nothing detected");
  }

  Serial.println();
  delay(200);
}
