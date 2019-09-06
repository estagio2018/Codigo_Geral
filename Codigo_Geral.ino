/*2019/03/12 - IFG-Câmpus Goiânia - Codigo_Geral_Base
  Criador: Victor Gabriel P. Santos
  **Código Base para o projeto do Nariz eletrônico.
  **Fixa Técnica:
    - Módulo Bluetooth Hc-05:
      + Rx necessita de divisor de tensão (5V~3,3V).
      
    -Sensores de Gás:
      + MQ02 - Detecção de Metano, Butano, GPL e Fumaça;
      + MQ03 - Detecção de Álcool, Etanol e Fumaça;
      + MQ04 - Detecção de Metano;
      + MQ05 - Detecção de H2, GLP, CH4, CO, Álcool e Gás Natural;
      + MQ06 - Detecção de GPL, Isobutano, Butano e Propano;
      + MQ07 - Detecção de Monóxido de Carbono;
      + MQ08 - Detecção de H2;
      + MQ09 - Detecção de CH4 e GPL;
      + MQ131 - Detecção de Ozônio;
      + MQ135 - Detecção de Benzeno, Álcool, Fumaça, Propano, Formaldeído e Hidrogênio, Amônia.
      
    -Sensor de Temperatura e Umidade DHT22.
    
    -Tiny RTC: ***COMUNICAÇÃO I2C***
      + SDA: pino 20;
      + SCL: pino 21.
      
    - Módulo SD card: ***COMUNICAÇÃO SPI***
      + Os seguintes pinos necessitam de um divisor de tensão (5V~3,3V).
         MOSI: pino 50;
         MISO: pino 51; (Exceção)
         SCK: pino 52.
*/
    
//Bibliotecas:
  #include <DHT.h>
  #include <Wire.h>
  #include <RTClib.h>
  #include <SPI.h>
  #include <SD.h>

// Configuração do DHT:
  #define DHT_Pin A11
  #define DHT_Type DHT22
  DHT TempHum(DHT_Pin, DHT_Type);
  float Humidity, Temperature;

// Configuração dos sensores de gás:
  #define MQ02 A2
  #define MQ03 A1
  #define MQ04 A0
  #define MQ05 A3
  #define MQ06 A4
  #define MQ07 A10
  #define MQ08 A5
  #define MQ09 A9
  #define MQ131 A6
  #define MQ135 A8
  #define MQX A7
  #define pulsePin 32
  #define pulseTime 60000
  int S[11] = {0};
  unsigned long tempo, tempo1, timedel;
  
// Configuração do Hc05:
  int Option, x, i = 5, Time = 1000;  

// Configuração do RTC:
  RTC_DS1307 rtc;

// Configuração do SD Card:
  #define CS 4
  File data;
  String Arquivo = "analise.txt";

// Variaveis Globais
  #define cooler 3
  byte lock = 1;

void mean(void);    void checkSensor(void);
void header(void);  void writeOption(void);
void reading(void); void getDate(void);
void getTime(void); void reportTime(void);
void report(void);  void pulse(void); 
void delData(void); void connection(void);
void MQSensor(void);

void setup(){
  //Sincronização
    tempo = millis(); tempo1 = millis(); timedel = millis();
  //Definir as funcões dos pinos
    pinMode(pulsePin, OUTPUT); pinMode(cooler, OUTPUT); //digitalWrite(pulsePin, HIGH);
  //Inicialização
    Serial.begin(9600); Serial3.begin(9600);
    Wire.begin(); TempHum.begin(); SD.begin(CS);
    while(!Serial3.available()){
      analogWrite(cooler,255); pulse();
    }
    Serial3.read(); Serial3.flush(); 
    rtc.begin(); rtc.isrunning(); rtc.adjust(DateTime(__DATE__, __TIME__));
  //Verificação dos Módulos
    Serial3.println("Inicializando...");
    connection();
  
  getDate(); getTime(); // Obtem data e hora do RTC.
  checkSensor(); // Checa se os sensores estão ligados.
  header(); Option = 48;
}

void loop(){
  writeOption(); delData(); connection(); pulse();
  getTime(); reading(); MQSensor(); report(); analogWrite(cooler, 255);
}

void connection(){
  if(!rtc.isrunning()){
    rtc.adjust(DateTime(__DATE__, __TIME__));
    if(lock){
      //Serial3.println("O relógio não está ativo!");
    }
  }
  if(!SD.begin(CS)){ 
    if(lock){
      //Serial3.println("Cartão SD não encontrado!"); Serial3.println("");
    }
  }
  if(rtc.isrunning() && SD.begin(CS)){
    if(lock){
      //Serial3.println("Dispositivos conectados!"); Serial3.println("");
    }
    lock = 0; return;
  }
  lock = 1;
}

void delData(){
  if(Option == 57){
    Serial3.println("Deseja limpar o arquivo de texto? S/n"); 
    while(!Serial3.available()); Option = Serial3.read(); 
    switch(Option){
      case 83: SD.remove(Arquivo);
               while((SD.exists(Arquivo)) && (millis() - timedel < 2000)){
                    SD.remove(Arquivo); timedel = millis();
               }
               if(SD.exists(Arquivo)) Serial3.println("Falha ao limpar cartão."); break;
      case 115: SD.remove(Arquivo);
                while((SD.exists(Arquivo)) && (millis() - timedel < 2000)){
                     SD.remove(Arquivo); timedel = millis();
                }
                if(SD.exists(Arquivo)) Serial3.println("Falha ao limpar cartão."); break;
      default: Option = 48; break;
    }
  }
}

void mean(){
  for(x=0; x<i; x++){
      S[0] += analogRead(MQ02); S[1] += analogRead(MQ03); S[2] += analogRead(MQ04); S[3] += analogRead(MQ05); 
      S[4] += analogRead(MQ06); S[6] += analogRead(MQ08); S[8] += analogRead(MQ131); S[9] += analogRead(MQ135);
      if(digitalRead(pulsePin) == HIGH){
        analogReference(INTERNAL1V1); S[5] += analogRead(MQ07); S[7] += analogRead(MQ09); analogReference(DEFAULT);
      }
      else {
        analogReference(DEFAULT); S[5] += analogRead(MQ07); S[7] += analogRead(MQ09);
      }
      delay(Time/i);
    }
    S[0] = int(S[0]/x); S[1] = int(S[1]/x); S[2] = int(S[2]/x); S[3] = int(S[3]/x); S[4] = int(S[4]/x);
    S[5] = int(S[5]/x); S[6] = int(S[6]/x); S[7] = int(S[7]/x); S[8] = int(S[8]/x); S[9] = int(S[9]/x);
}

void checkSensor(){
  mean();
  Serial3.println(""); Serial.println("");
  Serial3.println("Checagem de Sensores:"); Serial3.println("Checagem de Sensores:");
  if(S[0] != 0) {Serial3.println(F("   Sensor MQ02:   OK!")); Serial.print(F("Sensor MQ02:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ02:   Não Conectado!")); Serial.print(F("Sensor MQ02:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[1] != 0) {Serial3.println(F("   Sensor MQ03:   OK!")); Serial.print(F("Sensor MQ03:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ03:   Não Conectado!")); Serial.print(F("Sensor MQ03:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[2] != 0) {Serial3.println(F("   Sensor MQ04:   OK!")); Serial.print(F("Sensor MQ04:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ04:   Não Conectado!")); Serial.print(F("Sensor MQ04:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[3] != 0) {Serial3.println(F("   Sensor MQ05:   OK!")); Serial.print(F("Sensor MQ05:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ05:   Não Conectado!")); Serial.print(F("Sensor MQ05:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[4] != 0) {Serial3.println(F("   Sensor MQ06:   OK!")); Serial.print(F("Sensor MQ06:")); Serial.print("\t"); Serial.println("OK!");} 
      else {Serial3.println(F("   Sensor MQ06:   Não Conectado!")); Serial.print(F("Sensor MQ06:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[5] != 0) {Serial3.println(F("   Sensor MQ07:   OK!")); Serial.print(F("Sensor MQ07:")); Serial.print("\t"); Serial.println("OK!");} 
      else {Serial3.println(F("   Sensor MQ07:   Não Conectado!")); Serial.print(F("Sensor MQ07:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[6] != 0) {Serial3.println(F("   Sensor MQ08:   OK!")); Serial.print(F("Sensor MQ08:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ08:   Não Conectado!")); Serial.print(F("Sensor MQ08:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[7] != 0) {Serial3.println(F("   Sensor MQ09:   OK!")); Serial.print(F("Sensor MQ09:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ09:   Não Conectado!")); Serial.print(F("Sensor MQ09:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[8] != 0) {Serial3.println(F("   Sensor MQ131: OK!")); Serial.print(F("Sensor MQ131:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ131: Não Conectado!")); Serial.print(F("Sensor MQ131:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  if(S[9] != 0) {Serial3.println(F("   Sensor MQ135: OK!")); Serial.print(F("Sensor MQ135:")); Serial.print("\t"); Serial.println("OK!");}
      else {Serial3.println(F("   Sensor MQ135: Não Conectado!")); Serial.print(F("Sensor MQ135:")); Serial.print("\t"); Serial.println("Não Conectado!");}
  Serial3.println(""); Serial3.println(""); Serial.println(""); Serial.println(""); 
}

void MQSensor(){
  mean();
  Serial3.print("MQ02:  "); Serial3.print(S[0]); Serial3.print("   ");
  Serial3.print("MQ03:  "); Serial3.print(S[1]); Serial3.print("   ");
  Serial3.print("MQ04:  "); Serial3.print(S[2]); Serial3.print("   ");
  Serial3.print("MQ05:   "); Serial3.print(S[3]); Serial3.print("   ");
  Serial3.print("MQ06:    "); Serial3.print(S[4]); Serial3.println("   ");
  Serial3.print("MQ07:  "); Serial3.print(S[5]); Serial3.print("   ");
  Serial3.print("MQ08:  "); Serial3.print(S[6]); Serial3.print("   ");
  Serial3.print("MQ09:  "); Serial3.print(S[7]); Serial3.print("   ");
  Serial3.print("MQ131: "); Serial3.print(S[8]); Serial3.print("   ");
  Serial3.print("MQ135: "); Serial3.print(S[9]); Serial3.println("   "); Serial3.println("");
}

void header(){
  //Relatório no Cartão SD
    data = SD.open(Arquivo, FILE_WRITE);
    if(!data){
      //Serial3.println("   ***Erro ao abrir documento de texto.***   ");
    }
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
      
  //Relatório no monitor Serial
      Serial.print("."); Serial.println(""); Serial.println(""); Serial.print("Dados em média dos sensores:"); Serial.println(""); 
      Serial.print("\t"); Serial.print("Horas"); Serial.print("\t"); Serial.print("Temp."); Serial.print("\t");
      Serial.print("Umid."); Serial.print("\t"); Serial.print("MQ02"); Serial.print("\t"); Serial.print("MQ03"); Serial.print("\t"); 
      Serial.print("MQ04"); Serial.print("\t"); Serial.print("MQ05"); Serial.print("\t"); Serial.print("MQ06"); Serial.print("\t"); 
      Serial.print("MQ07"); Serial.print("\t"); Serial.print("MQ08"); Serial.print("\t"); Serial.print("MQ09"); Serial.print("\t");
      Serial.print("MQ131"); Serial.print("\t"); Serial.print("MQ0135"); Serial.print("\t"); Serial.println("");
}

void writeOption(){ 
  Serial3.available();
  if(Serial3.read() > 48){
    Option = Serial3.read(); Serial3.println(""); Serial3.print("Eu: ");
    Serial3.write(Option); Serial3.println(""); Serial3.println(""); Serial3.flush();
  }      
}

void reading(){
  if(millis() - tempo1 > (Time)){
    Serial3.print(F(" - "));
    Humidity = TempHum.readHumidity(); Temperature = TempHum.readTemperature();
    Serial3.print(F(" Temp.: ")); Serial3.print(Temperature); Serial3.print(F("°C "));
    Serial3.print(F(" Umid.: ")); Serial3.print(Humidity); Serial3.println(F("% "));
    tempo1 = millis();
  }
}

void getDate(){
  Serial3.println(""); DateTime now = rtc.now();
  if(now.day() < 10){ Serial3.print("0"); Serial3.print(now.day(), DEC);}
    else {Serial3.print(now.day(), DEC);} Serial3.print("/");
  if(now.month() < 10){ Serial3.print("0"); Serial3.print(now.month(), DEC);}
    else {Serial3.print(now.month(), DEC);} Serial3.print("/");
  if(now.year() < 10){ Serial3.print("0"); Serial3.print(now.year(), DEC);}
    else {Serial3.print(now.year(), DEC);} Serial3.print(" - ");
}

void getTime(){
  DateTime now = rtc.now();
  if(now.hour() < 10){ Serial3.print("0"); Serial3.print(now.hour(), DEC);}
    else{Serial3.print(now.hour(), DEC); } Serial3.print(":");
  if(now.minute() < 10){ Serial3.print("0"); Serial3.print(now.minute(), DEC);}
    else{Serial3.print(now.minute(), DEC);} Serial3.print(":");
  if(now.second() < 10){ Serial3.print("0"); Serial3.print(now.second(), DEC);}
    else{Serial3.print(now.second(), DEC);} 
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
  if(!data){ Serial3.println("***Erro ao abrir documento de texto.***");}
    data.print("\t"); Serial.print("\t"); reportTime(); data.print("\t"); data.print(Temperature); data.print("°C"); data.print("\t");
    Serial.print("\t"); Serial.print(Temperature); Serial.print("\t");
    data.print(Humidity); data.print("%"); data.print("\t"); Serial.print(Humidity); Serial.print("\t"); 
    if(S[0] < 100){data.print("0"); data.print(S[0]); Serial.print("0"); Serial.print(S[0]);}
      else {data.print(S[0]); Serial.print(S[0]);} data.print("\t"); Serial.print("\t");
    if(S[1] < 100){data.print("0"); data.print(S[1]); Serial.print("0"); Serial.print(S[1]);}
      else {data.print(S[1]); Serial.print(S[1]);} data.print("\t"); Serial.print("\t"); 
    if(S[2] < 100){data.print("0"); data.print(S[2]); Serial.print("0"); Serial.print(S[2]);}
      else {data.print(S[2]); Serial.print(S[2]);} data.print("\t"); Serial.print("\t");
    if(S[3] < 100){data.print("0"); data.print(S[3]); Serial.print("0"); Serial.print(S[3]);}
      else {data.print(S[3]); Serial.print(S[3]);} data.print("\t"); Serial.print("\t");
    if(S[4] < 100){data.print("0"); data.print(S[4]); Serial.print("0"); Serial.print(S[4]);}
      else {data.print(S[4]); Serial.print(S[4]);} data.print("\t"); Serial.print("\t");
    if(S[5] < 100){data.print("0"); data.print(S[5]); Serial.print("0"); Serial.print(S[5]);}
      else {data.print(S[5]); Serial.print(S[5]);} data.print("\t"); Serial.print("\t"); 
    if(S[6] < 100){data.print("0"); data.print(S[6]); Serial.print("0"); Serial.print(S[6]);}
      else {data.print(S[6]); Serial.print(S[6]);} data.print("\t"); Serial.print("\t");
    if(S[7] < 100){data.print("0"); data.print(S[7]); Serial.print("0"); Serial.print(S[7]);}
      else {data.print(S[7]); Serial.print(S[7]);} data.print("\t"); Serial.print("\t");
    if(S[8] < 100){data.print("0"); data.print(S[8]); Serial.print("0"); Serial.print(S[8]);}
      else {data.print(S[8]); Serial.print(S[8]);} data.print("\t"); Serial.print("\t"); 
    if(S[9] < 100){data.print("0"); data.print(S[9]); Serial.print("0"); Serial.print(S[9]);}
      else {data.print(S[9]); Serial.print(S[9]);} data.println(""); Serial.println(""); 
    data.close();
}

void pulse(){
  if((millis() - tempo) > pulseTime && digitalRead(pulsePin) == HIGH){
    digitalWrite(pulsePin, LOW); tempo = millis();
  }
  if((millis() - tempo) > (pulseTime + (pulseTime/3)) && digitalRead(pulsePin) == LOW){
    digitalWrite(pulsePin, HIGH); tempo = millis();
  }
}
