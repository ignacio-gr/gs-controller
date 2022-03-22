// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "GsControllerMain.h"

void setup() {
  Serial.begin(MONITOR_SPEED);
  interfazSerial.begin(MONITOR_SPEED);
  delay(1000);

  cPrintLn("Estación terrena UVigo SpaceLab");

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

/*
  static uint32_t lastPrint = millis();
  if (millis() - lastPrint > 5000) {
    position.printActualPosition();
    lastPrint = millis();
  }
*/
  position.listenGPredict();
  motion.checkPosition();
}
