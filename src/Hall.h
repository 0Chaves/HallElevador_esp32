#ifndef HALL_H
#define HALL_H
#include <Arduino.h>
#include "Button.h"
#include "Led.h"
#define ULTIMO_ANDAR 3
#define TOTAL_ANDARES (ULTIMO_ANDAR + 1)

class Hall {
  private:
    int elevadorAndar;              // andar da cabine
    bool elevadorChegada;         // status de chegada do elevador
    int portaStatus;            // status da porta do elevador

  public:
    Hall();
    void atualizar(Led* leds);
    // Getters:
    int getElevadorAndar();
    bool getElevadorChegada();
    int getPortaStatus();
    // Setters:
    void setElevadorAndar(int andar);
    void setElevadorChegada(bool chegada);
    void setPortaStatus(int status);
};

#endif