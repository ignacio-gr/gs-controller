
#include "PositionController.h"

void PositionController::listenGPredict() {
  if (interfazSerial.available()>0) {
    while (interfazSerial.available() > 0) {
      char c = interfazSerial.read();

      if (c == GET_POS) sendPosition();

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
  // P180.00 45.00\n
  if (!(buffer[0] == SET_POS)) return;

  ParseNumbers parseNumbers;
  uint8_t start = 1;
  int az_e = parseNumbers.getValue(buffer, &start, ',', false);
  int az_d = parseNumbers.getValue(buffer, &start, ' ', true);
  int el_e = parseNumbers.getValue(buffer, &start, ',', false);
  int el_d = parseNumbers.getValue(buffer, &start, '\n', true);

  if (az_e == -1 || az_d == -1 || el_e == -1 || el_d == -1) return;  // TODO ver bien que hacer con el error

  cPrintLn((String) "Set pos- >  az: " + az_e + "," + az_d + " el: " + el_e + "," + el_d);

  int32_t az = az_e * FACTOR + az_d * (FACTOR / 10000);
  int32_t el = el_e * FACTOR + el_d * (FACTOR / 10000);

  setNewPos(az, el);
}

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
    cPrintLn("Esta posicion no se puede alcanzar");
  }
  return true;
};

void PositionController::sendPosition() {
  iPrint("AZ");
  iPrint(getActualAzimut());
  iPrint(" EL");
  iPrint(getActualElevation());
  iPrintLn(" $");
}

void PositionController::printActualPosition(){
    /*
    cPrintLn("");
    cPrint("Position -> ");
    cPrint(" Azimut: ");
    cPrint(getActualAzimutFloat());
    cPrint(" Elevation: ");
    cPrint(getActualElevationFloat());
    cPrintLn("");

    cPrint("Steps actual -> ");
    cPrint(" Azimut: ");
    cPrint(getActualStepAzimut());
    cPrint(" Elevation: ");
    cPrint(getActualStepElevation());
    cPrintLn("");

    cPrint("Steps next ->   ");
    cPrint(" Azimut: ");
    cPrint(getNextStepAzimut());
    cPrint(" Elevation: ");
    cPrint(getNextStepElevation());
    cPrintLn("");
    */
};