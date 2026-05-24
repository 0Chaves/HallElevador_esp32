#include "Led.h"

Led::Led(int p){
  pino = p;
  estado = false;
}

void Led::init() {
  pinMode(pino, OUTPUT);
  desligar();
}

bool Led::ligar(){
  digitalWrite(pino, HIGH);
  estado = true;
  return estado;
}

bool Led::desligar(){
  digitalWrite(pino, LOW);
  estado = false;
  return estado;
}

bool Led::alternar(){
  if(estado){
    return desligar();
  }
  return ligar();
}
