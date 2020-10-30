#include "mbed.h"

InterruptIn button(p5);
DigitalOut red_led(p6);
DigitalOut yellow_led(p7);
DigitalOut blue_led(p8);

Timeout timeout;
Ticker ticker;

enum STATE
{
  on,
  off,
  alerta,
  inicia,
  vermelho,
  amarelo,
  verde
};

int current_state;
int previe_state;
int manual_count;
int stateButton;

void mudaVermelho(){
  printf("vermelho\n");
  red_led = 1;
  previe_state = current_state;
  current_state = vermelho;
  timeout.attach(&transitionState, 10);
}

void mudaAmarelo(){
  printf("amarelo\n");
  yellow_led = 1;
  previe_state = current_state;
  current_state = amarelo;
  timeout.attach(&transitionState, 4);
}

void mudaAmarelo1Hz(){
  yellow_led = !yellow_led;
  ticker.attach(mudaAmarelo1Hz, 0.5);
}

// O nome no codigo foi led verde, mas no Mbed simulator foi led azul...
void mudaVerde(){
  printf("verde\n");
  blue_led = 1;
  previe_state = current_state;
  current_state = verde;
  timeout.attach(&transitionState, 20);
}

void alertaando(){
  printf("alertaa\n");
  previe_state = current_state;
  current_state = alerta;
  mudaAmarelo1Hz();
}

void desliga(){
  printf("desligado\n");
  previe_state = current_state;
  current_state = off;
  estadoPadrao();
}

void manual_count_fun(){
  manual_count++;
}

void iniciaContador(){
  printf("Inciciando contador\n");
  timeout.detach();
  manual_count = 0;
  ticker.attach(manual_count_fun, 1);
}

void fimContador()
{
  printf("Finalizando contador\n");
  ticker.detach();
  stateButton = on;
  transitionState();
}

void transitionState(){
  estadoPadrao();
  if (current_state == inicia && previe_state == inicia)
  {
    printf("Iniciando vermelho\n");
    red_led = 1;
    previe_state = vermelho;
    current_state = vermelho;
    timeout.attach(&transitionState, 10);
  }
  else if (stateButton == off)
  {
    if (current_state == vermelho)
    {
      mudaVerde();
    }
    else if (current_state == verde)
    {
      mudaAmarelo();
    }
    else if (current_state == amarelo)
    {
      mudaVermelho();
    }
  }
  else if (stateButton == on)
  {
    stateButton = off;
    if (current_state == vermelho)
    {
      if (manual_count >= 3 && manual_count <= 10)
      {
        alertaando();
      }
      else if (manual_count > 10)
      {
        desliga();
      }
      else
      {
        mudaVermelho();
      }
    }
    else if (current_state == amarelo)
    {
      if (manual_count >= 3 && manual_count <= 10)
      {
        alertaando();
      }
      else if (manual_count > 10)
      {
        desliga();
      }
      else
      {
        mudaAmarelo();
      }
    }
    else if (current_state == verde)
    {
      if (manual_count >= 3 && manual_count <= 10)
      {
        alertaando();
      }
      else if (manual_count == 20 || manual_count <= 3)
      {
        mudaAmarelo();
      }
      else if (manual_count > 10)
      {
        desliga();
      }
      else
      {
        mudaVerde();
      }
    }
    else if (current_state == alerta)
    {
      if (manual_count >= 3 && manual_count <= 10)
      {
        if (previe_state == vermelho)
        {
          mudaVermelho();
        }
        else if (previe_state == amarelo)
        {
          mudaAmarelo();
        }
        else
        {
          mudaVerde();
        }
      }
      else if (manual_count > 10)
      {
        desliga();
      }
      else
      {
        current_state = previe_state;
        alertaando();
      }
    }
    else if (current_state == off)
    {
      if (manual_count > 10)
      {
        mudaVermelho();
      }
      else
      {
        desliga();
      }
    }
  }
}

void estadoPadrao(){
  red_led = 0;
  yellow_led = 0;
  blue_led = 0;

  timeout.detach();
  ticker.detach();
}

int main()
{
  current_state = inicia;
  previe_state = inicia;
  transitionState();

  button.rise(&iniciaContador);
  button.fall(&fimContador);

  while (1)
  {
    wait(1);
    printf("Contando botao %d\n", manual_count);
  }
}
