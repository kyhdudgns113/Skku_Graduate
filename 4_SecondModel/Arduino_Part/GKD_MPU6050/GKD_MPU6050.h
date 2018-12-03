#pragma once
#include <Wire.h>
#include <math.h>

#define GKD 2
#define MPU6050_ACCEL_XOUT_H 0x3B // R
#define MPU6050_PWR_MGMT_1 0x6B // R/W
#define MPU6050_PWR_MGMT_2 0x6C // R/W
#define MPU6050_WHO_AM_I 0x75 // R
#define MPU6050_I2C_ADDRESS 0x68

union accel_t_gyro_union {
  struct {
    uint8_t x_accel_h, x_accel_l;
    uint8_t y_accel_h, y_accel_l;
    uint8_t z_accel_h, z_accel_l;
    uint8_t t_h, t_l;
    uint8_t x_gyro_h, x_gyro_l;
    uint8_t y_gyro_h, y_gyro_l;
    uint8_t z_gyro_h, z_gyro_l;
  } reg;

  struct {
    int x_accel, y_accel, z_accel;
    int temperature;
    int x_gyro, y_gyro, z_gyro;
  } value;
};


typedef struct GyroKalman{
  float x_angle, x_bias;
  float P_00, P_01, P_10, P_11;
  float Q_angle, Q_gyro;
  float R_angle;
}GYRO_KALMAN;



const int lowX = -2150, highX = 2210;
const int lowY = -2150, highY = 2210;
const int lowZ = -2150, highZ = 2550;

static unsigned long prevSensoredTime = 0, curSensoredTime = 0;

static const float R_angle = 0.3, Q_angle = 0.01, Q_gyro = 0.04;


static int initIndex = 0, initSize = 5;
static int xCal = 0, yCal = 0, zCal = 1800;
static int xInit[5] = {0,0,0,0,0}, yInit[5] = {0,0,0,0,0}, zInit[5] = {0,0,0,0,0};
static GYRO_KALMAN angX, angY, angZ;

//
//	Calculate default value in initial state.
//	It will be calculated by program.
//
static int sensor_own_default_value_x = 0;
static int sensor_own_default_value_y = 0; //  시작상태에서의 default 값을 구한다.
static int sensor_own_default_value_z = 0; //  프로그램이 계산하여준다.

static int sensor_x_zero_value = 20;
static int sensor_y_zero_value = 20;      //  0도에서 시작하여 0도를 측정하였을때 나오는 값이다.
static int sensor_z_zero_value = 20;      //  작동하기 전에 그때그때 넣어준다. 


static unsigned long (*gkd_mil)();				  //  millis 함수를 헤더파일에서 못써서 함수포인터로 대체 


int MPU6050_read(int start, uint8_t *buffer, int size);
int MPU6050_write(int start, const uint8_t *pData, int size);
int MPU6050_write_reg(int reg, uint8_t data);
float angleInDegrees(int lo, int hi, int measured);
void initGyroKalman(struct GyroKalman *kalman, const float Q_angle, const float Q_gyro, const float R_angle);
void predict(struct GyroKalman *kalman, float dotAngle, float dt);
float update(struct GyroKalman *kalman, float angle_m);

void setup_function();

int init_sensor_value();
void get_sensor_value();

void readRawSignal(int *, int *, int *);
void readSignal(int *, int *, int *);


