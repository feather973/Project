/*
 * Controlling UltraSonic Sensor Turret 
 * Servo Motor -> 0 ~ 180
 * Senor : 2 -> Opposite Side
 * Detection Range : 2 ~ 100
 * Sequence :
 * 1. Servo Moves on Evey 1ms
 * 2. 2 UltraSensor On
 * 3. If Enemy is Detected from at least one Sensor, send Packet Message for Atmega128 to Control Laser Turret 
 * 4. Packet Message Form : Se_x___y___e_x___y___E
 * 5. S : Start
 * 6. E : End
 * 7. e : Quadra Number -> if 0, no Enemy
 * 8. x, y : Coordinate
*/
// UltraSonic Pin Definition
#define TRIGPIN1  D1
#define ECHOPIN1  D2
#define TRIGPIN2  D7
#define ECHOPIN2  D0

// Motor Pin Definition
#define MOTOR   D6

// Headerfile Include
#include <Servo.h>
#include <Ticker.h>

// Defining Struct -> for data to be sent
typedef struct {
  int e; // which Quadra
  int x; // x coordinate
  int y; // y coordinate
}p; // name this struct as 'p'

// Declaring Class
Ticker timer;
Servo motor1;

long duration; // for raw data from UltraSonic Sensor
int measure[2]; // for real data calculated from raw data, 0 for Ultra 1, 1 for Ultra 2
volatile int ang = 0; // angle for Servo Motor
volatile int pwm = 0; // rescaled from ang -> 0 ~ 2140
volatile int cnv = 0; // flag which indicated angle to be increased or decreased
char packet[23] = {0,}; // Character Array to be sent to ATmega 128
p pack[2]; // strcut type 'p' array

// Timer Interrupt Function -> For moving Servo
void timerCallback(){
  // rescaling angle -> pwm
  if(cnv==false) pwm = map(ang, 0, 180, 0, 2140) + 480;
  else pwm = map(ang, 180, 0, 2140, 0) +480;

  // move Servo motor with pwm 
  motor1.writeMicroseconds(pwm);

  // cnv == 0 -> ang increase, cnv == 1 -> ang decrease
  if(cnv==0) {
    ang++;
    if(ang>=180) cnv = 1;
    }
  else {
    ang--;
    if(ang<=0) cnv = 0;
  }
}

// Getting Distance Data from UltraSonic Sensor
void UltraSonic(int tri, int echo, int idx){
  // tri : TrigPin, echo : EchoPin, idx : Sensor Number
  digitalWrite(tri, LOW);
  delayMicroseconds(2);
  digitalWrite(tri, HIGH);
  delayMicroseconds(10);
  digitalWrite(tri, LOW);
  duration = pulseInLong(echo, HIGH);
  measure[idx] = duration * 0.034 / 2;
}

// Identifies which Quadra the Enemy is in now
int getQua(int idx, int x, int y){
  if(x>=0&&y>=0) return 1;
  else if(x<0&&y>=0) return 2;
  else if(x<0&&y<0) return 3;
  return 4;
}

// Getting x, y Coordinate
void getXY(int idx, int ang){
  // Since two UltraSonic Sensors are opposite,
  // Angle on Ultra Sensor 2 needs to 180 added  
   if(idx) ang += 180; 

   // x, y coodinate and Quadra Number
   pack[idx].x = (int)(cos((float)ang*PI/180) * (double)measure[idx]); 
   pack[idx].y = (int)(sin((float)ang*PI/180) * (double)measure[idx]);
   pack[idx].e = getQua(idx, pack[idx].x, pack[idx].y);

   // abs for Packet Format
   pack[idx].x = abs(pack[idx].x);
   pack[idx].y = abs(pack[idx].y);
}

// True : Enemy ditected, Fale : No Enemy
bool isThereEnemy(int ang){
  // flag : true -> Enemy ditected by at least on Ultra Sensor
  // false -> No Enemy on both UltraSensor 
  bool flag = false;
  for(int i=0;i<2;i++){
    getXY(i, ang);
    if(measure[i]<=100 && measure[i] >=2) flag = true;
    else {
      // No Enemy -> (900, 900)
      pack[i].x = 900;
      pack[i].y = 900;
      pack[i].e = 0;
    }
  }
  if(flag) return true;
  return false;
}

// Setup
void setup() {
  //Setting Pin
  pinMode(TRIGPIN1, OUTPUT);
  pinMode(ECHOPIN1, INPUT);
  pinMode(TRIGPIN2, OUTPUT);
  pinMode(ECHOPIN2, INPUT);
  motor1.attach(MOTOR);
  
  Serial.begin(9600); // Baud rate 9600

  // Servo Motor Check
  motor1.write(0);
  delay(1000);

  // Setting Timer Intterupt for every 1ms
  timer.attach(0.1, timerCallback);
}

// main funtion
void loop() {
  // UltraSonic Sensor Activation
  UltraSonic(TRIGPIN1, ECHOPIN1, 0);
  UltraSonic(TRIGPIN2, ECHOPIN2, 1);

  // if Enemy Detected -> Send Packet Message to ATmega 128 -> UART 1
  if(isThereEnemy(ang)){
    sprintf(packet, "Se%1dx%03dy%03de%1dx%03dy%03dE\n", 
    pack[0].e, pack[0].x, pack[0].y, 
    pack[1].e, pack[1].x, pack[1].y);
    Serial.print(packet);
  }
}
