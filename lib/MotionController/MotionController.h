#ifndef MOTION_CONTROLLER
#define MOTION_CONTROLLER

#include "../../include/ArduinoLib.h"
#include "../PositionController/PositionController.h"

#define SW_AZ_UP 47
#define SW_AZ_DOWN 51
#define SW_AZ_2ND 53
#define SW_AZ_REF 49

#define SW_EL_UP 39
#define SW_EL_DOWN 43
#define SW_EL_2ND 45
#define SW_EL_REF 41

#define AZ_PUL 25
#define AZ_DIR 23
#define AZ_EN 33

#define EL_PUL 29
#define EL_DIR 27
#define EL_EN 31

#define UP HIGH
#define DOWN LOW

#define MANUAL_AZ_UP 36
#define MANUAL_AZ_DOWN 34
#define MANUAL_EL_UP 30
#define MANUAL_EL_DOWN 32
#define MANUAL_STOP 28
#define MANUAL_LED_A 35
#define MANUAL_LED_B 37

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
  };

  void checkPosition();
  void initialCalibration();
  bool isCalibrated() { return calibrated; };

  bool isInParkingPosition() { return azIsInRef() && !elIsInRef(); };

  void manualMove();

 private:
  PositionController* position;
  bool calibrated = false;  // TODO false
  uint16_t timePulse = 50;  // minimo 50
  uint32_t timeBetweenPulses = 180000;
  uint32_t tBP = 180000;
  uint32_t tBPaz = 180000;
  uint32_t lastPulse = 0;
  uint32_t maxSpeed = 0;

  void autoSpeed() { maxSpeed = 8000; };
  void restartSpeed() { maxSpeed = 8000; };
  void calibrationSpeed() { maxSpeed = 150000; };

  void setDirection(uint8_t pin, uint8_t dir) {
    digitalWrite(pin, dir);
    delayMicroseconds(10);
  };
  bool checkDirection(uint8_t pin, uint8_t dir) { return (digitalRead(pin) == dir); };

  bool pulseMotorEl(uint8_t pin) {
    static unsigned long lastP = 0;
    if (tBP < maxSpeed) tBP = maxSpeed;
    if (micros() - lastP > tBP / 100) {
      digitalWrite(pin, LOW);
      delayMicroseconds(5);
      digitalWrite(pin, HIGH);

      if (tBP > 1) {
        float d = tBP * 0.001;
        if (d < 1) d = 1;
        tBP = tBP - d;
      }
      if ((micros() - lastP) > timeBetweenPulses / 100 * 2) tBP = timeBetweenPulses;

      lastP = micros();
      return true;
    }
    return false;
  };

  bool pulseMotorAz(uint8_t pin) {
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
      if ((micros() - lastP) > timeBetweenPulses / 100 * 2) tBPaz = timeBetweenPulses;

      lastP = micros();
      return true;
    }
    return false;
  };

  bool moveAzWest() {
    if (limitSwitchesAzimutDOWN()) return false;
    if (!checkDirection(AZ_DIR, DOWN)) tBPaz = timeBetweenPulses;

    setDirection(AZ_DIR, DOWN);
    if (pulseMotorAz(AZ_PUL)) {
      position->decAz();
      return true;
    }
    cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveAzEast() {
    if (limitSwitchesAzimutUP()) return false;
    if (!checkDirection(AZ_DIR, UP)) tBPaz = timeBetweenPulses;
    setDirection(AZ_DIR, UP);
    if (pulseMotorAz(AZ_PUL)) {
      position->incAz();
      return true;
    }
    cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveElNorth() {
    if (limitSwitchesElevationUP()) return false;
    if (!checkDirection(EL_DIR, UP)) tBP = timeBetweenPulses;
    setDirection(EL_DIR, UP);
    if (pulseMotorEl(EL_PUL)) {
      position->decEl();
      return true;
    }
    cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool moveElSouth() {
    if (limitSwitchesElevationDOWN()) return false;
    if (!checkDirection(EL_DIR, DOWN)) tBP = timeBetweenPulses;
    setDirection(EL_DIR, DOWN);
    if (pulseMotorEl(EL_PUL)) {
      position->incEl();
      return true;
    }
    cPrint(LowDebug, "This move is not allowed");
    return false;
  };

  bool azIsInRef() { return !digitalRead(SW_AZ_REF); };
  bool elIsInRef() { return !digitalRead(SW_EL_REF); };

  bool limitSwitchesAzimut() { return !digitalRead(SW_AZ_UP) || !digitalRead(SW_AZ_DOWN) || !digitalRead(SW_AZ_2ND); };
  bool limitSwitchesElevation() {
    return !digitalRead(SW_EL_UP) || !digitalRead(SW_EL_DOWN) || !digitalRead(SW_EL_2ND);
  };

  bool limitSwitchesAzimutUP() { return !digitalRead(SW_AZ_UP) || !digitalRead(SW_AZ_2ND); };
  bool limitSwitchesAzimutDOWN() { return !digitalRead(SW_AZ_DOWN) || !digitalRead(SW_AZ_2ND); };
  bool limitSwitchesElevationUP() { return !digitalRead(SW_EL_UP) || !digitalRead(SW_EL_2ND); };
  bool limitSwitchesElevationDOWN() { return !digitalRead(SW_EL_DOWN) || !digitalRead(SW_EL_2ND); };

  bool allSwitches() { return limitSwitchesAzimut() || limitSwitchesElevation(); }

  bool pulseStop();
  bool pulseAzUp();
  bool pulseAzDown();
  bool pulseElUp();
  bool pulseElDown();
};  // end class MotionController

#endif
