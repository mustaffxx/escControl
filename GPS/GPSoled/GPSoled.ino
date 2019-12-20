//Programa: Modulo GPS GY-NEO6MV2 com Arduino Uno
//Autor: Arduino e Cia

#include <TimeLib.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <U8glib.h>

//Pinos utilizados para conexao do modulo GY-NEO6MV2
static const int RXPin = 9, TXPin = 10;

//Objeto TinyGPS++
TinyGPSPlus gps;

//Conexao serial do modulo GPS
SoftwareSerial Serial_GPS(RXPin, TXPin);

//Definicoes do display Oled
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);

void draw()
{
  //Ajuste do horario/timezone
  GPS_Timezone_Adjust();
  //Comandos graficos para o display devem ser colocados aqui
  u8g.setFont(u8g_font_8x13B);
  u8g.drawRFrame(0, 0, 128, 64, 4);
  //Linha Data
  u8g.drawStr(10, 14, "D:        ");
  u8g.setPrintPos(34, 14);
  //Mostra o dia
  if (day() < 10)
  {
    u8g.print("0");
  }
  u8g.print(day());
  u8g.drawStr(51, 14, "/");
  u8g.setPrintPos(60, 14);
  //Mostra o mes
  if (month() < 10)
  {
    u8g.print("0");
  }
  u8g.print(month());
  u8g.drawStr(76, 14, "/");
  u8g.setPrintPos(85, 14);
  //Mostra o ano
  u8g.print(year());

  //Linha Horario
  u8g.drawStr(10, 29, "H:        ");
  u8g.setPrintPos(45, 29);
  //Mostra hora
  if (hour() < 10)
  {
    u8g.print("0");
  }
  u8g.print(hour());
  u8g.drawStr(61, 28, ":");
  u8g.setPrintPos(70, 29);
  //Mostra minutos
  if (minute() < 10)
  {
    u8g.print("0");
  }
  u8g.print(minute());
  u8g.drawStr(86, 28, ":");
  u8g.setPrintPos(95, 29);
  //Mostra segundos
  if (second() < 10)
  {
    u8g.print("0");
  }
  u8g.print(second());

  //Linha latitude
  u8g.drawStr(10, 44, "Lat:         ");
  u8g.setPrintPos(45, 44);
  u8g.print(gps.location.lat(), 5);

  //Linha longitude
  u8g.drawStr(10, 59, "Lon:         ");
  u8g.setPrintPos(45, 59);
  u8g.print(gps.location.lng(), 5);
}

//Ajuste o timezone de acordo com a regiao
const int UTC_offset = -3;

void setup()
{
  //Baud rate Arduino
  Serial.begin(115200);
  //Baud rate Modulo GPS
  Serial_GPS.begin(9600);

  pinMode(5, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);

  //Mostra informacoes iniciais no serial monitor
  Serial.println(F("Data, Hora, Latitude e Longitude"));
  Serial.println(F("Modulo GPS GY-NEO6MV2"));
  Serial.print(F("Biblioteca TinyGPS++ v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  //Definicoes do display Oled
  if ( u8g.getMode() == U8G_MODE_R3G3B2 )
  {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
}

void loop()
{
  //Conexao com modulo GPS
  while (Serial_GPS.available() > 0)
    if (gps.encode(Serial_GPS.read())) {
      digitalWrite(5, HIGH);
      displayInfo();
    }

  /*if (millis() > 5000 && gps.charsProcessed() < 10)
    {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
    }*/

  //Chama a rotina de desenho na tela
  u8g.firstPage();
  do
  {
    draw();
  }
  while ( u8g.nextPage() );
  digitalWrite(5, LOW);
  analogWrite(A0, 0);
  analogWrite(A1, 0);
  analogWrite(A2, 0);
  delay(1000);

}

void displayInfo()
{
  //Mostra informacoes no Serial Monitor
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6); //latitude
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6); //longitude
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.day()); //dia
    Serial.print(F("/"));
    Serial.print(gps.date.month()); //mes
    Serial.print(F("/"));
    Serial.print(gps.date.year()); //ano
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour()); //hora
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute()); //minuto
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second()); //segundo
    Serial.print(F(" "));
    Serial.print(" SAT=");
    Serial.println(gps.satellites.value());
    if (gps.satellites.value() == 0) {
      analogWrite(A0, 255);
      analogWrite(A1, 255);
      analogWrite(A2, 255);
    }
    else if (gps.satellites.value() == 1) {
      analogWrite(A0, 255);
      analogWrite(A1, 0);
      analogWrite(A2, 0);
    }
    else if (gps.satellites.value() == 2) {
      analogWrite(A0, 255);
      analogWrite(A1, 255);
      analogWrite(A2, 0);
    }
    else if (gps.satellites.value() == 3) {
      analogWrite(A0, 255);
      analogWrite(A1, 255);
      analogWrite(A2, 255);
      delay(50);
      analogWrite(A0, 0);
      analogWrite(A1, 0);
      analogWrite(A2, 0);
      delay(50);
      analogWrite(A0, 255);
      analogWrite(A1, 255);
      analogWrite(A2, 255);
      delay(50);
    }

  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}

void GPS_Timezone_Adjust()
{
  while (Serial_GPS.available())
  {
    if (gps.encode(Serial_GPS.read()))
    {
      int Year = gps.date.year();
      byte Month = gps.date.month();
      byte Day = gps.date.day();
      byte Hour = gps.time.hour();
      byte Minute = gps.time.minute();
      byte Second = gps.time.second();

      //Ajusta data e hora a partir dos dados do GPS
      setTime(Hour, Minute, Second, Day, Month, Year);
      //Aplica offset para ajustar data e hora
      //de acordo com a timezone
      adjustTime(UTC_offset * SECS_PER_HOUR);
    }
  }
}
