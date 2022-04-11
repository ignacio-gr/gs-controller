// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "GsControllerMain.h"

void setup() {
  Serial.begin(MONITOR_SPEED);
  interfazSerial.begin(9600);
  delay(1000);

  cPrintLn("Estación terrena UVigo SpaceLab");

  pinMode(MANUALPIN, INPUT_PULLUP);
  while (!digitalRead(MANUALPIN)) {
    motion.manualMove();
  }

  motion.initialCalibration();
  // position.clearStepsRef();
}

void loop() {
  if (!digitalRead(MANUALPIN)) {
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
