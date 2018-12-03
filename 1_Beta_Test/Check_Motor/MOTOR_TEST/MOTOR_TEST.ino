
int cnt = 0;

#define G_SERIAL_BAUDRATE 9600
#define ENA 5
#define ENB 10
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

}

void loop() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  int motor_speed = (cnt % 510) - 255;

  Serial.print("Motor speed is ");
  Serial.println(motor_speed);
  analogWrite(ENA, motor_speed);
  analogWrite(ENB, motor_speed);

  delay(500);
  cnt++;
}

