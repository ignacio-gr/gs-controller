#ifndef MOTION_CONTROLLER
#define MOTION_CONTROLLER

#include "../../include/ArduinoLib.h"
#include "../PositionController/PositionController.h"

#define SW_AZ_UP 22
#define SW_AZ_DOWN 22
#define SW_AZ_2ND 22
#define SW_AZ_REF 22

#define SW_EL_UP 22
#define SW_EL_DOWN 22
#define SW_EL_2ND 22
#define SW_EL_REF 22

#define AZ_PUL 23
#define AZ_DIR 23
#define AZ_EN 23

#define EL_PUL 23
#define EL_DIR 23
#define EL_EN 23

#define UP HIGH
#define DOWN LOW

class MotionController {
 public:
  MotionController(PositionController* position_) {
    position = position_;
    pinMode(AZ_DIR, OUTPUT);
    pinMode(AZ_DIR, OUTPUT);
    pinMode(AZ_EN, OUTPUT);
    pinMode(SW_AZ_UP, INPUT_PULLUP);
    pinMode(SW_AZ_DOWN, INPUT_PULLUP);
    pinMode(SW_AZ_2ND, INPUT_PULLUP);
    pinMode(SW_AZ_REF, INPUT_PULLUP);

    pinMode(EL_DIR, OUTPUT);
    pinMode(EL_DIR, OUTPUT);
    pinMode(EL_EN, OUTPUT);
    pinMode(SW_EL_UP, INPUT_PULLUP);
    pinMode(SW_EL_DOWN, INPUT_PULLUP);
    pinMode(SW_EL_2ND, INPUT_PULLUP);
    pinMode(SW_EL_REF, INPUT_PULLUP);
  };
  void checkSwitches();
  void checkPosition();
  void modoManual();
  void initialCalibration();

  bool azIsInRef() { return !digitalRead(SW_AZ_REF); };
  bool elIsInRef() { return !digitalRead(SW_EL_REF); };
  bool limitSwitchesAzimut(){
      return !digitalRead(SW_AZ_UP) || !digitalRead(SW_AZ_DOWN) || !digitalRead(SW_AZ_2ND);
  };
  bool limitSwitchesElevation(){
      return !digitalRead(SW_EL_UP) || !digitalRead(SW_EL_DOWN) || !digitalRead(SW_EL_2ND);
  };

 private:
  PositionController* position;
  bool calibrated = true;//TODO false
  uint16_t timePulse = 1;
  uint32_t timeBetweenPulses = 15;
  uint32_t lastPulse = 0;

  void setDirection(uint8_t pin, uint8_t dir) { digitalWrite(pin, dir); };
  bool pulseMotor(uint8_t pin) {
    if (micros() - lastPulse > timeBetweenPulses) {
      delayMicroseconds(timePulse);
      digitalWrite(pin, LOW);
      delayMicroseconds(1);
      digitalWrite(pin, HIGH);
      delayMicroseconds(timePulse);
      timeBetweenPulses = micros();
      return true;
    }
    return false;
  };
};

#endif
