#include "MotionController.h"

void MotionController::manualMove() {
  timeBetweenPulses = 400;
  if (digitalRead(MANUAL_STOP)) {  // BTN central no pulsado
    // cPrintLn("MANUAL_STOP");
    if (!digitalRead(MANUAL_AZ_UP)) {
      cPrintLn("MANUAL_AZ_UP");
      moveAzEast();
    }
    if (!digitalRead(MANUAL_AZ_DOWN)) {
      cPrintLn("MANUAL_AZ_DOWN");
      moveAzWest();
    }
    if (!digitalRead(MANUAL_EL_UP)) {
      cPrintLn("MANUAL_EL_UP");
      moveElSouth();
    }
    if (!digitalRead(MANUAL_EL_DOWN)) {
      cPrintLn("MANUAL_EL_DOWN");
      moveElNorth();
    }
  } else {  // BTN central pulsado
    float salto = 1.0;
    uint32_t pulses = position->getStepsByDegrees(salto);
    String logPulses = (String) " " + salto + " grados";

    if (pulseAzUp() && !limitSwitchesAzimutUP()) {
      cPrintLn("MANUAL_AZ_UP_" + logPulses);
      uint32_t count = moveXPulses(AZ_PUL, AZ_DIR, UP, pulses);
      position->incAz(count);
    }
    if (pulseAzDown() && !limitSwitchesAzimutDOWN()) {
      cPrintLn("MANUAL_AZ_DOWN_" + logPulses);
      uint32_t count = moveXPulses(AZ_PUL, AZ_DIR, DOWN, pulses);
      position->decAz(count);
    }
    if (pulseElUp() && !limitSwitchesElevationDOWN()) {
      cPrintLn("MANUAL_EL_UP_" + logPulses);
      uint32_t count = moveXPulses(EL_PUL, EL_DIR, DOWN, pulses);
      position->incEl(count);
    }
    if (pulseElUp() && !limitSwitchesElevationUP()) {
      cPrintLn("MANUAL_EL_DOWN_" + logPulses);
      uint32_t count = moveXPulses(EL_PUL, EL_DIR, UP, pulses);
      position->decEl(count);
    }
  }
  static uint32_t lastPrint = millis();
  if (millis() - lastPrint > 5000) {
    cPrintLn("Manual");
    // position->printActualPosition();
    lastPrint = millis();
  }
}

void MotionController::checkPosition() {
  if (!calibrated) return;

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
    if (position->getActualStepElevation() < position->getNextStepElevation()) {
      moveElNorth();
    } else {
      moveElSouth();
    }
  }
}  // end checkPosition

void MotionController::initialCalibration() {
  if (!isInParkingPosition()) {
    cPrintLn("La antena no esta correctamente posicionada para iniciar automaticamente.");
    cPrintLn("Por favor mueve la antena manualmente para poder continuar");
    while (!isInParkingPosition()) {
      manualMove();
    }
  }

  cPrintLn("Iniciando calibracion automatica de la antena");

  while (!pulseStop())
    ;
  while (!pulseStop())
    ;

  timeBetweenPulses = 1000;

  // Azimut calibration
  while (!azIsInRef())
    ;

  uint32_t countAzUp = 0;
  uint32_t countAzDown = 0;
  cPrintLn("Move AZ UP");
  while (azIsInRef()) {
      moveAzEast();
    countAzUp++;
    //cPrintLn(countAzUp);
  }
  cPrintLn(countAzUp);
  delay(1000);
  cPrintLn("Move AZ DOWN");
  while (azIsInRef() || countAzDown < countAzUp) {
      moveAzWest();
    countAzDown++;
    //cPrintLn(countAzDown);
  }
  cPrintLn(countAzDown);
  delay(1000);
  cPrintLn("Move AZ to Center");
  uint32_t diffAz = countAzDown / 2;
  for (uint32_t i = 0; i < diffAz; i++) {
      moveAzEast();
    //cPrintLn(i);
  }
  cPrintLn("AZ in center of Ref");

  position->clearAzSteps();

  // while (true);

  // Elevation calibration

  timeBetweenPulses = 2500;

  cPrintLn("Found the ref switch");
  while (!elIsInRef()) {
      moveElNorth();
  }

  delay(1000);
  cPrintLn("Move EL UP");
  uint32_t countElUp = 0;//TODO quitar este contador y este movimiento ya no es necesario
  /*while (elIsInRef()) {
      moveElSouth();
    countElUp++;
    // cPrintLn(countElUp);
  }*/
  delay(1000);
  cPrintLn("Move EL DOWN");
  uint32_t countElDown = 0;
  while (elIsInRef() || countElDown < countElUp) {
      moveElNorth();
    countElDown++;
    // cPrintLn(countElDown);
  }
  delay(1000);
  cPrintLn("Move EL to Center");
  uint32_t diffEl = countElDown / 2;
  for (uint32_t i = 0; i < diffEl; i++) {
      moveElSouth();
    // cPrintLn(i);
  }
  cPrintLn("EL in center of Ref");

  position->clearElSteps();

  cPrintLn("Move to 0");
  float salto = 18.0;
  uint32_t pulses = position->getStepsByDegrees(salto);
  // cPrintLn((String) " " + salto + " grados");
  delay(1000);
  while (pulses > 0) {
    setDirection(EL_DIR, DOWN);
    if (pulseMotor(EL_PUL)) pulses--;
  }

  // TODO poner maximo de seguridad de movimiento

  cPrint("Calibration OK -> Azimut : ");
  // cPrintLn(countAzDown);
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

bool MotionController::pulseAzUp() {
  static bool ant = true;
  bool btn = !digitalRead(MANUAL_AZ_UP);
  if (btn && !ant) {
    delay(20);
    btn = !digitalRead(MANUAL_AZ_UP);
    if (btn && !ant) {
      ant = btn;
      return true;
    }
  }
  ant = btn;
  return false;
};

bool MotionController::pulseAzDown() {
  static bool ant = true;
  bool btn = !digitalRead(MANUAL_AZ_DOWN);
  if (btn && !ant) {
    delay(20);
    btn = !digitalRead(MANUAL_AZ_DOWN);
    if (btn && !ant) {
      ant = btn;
      return true;
    }
  }
  ant = btn;
  return false;
};

bool MotionController::pulseElUp() {
  static bool ant = true;
  bool btn = !digitalRead(MANUAL_EL_UP);
  if (btn && !ant) {
    delay(20);
    btn = !digitalRead(MANUAL_EL_UP);
    if (btn && !ant) {
      ant = btn;
      return true;
    }
  }
  ant = btn;
  return false;
};

bool MotionController::pulseElDown() {
  static bool ant = true;
  bool btn = !digitalRead(MANUAL_EL_DOWN);
  if (btn && !ant) {
    delay(20);
    btn = !digitalRead(MANUAL_EL_DOWN);
    if (btn && !ant) {
      ant = btn;
      return true;
    }
  }
  ant = btn;
  return false;
};