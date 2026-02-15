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
#define IR_PIN 12   // Bottom IR sensor (HIGH = white line)

// ===== CONSTANTS =====
#define MAX_SPEED 200
#define SEARCH_SPEED 150
#define ATTACK_DISTANCE 36.5   // cm

// ===== INTERVAL CONTROL =====
#define MOVE_INTERVAL 120   // ms moving forward
#define PAUSE_INTERVAL 40   // ms pause to read sensor

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

  digitalWrite(STBY, HIGH);

  Serial.begin(9600);
  Serial.println("=== Interval Sumo Mode ===");
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

void spinRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, SEARCH_SPEED);
  analogWrite(PWMB, SEARCH_SPEED);
}

void stopMotors() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
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

// ===== LINE DETECTION =====
bool onWhiteLine() {
  return digitalRead(IR_PIN) == HIGH;
}

// ===== MAIN LOOP =====
void loop() {
  long distance = getDistance();

  // ===== WHITE LINE SAFETY =====
  if (onWhiteLine()) {
    Serial.println("WHITE LINE → RETREAT");
    backward(MAX_SPEED);
    delay(300);
    spinRight();
    delay(250);
    return;
  }

  // ===== ATTACK MODE =====
  if (distance > 0 && distance <= ATTACK_DISTANCE) {
    Serial.println("ATTACK → INTERVAL FORWARD");
    forward(MAX_SPEED);
    delay(MOVE_INTERVAL);

    stopMotors();
    delay(PAUSE_INTERVAL);
  }
  else {
    // ===== SEARCH MODE =====
    Serial.println("SEARCH → SPIN");
    spinRight();
    delay(80);
  }
}
