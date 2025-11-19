#include <Wire.h>
#include <ZumoShield.h>

ZumoReflectanceSensorArray reflectanceSensors;

// Define an array for holding sensor values.
#define NUM_SENSORS 6
uint16_t sensorValues[NUM_SENSORS];

void setup()
{
  Serial.begin(9600);
  reflectanceSensors.init();
 }
// 흰색에 대한 센서 출력값은 검은색에 비해 작음.
// 흰색은 작은 값, 검은색은 큰 값을 반환함.
void loop(){
  // Read the reflectance sensors.
  reflectanceSensors.read(sensorValues, QTR_EMITTERS_ON);
  for(int idx = 0; idx<NUM_SENSORS; idx++){
    Serial.print(sensorValues[idx]); Serial.print("\t");
  }
  Serial.println();
  delay(200);
}
