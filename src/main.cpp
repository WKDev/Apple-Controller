// Apple Controller
// Started to write : 210527

// Sampling Rate = 1000Hz;
// GyroFilter = 256Hz;
// Acc 필터 : 260Hz, Gyro 필터 : 256Hz, 샘플링 8KHz;
// ACC_fullScaleRange = ±250deg / s;
// GYRO_fullScaleRange = ±2g / s;

/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.
This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").
Contact information
-------------------
Kristian Lauszus, TKJ Electronics
Web : http://www.tkjelectronics.com
e-mail : kristianl@tkjelectronics.com
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <BleMouse.h>

Adafruit_MPU6050 mpu;
BleMouse bleMouse;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ;

double temp;

const double RADIAN_TO_DEGREE = 180 / 3.14159;
const double DEG_PER_SEC = 32767 / 250; // 1초에 회전하는 각도
const double ALPHA = 1 / (1 + 0.04);
// GyX, GyY, GyZ 값의 범위 : -32768 ~ +32767 (16비트 정수범위)

unsigned long now = 0;  // 현재 시간 저장용 변수
unsigned long past = 0; // 이전 시간 저장용 변수
double dt = 0;          // 한 사이클 동안 걸린 시간 변수

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;

void caliSensor();

void setup()
{
  Serial.begin(115200);

  bleMouse.begin();

  while (!Serial)
    delay(10);

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  past = millis(); // past에 현재 시간 저장

  caliSensor();
  delay(100);
}

void loop()
{
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  // 0x3B 주소에는 데이터가 있는 레지스터, 14바이트를 읽는다
  AcX = int(a.acceleration.x); // Accel X-축 값: HIGH | LOW
  AcY = int(a.acceleration.y); // Accel Y-축 값: HIGH | LOW
  AcZ = int(a.acceleration.z); // Accel Z

  temp = t.temperature; // 온도 값

  GyX = g.gyro.x; // x축 자이로 값
  GyY = g.gyro.y; // y축 자이로 값
  GyZ = g.gyro.z; // z축

  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  // 가속도 센서로는 Z축 회전각 계산 불가함.

  // 가속도 현재 값에서 초기평균값을 빼서 센서값에 대한 보정
  angleGyX += ((GyX - averGyX) / DEG_PER_SEC) * dt; //각속도로 변환
  angleGyY += ((GyY - averGyY) / DEG_PER_SEC) * dt;
  angleGyZ += ((GyZ - averGyZ) / DEG_PER_SEC) * dt;

  // 상보필터 처리를 위한 임시각도 저장
  double angleTmpX = angleFiX + angleGyX * dt;
  double angleTmpY = angleFiY + angleGyY * dt;
  double angleTmpZ = angleFiZ + angleGyZ * dt;

  // (상보필터 값 처리) 임시 각도에 0.96가속도 센서로 얻어진 각도 0.04의 비중을 두어 현재 각도를 구함.
  angleFiX = ALPHA * angleTmpX + (1.0 - ALPHA) * angleAcX;
  angleFiY = ALPHA * angleTmpY + (1.0 - ALPHA) * angleAcY;
  angleFiZ = angleGyZ; // Z축은 자이로 센서만을 이용하열 구함.
  // Serial.print("AngleAcX:");
  // Serial.print(angleAcX);
  Serial.print("\t X:");
  Serial.print(angleFiX);
  // Serial.print("\t AngleAcY:");
  // Serial.print(angleAcY);
  Serial.print("\t Y:");
  Serial.print(angleFiY);
  // Serial.print("\t AngleAcZ:");
  // Serial.print(angleGyZ);
  Serial.print("\t Z:");
  Serial.println(angleFiZ);

  if (bleMouse.isConnected())
  {

    // unsigned long startTime;

    // Serial.println("Scroll up");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, 0, 1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Scroll down");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, 0, -1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Scroll left");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, 0, 0, -1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Scroll right");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, 0, 0, 1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Move mouse pointer up");
    // // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, -1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Move mouse pointer down");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(0, 1);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Move mouse pointer left");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(-1, 0);
    //   delay(100);
    // }
    // delay(500);

    // Serial.println("Move mouse pointer right");
    // startTime = millis();
    // while (millis() < startTime + 2000)
    // {
    //   bleMouse.move(1, 0);
    //   delay(100);
    // }
    // delay(500);
    // Z, X
    //move(horizontal, vertical)
    bleMouse.move(angleFiZ, angleFiX);
    delay(10);
  }
}

void getDT()
{
  now = millis();
  dt = (now - past) / 1000.0;
  past = now;
}

// 센서의 초기값을 10회 정도 평균값으로 구하여 저장하는 함수
void caliSensor()
{
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  double sumAcX = 0, sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0, sumGyY = 0, sumGyZ = 0;
  for (int i = 0; i < 10; i++)
  {
    sumAcX += a.acceleration.x;
    sumAcY += a.acceleration.x;
    sumAcZ += a.acceleration.x;
    sumGyX += g.gyro.x;
    sumGyY += g.gyro.y;
    sumGyZ += g.gyro.z;
    delay(50);
  }
  averAcX = sumAcX / 10;
  averAcY = sumAcY / 10;
  averAcZ = sumAcY / 10;
  averGyX = sumGyX / 10;
  averGyY = sumGyY / 10;
  averGyZ = sumGyZ / 10;
}
// [출처] mpu6050.ino 를 풀어보기|작성자 초초