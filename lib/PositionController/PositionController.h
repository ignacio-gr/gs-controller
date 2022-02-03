#ifndef POSITION_CONTROLLER
#define POSITION_CONTROLLER

#include "../../include/ArduinoLib.h"

#define PRECISION 14  // 1 step = 0,00014 grados * 100.000
#define FACTOR 100000

// TODO establecer puntos iniciales de referencia, en grados
#define ERROR_EL 3 * FACTOR
#define ERROR_AZ 10 * FACTOR

// TODO establecer limites, en steps, establecerlos por lectura empirica no por grados, los pasos que esta los limites
// con respecto al Ref siempre sera igual da igual al calibracion
#define LIMIT_AZ_UP (180 + 3) * FACTOR / PRECISION
#define LIMIT_AZ_DOWN (-180 + 3) * FACTOR / PRECISION
#define LIMIT_EL_UP (180 + 10) * FACTOR / PRECISION
#define LIMIT_EL_DOWN (0 + 10) * FACTOR / PRECISION

class PositionController {
 public:
  float getActualAzimutFloat() { return (float)getActualAzimut() / FACTOR + 180; };
  float getActualElevationFloat() { return (float)getActualElevation() / FACTOR + 90; };
  int32_t getActualAzimut() { return azStepActual * PRECISION - ERROR_AZ; };
  int32_t getActualElevation() { return elStepActual * PRECISION - ERROR_EL; };

  int32_t getStepsByDegrees(int32_t degress) { return degress / PRECISION; };
  int32_t getStepsByDegrees(float degress) { return getStepsByDegrees(degress * FACTOR); };
  int32_t incAz() { return azStepActual++; };
  int32_t decAz() { return azStepActual--; };
  int32_t incEl() { return elStepActual++; };
  int32_t decEl() { return elStepActual--; };
  void clearAzSteps() {
    azStepActual = 0;
    azStepNext = 0;
  };
  void clearElSteps() {
    elStepActual = 0;
    elStepNext = 0;
  };

  int32_t getActualStepAzimut() { return azStepActual; };
  int32_t getActualStepElevation() { return elStepActual; };
  int32_t getNextStepAzimut() { return azStepNext; };
  int32_t getNextStepElevation() { return elStepNext; };

  int getValueByString(uint8_t* buf, uint8_t* s, char endSymbol, bool decimal = false);
  bool isNumber(uint8_t c) { return (c >= 48 && c <= 57); };
  uint8_t getNumber(uint8_t c) {
    if (isNumber(c)) return c - 48;
    return 0;
  };
  void listenGPredict();

 private:
  int32_t azStepActual = 0;
  int32_t elStepActual = 0;
  int32_t azStepNext = 0;
  int32_t elStepNext = 0;

  bool setNewPos(int32_t az, int32_t el);
};

#endif
