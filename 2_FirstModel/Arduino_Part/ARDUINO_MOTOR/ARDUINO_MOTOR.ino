#include <Stepper.h>
#include <GKD_MPU6050.h>

#define G_SERIAL_BAUDRATE 115200
#define G_END_VALUE       1800
#define G_DELAY           10
#define ENA 5
#define ENB 10
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9

const int HOW_MANY_GET_SENSOR_VALUE = 1;
const int DELAY_IN_GET_SENSOR_VALUE = 0;
const double MOTOR_A_CONSTANT = 1;
const double MOTOR_B_CONSTANT = 1;

#define SENSOR_ZERO_VALUE (10)      //  0도에서 시작하여 0도를 측정하였을때 나오는 값이다.
                                    //  작동하기 전에 그때그때 넣어준다.
                                     
int sensor_own_default_value = 0; //  시작상태에서의 default 값을 구한다.
                                  //  프로그램이 계산하여준다.
int previous_speed = 0;
int present_speed = 0;
int here_position = 45;

int get_sensor_value(int);
void send_signal();void send_signal_test();
void receive_signal();
void set_motor_speed();void set_motor_speed_test();
void move_motor();
void check_signal();
void stop_motor();
void check_signal_damn();

int sensor_x, sensor_y, sensor_z;
int signal_received = 100;  //  모터의 속도를 그대로 받아온다.
int &motor_speed = signal_received;

void setup() {
  unsigned char c;
  Serial.begin(G_SERIAL_BAUDRATE);  

  initGyroKalman(&angX, Q_angle, Q_gyro, R_angle);
  initGyroKalman(&angY, Q_angle, Q_gyro, R_angle);
  initGyroKalman(&angZ, Q_angle, Q_gyro, R_angle);
  
  MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
  MPU6050_read (MPU6050_PWR_MGMT_2, &c, 1);
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);

  get_sensor_value(0);
  delay(100);
  get_sensor_value(0);
  delay(100);
  get_sensor_value(0);
  delay(100);
  sensor_own_default_value += get_sensor_value(0);
  delay(100);
  sensor_own_default_value += get_sensor_value(0);
  delay(100);
  sensor_own_default_value += get_sensor_value(0);
  delay(100);
  sensor_own_default_value += get_sensor_value(0);
  delay(100);
  sensor_own_default_value /= 4;
  Serial.print("Default Value is ");
  Serial.println(sensor_own_default_value);
  move_motor();
}

/*
 *          LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
 *          LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
 */
void loop() {
  get_sensor_value(1);
  send_signal();
  receive_signal();
  set_motor_speed();
  move_motor();
}
/*
 *          ENDLOOP   ENDLOOP   ENDLOOP   ENDLOOP
 */

int get_sensor_value(int _input) {
  int i = 0;
  sensor_x = sensor_y = sensor_z = 0;
  
  int error;
  double dT;
  uint8_t swap;
  
  accel_t_gyro_union accel_t_gyro;    
  curSensoredTime = millis();  
  error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  
#define SWAP(x,y) swap = x; x = y; y = swap
  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
  
  if(prevSensoredTime > 0) {
    int gx1 = 0, gy1 = 0, gz1 = 0, k;
    float gx2=0, gy2=0, gz2 = 0;
    int loopTime = curSensoredTime - prevSensoredTime;

    gx2 = angleInDegrees(lowX, highX, accel_t_gyro.value.x_gyro);
    gy2 = angleInDegrees(lowY, highY, accel_t_gyro.value.y_gyro);
    gz2 = angleInDegrees(lowZ, highZ, accel_t_gyro.value.z_gyro);

    predict(&angX, gx2, loopTime);
    predict(&angY, gy2, loopTime);
    predict(&angZ, gz2, loopTime);

    gx1 = update(&angX, accel_t_gyro.value.x_accel) / 10;
    gy1 = update(&angY, accel_t_gyro.value.y_accel) / 10;
    gz1 = update(&angZ, accel_t_gyro.value.z_accel) / 10;

    if(initIndex < initSize) {
      xInit[initIndex] = gx1;
      yInit[initIndex] = gy1;
      zInit[initIndex] = gz1;
      if(initIndex == initSize - 1) {
        int sumX = 0; int sumY = 0; int sumZ = 0;
        for(k = 1; k <= initSize; k++) {
          sumX += xInit[k];
          sumY += yInit[k];
          sumZ += zInit[k];
        }
        xCal -= sumX/(initSize -1);
        yCal -= sumY/(initSize -1);
        zCal = (sumZ/(initSize -1) - zCal);
      }
      
      initIndex++;
    }
    else {
        gx1 += xCal;
        gy1 += yCal;
        gz1 += zCal;
    }
    sensor_x += gx1;
    sensor_y += gy1;
    sensor_z += gz1;
  }
  prevSensoredTime = curSensoredTime;
  if(_input) {
    //
    //    센서의 값을 보정해준다.
    //
    sensor_y = sensor_y + sensor_own_default_value - SENSOR_ZERO_VALUE;
  } 

  return sensor_y;
}

void send_signal() {
  int sending_signal = (sensor_y + 1800) / 40;
  
  if(abs(sensor_y) >= G_END_VALUE) {
    Serial.println("END");
    Serial.end();
  }
  else {
    Serial.println(sending_signal);
  }
}

void receive_signal() {
  signal_received = -1;
  while(signal_received == -1) {
    Serial.flush();
    signal_received = Serial.read();
  }
}

void set_motor_speed() {
  /*
   * 모터 속도는 0 ~ 127로 전송받는다.
   * 64 일때를 0 이라고 한다.
   * 최종 속도는 -255 ~ 255 로 오게끔 한다.
   */
  motor_speed = (signal_received - 64) * 4;
  if(motor_speed < 0) {
    motor_speed = max(motor_speed, -255);
  }
  else {
    motor_speed = min(motor_speed, 255);
  }
}

void move_motor() {
  int tmp_speed = motor_speed;

  digitalWrite(IN1, tmp_speed > 0 ? LOW : HIGH);
  digitalWrite(IN2, tmp_speed > 0 ? HIGH : LOW);

  digitalWrite(IN3, tmp_speed > 0 ? LOW : HIGH);
  digitalWrite(IN4, tmp_speed > 0 ? HIGH : LOW);

  analogWrite(ENA, tmp_speed * MOTOR_A_CONSTANT);
  analogWrite(ENB, tmp_speed * MOTOR_B_CONSTANT);
}


