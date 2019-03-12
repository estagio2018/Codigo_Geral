/*2019-03-12 - IFG-Câmpus Goiânia - Codigo_Geral_Base
  Editor: Victor Gabriel P. Santos
  **Código Base Geral para o projeto do Nariz eletrônico.
  **Fixa Técnica:
    - Módulo Bluetooth Hc-05:
      + Rx necessita de divisor de tensão (5V~3,3V).
    -Sensores de Gás:
      + MQ02;
      + MQ03;
      + MQ04;
      + MQ05;
      + MQ06;
      + MQ07;
      + MQ08;
      + MQ09;
      + MQ131;
      + MQ135;
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
  
// Configuração do Hc05:
  SoftwareSerial Cell(2,3);
  int Option, Time, Marker;
  
void setup(){
  TempHum.begin();
  Cell.begin(9600);
  Marker=0;

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
    default: Cell.println("");
             Cell.println("Desculpe, essa opção não");
             Cell.println("está disponível.");
             Cell.println(""); 
             Cell.println("Por favor digite uma das opções a seguir:"); break;
    }
      
}

void reading(){
  float Humidity = TempHum.readHumidity();
  float Temperature = TempHum.readTemperature();
  Cell.print("Temperatura= "); Cell.print(Temperature); Cell.print("°C ");
  Cell.print(" Umidade= "); Cell.print(Humidity); Cell.println("% ");
  delay(Time*1000);
  Marker = 1;
}
  
  
  
  
  
  
  
