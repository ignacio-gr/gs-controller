// Author  : Andres Vieitez
// Company : UVigo SpaceLab

#define MANUALPIN 35


////////////////////////////
//// Librerias externas ////
////////////////////////////
#include "ArduinoLib.h"

////////////////////////////
//// Librerias Propias  ////
////////////////////////////
#include "../lib/PositionController/PositionController.h"
#include "../lib/MotionController/MotionController.h"


////////////////////////////
////       Clases       ////
////////////////////////////
HardwareSerial& interfazSerial = Serial2;
PositionController position;
MotionController motion(&position);



////////////////////////////
////      Functions     ////
////////////////////////////
