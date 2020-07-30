// including needed headerfile
#include <Wire.h>
#include "I2Cdev.h"
#include "Gyro.h"
#include <Servo.h>

// declaring objects for Gyro-sensor and servo motors
Gyro accelgyro;
Servo motor1, motor2, motor3;

// parameter which would be used for complementary filter
unsigned long prev_time = 0, pres_time = 0, dt;

double deg_p,deg_r;
double angle_p, angle_r;

int val; // variable for controlling continous rotation servo motor

// variables for raw data of acceleration and gytoscope of x, y, z - axis
int16_t ax, ay, az;
int16_t gx, gy, gz;

// variables for converted data of acceleration and gytoscope of x, y, z - axis
float Axyz[3];
float Gxyz[3];

void setup()
{
    Wire.begin();         // Arudino Uno R3 would be the master
    Serial.begin(38400);  // baud_rate 38400 bps

    accelgyro.initialize(); // initializing the sensor

    // attaching pin no.9, 10, 11
    motor1.attach(9);
    motor2.attach(10);
    motor3.attach(11);

    // starting to measure the time
    prev_time = millis();
}

void loop()
{  
    // get a raw data from the sensor
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // converting from raw data to exact data
    Axyz[0] = (double)ax / 16384;
    Axyz[1] = (double)ay / 16384;
    Axyz[2] = (double)az / 16384;
    Gxyz[1] = (double)gy * 250 / 32768;
    Gxyz[2] = (double)gz * 250 / 32768;

    pres_time = millis(); // measure the time
    dt = pres_time - prev_time; // time variable which would be used in complemantary filter
    prev_time = pres_time; // saving current time data

    deg_p = atan2(Axyz[0], Axyz[2]) * 180 / PI ;  //rad to deg
    deg_r = atan2(Axyz[1], Axyz[2]) * 180 / PI ;  //rad to deg

    // complementary filter
    angle_p = (0.85 * (angle_p + (Gxyz[1] * (dt * 0.0001)))) + (0.15 * deg_p) ;
    angle_r = (0.85 * (angle_r + (Gxyz[2] * (dt * 0.0001)))) + (0.15 * deg_r) ;

    // scaling PWM from Gyroscope data to choose interactive velocity
    if(Gxyz[2] <0){
      val = map(abs(Gxyz[2]), 0, 700, 90, 180);
    }
    
    else if(Gxyz[2] > 0){
      val = map(abs(Gxyz[2]), 0, 1100, 90, 0);
    }
    else val = 90;

    // move servo motors
    motor1.write(90 + angle_p);
    motor2.write(90 + angle_r);
    motor3.write(val);
}
