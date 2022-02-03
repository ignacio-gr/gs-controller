// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "../include/GsControllerMain.h"
void setup() {
  Serial.begin(MONITOR_SPEED);
  gPredictSerial.begin(MONITOR_SPEED);
  interfazSerial.begin(MONITOR_SPEED);
}

void loop() {
  static uint32_t lastPrint = millis();
  if (millis() - lastPrint > 5000) {
    printActualPosition();
    lastPrint = millis();
  }
  /*
  static uint32_t lastInc = micros();
  if (micros() - lastInc > 100) {
    position.incAz();
    position.incEl();
    lastInc = micros();
  }*/
  position.listenGPredict();
  motion.checkPosition();
}

void printActualPosition() {
  Serial.println("");
  Serial.print("Position -> ");
  Serial.print(" Azimut: ");
  Serial.print(position.getActualAzimutFloat());
  Serial.print(" Elevation: ");
  Serial.print(position.getActualElevationFloat());
  Serial.println("");

  Serial.print("Steps actual -> ");
  Serial.print(" Azimut: ");
  Serial.print(position.getActualStepAzimut());
  Serial.print(" Elevation: ");
  Serial.print(position.getActualStepElevation());
  Serial.println("");


  Serial.print("Steps next ->   ");
  Serial.print(" Azimut: ");
  Serial.print(position.getNextStepAzimut());
  Serial.print(" Elevation: ");
  Serial.print(position.getNextStepElevation());
  Serial.println("");
}