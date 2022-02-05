#ifndef MOTION_CONTROLLER
#define MOTION_CONTROLLER

#include "../../include/ArduinoLib.h"
#include "../PositionController/PositionController.h"

#define SW_AZ_UP 51
#define SW_AZ_DOWN 47
#define SW_AZ_2ND 53
#define SW_AZ_REF 49

#define SW_EL_UP 43
#define SW_EL_DOWN 39
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
#define MANUAL_AZ_DONW  34
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
    pinMode(MANUAL_AZ_DONW, INPUT_PULLUP);
    pinMode(MANUAL_EL_UP, INPUT_PULLUP);
    pinMode(MANUAL_EL_DOWN, INPUT_PULLUP);
    pinMode(MANUAL_STOP, INPUT_PULLUP);
  };

  void checkPosition();
  void initialCalibration();
  bool isCalibrated() { return calibrated; };

  bool azIsInRef() { return !digitalRead(SW_AZ_REF); };
  bool elIsInRef() { return !digitalRead(SW_EL_REF); };
  bool limitSwitchesAzimut() { return !digitalRead(SW_AZ_UP) || !digitalRead(SW_AZ_DOWN) || !digitalRead(SW_AZ_2ND); };
  bool limitSwitchesElevation() {
    return !digitalRead(SW_EL_UP) || !digitalRead(SW_EL_DOWN) || !digitalRead(SW_EL_2ND);
  };

  void manualMove();

 private:
  PositionController* position;
  bool calibrated = false;  // TODO false
  uint16_t timePulse = 50;
  uint32_t timeBetweenPulses = 1;
  uint32_t lastPulse = 0;

  void restartSpeed() { timeBetweenPulses = 15; };

  void setDirection(uint8_t pin, uint8_t dir) { digitalWrite(pin, dir); };

  bool pulseMotor(uint8_t pin) {
    if (micros() - lastPulse > timeBetweenPulses) {
      delayMicroseconds(timePulse);
      digitalWrite(pin, LOW);
      delayMicroseconds(timePulse);
      digitalWrite(pin, HIGH);
      delayMicroseconds(timePulse);
      lastPulse = micros();
      return true;
    }
    return false;
  };
};

#endif
