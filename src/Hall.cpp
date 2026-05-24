#include "Hall.h"

Hall::Hall(){
    elevadorAndar = 0;
    elevadorChegada = false;
    portaStatus = 1;
}

void Hall::atualizar(Led* leds){
    for(int i = 0; i<TOTAL_ANDARES; i++){
        if(i == elevadorAndar){
            leds[i].ligar();
        }
        else{
            leds[i].desligar();
        }
    }
    if(elevadorChegada && portaStatus == 1){
        leds[TOTAL_ANDARES+1].ligar();
        delay(2000);
        leds[TOTAL_ANDARES+1].desligar();
    }
}

int Hall::getElevadorAndar(){
    return elevadorAndar;
}

bool Hall::getElevadorChegada(){
    return elevadorChegada;
}

int Hall::getPortaStatus(){
    return portaStatus;
}

void Hall::setElevadorAndar(int andar){
    elevadorAndar = andar;
}

void Hall::setElevadorChegada(bool chegada){
    elevadorChegada = chegada;
}

void Hall::setPortaStatus(int status){
    portaStatus = status;
}