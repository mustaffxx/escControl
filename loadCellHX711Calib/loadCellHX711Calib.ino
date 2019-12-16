 /*******************************************************************************
* Calibracao do Peso em kg (v1.0)
* 
* Codigo para realizar a calibracao do peso medido pela celula de carga. Para
* a calibraca serao utilizados os caracteres "n"(-1), "m"(+1), "v"(-10), "b"(+10), 
* "x"(-100), "c"(+100), para ajuste fino do fator de 
* calibracao da escala. Utilize uma peca com um peso conhecido para calibrar 
* corretamente a escala.
* 
* Copyright 2019 RoboCore.
* Escrito por Giovanni de Castro (10/04/2019).
* Creditos - Nathan Seidle - Sparkfun Electronics (2014)
*          - Bogde - GitHub library
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version (<https://www.gnu.org/licenses/>).
*******************************************************************************/

#include <HX711.h> //adiciona a biblioteca ao codigo

//configuracao dos pinos para o modulo HX711
const int PINO_DT = 5;
const int PINO_SCK = 6;

const int TEMPO_ESPERA = 200; //declaracao da variavel de espera

HX711 escala; //declaracao do objeto escala na classe HX711 da biblioteca

float fator_calibracao = -45000; //pre-definicao da variavel de calibracao

char comando; //declaracao da variavel que ira receber os comandos para alterar o fator de calibracao

void setup ()
{

  //mensagens do monitor serial
  Serial.begin(9600);
  Serial.println("Celula de carga - Calibracao de Peso");
  Serial.println("Posicione um peso conhecido sobre a celula ao comecar as leituras");

  escala.begin (PINO_DT, PINO_SCK); //inicializacao e definicao dos pinos DT e SCK dentro do objeto ESCALA

  //realiza uma media entre leituras com a celula sem carga 
  float media_leitura = escala.read_average(); 
  Serial.print("Media de leituras com Celula sem carga: ");
  Serial.print(media_leitura);
  Serial.println();

  escala.tare(); //zera a escala
}

void loop ()
{
  escala.set_scale(fator_calibracao); //ajusta a escala para o fator de calibracao

  //verifica se o modulo esta pronto para realizar leituras
  if (escala.is_ready())
  {
    //mensagens de leitura no monitor serial
    Serial.print("Leitura: ");
    Serial.print(escala.get_units(1), 1); //retorna a leitura da variavel escala com a unidade quilogramas
    Serial.print(" kg");
    Serial.print(" \t Fator de Calibracao = ");
    Serial.print(fator_calibracao);
    Serial.println();

  //alteracao do fator de calibracao
    if(Serial.available())
      {
        comando = Serial.read();
        switch (comando)
        {
          case 'x':
          fator_calibracao = fator_calibracao - 100;
          break;
          case 'c':
          fator_calibracao = fator_calibracao + 100;
          break;
          case 'v':
          fator_calibracao = fator_calibracao - 10;
          break;
          case 'b':
          fator_calibracao = fator_calibracao + 10;
          break;
          case 'n':
          fator_calibracao = fator_calibracao - 1;
          break;
          case 'm':
          fator_calibracao = fator_calibracao + 1;
          break;
        }
      }
    }
    else
    {
      Serial.print("HX-711 ocupado");
    }
  delay(TEMPO_ESPERA);
}
