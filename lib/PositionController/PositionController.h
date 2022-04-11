#ifndef POSITION_CONTROLLER
#define POSITION_CONTROLLER

#include "../../include/ArduinoLib.h"

// 1 step = 0.00014 grados * 100000 // 14 grados por 100000 steps con 3200 uStep -> 15.625 pasos completos
#define PRECISION 14
#define FACTOR 100000
#define MICRO_STEPS_REF 3200
#define MICRO_STEPS 400

// TODO establecer puntos iniciales de referencia, en grados
#define REF_EL 74.80
#define REF_AZ 0.0

// TODO establecer limites, en steps, establecerlos por lectura empirica no por grados, los pasos que esta los limites
// con respecto al Ref siempre sera igual da igual al calibracion
#define LIMIT_AZ_UP ((170 + 1) * FACTOR / PRECISION * MICRO_STEPS / MICRO_STEPS_REF)
#define LIMIT_AZ_DOWN ((-150 + 1) * FACTOR / PRECISION * MICRO_STEPS / MICRO_STEPS_REF)
#define LIMIT_EL_UP ((180 + 1) * FACTOR / PRECISION * MICRO_STEPS / MICRO_STEPS_REF)
#define LIMIT_EL_DOWN ((0 - 1) * FACTOR / PRECISION * MICRO_STEPS / MICRO_STEPS_REF)

class PositionController {
 public:
  int32_t incAz() { return azStepActual++; };
  int32_t decAz() { return azStepActual--; };
  int32_t incEl() { return elStepActual++; };
  int32_t decEl() { return elStepActual--; };

  void clearStepsRef() {
    azStepActual = getStepsByDegrees((float)(0.0));
    azStepNext = azStepActual;

    elStepActual = getStepsByDegrees((float)(90.0 - 15.20));
    elStepNext = elStepActual;
  }

  void setAzElNext(float az, float el) {
    azStepNext = getStepsByDegrees(az);
    elStepNext = getStepsByDegrees(el);
  };

  void stopAutoMove() {
    azStepNext = azStepActual;
    elStepNext = elStepActual;
  };

  int32_t getActualStepAzimut() { return azStepActual; };
  int32_t getActualStepElevation() { return elStepActual; };
  int32_t getNextStepAzimut() { return azStepNext; };
  int32_t getNextStepElevation() { return elStepNext; };

  void listenGPredict();

  void sendPosition();

 private:
  int32_t azStepActual = 0;
  int32_t elStepActual = 0;
  int32_t azStepNext = 0;
  int32_t elStepNext = 0;

  bool setNewPos(int32_t az, int32_t el);
  void parseCommandAndApply();

  String buffer = "";
  bool saveCommand = false;

  
  float getActualAzimutFloat() {
    float az = (float)getActualAzimut() / FACTOR;
    if (az < 0) az = az + 360;
    return az;
  };
  float getActualElevationFloat() { return (float)getActualElevation() / FACTOR; };
  int32_t getActualAzimut() { return azStepActual * PRECISION * MICRO_STEPS_REF / MICRO_STEPS; };
  int32_t getActualElevation() { return elStepActual * PRECISION * MICRO_STEPS_REF / MICRO_STEPS; };

  int32_t getStepsByDegrees(int32_t degress) { return degress / PRECISION * MICRO_STEPS / MICRO_STEPS_REF; };
  int32_t getStepsByDegrees(float degress) { return getStepsByDegrees((int32_t)(degress * FACTOR)); };

};

#endif
