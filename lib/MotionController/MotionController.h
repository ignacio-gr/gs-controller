#ifndef MOTION_CONTROLLER
#define MOTION_CONTROLLER

#include "../../include/ArduinoLib.h"
#include "../PositionController/PositionController.h"

#define SW_AZ_UP 59
#define SW_AZ_DOWN 58
#define SW_AZ_2ND 61
#define SW_AZ_REF 60

#define SW_EL_UP 55
#define SW_EL_DOWN 54
#define SW_EL_2ND 57
#define SW_EL_REF 56


#define AZ_PUL 67
#define AZ_DIR 66
#define AZ_EN 62

#define EL_PUL 65
#define EL_DIR 64
#define EL_EN 63

#define UP HIGH
#define DOWN LOW

#define MANUAL_AZ_UP 2
#define MANUAL_AZ_DOWN 5
#define MANUAL_EL_UP 6
#define MANUAL_EL_DOWN 8
#define MANUAL_STOP 4
#define MANUAL_START 7
#define NOT_USED_REMOTE 35

/*
east 36
stop 34
west 32
up 30
down 37
start 28


#define MANUAL_AZ_UP 36
#define MANUAL_AZ_DOWN 34
#define MANUAL_EL_UP 30
#define MANUAL_EL_DOWN 32
#define MANUAL_STOP 28
#define MANUAL_START 35
#define NOT_USED_REMOTE 37
*/

class MotionController {
 public:
  MotionController(PositionController* position_) {
    position = position_;
    pinMode(AZ_PUL, OUTPUT);
    pinMode(AZ_DIR, OUTPUT);
    pinMode(AZ_EN, OUTPUT);
    pinMode(SW_AZ_UP, INPUT_PULLUP);
    pinMode(SW_AZ_DOWN, INPUT_PULLUP);
    pinMode(SW_AZ_2ND, INPUT_PULLUP);
    pinMode(SW_AZ_REF, INPUT_PULLUP);

    pinMode(EL_PUL, OUTPUT);
    pinMode(EL_DIR, OUTPUT);
    pinMode(EL_EN, OUTPUT);
    pinMode(SW_EL_UP, INPUT_PULLUP);
    pinMode(SW_EL_DOWN, INPUT_PULLUP);
    pinMode(SW_EL_2ND, INPUT_PULLUP);
    pinMode(SW_EL_REF, INPUT_PULLUP);

    pinMode(MANUAL_AZ_UP, INPUT_PULLUP);
    pinMode(MANUAL_AZ_DOWN, INPUT_PULLUP);
    pinMode(MANUAL_EL_UP, INPUT_PULLUP);
    pinMode(MANUAL_EL_DOWN, INPUT_PULLUP);
    pinMode(MANUAL_STOP, INPUT_PULLUP);
    pinMode(MANUAL_START, INPUT_PULLUP);

    pinMode(NOT_USED_REMOTE, INPUT);
  };

  void checkPosition();
  void initialCalibration();
  bool isCalibrated() { return calibrated; };

  bool isInParkingPosition() { return azIsInRef() && !elIsInRef(); };

  void manualMove();

 private:
  PositionController* position;

  bool calibrated = false;
  bool isManual = false;

  uint32_t lastPulse = 0;
  uint16_t timePulse = 50;     // 50
  uint32_t tBPel = 180000;     // 180000
  uint32_t tBPaz = 180000;     // 180000
  uint32_t maxSpeed = 150000;  // 8000
  uint32_t minSpeed = 180000;  // 180000
  uint32_t startDeceleration = 3175;

  void autoSpeed() { maxSpeed = 50000; };           // 8000 // 9000
  void restartSpeed() { maxSpeed = 50000; };        // 8000 // 9000
  void calibrationSpeed() { maxSpeed = 150000; };  // 150000

  bool safetyBroken() { return digitalRead(MANUAL_STOP); };

  void setDirection(uint8_t pin, uint8_t dir) {
    digitalWrite(pin, dir);
    delayMicroseconds(10);  // TODO estudiar quitar esto
  };

  bool checkDirection(uint8_t pin, uint8_t dir) { return (digitalRead(pin) == dir); };

  void changeSpeedAcAndDecElevation() {
    uint32_t dif = position->getDifEl();
    if (dif < startDeceleration && !isManual) {
      if (tBPel > 1) {
        float d = tBPel * 0.001;
        if (d < 1) d = 1;
        tBPel = tBPel + d;
        if (tBPel > minSpeed) tBPel = minSpeed;
      }
    } else if (tBPel > maxSpeed) {
      float d = tBPel * 0.001;
      if (d < 1) d = 1;
      tBPel = tBPel - d;
    }
  };

  bool pulseMotorEl(uint8_t pin) {
    if (safetyBroken()) return false;
    static unsigned long lastP = 0;
    if (tBPel < maxSpeed) tBPel = maxSpeed;
    if (micros() - lastP > tBPel / 100) {
      digitalWrite(pin, LOW);
      delayMicroseconds(5);
      digitalWrite(pin, HIGH);

      changeSpeedAcAndDecElevation();
      if ((micros() - lastP) > minSpeed / 100 * 2) tBPel = minSpeed;

      lastP = micros();
      return true;
    }
    return false;
  };

  bool pulseMotorAz(uint8_t pin) {
    if (safetyBroken()) return false;
    static unsigned long lastP = 0;
    if (tBPaz < maxSpeed) tBPaz = maxSpeed;
    if (micros() - lastP > tBPaz / 100) {
      digitalWrite(pin, LOW);
      delayMicroseconds(5);
      digitalWrite(pin, HIGH);

      if (tBPaz > 1) {
        float d = tBPaz * 0.001;
        if (d < 1) d = 1;
        tBPaz = tBPaz - d;
      }
      if ((micros() - lastP) > minSpeed / 100 * 2) tBPaz = minSpeed;

      lastP = micros();
      return true;
    }
    return false;
  };

  bool moveAzWest() {
    //cPrintLn("moveAzWest");
    if (limitSwitchesAzimutDOWN()) return false;
    if (!checkDirection(AZ_DIR, DOWN)) tBPaz = minSpeed;

    setDirection(AZ_DIR, DOWN);
    if (pulseMotorAz(AZ_PUL)) {
      position->decAz();
      return true;
    }
    //cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveAzEast() {
    //cPrintLn("moveAzEast");
    if (limitSwitchesAzimutUP()) return false;
    if (!checkDirection(AZ_DIR, UP)) tBPaz = minSpeed;
    setDirection(AZ_DIR, UP);
    if (pulseMotorAz(AZ_PUL)) {
      position->incAz();
      return true;
    }
    //cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveElNorth() {
    
    //cPrintLn("moveElNorth");
    if (limitSwitchesElevationUP()) return false;
    if (!checkDirection(EL_DIR, UP)) tBPel = minSpeed;
    setDirection(EL_DIR, UP);
    if (pulseMotorEl(EL_PUL)) {
      position->decEl();
      return true;
    }
    //cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveElSouth() {
    
    //cPrintLn("moveElSouth");
    if (limitSwitchesElevationDOWN()) return false;
    if (!checkDirection(EL_DIR, DOWN)) tBPel = minSpeed;
    setDirection(EL_DIR, DOWN);
    if (pulseMotorEl(EL_PUL)) {
      position->incEl();
      return true;
    }
    //cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool azIsInRef() { return digitalRead(SW_AZ_REF); };
  bool elIsInRef() { return digitalRead(SW_EL_REF); };

  bool limitSwitchesAzimutUP() { return digitalRead(SW_AZ_UP) || digitalRead(SW_AZ_2ND); };
  bool limitSwitchesAzimutDOWN() { return digitalRead(SW_AZ_DOWN) || digitalRead(SW_AZ_2ND); };
  bool limitSwitchesElevationUP() { return digitalRead(SW_EL_UP) || digitalRead(SW_EL_2ND); };
  bool limitSwitchesElevationDOWN() { return digitalRead(SW_EL_DOWN) || digitalRead(SW_EL_2ND); };

  bool pulseStop();

};  // end class MotionController

#endif
