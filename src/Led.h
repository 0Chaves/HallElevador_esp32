#ifndef LED_H
#define LED_H
#include <Arduino.h>

class Led {
  private:
    int pino;
    bool estado;
  
  public:
    Led(int pino);
    void init();
    bool ligar();
    bool desligar();
    bool alternar();
};
#endif