#include "NineAxesMotion.h"        //Contains the bridge code between the API and the Arduino Environment
#include <Wire.h>

NineAxesMotion mySensor;                 //Object that for the sensor
unsigned long lastStreamTime = 0;     //To store the last streamed time stamp
const int streamPeriod = 40;          //To stream at 25Hz without using additional timers (time period(ms) =1000/frequency(Hz))
bool updateSensorData = true;         //Flag to update the sensor data. Default is true to perform the first read before the first stream

void setup() //This code is executed once
{
  //Peripheral Initialization
  Serial.begin(9600);           //Initialize the Serial Port to view information on the Serial Monitor
  
  //Serial.print('{'); // ouverture du paquet
  I2C.begin();//Initialize I2C communication to the let the library communicate with the sensor.
  //Sensor Initialization
  mySensor.initSensor();          //The I2C Address can be changed here inside this function in the library
  mySensor.setOperationMode(OPERATION_MODE_NDOF);   //Can be configured to other operation modes as desired
  mySensor.setUpdateMode(MANUAL);  //The default is AUTO. Changing to manual requires calling the relevant update functions prior to calling the read functions
  //Setting to MANUAL requires lesser reads to the sensor
  mySensor.updateAccelConfig();
  updateSensorData = true;
}

void loop() //This code is looped forever
{
  if (updateSensorData)  //Keep the updating of data as a separate task
  {
    mySensor.updateAccel();        //Update the Accelerometer data
    mySensor.updateLinearAccel();  //Update the Linear Acceleration data
    mySensor.updateGravAccel();    //Update the Gravity Acceleration data
    //mySensor.updateCalibStatus();  //Update the Calibration Status
    updateSensorData = false;
  }
  if ((millis() - lastStreamTime) >= streamPeriod)
  {
    lastStreamTime = millis();
    mySensor.updateEuler();        //Update the Euler data into the structure of the object
    mySensor.updateCalibStatus();  //Update the Calibration Status

    //Serial.print(" 'Heading':");
    //Serial.print(mySensor.readEulerHeading()); //Heading data en degrés
    //Serial.print(",");
    float a = 0 ;
    a = mySensor.readEulerHeading();
    serialFloatPrint(a);
    Serial.println();


    updateSensorData = true;
  }
}

void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  Serial.print("f:");
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    
    Serial.print(c1);
    Serial.print(c2);
  }
}
