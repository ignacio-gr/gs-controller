#include "MotionController.h"

void MotionController::manualMove() {
  autoSpeed();
  isManual = true;
/*cPrint("MANUAL_AZ_UP");
cPrintLn(!digitalRead(MANUAL_AZ_UP));
cPrint("MANUAL_AZ_DOWN");
cPrintLn(!digitalRead(MANUAL_AZ_DOWN));
cPrint("MANUAL_EL_UP");
cPrintLn(!digitalRead(MANUAL_EL_UP));
cPrint("MANUAL_EL_DOWN");
cPrintLn(!digitalRead(MANUAL_EL_DOWN));*/
  if (!digitalRead(MANUAL_AZ_UP)) moveAzEast();
  if (!digitalRead(MANUAL_AZ_DOWN)) moveAzWest();
  if (!digitalRead(MANUAL_EL_UP)) moveElSouth();
  if (!digitalRead(MANUAL_EL_DOWN)) moveElNorth();

  position->stopAutoMove();

  /*
  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();
    position->sendPosition();
  }
  */
}  // end Manual

void MotionController::checkPosition() {
  autoSpeed();
  isManual = false;

  // Azimut
  if (position->getActualStepAzimut() != position->getNextStepAzimut()) {
    if (position->getActualStepAzimut() < position->getNextStepAzimut()) {
      moveAzEast();
    } else {
      moveAzWest();
    }
  }

  // Elevation
  if (position->getActualStepElevation() != position->getNextStepElevation()) {
    if (position->getActualStepElevation() > position->getNextStepElevation()) {
      moveElNorth();
    } else {
      moveElSouth();
    }
  }
  /*
    static unsigned long last = 0;
    if (millis() - last > 5000) {
      last = millis();
      position->sendPosition();
    }
    */
}  // end checkPosition

void MotionController::initialCalibration() {
  while (digitalRead(MANUAL_STOP))
    ;

    

  // TODO ahora EL no toca le Switch de ref, hacer que no lo toque o dejar empezar tocandolo?
  if (!isInParkingPosition()) {
    cPrintLn("La antena no esta correctamente posicionada para iniciar automaticamente.");
    cPrintLn("Por favor mueve la antena manualmente para poder continuar");
    while (!isInParkingPosition()) {
      manualMove();
    }
  }

  cPrintLn("Iniciando calibracion automatica de la antena");

  calibrationSpeed();

  // Azimut calibration
  while (!azIsInRef())
    ;

  uint32_t countAzUp = 0;
  uint32_t countAzDown = 0;
  cPrintLn("Move AZ UP");
  while (azIsInRef()) {
    moveAzEast();
    countAzUp++;
  }
  cPrintLn(countAzUp);
  delay(1000);
  cPrintLn("Move AZ DOWN");
  while (azIsInRef() || countAzDown < countAzUp) {
    moveAzWest();
    countAzDown++;
  }
  cPrintLn(countAzDown);
  delay(1000);
  cPrintLn("Move AZ to Center");
  uint32_t diffAz = countAzDown / 2;
  for (uint32_t i = 0; i < diffAz; i++) {
    moveAzEast();
  }
  cPrintLn("AZ in center of Ref");

  delay(1000);

  // Elevation calibration

  cPrintLn("Found the ref switch");
  while (!elIsInRef()) {
    moveElNorth();
  }
  /*
  delay(1000);
  cPrintLn("Move EL UP");
  uint32_t countElUp = 0;//TODO quitar este contador y este movimiento ya no es necesario
  while (elIsInRef()) {
      moveElSouth();
    countElUp++;
  }*/
  delay(2000);
  cPrintLn("Move EL DOWN");
  uint32_t countElDown = 0;
  while (elIsInRef() || countElDown < 100) {
    moveElNorth();
    countElDown++;
  }
  delay(2000);
  cPrintLn("Move EL to Center");
  uint32_t diffEl = countElDown / 2;
  for (uint32_t i = 0; i < diffEl; i++) {
    moveElSouth();
  }
  cPrintLn("EL in center of Ref");

  position->clearStepsRef();

  delay(500);
  cPrintLn("Move to (0.0 90.0)");
  position->setAzElNext(0.0, 90.0);
  autoSpeed();

  cPrint("Calibration OK -> Azimut : ");
  cPrintLn(countAzDown);
  cPrint("Calibration OK -> Elevation : ");
  cPrintLn(countElDown);
  cPrintLn("Finalizada la calibracion automatica de la antena");

  calibrated = true;
}  // end initialCalibration

///////////////////////////
/////     Helpers     /////
///////////////////////////

bool MotionController::pulseStop() {
  static bool ant = true;
  bool btn = !digitalRead(MANUAL_STOP);
  if (btn && !ant) {
    delay(20);
    btn = !digitalRead(MANUAL_STOP);
    if (btn && !ant) {
      ant = btn;
      return true;
    }
  }
  ant = btn;
  return false;
};
