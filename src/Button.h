#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button{
  private:
    int pin;
    int status;

  public:
    Button(int p);
    void init();
    int readStatus();
};
#endif