#include "MotionController.h"

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
  if (calibrated) return;

  // Azimut calibration
  if (!azIsInRef()) return;
  int32_t countAzUp = 0;
  int32_t countAzDown = 0;
  while (!azIsInRef()) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_DIR);
    countAzUp++;
  }
  while (!azIsInRef() || countAzDown < countAzUp) {
    setDirection(AZ_DIR, DOWN);
    pulseMotor(AZ_DIR);
    countAzDown++;
  }
  int32_t diffAz = countAzDown / 2;
  for (size_t i = 0; i < diffAz; i++) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_DIR);
  }

  position->clearAzSteps();

  // Elevation calibration
  if (!elIsInRef()) return;
  int32_t countElUp = 0;
  int32_t countElDown = 0;
  while (!elIsInRef()) {
    setDirection(EL_DIR, UP);
    pulseMotor(EL_DIR);
    countElUp++;
  }
  while (!elIsInRef() || countElDown < countElUp) {
    setDirection(EL_DIR, DOWN);
    pulseMotor(EL_DIR);
    countElDown++;
  }
  int32_t diffEl = countElDown / 2;
  for (size_t i = 0; i < diffEl; i++) {
    setDirection(EL_DIR, UP);
    pulseMotor(EL_DIR);
  }

  position->clearElSteps();

  // TODO poner maximo de seguridad de movimiento

  cPrint("Calibration OK -> Azimut : ");
  cPrint(diffAz);
  cPrint("Calibration OK -> Elevation : ");
  cPrint(diffAz);
  cPrintLn("");

  calibrated = true;
}