#include <Arduino.h>
#include "DirectionController.h"

Direction determineDirection(int backwardPin, int forwardPin) {
  bool forwards = digitalRead(forwardPin) == HIGH;
  bool backwards = digitalRead(backwardPin) == HIGH;

  if (forwards && !backwards) {
    return FORWARD;
  } else if (!forwards && backwards) {
    return BACKWARD;
  }

  return STOP;
}
