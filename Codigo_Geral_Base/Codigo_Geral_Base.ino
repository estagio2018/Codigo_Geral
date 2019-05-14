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
  float Humidity, Temperature, Temp_M, Hum_M;
  
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
  #define MQ_pin 9
  #define pulseTime 60000
  int S2, S3, S4, S5, S6, S7, S8, S9, S131, S135 = 0;
  float S2_M, S3_M, S4_M, S5_M, S6_M, S7_M, S8_M, S9_M, S131_M, S135_M;
  unsigned long tempo, tempo1;
  
// Configuração do Hc05:
  int Option, x;
  int Time = 1;

// Configuração do RTC:
  RTC_DS1307 Ck;
                                                                                 
// Configuração do SD Card:
  const int CS = 4;
  File data;

void mean(void); void checkSensor(void);
void header(void); void writeOption(void);
void reading(void); void getDate(void);
void getTime(void); void reportTime(void);
void report(void); void pulse(void);

void setup(){
  tempo, tempo1 = millis(); Serial.begin(9600); TempHum.begin(); Serial1.begin(9600); Wire.begin(); Ck.begin();
  while(!Serial1.available()){pulse();}
  Serial1.println("Inicializando..."); Serial1.println(""); Time = 1000;
  pinMode(MQ_02, INPUT); pinMode(MQ_03, INPUT); pinMode(MQ_04, INPUT); pinMode(MQ_05, INPUT); pinMode(MQ_06, INPUT);
  pinMode(MQ_07, INPUT); pinMode(MQ_08, INPUT); pinMode(MQ_09, INPUT); pinMode(MQ_131, INPUT); pinMode(MQ_135, INPUT);
  while(!Ck.isrunning()){ Serial1.println("O relógio não está funcionando!"); Ck.adjust(DateTime(__DATE__, __TIME__)); pulse();}
  getDate(); getTime(); // Obtem data e hora do RTC.
  mean(); checkSensor(); // Checa se os sensores estão ligados.
  Serial1.print("Procurando Cartão SD..."); delay(1000);
  while(!SD.begin(CS)){ Serial1.println(" Cartão SD não encontrado!!!"); Serial1.println(""); Serial1.println(""); pulse();}
  Serial1.write("Cartão SD conectado!!!"); Serial1.println(""); Serial1.println("");
  header(); Option = '1'; Option = '0';
}

void loop(){
  if(Option == 48){
    Serial1.println(F(""));
    Serial1.println(F("Escolha uma das opções abaixo:"));
    Serial1.println(F("============================"));
    Serial1.println(F("  1  -  informe a cada segundo."));
    Serial1.println(F("  2  -  informe a cada 5 segundos."));
    Serial1.println(F("  3  -  informe a cada minuto."));
    Serial1.println(F("  4  -  Reiniciar."));
    Serial1.println(F("============================"));
    Serial1.println(F(""));
  }
  if(Serial1.available()){Option = Serial1.read();}
  switch(Option){
    case '1': Time = 1000; writeOption(); getTime(); reading(); break;
    case '2': Time = 5000; writeOption(); getTime(); reading(); break;
    case '3': Time = 60000; writeOption(); getTime(); reading(); break;
    case '4': writeOption(); Option = '1'; break;
    default: writeOption(); Serial1.println(""); Serial1.println(F(""));
             Serial1.println(F("Desculpe, essa opção não"));
             Serial1.println(F("está disponível.")); Serial1.println(F("")); break;
    }
}

void mean(){
  for(x=0; x<5; x++){
    S2 += analogRead(MQ_02); S3 += analogRead(MQ_03); S4 += analogRead(MQ_04); S5 += analogRead(MQ_05); S6 += analogRead(MQ_06);
    S8 += analogRead(MQ_08); S131 += analogRead(MQ_131); S135 += analogRead(MQ_135);
    if (digitalRead(MQ_pin) == HIGH){analogReference(INTERNAL1V1); S7 += analogRead(MQ_07); S9 += analogRead(MQ_09); analogReference(DEFAULT);}
    else{analogReference(DEFAULT); S7 += analogRead(MQ_07); S9 += analogRead(MQ_09);}
    Humidity = TempHum.readHumidity(); Temperature = TempHum.readTemperature(); delay(Time/5);}
    S2_M = S2/x; S3_M = S3/x; S4_M = S4/x; S5_M = S5/x; S6_M = S6/x; S7_M = S7/x; S8_M = S8/x; S9_M = S9/x; S131_M = S131/x; S135_M = S135/x;
    Hum_M = Humidity/x; Temp_M = Temperature/x;
}

void checkSensor(){
  Serial1.println("");
  Serial1.println("Checagem de Sensores:");
  if(S2_M >= 360 && S2_M >= 585) {Serial1.println(F("Sensor MQ02:   OK!"));} else {Serial1.println(F("Sensor MQ02:   Não Conectado!"));}
  if(S3_M >= 360 && S3_M >= 585) {Serial1.println(F("Sensor MQ03:   OK!"));} else {Serial1.println(F("Sensor MQ03:   Não Conectado!"));}
  if(S4_M >= 360 && S4_M >= 585) {Serial1.println(F("Sensor MQ04:   OK!"));} else {Serial1.println(F("Sensor MQ04:   Não Conectado!"));}
  if(S5_M >= 360 && S5_M >= 585) {Serial1.println(F("Sensor MQ05:   OK!"));} else {Serial1.println(F("Sensor MQ05:   Não Conectado!"));}
  if(S6_M >= 360 && S6_M >= 585) {Serial1.println(F("Sensor MQ06:   OK!"));} else {Serial1.println(F("Sensor MQ06:   Não Conectado!"));}
  if(S7_M >= 360 && S7_M >= 585) {Serial1.println(F("Sensor MQ07:   OK!"));} else {Serial1.println(F("Sensor MQ07:   Não Conectado!"));}
  if(S8_M >= 360 && S8_M >= 585) {Serial1.println(F("Sensor MQ08:   OK!"));} else {Serial1.println(F("Sensor MQ08:   Não Conectado!"));}
  if(S9_M >= 360 && S9_M >= 585) {Serial1.println(F("Sensor MQ09:   OK!"));} else {Serial1.println(F("Sensor MQ09:   Não Conectado!"));}
  if(S131_M >= 360 && S131_M >= 585) {Serial1.println(F("Sensor MQ131: OK!"));} else {Serial1.println(F("Sensor MQ131: Não Conectado!"));}
  if(S135_M >= 360 && S135_M >= 585) {Serial1.println(F("Sensor MQ135: OK!"));} else {Serial1.println(F("Sensor MQ135: Não Conectado!"));}
  Serial1.println(""); Serial1.println("");
}

void header(){
  data = SD.open("analise.txt", FILE_WRITE);
  if(!data){Serial1.println("   ***Erro ao abrir documento de texto.***   ");}
  else{
    data.print("Experimento iniciado no dia ");
    DateTime now = Ck.now();
    if(now.day() < 10){ data.print("0"); data.print(now.day(), DEC);} else {data.print(now.day(), DEC);} data.print("/");
    if(now.month() < 10){ data.print("0"); data.print(now.month(), DEC);} else {data.print(now.month(), DEC);} data.print("/");
    if(now.year() < 10){ data.print("0"); data.print(now.year(), DEC);} else {data.print(now.year(), DEC);} data.print(" às ");
    reportTime(); data.print("."); data.println(""); data.println(""); data.print("    Dados em média dos sensores:"); data.println(""); 
    data.print("        Time   Temperature Humididty   MQ02     MQ03     MQ04     MQ05     MQ06     MQ07     MQ08     MQ09    MQ131    MQ135");data.close();}
}

void writeOption(){ Serial1.println(""); Serial1.print("Eu: "); Serial1.write(Option); Serial1.println(""); Serial1.println("");}      

void reading(){
  if(millis() - tempo1 > (Time)){
    Serial1.print(F(" - "));
    Humidity = TempHum.readHumidity(); Temperature = TempHum.readTemperature();
    Serial1.print(F("Temperatura= ")); Serial1.print(Temperature); Serial1.print(F("°C "));
    Serial1.print(F(" Umidade= ")); Serial1.print(Humidity); Serial1.println(F("% "));
    tempo1 = millis();
  }
}

void getDate(){
  Serial1.println(""); DateTime now = Ck.now();
  if(now.day() < 10){ Serial1.print("0"); Serial1.print(now.day(), DEC);}
  else {Serial1.print(now.day(), DEC);} Serial1.print("/");
  if(now.month() < 10){ Serial1.print("0"); Serial1.print(now.month(), DEC);}
  else {Serial1.print(now.month(), DEC);} Serial1.print("/");
  if(now.year() < 10){ Serial1.print("0"); Serial1.print(now.year(), DEC);}
  else {Serial1.print(now.year(), DEC);} Serial1.print(" - ");
}

void getTime(){
  DateTime now = Ck.now();
  if(now.hour() < 10){ Serial1.print("0"); Serial1.print(now.hour(), DEC);}
  else{Serial1.print(now.hour(), DEC);} Serial1.print(":");
  if(now.minute() < 10){ Serial1.print("0"); Serial1.print(now.minute(), DEC);}
  else{Serial1.print(now.minute(), DEC);} Serial1.print(":");
  if(now.second() < 10){ Serial1.print("0"); Serial1.print(now.second(), DEC);}
  else{Serial1.print(now.second(), DEC);}
}

void reportTime(){
  //data = SD.open("analise.txt", FILE_WRITE);
  //if(!data){ Serial1.println("***Erro ao abrir documento de texto.***");}
  //else{
    DateTime now = Ck.now();
    if(now.hour() < 10){ data.print("0"); data.print(now.hour(), DEC);} else{data.print(now.hour(), DEC);} data.print(":");
    if(now.minute() < 10){ data.print("0"); data.print(now.minute(), DEC);} else{data.print(now.minute(), DEC);} data.print(":");
    if(now.second() < 10){ data.print("0"); data.print(now.second(), DEC);} else{data.print(now.second(), DEC);}
  //}
}

void report(){
  data = SD.open("analise.txt", FILE_WRITE); mean();
  if(!data){ Serial1.println("***Erro ao abrir documento de texto.***");}
  else{
    data.print("      "); reportTime(); data.print("   "); data.print(Temperature); data.print("°C"); data.print("    ");
    data.print(Humidity); data.print("%"); data.print("   "); data.print(S2_M); data.print("ppm  ");
    data.print(S3_M); data.print("ppm  "); data.print(S4_M); data.print("ppm  ");
    data.print(S5_M); data.print("ppm  "); data.print(S6_M); data.print("ppm  "); data.print(S7_M); data.print("ppm  "); 
    data.print(S8_M); data.print("ppm  "); data.print(S9_M); data.print("ppm  "); data.print(S131_M); data.print("ppm  "); 
    data.print(S135_M); data.print("ppm  "); data.close();
  }
}

void pulse(){
  if((millis() - tempo) > pulseTime && digitalRead(MQ_pin) == HIGH){
    digitalWrite(MQ_pin, LOW); tempo = millis();
  }
  if((millis() - tempo) > (pulseTime + (pulseTime/3)) && digitalRead(MQ_pin) == LOW){
    digitalWrite(MQ_pin, HIGH); tempo = millis();
  }
}


/*
 * TEXTO ESCRITO NO DOCUMENTO .txt
    Time   Temperature Humididty   MQ02     MQ03     MQ04     MQ05     MQ06     MQ07     MQ08     MQ09    MQ131    MQ135
  15:53:34   25.43°C    30.12%   1234ppm  1234ppm  1234ppm  1234pmm  1234pmm  1234ppm  1234ppm  1234ppm  1234ppm  1234ppm 
 */
