// maze_car_pwm_correction.ino

#define ENA  5
#define ENB  6
#define ENC  9
#define END  10
#define IN1 22 
#define IN2 23
#define IN3 24  
#define IN4 25
#define IN5 26  
#define IN6 27
#define IN7 28  
#define IN8 29
#define TRIG 11
#define ECHO 12
#define OBSTACLE_DIST_CM 20

const int   BASE_SPEED = 180;
const float K_FL = 1.00;
const float K_FR = 0.85;
const float K_RL = 0.90;
const float K_RR = 0.90 * 0.85;

int correctedPWM(float base, float k) {
  return constrain((int)(base * k), 0, 255);
}
void setMotorDir(int a, int b, bool fwd) {
  digitalWrite(a, fwd ? HIGH : LOW);
  digitalWrite(b, fwd ? LOW : HIGH);
}
void driveForward(int spd = BASE_SPEED) {
  setMotorDir(IN1,IN2,true); setMotorDir(IN3,IN4,true);
  setMotorDir(IN5,IN6,true); setMotorDir(IN7,IN8,true);
  analogWrite(ENA, correctedPWM(spd, K_FL));
  analogWrite(ENB, correctedPWM(spd, K_FR));
  analogWrite(ENC, correctedPWM(spd, K_RL));
  analogWrite(END, correctedPWM(spd, K_RR));
}
void driveStop() {
  analogWrite(ENA,0); analogWrite(ENB,0);
  analogWrite(ENC,0); analogWrite(END,0);
}
void turnRight(int ms) {
  setMotorDir(IN1,IN2,true);  setMotorDir(IN3,IN4,false);
  setMotorDir(IN5,IN6,true);  setMotorDir(IN7,IN8,false);
  analogWrite(ENA, correctedPWM(BASE_SPEED, K_FL));
  analogWrite(ENB, correctedPWM(BASE_SPEED, K_FR));
  analogWrite(ENC, correctedPWM(BASE_SPEED, K_RL));
  analogWrite(END, correctedPWM(BASE_SPEED, K_RR));
  delay(ms); driveStop();
}
void turnLeft(int ms) {
  setMotorDir(IN1,IN2,false); setMotorDir(IN3,IN4,true);
  setMotorDir(IN5,IN6,false); setMotorDir(IN7,IN8,true);
  analogWrite(ENA, correctedPWM(BASE_SPEED, K_FL));
  analogWrite(ENB, correctedPWM(BASE_SPEED, K_FR));
  analogWrite(ENC, correctedPWM(BASE_SPEED, K_RL));
  analogWrite(END, correctedPWM(BASE_SPEED, K_RR));
  delay(ms); driveStop();
}
float getDistanceCM() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long dur = pulseIn(ECHO, HIGH, 30000);
  return (dur == 0) ? 999.0 : dur * 0.0343 / 2.0;
}
void setup() {
  int pins[] = {ENA,ENB,ENC,END,IN1,IN2,IN3,IN4,IN5,IN6,IN7,IN8,TRIG};
  for(int p : pins) pinMode(p, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}
void loop() {
  float dist = getDistanceCM();
  Serial.print("dist: "); Serial.println(dist);
  if (dist < OBSTACLE_DIST_CM) {
    driveStop(); delay(100);
    setMotorDir(IN1,IN2,false); setMotorDir(IN3,IN4,false);
    setMotorDir(IN5,IN6,false); setMotorDir(IN7,IN8,false);
    analogWrite(ENA, correctedPWM(BASE_SPEED, K_FL));
    analogWrite(ENB, correctedPWM(BASE_SPEED, K_FR));
    analogWrite(ENC, correctedPWM(BASE_SPEED, K_RL));
    analogWrite(END, correctedPWM(BASE_SPEED, K_RR));
    delay(300); driveStop(); delay(100);
    turnRight(600);
  } else {
    driveForward();
  }
  delay(50);
}