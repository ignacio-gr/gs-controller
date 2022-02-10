#include "MotionController.h"

void MotionController::manualMove() {
  if (digitalRead(MANUAL_STOP)) {  // BTN central no pulsado
    // cPrintLn("MANUAL_STOP");
    if (pulseAzUp() && limitSwitchesAzimutUP()) {
      cPrintLn("MANUAL_AZ_UP");
      setDirection(AZ_DIR, UP);
      if (pulseMotor(AZ_PUL)) position->incAz();
    }
    if (pulseAzDown() && limitSwitchesAzimutDOWN()) {
      cPrintLn("MANUAL_AZ_DOWN");
      setDirection(AZ_DIR, DOWN);
      if (pulseMotor(AZ_PUL)) position->decAz();
    }
    if (pulseElUp() && limitSwitchesElevationUP()) {
      cPrintLn("MANUAL_EL_UP");
      setDirection(EL_DIR, UP);
      if (pulseMotor(EL_PUL)) position->incEl();
    }
    if (pulseElDown() && limitSwitchesElevationDOWN()) {
      cPrintLn("MANUAL_EL_DOWN");
      setDirection(EL_DIR, DOWN);
      if (pulseMotor(EL_PUL)) position->decEl();
    }
  } else {  // BTN central pulsado
    float salto = 1.0;
    uint32_t pulses = position->getStepsByDegrees(salto);
    String logPulses = (String) " " + salto + " grados";

    if (pulseAzUp() && limitSwitchesAzimutUP()) {
      cPrintLn("MANUAL_AZ_UP_" + logPulses);
      setDirection(AZ_DIR, UP);
      uint32_t count = moveXPulses(AZ_PUL, AZ_DIR, UP, pulses);
      position->incAz(count);
    }
    if (pulseAzDown() && limitSwitchesAzimutDOWN()) {
      cPrintLn("MANUAL_AZ_DOWN_" + logPulses);
      setDirection(AZ_DIR, DOWN);
      uint32_t count = moveXPulses(AZ_PUL, AZ_DIR, DOWN, pulses);
      position->decAz(count);
    }
    if (pulseElUp() && limitSwitchesElevationUP()) {
      cPrintLn("MANUAL_EL_UP_" + logPulses);
      setDirection(EL_DIR, UP);
      uint32_t count = moveXPulses(EL_PUL, EL_DIR, UP, pulses);
      position->incEl(count);
    }
    if (pulseElUp() && limitSwitchesElevationDOWN()) {
      cPrintLn("MANUAL_EL_DOWN_" + logPulses);
      setDirection(EL_DIR, DOWN);
      uint32_t count = moveXPulses(EL_PUL, EL_DIR, DOWN, pulses);
      position->decEl(count);
    }
  }
  position->printActualPosition();
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

  // Azimut calibration
  while (!azIsInRef())
    ;
  uint32_t countAzUp = 0;
  uint32_t countAzDown = 0;
  cPrintLn("Move AZ UP");
  while (azIsInRef()) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_PUL);
    countAzUp++;
    cPrintLn(countAzUp);
  }
  cPrintLn(countAzUp);
  cPrintLn("Move AZ DOWN");
  while (azIsInRef() || countAzDown < countAzUp) {
    setDirection(AZ_DIR, DOWN);
    pulseMotor(AZ_PUL);
    countAzDown++;
    cPrintLn(countAzDown);
  }
  cPrintLn(countAzDown);
  cPrintLn("Move AZ to Center");
  uint32_t diffAz = countAzDown / 2;
  for (uint32_t i = 0; i < diffAz; i++) {
    setDirection(AZ_DIR, UP);
    pulseMotor(AZ_PUL);
    cPrintLn(i);
  }
  cPrintLn("AZ in center of Ref");

  position->clearAzSteps();

  while (true)
    ;

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