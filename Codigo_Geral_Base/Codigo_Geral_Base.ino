/*2019-03-12 - IFG-Câmpus Goiânia - Codigo_Geral_Base
  Editor: Victor Gabriel P. Santos
  **Código Base Geral para o projeto do Nariz eletrônico.
  **Fixa Técnica:
    - Módulo Bluetooth Hc-05:
      + Rx necessita de divisor de tensão (5V~3,3V).
    -Sensores de Gás:
      + MQ02 - Detecção de (...);
      + MQ03 - Detecção de (...);
      + MQ04 - Detecção de (...);
      + MQ05 - Detecção de (...);
      + MQ06 - Detecção de (...);
      + MQ07 - Detecção de (...);
      + MQ08 - Detecção de (...);
      + MQ09 - Detecção de (...);
      + MQ131 - Detecção de (...);
      + MQ135 - Detecção de (...);
    -Sensor de Temperatura e Umidade DHT22.
    -Tiny RTC: ***COMUNICAÇÃO I2C***
      + SDA: pino 20;
      + SCL: pino 21.
    - Módulo SD card: ***COMUNICAÇÃO SPI***
      + MOSI: pino 50;
      + MISO: pino 51;
      + SCK: pino 52.
*/
    
    
//Bibliotecas:
  #include <DHT.h>
  #include <Wire.h>
  #include <RTClib.h>
  #include <SPI.h>
  #include <SD.h>

// Configuração do DHT:
  #define DHT_Pin A0
  #define DHT_Type DHT22
  DHT TempHum(DHT_Pin, DHT_Type);
  
// Configuração dos sensores de gás:
  #define MQ_02 A1
  #define MQ_03 A2
  #define MQ_04 A3
  #define MQ_05 A4
  #define MQ_06 A5
  #define MQ_07 A6
  #define MQ_08 A7
  #define MQ_09 A8
  #define MQ_131 A9
  #define MQ_135 A10
  int S2, S3, S4, S5, S6, S7, S8, S9, S131, S135 = 0;
  float S2_M, S3_M, S4_M, S5_M, S6_M, S7_M, S8_M, S9_M, S131_M, S135_M;
  
// Configuração do Hc05:
  int Option, Marker;
  int Time = 1;

// Configuração do RTC:
  RTC_DS1307 Ck;

// Configuração do SD Card:
  const int CS = 4;
  
void setup(){
  Serial.begin(9600); TempHum.begin(); Serial1.begin(9600); Wire.begin(); Ck.begin();
  while(!Serial1.available());
  Serial1.println("Inicializando..."); Serial1.println(""); delay(1000);
  pinMode(MQ_02, INPUT); pinMode(MQ_03, INPUT); pinMode(MQ_04, INPUT); pinMode(MQ_05, INPUT); pinMode(MQ_06, INPUT);
  pinMode(MQ_07, INPUT); pinMode(MQ_08, INPUT); pinMode(MQ_09, INPUT); pinMode(MQ_131, INPUT); pinMode(MQ_135, INPUT);
  for(int x=0; x<5; x++){
    S2 += analogRead(MQ_02); S3 += analogRead(MQ_03); S4 += analogRead(MQ_04); S5 += analogRead(MQ_05); S6 += analogRead(MQ_06);
    S7 += analogRead(MQ_07); S8 += analogRead(MQ_08); S9 += analogRead(MQ_09); S131 += analogRead(MQ_131); S135 += analogRead(MQ_135);
    S2_M = S2/5; S3_M = S3/5; S4_M = S4/5; S5_M = S5/5; S6_M = S6/5; S7_M = S7/5; S8_M = S8/5; S9_M = S9/5; S131_M = S131/5; S135_M = S135/5;
  }
  while(!Ck.isrunning()){
    Serial1.println("O relógio não está funcionando!");
    Ck.adjust(DateTime(__DATE__, __TIME__));
  }
  getDate(); // Obtem a data do RTC.
  getTime(); // Obtem a hora do RTC.
  checkSensor(); // Checa se os sensores estão ligados.
  Serial1.print("Procurando Cartão SD..."); delay(1000);
  while(!SD.begin(CS)){ Serial1.println(" Cartão SD não encontrado!!!"); Serial1.println(""); Serial1.println("");}
  Serial1.write("Cartão SD conectado!!!"); Serial1.println(""); Serial1.println("");
  Marker = 0;
  Option = Serial1.read();
  Option = 0;
}

void loop(){
  if(Option == 0){
    Serial1.println(F(""));
    Serial1.println(F("Escolha uma das opções abaixo:"));
    Serial1.println(F("============================"));
    Serial1.println(F("  1  -  informe a cada segundo."));
    Serial1.println(F("  2  -  informe a cada 2 segundos."));
    Serial1.println(F("  3  -  informe a cada 5 segundos."));
    Serial1.println(F("  4  -  informe a cada 10 segundos."));
    Serial1.println(F("  5  -  informe a cada 30 segundos."));
    Serial1.println(F("  6  -  informe a cada minuto."));
    Serial1.println(F("  7  -  informe a cada 5 minutos."));
    Serial1.println(F("  8  -  informe a cada 10 minutos."));
    Serial1.println(F("  9  -  Reiniciar."));
    Serial1.println(F("============================"));
    Serial1.println(F(""));
  }
  while(!Serial1.available()){
    if(Marker != 0) {getTime(); reading();}
  }
  Option = Serial1.read();
  switch(Option - 48){
    case 1: Time = 1; writeOption(); getTime(); reading(); break;
    case 2: Time = 2; writeOption(); getTime(); reading(); break;
    case 3: Time = 5; writeOption(); getTime(); reading(); break;
    case 4: Time = 10; writeOption(); getTime(); reading(); break;
    case 5: Time = 30; writeOption(); getTime(); reading(); break;
    case 6: Time = 60; writeOption(); getTime(); reading(); break;
    case 7: Time = 300; writeOption(); getTime(); reading(); break;
    case 8: Time = 600; writeOption(); getTime(); reading(); break;
    case 9: writeOption(); Option = 0; Marker = 0; break;
    default: writeOption(); 
             Serial1.println(""); 
             Serial1.println(F(""));
             Serial1.println(F("Desculpe, essa opção não"));
             Serial1.println(F("está disponível."));
             Serial1.println(F("")); break;
    }
}

void checkSensor(){
  Serial1.println("");
  Serial1.println("Checagem de Sensores:");
  if(S2_M >= 360 && S2_M >= 585) {Serial1.println(F("Sensor MQ02:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ02:   Não Conectado!")); delay(300);}
  if(S3_M >= 360 && S3_M >= 585) {Serial1.println(F("Sensor MQ03:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ03:   Não Conectado!")); delay(300);}
  if(S4_M >= 360 && S4_M >= 585) {Serial1.println(F("Sensor MQ04:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ04:   Não Conectado!")); delay(300);}
  if(S5_M >= 360 && S5_M >= 585) {Serial1.println(F("Sensor MQ05:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ05:   Não Conectado!")); delay(300);}
  if(S6_M >= 360 && S6_M >= 585) {Serial1.println(F("Sensor MQ06:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ06:   Não Conectado!")); delay(300);}
  if(S7_M >= 360 && S7_M >= 585) {Serial1.println(F("Sensor MQ07:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ07:   Não Conectado!")); delay(300);}
  if(S8_M >= 360 && S8_M >= 585) {Serial1.println(F("Sensor MQ08:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ08:   Não Conectado!")); delay(300);}
  if(S9_M >= 360 && S9_M >= 585) {Serial1.println(F("Sensor MQ09:   OK!")); delay(300);} else {Serial1.println(F("Sensor MQ09:   Não Conectado!")); delay(300);}
  if(S131_M >= 360 && S131_M >= 585) {Serial1.println(F("Sensor MQ131: OK!")); delay(300);} else {Serial1.println(F("Sensor MQ131: Não Conectado!")); delay(300);}
  if(S135_M >= 360 && S135_M >= 585) {Serial1.println(F("Sensor MQ135: OK!")); delay(300);} else {Serial1.println(F("Sensor MQ135: Não Conectado!")); delay(300);}
  Serial1.println(""); Serial1.println("");
  delay(500);
}

void writeOption(){
  Serial1.println(""); Serial1.write(Option); Serial1.println(""); Serial1.println("");
}
      
      
void reading(){
  float Humidity = TempHum.readHumidity();
  float Temperature = TempHum.readTemperature();
  Serial1.print(F("Temperatura= ")); Serial1.print(Temperature); Serial1.print(F("°C "));
  Serial1.print(F(" Umidade= ")); Serial1.print(Humidity); Serial1.println(F("% "));
  delay(Time*1000); Marker = 1;
}

void getDate(){
  Serial1.println("");
  DateTime now = Ck.now();
  Serial1.print(now.day(), DEC); Serial1.print("/");
  Serial1.print(now.month(), DEC); Serial1.print("/");
  Serial1.print(now.year(), DEC); Serial1.print(" - ");
}

void getTime(){
  DateTime now = Ck.now();
  Serial1.print(now.hour(), DEC); Serial1.print(":");
  Serial1.print(now.minute(), DEC); Serial1.print(":");
  Serial1.print(now.second(), DEC);
}
