// ===== MOTOR DRIVER (TB6612FNG) =====
#define STBY 8
#define PWMA 6
#define AIN1 2
#define AIN2 3
#define BIN1 4
#define BIN2 7
#define PWMB 5

// ===== SENSORS =====
#define TRIG_PIN 10
#define ECHO_PIN 11
#define IR_PIN 12   // Bottom IR sensor (LOW = ground, HIGH = lifted)

// ===== CONSTANTS =====
#define MAX_SPEED 255
#define SEARCH_SPEED 150
#define ATTACK_DISTANCE 36.5   // cm

// ===== RADIUS LIMIT SYSTEM =====
#define MAX_RADIUS 35.0        // cm (allowed distance from start)

// Calibrate this value experimentally!
// (how many cm the robot moves per ms at full speed)
#define CM_PER_MS_AT_MAX_SPEED 0.015

float estimatedDistance = 0.0; // cm from start
unsigned long lastMoveTime = 0;

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

  digitalWrite(STBY, HIGH); // Enable motors

  Serial.begin(9600);
  Serial.println("=== Sumo Robot Running ===");

  lastMoveTime = millis();
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

void search() { // spin in place
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, SEARCH_SPEED);
  analogWrite(PWMB, SEARCH_SPEED);
}

// ===== DISTANCE ESTIMATION =====
void updateDistance(int speed) {
  unsigned long now = millis();
  unsigned long dt = now - lastMoveTime;
  lastMoveTime = now;

  float speedFactor = speed / 255.0;
  estimatedDistance += dt * CM_PER_MS_AT_MAX_SPEED * speedFactor;
}

void resetMotionTimer() {
  lastMoveTime = millis();
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

// ===== IR SENSOR =====
bool lifted() {
  return digitalRead(IR_PIN) == HIGH;
}

// ===== MAIN LOOP =====
void loop() {
  long distance = getDistance();

  // ===== RADIUS SAFETY CHECK =====
  if (estimatedDistance >= MAX_RADIUS) {
    Serial.println("RADIUS LIMIT REACHED → BACKING UP");
    backward(MAX_SPEED);
    delay(300);
    estimatedDistance = 0.0;   // reset after retreat
    resetMotionTimer();
    return;
  }

  // ===== BEHAVIOR PRIORITY =====
  if (!lifted()) {
    Serial.println("EDGE DETECTED → BACKWARD");
    backward(MAX_SPEED);
    resetMotionTimer();
  }
  else if (distance > 0 && distance <= ATTACK_DISTANCE) {
    updateDistance(MAX_SPEED);
    Serial.print(" | MODE: ATTACK → FORWARD");
    forward(MAX_SPEED);
  }
  else {
    search();                 // spinning doesn't increase radius
    Serial.print(" | MODE: SEARCH → SPIN");
    resetMotionTimer();
  }

  delay(50);
}
