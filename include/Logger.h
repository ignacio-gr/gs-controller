
#ifndef IPLACE_LOGGER
#define IPLACE_LOGGER

#include <Arduino.h>

enum { NotDebug, LowDebug, M, H } LevelDebug;

#define LEVEL_DEBUG LowDebug

template <typename T>
void cPrint(T const& value) {
  Serial.print(value);
}

template <typename T>
void cPrint(uint8_t l, T const& value) {
  if (l < LEVEL_DEBUG) return;
  Serial.print(value);
}

template <typename T>
void cPrint(T const& value, int format) {
  Serial.print(value, format);
}

template <typename T>
void cPrintLn(T const& value) {
  Serial.println(value);
}

template <typename T>
void cPrintLn(T const& value, int format) {
  Serial.println(value, format);
}

// Interfaz serial comunication
extern HardwareSerial& interfazSerial;

template <typename T>
void iPrint(T const& value) {
  interfazSerial.print(value);
}

template <typename T>
void iPrintLn(T const& value) {
  interfazSerial.println(value);
}

#endif