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
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// ===== IR CONFIRMATION (BLACK LINE / UNDER ROBOT) =====
bool dangerBelow() {
  unsigned long start = millis();
  while (millis() - start < IR_CONFIRM_TIME) {
    if (digitalRead(IR_PIN) == HIGH) {
      return false; // safe floor detected again
    }
  }
  return true; // confirmed danger (edge or robot)
}

// ===== MAIN LOOP =====
void loop() {
  long distance = getDistance();

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
}
