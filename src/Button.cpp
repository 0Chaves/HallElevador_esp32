#include "Button.h"

Button::Button(int p){
  pin = p;
  status = LOW;
}

void Button::init(){
  pinMode(pin, INPUT_PULLDOWN);
  status = digitalRead(pin);
}

int Button::readStatus(){
  status = digitalRead(pin);
  return status;
}