#include "GKD_MPU6050.h"
#include <Wire.h>

int MPU6050_read(int start, uint8_t *buffer, int size) {
  int i, n, error;
  
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  
  n = Wire.write(start);
  if (n != 1)
    return (-10);
  
  n = Wire.endTransmission(false);
  if (n != 0)
    return (n);
  
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);
  return (0); 
}

int MPU6050_write(int start, const uint8_t *pData, int size) {
  int n, error;
  
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  
  n = Wire.write(start);
  if (n != 1)
    return (-20);
    
  n = Wire.write(pData, size);
  if (n != size)
    return (-21);
    
  error = Wire.endTransmission(true);
  if (error != 0)
    return (error);
  return (0);
}

int MPU6050_write_reg(int reg, uint8_t data) {
  int error;
  error = MPU6050_write(reg, &data, 1);
  return (error);
}

float angleInDegrees(int lo, int hi, int measured) {
  float x = (hi - lo)/180.0;
  return (float)measured/x;
}

void initGyroKalman(struct GyroKalman *kalman, const float Q_angle, const float Q_gyro, const float R_angle) {
  kalman->Q_angle = Q_angle;
  kalman->Q_gyro = Q_gyro;
  kalman->R_angle = R_angle;
  
  kalman->P_00 = 0;
  kalman->P_01 = 0;
  kalman->P_10 = 0;
  kalman->P_11 = 0;
}

void predict(struct GyroKalman *kalman, float dotAngle, float dt) {
  kalman->x_angle += dt * (dotAngle - kalman->x_bias);
  kalman->P_00 += -1 * dt * (kalman->P_10 + kalman->P_01) + dt*dt * kalman->P_11 + kalman->Q_angle;
  kalman->P_01 += -1 * dt * kalman->P_11;
  kalman->P_10 += -1 * dt * kalman->P_11;
  kalman->P_11 += kalman->Q_gyro;
}

float update(struct GyroKalman *kalman, float angle_m) {
  const float y = angle_m - kalman->x_angle;
  const float S = kalman->P_00 + kalman->R_angle;
  const float K_0 = kalman->P_00 / S;
  const float K_1 = kalman->P_10 / S;
  kalman->x_angle += K_0 * y;
  kalman->x_bias += K_1 * y;
  kalman->P_00 -= K_0 * kalman->P_00;
  kalman->P_01 -= K_0 * kalman->P_01;
  kalman->P_10 -= K_1 * kalman->P_00;
  kalman->P_11 -= K_1 * kalman->P_01;
  return kalman->x_angle;
}


void setup_function() {
	
}

int init_sensor_value() {
  int i = 0;
  
//  for(i = 0; i < HOW_MANY_GET_SENSOR_VALUE; i++) {
    int error;
    double dT;
    uint8_t swap;
    
    accel_t_gyro_union accel_t_gyro;    
    curSensoredTime = gkd_mil();  
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
      sensor_own_default_value_x += gx1;
      sensor_own_default_value_y += gy1;
      sensor_own_default_value_z += gz1;
    }
    prevSensoredTime = curSensoredTime;
    
    
    

    return 0;  
}

void get_sensor_value() {
  int i = 0;
  
//  for(i = 0; i < HOW_MANY_GET_SENSOR_VALUE; i++) {
    int error;
    double dT;
    uint8_t swap;
    
    accel_t_gyro_union accel_t_gyro;    
    curSensoredTime = gkd_mil(); 
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
    }
    prevSensoredTime = curSensoredTime;
}

void readSignal(int *address_x, int *address_y, int *address_z) {
  int i = 0;
  (*address_x) = (*address_y) = (*address_z) = 0;

  int error;
  double dT;
  uint8_t swap;
  
  accel_t_gyro_union accel_t_gyro;    
  curSensoredTime = gkd_mil();
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
    (*address_x) += gx1;
    (*address_y) += gy1;
    (*address_z) += gz1;
  }
  prevSensoredTime = curSensoredTime;


  //
  //    센서의 값을 보정해준다.
  //
  (*address_x) = (*address_x) + sensor_own_default_value_x - sensor_x_zero_value;
  (*address_y) = (*address_y) + sensor_own_default_value_y - sensor_y_zero_value;
  (*address_z) = (*address_z) + sensor_own_default_value_z - sensor_z_zero_value;
    
}

void readRawSignal(int *address_X, int *address_Y, int *address_Z) {
  int i = 0;
  (*address_X) = (*address_Y) = (*address_Z) = 0;
  
  int error;
  double dT;
  uint8_t swap;
    
  accel_t_gyro_union accel_t_gyro;    
  curSensoredTime = gkd_mil();
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
    (*address_X) += gx1;
    (*address_Y) += gy1;
    (*address_Z) += gz1;
  }
  prevSensoredTime = curSensoredTime;


//  Serial.print((*address_x)), Serial.println(" ");
//  Serial.print((*address_y)), Serial.println(" ");
//  Serial.println((*address_z));
}






