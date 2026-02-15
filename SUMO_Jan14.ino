// ===== MOTOR DRIVER (TB6612FNG - I think?) =====
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
#define MAX_SPEED 255
#define SEARCH_SPEED 150
#define ATTACK_DISTANCE 36.5   // cm
#define IR_CONFIRM_TIME 25   // ms (noise filter)

// ===== SETUP =====
void setup() {
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  digitalWrite(STBY, HIGH); // enable motors
}

// ===== MOTOR CONTROL =====
void forward(int speed) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
}

void backward(int speed) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
}

void search() {
  // spin in place to find opponent
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, SEARCH_SPEED);
  analogWrite(PWMB, SEARCH_SPEED);
}

// ===== ULTRASONIC =====
long getDistance() {
  // Make sure trigger starts LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  // Send 10 microsecond HIGH pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout (~5m)

  if (duration == 0) {
    // No object detected within range
    return -1;
  }

  // Convert time (microseconds) to distance in cm
  long distance = duration * 0.0343 / 2;

  // Optional: limit to max distance
  if (distance > 400) distance = 400; // max sensor range ~4m

  return distance;
}

// ===== IR CONFIRMATION (BLACK LINE / UNDER ROBOT) =====
bool dangerBelow() {
  unsigned long start = millis();
  while (millis() - start < IR_CONFIRM_TIME) {
    if (digitalRead(IR_PIN) == LOW) {
      return false; // safe floor detected again
    }
  }
  return true; // confirmed danger (edge or robot)
}

// ===== MAIN LOOP =====
void loop() {
  long distance = getDistance();
  int irState = digitalRead(IR_PIN);


  // DEFENSE MODE: another robot is underneath us
  if (digitalRead(IR_PIN) == LOW && dangerBelow()) {
    backward(MAX_SPEED);
    delay(250);  // back up enough to be safe
  }

  // ATTACK MODE
  else if (distance <= ATTACK_DISTANCE) {
    forward(MAX_SPEED);
  }

  // SEARCH MODE
  else {
    search();
  }

  // ---- SERIAL OUTPUT: sensor readings ----
  Serial.print("Distance: ");
  if (distance == -1) Serial.print("No echo");
  else Serial.print(distance); Serial.print(" cm");

  Serial.print(" | IR: ");
  Serial.print(irState == LOW ? "HIGH (safe)" : "LOW (triggered)");

  // ---- MOTOR DECISION ----
  if (dangerBelow()) {
    Serial.print(" | MODE: DEFENSE | Motor moving: BACKWARD");
    backward(MAX_SPEED);
  }
  else if (distance > 0 && distance <= ATTACK_DISTANCE) {
    Serial.print(" | MODE: ATTACK | Motor moving: FORWARD");
    forward(MAX_SPEED);
  }
  else {
    Serial.print(" | MODE: SEARCH | Motor moving: SEARCH SPIN");
    search();
  }

  Serial.println();
  delay(200); // small delay to avoid flooding Serial Monitor
  
}
