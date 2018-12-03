#define G_SERIAL_BAUDRATE 115200
#define G_END_VALUE       1800
#define G_DELAY           10

                                     
int sensor_own_default_value = 0; //  시작상태에서의 default 값을 구한다.
                                  //  프로그램이 계산하여준다.
int previous_speed = 0;
int present_speed = 0;
int here_position = 45;

void send_signal_test();
void receive_signal();
void set_motor_speed_test();
void check_signal();

int signal_received = 100;  //  모터의 속도를 그대로 받아온다.

void setup() {
  Serial.begin(115200);
  for(int i = 5; i < 13; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  send_signal_test();
  receive_signal();
}

void send_signal_test() {

  previous_speed = present_speed;
  present_speed = signal_received;

  int temp_position = here_position + 3 * present_speed + previous_speed;
  int sending_signal = (temp_position + 1800) / 40;
  
  if(abs(temp_position) >= G_END_VALUE) {
    Serial.println("END");
    Serial.end();
  }
  else {
    here_position = temp_position;
    Serial.println(sending_signal);
  }
}

void receive_signal() {
  signal_received = -1;
  while(signal_received == -1) {
    Serial.flush();
    signal_received = Serial.read();
  }
  check_signal();
}

void check_signal() {
  int beat = 1, i = 0;
  for(i = 5; i < 13; i++) {
    digitalWrite(i, signal_received & beat);
    beat = beat << 1;
  }
}



