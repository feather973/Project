#include "Gyro.h"

/*
   Setting Default Address
*/
Gyro::Gyro() {
  devAddr = DEFAULT_ADDR;
}

/*
   Setting Specific address
*/
Gyro::Gyro(uint8_t address) {
  devAddr = address;
}
/*
   Initialize
*/
void Gyro::initialize() {
  /*Setting Clock Source to use X gyro which is better than internal clock source*/
  setClockSource(CLOCK_PLL_XGYRO);
  /*Setting acceleromter and gyroscope to most sensitive setting*/
  setFullScaleGyroRange(GYRO_FS_250);
  setFullScaleAccelRange(ACCEL_FS_2);
  /*Activating the device and take out of sleep mode*/
  setSleepEnabled(false);
}


/*
* Set clock source setting.
*/
void Gyro::setClockSource(uint8_t source) {
  I2Cdev::writeBits(devAddr, RA_PWR_MGMT_1, PWR1_CLKSEL_BIT, PWR1_CLKSEL_LENGTH, source);
}

/*
* Set full-scale gyroscope range.
*/
void Gyro::setFullScaleGyroRange(uint8_t range) {
  I2Cdev::writeBits(devAddr, RA_GYRO_CONFIG, GCONFIG_FS_SEL_BIT, GCONFIG_FS_SEL_LENGTH, range);
}

/** Set full-scale accelerometer range.
*/
void Gyro::setFullScaleAccelRange(uint8_t range) {
  I2Cdev::writeBits(devAddr, RA_ACCEL_CONFIG, ACONFIG_AFS_SEL_BIT, ACONFIG_AFS_SEL_LENGTH, range);
}

/** Set sleep mode status.
*/
void Gyro::setSleepEnabled(bool enabled) {
  I2Cdev::writeBit(devAddr, RA_PWR_MGMT_1, PWR1_SLEEP_BIT, enabled);
}

/** Get raw 6-axis motion sensor readings (accel/gyro).
*/
void Gyro::getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
  I2Cdev::readBytes(devAddr, RA_ACCEL_XOUT_H, 14, buffer);
  *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
  *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
  *az = (((int16_t)buffer[4]) << 8) | buffer[5];
  *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
  *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
  *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}
