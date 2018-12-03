#include <GKD_MPU6050.h>

#define SENSOR_ZERO_VALUE 20      //  0도에서 시작하여 0도를 측정하였을때 나오는 값이다.
                                  //  작동하기 전에 그때그때 넣어준다.   

int sensor_own_default_value = 0; //  시작상태에서의 default 값을 구한다.
                                  //  프로그램이 계산하여준다.

int cnt_print = 0;

#define G_SERIAL_BAUDRATE 9600
#define ENA 5
#define ENB 10
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9

int MOTOR_SPEED = 80.0;

//GYRO_KALMAN angX, angY, angZ;
//int xInit[5] = {0,0,0,0,0}, yInit[5] = {0,0,0,0,0}, zInit[5] = {0,0,0,0,0};
int sensor_x, sensor_y, sensor_z;

const double A_CONST = 1.0;
const double B_CONST = 1.0;

int init_sensor_value(int);
void get_sensor_value(int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

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
  sensor_own_default_value += init_sensor_value(1);
  delay(100);
  sensor_own_default_value += init_sensor_value(1);
  delay(100);
  sensor_own_default_value += init_sensor_value(1);
  delay(100);
  sensor_own_default_value += init_sensor_value(1);
  delay(100);
  sensor_own_default_value /= 4;
  Serial.print("Default Value is ");
  Serial.println(sensor_own_default_value);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4, HIGH);

  get_sensor_value(1);
  MOTOR_SPEED = sensor_y / 10;
  
  analogWrite(ENA, MOTOR_SPEED * A_CONST);
  analogWrite(ENB, MOTOR_SPEED * B_CONST);

  
  delay(100);
}


int init_sensor_value(int _input) {
  int i = 0;
  sensor_x = sensor_y = sensor_z = 0;
  
//  for(i = 0; i < HOW_MANY_GET_SENSOR_VALUE; i++) {
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
          //gz1 += zCal;
      }
      sensor_x += gx1;
      sensor_y += gy1;
      sensor_z += gz1;
    }
    prevSensoredTime = curSensoredTime;
//  }
/*
  sensor_x /= HOW_MANY_GET_SENSOR_VALUE;
  sensor_y /= HOW_MANY_GET_SENSOR_VALUE;
  sensor_z /= HOW_MANY_GET_SENSOR_VALUE;*/
  if(_input) {
//  Serial.print("Sensor value is : ");
//  Serial.print(sensor_x);
//  Serial.print(" ");
    Serial.print(sensor_y);
//  Serial.print(" ");
//  Serial.print(sensor_z);
    Serial.println(""); 
  }
  return sensor_y;  
}


void get_sensor_value(int _input) {
  int i = 0;
  sensor_x = sensor_y = sensor_z = 0;
  
//  for(i = 0; i < HOW_MANY_GET_SENSOR_VALUE; i++) {
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
          //gz1 += zCal;
      }
      sensor_x += gx1;
      sensor_y += gy1;
      sensor_z += gz1;
    }
    prevSensoredTime = curSensoredTime;
//  }
/*
  sensor_x /= HOW_MANY_GET_SENSOR_VALUE;
  sensor_y /= HOW_MANY_GET_SENSOR_VALUE;
  sensor_z /= HOW_MANY_GET_SENSOR_VALUE;*/
  if(_input) {

    //
    //    센서의 값을 보정해준다.
    //
    sensor_y = sensor_y + sensor_own_default_value - SENSOR_ZERO_VALUE;
    
//  Serial.print("Sensor value is : ");
//  Serial.print(sensor_x);
//  Serial.print(" ");
    Serial.print("result is ");
    Serial.print(sensor_y);
//  Serial.print(" ");
//  Serial.print(sensor_z);
    Serial.println(""); 
  } 
}
