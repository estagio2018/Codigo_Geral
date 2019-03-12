/*2019-03-12 - IFG-Câmpus Goiânia - Codigo_Geral_Base
  Editor: Victor Gabriel P. Santos
  **Código Base Geral para o projeto do Nariz eletrônico.
  **Fixa Técnica:
    - Módulo Bluetooth Hc-05:
      + Rx necessita de divisor de tensão (5V~3,3V).
    -Sensores de Gás:
      + MQ02 - Detecção de ;
      + MQ03 - Detecção de ;
      + MQ04 - Detecção de ;
      + MQ05 - Detecção de ;
      + MQ06 - Detecção de ;
      + MQ07 - Detecção de ;
      + MQ08 - Detecção de ;
      + MQ09 - Detecção de ;
      + MQ131 - Detecção de ;
      + MQ135 - Detecção de ;
    -Sensor de Temperatura e Umidade DHT22.
*/
    
    
  //Bibliotecas:
  #include <SoftwareSerial.h>
  #include <DHT.h>

// Configuração do DHT:
  # Define DHT_Pin A0
  # Define DHT_Type DHT22
  # DHT TempHum(DHT_Pin, DHT_Type)
  
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
  int S1, S2, S3, S4, S5, S6, S7, S8, S9, S131, S135;
  
// Configuração do Hc05:
  SoftwareSerial Cell(2,3);
  int Option, Time, Marker;
  
void setup(){
  TempHum.begin();
  Cell.begin(9600);
  Marker=0;
  pinMode(MQ_02, INPUT); pinMode(MQ_03, INPUT); pinMode(MQ_04, INPUT); pinMode(MQ_05, INPUT); pinMode(MQ_06, INPUT);
  pinMode(MQ_07, INPUT); pinMode(MQ_08, INPUT); pinMode(MQ_09, INPUT); pinMode(MQ_131, INPUT); pinMode(MQ_135, INPUT);
  S2 = analogRead(MQ_02); S3 = analogRead(MQ_03); S4 = analogRead(MQ_04); S5 = analogRead(MQ_05); S6 = analogRead(MQ_06);
  S7 = analogRead(MQ_07); S8 = analogRead(MQ_08); S9 = analogRead(MQ_09); S131 = analogRead(MQ_131); S135 = analogRead(MQ_135);
  checkSensor(); // Checa se os sensores estão ligados.
}

void loop(){
  Cell.println(F(""));
  Cell.println(F("Escolha uma das opções abaixo:"));
  Cell.println(F("============================"));
  Cell.println(F("  1  -  informe a cada segundo."));
  Cell.println(F("  2  -  informe a cada 2 segundos."));
  Cell.println(F("  3  -  informe a cada 5 segundos."));
  Cell.println(F("  4  -  informe a cada 10 segundos."));
  Cell.println(F("  5  -  informe a cada 30 segundos."));
  Cell.println(F("  6  -  informe a cada minuto."));
  Cell.println(F("  7  -  informe a cada 5 minutos."));
  Cell.println(F("  8  -  informe a cada 10 minutos."));
  Cell.println(F("  9  -  Reiniciar."));
  Cell.println(F("============================"));
  Cell.println(F(""));
  while(!Cell.available()){
    if(Marker != 0){
      reading();
    }
  }
  Option = Cell.read();
  switch(Option-48){
    case 1: Time = 1; reading(); break;
    case 2: Time = 2; reading(); break;
    case 3: Time = 5; reading(); break;
    case 4: Time = 10; reading(); break;
    case 5: Time = 30; reading(); break;
    case 6: Time = 60; reading(); break;
    case 7: Time = 300; reading(); break;
    case 8: Time = 600; reading(); break;
    case 9: setup(); break;
    default: Cell.println(F(""));
             Cell.println(F("Desculpe, essa opção não"));
             Cell.println(F("está disponível."));
             Cell.println(F("")); 
             Cell.println(F("Por favor digite uma das opções a seguir:")); break;
    }
      
}

void checkSensor(){
  Cell.println(F("Digite qualquer coisa."));
  if(Cell.available()){
    if(S2!=0) {Cell.println(F("Sensor MQ02: OK!")); delay(300);}
    if(S3!=0) {Cell.println(F("Sensor MQ03: OK!")); delay(300);}
    if(S4!=0) {Cell.println(F("Sensor MQ04: OK!")); delay(300);}
    if(S5!=0) {Cell.println(F("Sensor MQ05: OK!")); delay(300);}
    if(S6!=0) {Cell.println(F("Sensor MQ06: OK!")); delay(300);}
    if(S7!=0) {Cell.println(F("Sensor MQ07: OK!")); delay(300);}
    if(S8!=0) {Cell.println(F("Sensor MQ08: OK!")); delay(300);}
    if(S9!=0) {Cell.println(F("Sensor MQ09: OK!")); delay(300);}
    if(S131!=0) {Cell.println(F("Sensor MQ131: OK!")); delay(300);}
    if(S135!=0) {Cell.println(F("Sensor MQ135: OK!")); delay(300);} 
  }
  Cell.println(""); Cell.println("");
  delay(500);
}
      
      
void reading(){
  float Humidity = TempHum.readHumidity();
  float Temperature = TempHum.readTemperature();
  Cell.print(F("Temperatura= ")); Cell.print(Temperature); Cell.print(F("°C "));
  Cell.print(F(" Umidade= ")); Cell.print(Humidity); Cell.println(F("% "));
  delay(Time*1000);
  Marker = 1;
}


  
  
  
  
  
