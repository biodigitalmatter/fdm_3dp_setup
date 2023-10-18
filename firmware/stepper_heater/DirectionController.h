#ifndef DIRECTIONCONTROLLER_H
#define DIRECTIONCONTROLLER_H

enum Direction {
  STOP,
  FORWARD,
  BACKWARD
};

Direction determineDirection(int backwardPin, int forwardPin);

#endif
