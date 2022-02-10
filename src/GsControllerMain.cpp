// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "GsControllerMain.h"
void setup() {
  Serial.begin(MONITOR_SPEED);
  gPredictSerial.begin(MONITOR_SPEED);
  interfazSerial.begin(MONITOR_SPEED);
  delay(1000);

  cPrintLn("Estación terrena UVigo SpaceLab");

  // TODO delete this while
  pinMode(13, INPUT_PULLUP);
  while (!digitalRead(13)) {
    motion.manualMove();
  }

  motion.initialCalibration();
}

void loop() {
  while (!digitalRead(13)) {
    motion.manualMove();
  }

  static uint32_t lastPrint = millis();
  if (millis() - lastPrint > 5000) {
    printActualPosition();
    lastPrint = millis();
  }

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