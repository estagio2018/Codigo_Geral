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
  int S2_M, S3_M, S4_M, S5_M, S6_M, S7_M, S8_M, S9_M, S131_M, S135_M;
  unsigned long tempo, tempo1, timedel;
  
// Configuração do Hc05:
  int Option, option, x;
  int Time = 1000;  

// Configuração do RTC:
  RTC_DS1307 rtc;
                                                                                 
// Configuração do SD Card:
  #define CS 4
  File data;
  String Arquivo = "analise.txt";

void mean(void);    void checkSensor(void);
void header(void);  void writeOption(void);
void reading(void); void getDate(void);
void getTime(void); void reportTime(void);
void report(void);  void pulse(void); void delData(void);

void setup(){
  //Inicialização
  tempo, tempo1, timedel = millis(); Serial.begin(9600); TempHum.begin(); Serial1.begin(9600); Wire.begin(); rtc.begin();
  while(!Serial1.available()); Serial1.read(); Serial.read(); Serial1.flush(); Serial.flush(); SD.begin(CS); rtc.isrunning(); rtc.adjust(DateTime(__DATE__, __TIME__));
  
  //Verificação dos Módulos
  Serial1.println("Inicializando..."); Serial1.println("");
  while(!rtc.isrunning()){ Serial1.println("O relógio não está funcionando!"); rtc.adjust(DateTime(__DATE__, __TIME__)); }
  Serial1.print("Procurando Cartão SD... "); delay(1000);
  if(!SD.begin(CS)){ Serial1.println("Cartão SD não encontrado!!!"); Serial1.println(""); Serial1.println("");}
  else {Serial1.write("Cartão SD conectado!!!"); Serial1.println(""); Serial1.println("");}
  
  getDate(); getTime(); // Obtem data e hora do RTC.
  checkSensor(); // Checa se os sensores estão ligados.
  header(); Option = 48;
}

void loop(){
  writeOption(); delData(); getTime(); reading(); MQSensor(); report();
}

void delData(){
  if(Option == 9){
    Serial1.println("Deseja limpar o arquivo de texto? S/n"); while(!Serial1.available()); 
    Option = Serial1.read(); 
    switch(Option){
      case 83: SD.remove(Arquivo);
               while((SD.exists(Arquivo)) && (millis() - timedel < 2000)){
                    SD.remove(Arquivo); timedel = millis();} if(SD.exists(Arquivo)) Serial1.println("Falha ao limpar cartão."); break;
      case 115: SD.remove(Arquivo);
                while((SD.exists(Arquivo)) && (millis() - timedel < 2000)){
                     SD.remove(Arquivo); timedel = millis();} if(SD.exists(Arquivo)) Serial1.println("Falha ao limpar cartão."); break;
    }
    
  }
}
void mean(){
  S2 = 0; S3 = 0; S4 = 0; S5 = 0; S6 = 0; S7 = 0; S8 = 0; S9 = 0; S131 = 0; S135 = 0;
  for(x=0; x<5; x++){
      S2 += analogRead(MQ_02); S3 += analogRead(MQ_03); S4 += analogRead(MQ_04); S5 += analogRead(MQ_05); S6 += analogRead(MQ_06);
      S8 += analogRead(MQ_08); S131 += analogRead(MQ_131); S135 += analogRead(MQ_135);
      if (digitalRead(MQ_pin) == HIGH){analogReference(INTERNAL1V1); S7 += analogRead(MQ_07); S9 += analogRead(MQ_09); analogReference(DEFAULT);}
        else{analogReference(DEFAULT); S7 += analogRead(MQ_07); S9 += analogRead(MQ_09);}
      Humidity = TempHum.readHumidity(); Temperature = TempHum.readTemperature(); delay(Time/5);
    }
    S2_M = int(S2/x); S3_M = int(S3/x); S4_M = int(S4/x); S5_M = int(S5/x); S6_M = int(S6/x);
    S7_M = int(S7/x); S8_M = int(S8/x); S9_M = int(S9/x); S131_M = int(S131/x); S135_M = int(S135/x);
    Hum_M = Humidity/x; Temp_M = Temperature/x;
    
}

void checkSensor(){
  mean();
  Serial1.println(""); Serial.println("");
  Serial1.println("Checagem de Sensores:"); Serial1.println("Checagem de Sensores:");
  if(S2_M != 0) {Serial1.println(F("   Sensor MQ02:   OK!")); Serial.print(F("Sensor MQ02:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ02:   Não Conectado!")); Serial.print(F("Sensor MQ02:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S3_M != 0) {Serial1.println(F("   Sensor MQ03:   OK!")); Serial.print(F("Sensor MQ03:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ03:   Não Conectado!")); Serial.print(F("Sensor MQ03:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S4_M != 0) {Serial1.println(F("   Sensor MQ04:   OK!")); Serial.print(F("Sensor MQ04:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ04:   Não Conectado!")); Serial.print(F("Sensor MQ04:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S5_M != 0) {Serial1.println(F("   Sensor MQ05:   OK!")); Serial.print(F("Sensor MQ05:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ05:   Não Conectado!")); Serial.print(F("Sensor MQ05:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S6_M != 0) {Serial1.println(F("   Sensor MQ06:   OK!")); Serial.print(F("Sensor MQ06:")); Serial.print("\t"); Serial.println("OK!");} 
      else {Serial1.println(F("   Sensor MQ06:   Não Conectado!")); Serial.print(F("Sensor MQ06:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S7_M != 0) {Serial1.println(F("   Sensor MQ07:   OK!")); Serial.print(F("Sensor MQ07:")); Serial.print("\t"); Serial.println("OK!");} 
      else {Serial1.println(F("   Sensor MQ07:   Não Conectado!")); Serial.print(F("Sensor MQ07:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S8_M != 0) {Serial1.println(F("   Sensor MQ08:   OK!")); Serial.print(F("Sensor MQ08:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ08:   Não Conectado!")); Serial.print(F("Sensor MQ08:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S9_M != 0) {Serial1.println(F("   Sensor MQ09:   OK!")); Serial.print(F("Sensor MQ09:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ09:   Não Conectado!")); Serial.print(F("Sensor MQ09:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S131_M != 0) {Serial1.println(F("   Sensor MQ131: OK!")); Serial.print(F("Sensor MQ131:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ131: Não Conectado!")); Serial.print(F("Sensor MQ131:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S135_M != 0) {Serial1.println(F("   Sensor MQ135: OK!")); Serial.print(F("Sensor MQ135:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial1.println(F("   Sensor MQ135: Não Conectado!")); Serial.print(F("Sensor MQ135:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  Serial1.println(""); Serial1.println(""); Serial.println(""); Serial.println(""); 
}

void MQSensor(){
  mean();
  Serial1.print(F("Sensor MQ02:  ")); Serial1.print(S2_M); Serial1.print("   ");
  Serial1.print(F("Sensor MQ03:  ")); Serial1.print(S3_M); Serial1.println("   ");
  Serial1.print(F("Sensor MQ04:  ")); Serial1.print(S4_M); Serial1.print("   ");
  Serial1.print(F("Sensor MQ05:  ")); Serial1.print(S5_M); Serial1.println("   ");
  Serial1.print(F("Sensor MQ06:  ")); Serial1.print(S6_M); Serial1.print("   ");
  Serial1.print(F("Sensor MQ07:  ")); Serial1.print(S7_M); Serial1.println("   ");
  Serial1.print(F("Sensor MQ08:  ")); Serial1.print(S8_M); Serial1.print("   ");
  Serial1.print(F("Sensor MQ09:  ")); Serial1.print(S9_M); Serial1.println("   ");
  Serial1.print(F("Sensor MQ131: ")); Serial1.print(S131_M); Serial1.print("   ");
  Serial1.print(F("Sensor MQ135: ")); Serial1.print(S135_M); Serial1.println("   "); Serial1.println("");
}

void header(){
  data = SD.open(Arquivo, FILE_WRITE);
  if(!data){
    Serial1.println("   ***Erro ao abrir documento de texto.***   ");}
    data.print("Experimento iniciado no dia ");
    DateTime now = rtc.now();
    if(now.day() < 10){ data.print("0"); Serial.print("0"); data.print(now.day(), DEC); Serial.print(now.day(), DEC);} 
      else {data.print(now.day(), DEC); Serial.print(now.day(), DEC);} data.print("/"); Serial.print("/");
    if(now.month() < 10){ data.print("0"); Serial.print("0"); data.print(now.month(), DEC); Serial.print(now.month(), DEC);}
      else {data.print(now.month(), DEC); Serial.print(now.month(), DEC);} data.print("/"); Serial.print("/");
    if(now.year() < 10){ data.print("0"); data.print(now.year(), DEC); Serial.print("0"); Serial.print(now.year(), DEC);}
      else {data.print(now.year(), DEC); Serial.print(now.year(), DEC);} data.print(" às "); Serial.print(" às ");
    reportTime(); data.print("."); data.println(""); data.println(""); data.print("    Dados em média dos sensores:"); data.println(""); 
    data.print("\t"); data.print("Horas"); data.print("\t"); data.print("Temp."); data.print("\t");
    data.print("Umid."); data.print("\t"); data.print("MQ02"); data.print("\t"); data.print("MQ03"); data.print("\t"); 
    data.print("MQ04"); data.print("\t"); data.print("MQ05"); data.print("\t"); data.print("MQ06"); data.print("\t"); 
    data.print("MQ07"); data.print("\t"); data.print("MQ08"); data.print("\t"); data.print("MQ09"); data.print("\t");
    data.print("MQ131"); data.print("\t"); data.print("MQ0135"); data.print("\t"); data.println(""); data.close();

    Serial.print("."); Serial.println(""); Serial.println(""); Serial.print("Dados em média dos sensores:"); Serial.println(""); 
    Serial.print("\t"); Serial.print("Horas"); Serial.print("\t"); Serial.print("Temp."); Serial.print("\t");
    Serial.print("Umid."); Serial.print("\t"); Serial.print("MQ02"); Serial.print("\t"); Serial.print("MQ03"); Serial.print("\t"); 
    Serial.print("MQ04"); Serial.print("\t"); Serial.print("MQ05"); Serial.print("\t"); Serial.print("MQ06"); Serial.print("\t"); 
    Serial.print("MQ07"); Serial.print("\t"); Serial.print("MQ08"); Serial.print("\t"); Serial.print("MQ09"); Serial.print("\t");
    Serial.print("MQ131"); Serial.print("\t"); Serial.print("MQ0135"); Serial.print("\t"); Serial.println("");
}

void writeOption(){ 
  if(Serial1.available() > 48){
    Serial1.read(); Serial1.println(""); Serial1.print("Eu: "); Serial1.write(Option); Serial1.println(""); Serial1.println(""); Serial1.flush();
  }      
}

void reading(){
  if(millis() - tempo1 > (Time)){
    Serial1.print(F(" - "));
    Humidity = TempHum.readHumidity(); Temperature = TempHum.readTemperature();
    Serial1.print(F(" Temp.: ")); Serial1.print(Temperature); Serial1.print(F("°C "));
    Serial1.print(F(" Umid.: ")); Serial1.print(Humidity); Serial1.println(F("% "));
    tempo1 = millis();
  }
}

void getDate(){
  Serial1.println(""); DateTime now = rtc.now();
  if(now.day() < 10){ Serial1.print("0"); Serial1.print(now.day(), DEC);}
    else {Serial1.print(now.day(), DEC);} Serial1.print("/");
  if(now.month() < 10){ Serial1.print("0"); Serial1.print(now.month(), DEC);}
    else {Serial1.print(now.month(), DEC);} Serial1.print("/");
  if(now.year() < 10){ Serial1.print("0"); Serial1.print(now.year(), DEC);}
    else {Serial1.print(now.year(), DEC);} Serial1.print(" - ");
}

void getTime(){
  DateTime now = rtc.now();
  if(now.hour() < 10){ Serial1.print("0"); Serial1.print(now.hour(), DEC);}
    else{Serial1.print(now.hour(), DEC); } Serial1.print(":");
  if(now.minute() < 10){ Serial1.print("0"); Serial1.print(now.minute(), DEC);}
    else{Serial1.print(now.minute(), DEC);} Serial1.print(":");
  if(now.second() < 10){ Serial1.print("0"); Serial1.print(now.second(), DEC);}
    else{Serial1.print(now.second(), DEC);} 
}

void reportTime(){
    DateTime now = rtc.now();
    if(now.hour() < 10){ data.print("0"); data.print(now.hour(), DEC); Serial.print("0"); Serial.print(now.hour(), DEC);}
      else{data.print(now.hour(), DEC); Serial.print(now.hour(), DEC);} data.print(":"); Serial.print(":");
    if(now.minute() < 10){ data.print("0"); data.print(now.minute(), DEC); Serial.print("0"); Serial.print(now.minute(), DEC);}
      else{data.print(now.minute(), DEC); Serial.print(now.minute(), DEC);} data.print(":"); Serial.print(":");
    if(now.second() < 10){ data.print("0"); data.print(now.second(), DEC); Serial.print("0"); Serial.print(now.second(), DEC);}
      else{data.print(now.second(), DEC); Serial.print(now.second(), DEC);}
}

void report(){
  data = SD.open(Arquivo, FILE_WRITE);
  if(!data){ Serial1.println("***Erro ao abrir documento de texto.***");}
    data.print("\t"); Serial.print("\t"); reportTime(); data.print("\t"); data.print(Temperature); data.print("°C"); data.print("\t");
    Serial.print("\t"); Serial.print(Temperature); Serial.print("\t");
    data.print(Humidity); data.print("%"); data.print("\t"); Serial.print(Humidity); Serial.print("\t"); 
    if(S2_M < 100){data.print("0"); data.print(S2_M); Serial.print("0"); Serial.print(S2_M);}
      else {data.print(S2_M); Serial.print(S2_M);} data.print("\t"); Serial.print("\t");
    if(S3_M < 100){data.print("0"); data.print(S3_M); Serial.print("0"); Serial.print(S3_M);}
      else {data.print(S3_M); Serial.print(S3_M);} data.print("\t"); Serial.print("\t"); 
    if(S4_M < 100){data.print("0"); data.print(S4_M); Serial.print("0"); Serial.print(S4_M);}
      else {data.print(S4_M); Serial.print(S4_M);} data.print("\t"); Serial.print("\t");
    if(S5_M < 100){data.print("0"); data.print(S5_M); Serial.print("0"); Serial.print(S5_M);}
      else {data.print(S5_M); Serial.print(S5_M);} data.print("\t"); Serial.print("\t");
    if(S6_M < 100){data.print("0"); data.print(S6_M); Serial.print("0"); Serial.print(S6_M);}
      else {data.print(S6_M); Serial.print(S6_M);} data.print("\t"); Serial.print("\t");
    if(S7_M < 100){data.print("0"); data.print(S7_M); Serial.print("0"); Serial.print(S7_M);}
      else {data.print(S7_M); Serial.print(S7_M);} data.print("\t"); Serial.print("\t"); 
    if(S8_M < 100){data.print("0"); data.print(S8_M); Serial.print("0"); Serial.print(S8_M);}
      else {data.print(S8_M); Serial.print(S8_M);} data.print("\t"); Serial.print("\t");
    if(S9_M < 100){data.print("0"); data.print(S9_M); Serial.print("0"); Serial.print(S9_M);}
      else {data.print(S9_M); Serial.print(S9_M);} data.print("\t"); Serial.print("\t");
    if(S131_M < 100){data.print("0"); data.print(S131_M); Serial.print("0"); Serial.print(S131_M);}
      else {data.print(S131_M); Serial.print(S131_M);} data.print("\t"); Serial.print("\t"); 
    if(S135_M < 100){data.print("0"); data.print(S135_M); Serial.print("0"); Serial.print(S135_M);}
      else {data.print(S135_M); Serial.print(S135_M);} data.println(""); Serial.println(""); data.close();
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
  15:53:34   25.43°C    30.12%     1234     1234     1234     1234     1234     1234     1234     1234    1234     1234 
 */
