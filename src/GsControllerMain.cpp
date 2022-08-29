// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "GsControllerMain.h"

void setup() {
  Serial.begin(MONITOR_SPEED);
  interfazSerial.begin(9600);
  delay(1000);

  cPrintLn("Estación terrena UVigo SpaceLab");

  if (digitalRead(MANUAL_STOP)) cPrintLn("El mando esta apagado");

  pinMode(MANUAL_START, INPUT_PULLUP);
  while (!digitalRead(MANUAL_START)) {
    motion.manualMove();
  }

  motion.initialCalibration();
  // position.clearStepsRef();
}

void loop() {
  if (!digitalRead(MANUAL_START)) {
    motion.manualMove();
  } else {
    motion.checkPosition();
  }

  position.listenGPredict();

  /*
    static unsigned long last = 0;
    if (millis() - last > 5000) {
      last = millis();
      position->sendPosition();
    }
    */
}
