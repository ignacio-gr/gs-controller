// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#include "GsControllerMain.h"
void setup() {
  Serial.begin(MONITOR_SPEED);
  gPredictSerial.begin(MONITOR_SPEED);
  interfazSerial.begin(MONITOR_SPEED);
  delay(1000);

  cPrintLn("Estación terrena UVigo SpaceLab");

  if (!(motion.azIsInRef() && motion.elIsInRef())) {
    cPrintLn("La antena no esta correctamente posicionada para iniciar automaticamente.");
    cPrintLn("Por favor mueve la antena manualmente para poder continuar");
    while (!(motion.azIsInRef() && motion.elIsInRef())) {
      // TODO movimientos manuales para posicionar la antena tocando Ref
      motion.manualMove();
    }
  }

    while (true) {
      // TODO delete this while
      motion.manualMove();
    }

  delay(1000);
  motion.initialCalibration();
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