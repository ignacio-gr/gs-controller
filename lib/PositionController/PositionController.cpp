
#include "PositionController.h"

void PositionController::listenGPredict() {
  int count = gPredictSerial.available();
  if (count > 0) {
    char comando[50];
    memset(comando, 0, sizeof(comando));
    uint8_t pos = 0;

    while (gPredictSerial.available() > 0) {
      delay(5);
      comando[pos] = gPredictSerial.read();
      pos++;
      if (pos > count) break;
    }
    cPrint("comando recivido -> ");
    cPrintLn(comando);

    //  AZ EL
    if (comando[0] == 'A' && comando[1] == 'Z' && comando[2] == ' ' && comando[3] == 'E' && comando[4] == 'L') {
      cPrintLn("Comando recivido de lectura de posicion");

      gPrint("AZ");
      gPrint(getActualAzimutFloat());
      gPrint(" EL");
      gPrint(getActualElevationFloat());
      gPrintLn("");
    }

    // AZ57.1 EL15.1 UP000 XXX DN000 XXX
    if (comando[0] == 'A' && comando[1] == 'Z' && isNumber(comando[2])) {
      cPrintLn("Comando recivido de escritura de posicion");

      uint8_t start = 2;
      uint16_t value = getValueByString((uint8_t*)comando, &start, '.');
      uint16_t decimal = getValueByString((uint8_t*)comando, &start, ' ', true);

      uint32_t degreesAz = value * FACTOR + decimal * (FACTOR / 10000);

      if (!(comando[start] == 'E' && comando[start + 1] == 'L' && isNumber(comando[start + 2]))) return;
      start = start + 2;
      uint16_t valueEL = getValueByString((uint8_t*)comando, &start, '.');
      uint16_t decimalEl = getValueByString((uint8_t*)comando, &start, ' ', true);
      uint32_t degreesEl = valueEL * FACTOR + decimalEl * (FACTOR / 10000);

      // Response to gPredict
      gPrint(comando);  // TODO construir, non devolver o mesmo. Se devolve mesmos valores ou los actuales?
      gPrintLn(" +OK");
      setNewPos(degreesAz, degreesEl);
    }
  }  // if available
};

bool PositionController::setNewPos(int32_t az, int32_t el) {
  int32_t azStepNextAux = getStepsByDegrees(az - 180 * FACTOR + ERROR_AZ);
  int32_t elStepNextAux = getStepsByDegrees(el + ERROR_EL);
  cPrint("New position -> Azimut: ");
  cPrint((float)az / FACTOR);
  cPrint("º Elevation: ");
  cPrint((float)el / FACTOR);
  cPrintLn("º");

  if (azStepNextAux < LIMIT_AZ_UP && azStepNextAux > LIMIT_AZ_DOWN && elStepNextAux < LIMIT_EL_UP &&
      elStepNextAux > LIMIT_EL_DOWN) {
    azStepNext = azStepNextAux;
    elStepNext = elStepNextAux;
  } else {
    cPrintLn("Esta posicion no se peude alcanzar");
  }
  return true;
};

int PositionController::getValueByString(uint8_t* buf, uint8_t* s, char endSymbol, bool decimal) {
  int value = -1;
  uint8_t start = *s;
  if (isNumber(buf[start]) && buf[start + 1] == endSymbol) {
    value = getNumber(buf[start]);
    *s += 2;
    if (decimal) value = value * 1000;
  }
  if (isNumber(buf[start]) && isNumber(buf[start + 1]) && buf[start + 2] == endSymbol) {
    value = getNumber(buf[start]) * 10;
    value += getNumber(buf[start + 1]);
    *s += 3;
    if (decimal) value = value * 100;
  }
  if (isNumber(buf[start]) && isNumber(buf[start + 1]) && isNumber(buf[start + 2]) && buf[start + 3] == endSymbol) {
    value = getNumber(buf[start]) * 100;
    value += getNumber(buf[start + 1]) * 10;
    value += getNumber(buf[start + 2]);
    *s += 4;
    if (decimal) value = value * 10;
  }
  if (isNumber(buf[start]) && isNumber(buf[start + 1]) && isNumber(buf[start + 2]) && isNumber(buf[start + 3]) &&
      buf[start + 4] == endSymbol) {
    value = getNumber(buf[start]) * 1000;
    value += getNumber(buf[start + 1]) * 100;
    value += getNumber(buf[start + 2]) * 10;
    value += getNumber(buf[start + 3]);
    *s += 5;
  }

  if (value > 9999) value = -1;

  return value;
};
