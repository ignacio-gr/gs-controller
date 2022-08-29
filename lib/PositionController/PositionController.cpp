
#include "PositionController.h"

void PositionController::listenGPredict() {
  if (interfazSerial.available() > 0) {
    while (interfazSerial.available() > 0) {
      char c = interfazSerial.read();

      if (c == GET_POS) sendPosition();
      if (c == STOP) stopAutoMove();
      if (c == 'q') stopAutoMove();

      if (c == SET_POS) {
        saveCommand = true;
        buffer = "";
      }
      if (saveCommand && c == END_COMMAND) {
        saveCommand = false;
        parseCommandAndApply();
        buffer += END_COMMAND;
        interfazSerial.println(buffer.c_str());
      }
      if (saveCommand) buffer += c;
    }
  }  // if available
};

void PositionController::parseCommandAndApply() {
  // P180,00 45,00\n
  if (!(buffer[0] == SET_POS)) return;
  if (!(buffer[1] == ' ')) return;

  ParseNumbers parseNumbers;
  uint8_t start = 2;
  float az = parseNumbers.getValueFloat(buffer, &start, ' ');
  float el = parseNumbers.getValueFloat(buffer, &start, '\n');
  //cPrintLn((String) "Received pos- > Az: " + az + " El: " + el);
  if (az > 180) az = az - 360;
  setNewPos(az * FACTOR, el * FACTOR);
}

bool PositionController::setNewPos(int32_t az, int32_t el) {
  int32_t azStepNextAux = getStepsByDegrees(az);
  int32_t elStepNextAux = getStepsByDegrees(el);
  /*
  cPrint("New position -> Azimut: ");
  cPrint((float)az / FACTOR);
  cPrint("º Elevation: ");
  cPrint((float)el / FACTOR);
  cPrintLn("º");
  */
  if (azStepNextAux < LIMIT_AZ_UP && azStepNextAux > LIMIT_AZ_DOWN && elStepNextAux < LIMIT_EL_UP &&
      elStepNextAux > LIMIT_EL_DOWN) {
    azStepNext = azStepNextAux;
    elStepNext = elStepNextAux;
  } else {
    cPrint("New position -> Azimut: ");
    cPrint((float)az / FACTOR);
    cPrint("º Elevation: ");
    cPrint((float)el / FACTOR);
    cPrintLn("º");
    cPrintLn("Esta posicion no se puede alcanzar");
  }
  return true;
};

void PositionController::sendPosition() {
  iPrint("A");
  iPrintLn(getActualAzimutFloat());
  iPrint("E");
  iPrintLn(getActualElevationFloat());
  /*
  cPrint("A");
  cPrintLn(getActualAzimutFloat());
  cPrint("E");
  cPrintLn(getActualElevationFloat());
  */
}
