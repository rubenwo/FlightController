#include "drivers/imu.h"

volatile bool mpuInterrupt = false;
void dmpDataReady()
{
  mpuInterrupt = true;
}
IMU::IMU(const uint interrupt_pin) : interrupt_pin(interrupt_pin)
{
  mpu.initialize();
  pinMode(interrupt_pin, INPUT);

  devStatus = mpu.dmpInitialize();
  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(51);
  mpu.setYGyroOffset(22);
  mpu.setZGyroOffset(-23);
  mpu.setZAccelOffset(1830); // 1688 factory default for my test chip
  mpu.setXAccelOffset(274);
  mpu.setYAccelOffset(63);
  
  if (devStatus == 0)
  {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();

    mpu.setDMPEnabled(true);
    attachInterrupt(interrupt_pin, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    Serial.println("DMP initialization failed!");
  }
}

IMU::~IMU()
{
}

void IMU::update()
{
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) // Get the Latest packet
  {
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  }
}