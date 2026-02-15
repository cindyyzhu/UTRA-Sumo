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
#define MAX_SPEED 255
#define SEARCH_SPEED 150
#define ATTACK_DISTANCE 36.5   // cm
#define IR_CONFIRM_TIME 25     // ms (noise filter)

// ===== SETUP =====
void setup() {
  // Motor pins
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  // Sensors
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  digitalWrite(STBY, HIGH); // ✅ ENABLE motors

  Serial.begin(9600);
  Serial.println("=== Sumo Robot Running ===");
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
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, SEARCH_SPEED);
  analogWrite(PWMB, SEARCH_SPEED);
}

// ===== ULTRASONIC =====
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

// ===== IR CONFIRMATION (non-blocking) =====
bool dangerBelow() {
  // simple check, non-blocking
  return digitalRead(IR_PIN) == HIGH; // 
}

// ===== MAIN LOOP =====
void loop() {
  long distance = getDistance();
  int irState = digitalRead(IR_PIN);

  // ---- SERIAL OUTPUT: sensor readings ----
  Serial.print("Distance: ");
  if (distance == -1) Serial.print("No echo");
  else Serial.print(distance); Serial.print(" cm");

  Serial.print(" | IR: ");
  Serial.print(irState == LOW ? "HIGH (trigged)" : "LOW (safe)");

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
