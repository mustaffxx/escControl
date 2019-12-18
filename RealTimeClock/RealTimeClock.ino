#include <Wire.h>
#include "RTClib.h" 

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

void setup(){
  Serial.begin(9600); 
  if(! rtc.begin()) { /
    Serial.println("DS3231 não encontrado"); 
    while(1); 
  }
  if(rtc.lostPower()){ 
    Serial.println("DS3231 OK!"); 
    //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
    //rtc.adjust(DateTime(2019, 12, 18, 16, 00, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
  //rtc.adjust(DateTime(2019, 12, 18, 16, 07, 30)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  
  delay(100);
}

void loop () {
    DateTime now = rtc.now(); 
    Serial.print("Data: "); 
    Serial.print(now.day(), DEC); 
    Serial.print('/'); 
    Serial.print(now.month(), DEC);
    Serial.print('/'); 
    Serial.print(now.year(), DEC); 
    Serial.print(" / Dia: "); 
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); 
    Serial.print(" / Horas: "); 
    Serial.print(now.hour(), DEC); 
    Serial.print(':'); 
    Serial.print(now.minute(), DEC);
    Serial.print(':'); 
    Serial.print(now.second(), DEC); 
    Serial.println(); 
    delay(1000);
}
