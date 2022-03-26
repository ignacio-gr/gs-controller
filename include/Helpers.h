

#ifndef HELPERS_VIEITEZ
#define HELPERS_VIEITEZ
#include "ArduinoLib.h"

class ParseNumbers {
 private:
  bool isNumber(uint8_t c) { return (c >= 48 && c <= 57); };
  uint8_t getNumber(uint8_t c) {
    if (isNumber(c)) return c - 48;
    return 0;
  };

 public:
 
  float getValueFloat(String buf, uint8_t* s, char endSymbol) {
    String bufAux = buf.substring(*s);
    bufAux.replace(',', '.');
    String v = bufAux.substring(0, bufAux.indexOf(endSymbol));
    *s += v.length() + 1;
    return v.toFloat();
  }
  
  int getValue(uint8_t* buf, uint8_t* s, char endSymbol, bool decimal = false) {
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

  int getValue(String buf, uint8_t* s, char endSymbol, bool decimal) {
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
};

#endif