#include "I2Cdev.h"
#include <avr/pgmspace.h>

#define DEFAULT_ADDR          0x68
#define RA_GYRO_CONFIG           0x1B
#define RA_ACCEL_CONFIG          0x1C
#define RA_ACCEL_XOUT_H          0x3B
#define RA_PWR_MGMT_1            0x6B
#define GCONFIG_FS_SEL_BIT          4
#define GCONFIG_FS_SEL_LENGTH       2
#define GYRO_FS_250              0x00
#define ACONFIG_AFS_SEL_BIT         4
#define ACONFIG_AFS_SEL_LENGTH      2
#define PWR1_SLEEP_BIT              6
#define PWR1_CLKSEL_BIT             2
#define PWR1_CLKSEL_LENGTH          3
#define CLOCK_PLL_XGYRO          0x01
#define ACCEL_FS_2          0x00

class Gyro {
    public:
        Gyro();
        Gyro(uint8_t address);
        void initialize();
        void setClockSource(uint8_t source);
        void setFullScaleGyroRange(uint8_t range);
        void setFullScaleAccelRange(uint8_t range);
        void getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);
        void setSleepEnabled(bool enabled);

    private:
        uint8_t devAddr;
        uint8_t buffer[14];
};
