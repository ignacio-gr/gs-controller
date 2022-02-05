#include "MotionController.h"

void MotionController::manualMove() {
  if (!digitalRead(MANUAL_AZ_UP)) {
    cPrintLn("MANUAL_AZ_UP");
    setDirection(AZ_DIR, UP);
    if (pulseMotor(AZ_PUL)) position->incAz();
  }
  if (!digitalRead(MANUAL_AZ_DONW)) {
    cPrintLn("MANUAL_AZ_DONW");
    setDirection(AZ_DIR, DOWN);
    if (pulseMotor(AZ_PUL)) position->decAz();
  }
  if (!digitalRead(MANUAL_EL_UP)) {
    cPrintLn("MANUAL_EL_UP");
    setDirection(EL_DIR, UP);
    if (pulseMotor(EL_PUL)) position->incEl();
  }
  if (!digitalRead(MANUAL_EL_DOWN)) {
    cPrintLn("MANUAL_EL_DOWN");
    setDirection(EL_DIR, DOWN);
    if (pulseMotor(EL_PUL)) position->decEl();
  }
  if (!digitalRead(MANUAL_STOP)) {
    cPrintLn("MANUAL_STOP");
  }
}

void MotionController::checkPosition() {
  if (!calibrated) return;

  // Azimut
  if (position->getActualStepAzimut() != position->getNextStepAzimut() && !limitSwitchesAzimut()) {
    if (position->getActualStepAzimut() < position->getNextStepAzimut()) {
      setDirection(AZ_DIR, UP);
      if (pulseMotor(AZ_PUL)) position->incAz();
    } else {
      setDirection(AZ_DIR, DOWN);
      if (pulseMotor(AZ_PUL)) position->decAz();
    }
  }

  // Elevation
  if (position->getActualStepElevation() != position->getNextStepElevation() && !limitSwitchesElevation()) {
    if (position->getActualStepElevation() < position->getNextStepElevation()) {
      setDirection(EL_DIR, UP);
      if (pulseMotor(EL_PUL)) position->incEl();
    } else {
      setDirection(EL_DIR, DOWN);
      if (pulseMotor(EL_PUL)) position->decEl();
    }
  }
}

void MotionController::initialCalibration() {
  cPrintLn("Iniciando calibracion automatica de la antena");

  // Azimut calibration
  while (!azIsInRef())
    ;
  uint32_t countAzUp = 0;
  uint32_t countAzDown = 0;
  cPrintLn("Move AZ UP");
  while (azIsInRef()) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_DIR);
    countAzUp++;
  }
  cPrintLn(countAzUp);
  cPrintLn("Move AZ DOWN");
  while (azIsInRef() || countAzDown < countAzUp) {
    setDirection(AZ_DIR, DOWN);
    pulseMotor(AZ_DIR);
    countAzDown++;
  }
  cPrintLn(countAzDown);
  cPrintLn("Move AZ to Center");
  uint32_t diffAz = countAzDown / 2;
  for (uint32_t i = 0; i < diffAz; i++) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_DIR);
  }
  cPrintLn("AZ in center of Ref");

  position->clearAzSteps();

  // Elevation calibration
  while (!elIsInRef())
    ;
  uint32_t countElUp = 0;
  uint32_t countElDown = 0;
  cPrintLn("Move EL UP");
  while (elIsInRef()) {
    setDirection(EL_DIR, UP);
    pulseMotor(EL_DIR);
    countElUp++;
  }
  cPrintLn("Move EL DOWN");
  while (!elIsInRef() || countElDown < countElUp) {
    setDirection(EL_DIR, DOWN);
    pulseMotor(EL_DIR);
    countElDown++;
  }
  cPrintLn("Move EL to Center");
  uint32_t diffEl = countElDown / 2;
  for (uint32_t i = 0; i < diffEl; i++) {
    setDirection(EL_DIR, UP);
    pulseMotor(EL_DIR);
  }
  cPrintLn("EL in center of Ref");

  position->clearElSteps();

  // TODO poner maximo de seguridad de movimiento

  cPrint("Calibration OK -> Azimut : ");
  cPrintLn(countAzDown);
  cPrint("Calibration OK -> Elevation : ");
  cPrintLn(countElDown);
  cPrintLn("Finalizada la calibracion automatica de la antena");

  calibrated = true;
}