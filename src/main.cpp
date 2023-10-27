#include <Arduino.h>
#include "BluetoothSerial.h"
#include <HardwareSerial.h>

BluetoothSerial SerialBT;

//Hola x2

//Pines libres 4, 13, 12, 15, 14

//Pico pins (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)

const int TRIG_Pin = 16;      // Pines del para el sensor ultrasonico
const int ECHO_Pin = 2;
const int INFRAIZQ_Pin = 32; //Pines para los sensores
const int INFRADER_Pin = 33;
const int RXD1 = 13;
const int TXD1 = 12;

char receivedChar;           // received value will be stored as CHAR in this variable
int Luces_bit = 0;           //Estado de las luces
float tiempo_espera;          //Salida sensor ultrasonico
float distancia;             //Distancia medida por el ultrasonico
int modo = 0;           //En 0 es manual y 1 es automatico
int INFRAIZQ_bit;
int INFRADER_bit;
int Buzzer_bit = 0;
uint32_t mseg_Ultrasonico = 0;
uint32_t mseg_Buzzer = 0;



//Rutina de lectura del ultrasonico

void ultrasonico(); 
float timeToCm(float time);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32 CAR"); //Nombre del dispositivo Bluetooth
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1); //Inicio de la comunicación serial con el pico
  
  pinMode(ECHO_Pin, INPUT);
  pinMode(TRIG_Pin, OUTPUT);
  pinMode(INFRAIZQ_Pin, INPUT);
  pinMode(INFRADER_Pin, INPUT);
}


void loop() {
  if(millis() - mseg_Ultrasonico >= 700) {
    ultrasonico(); //Mide distancia
    mseg_Ultrasonico = millis();
  }
  
  INFRADER_bit = digitalRead(INFRADER_Pin);
  INFRAIZQ_bit = digitalRead(INFRAIZQ_Pin);

  if (modo) { //Automatico
    if(!INFRAIZQ_bit && !INFRADER_bit) {
      Serial1.write('F');
    } else if(!INFRAIZQ_bit && INFRADER_bit) {
      Serial1.write('R');
    } else if(INFRAIZQ_bit && !INFRADER_bit) {
      Serial1.write('L');
    } else {
      Serial1.write('S');
    }
  }

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {
    
    receivedChar =(char)SerialBT.read();
    Serial1.write(receivedChar); //Envio a RasberryPi Pico

    if(receivedChar != '\n') {
      Serial.print ("Received:");//print on serial monitor
      Serial.println(receivedChar);//print on serial monitor 
      } 

    if(receivedChar == 'Y') { //Modo del carro
      if (modo) {
        modo = 0;
        Serial1.write('S');
      }
      else {
        modo = 1;
      } 
    }
    
    if (!modo) { //Manual
      if(receivedChar == 'F') {
        Serial1.write('F');
      }
      if(receivedChar == 'G') {
        Serial1.write('G');
      }         
      if(receivedChar == 'L') {
        Serial1.write('L');
      }        
      if(receivedChar == 'R') {
        Serial1.write('R');
      }
      if(receivedChar == 'Q') {
        Serial1.write('Q');
      }
      if(receivedChar == 'E') {
        Serial1.write('E');
      }
      if(receivedChar == 'Z') {
        Serial1.write('Z');
      }
      if(receivedChar == 'C') {
        Serial1.write('C');
      }
      if(receivedChar == 'S') {
        Serial1.write('S');
      }
    }
    
    if (receivedChar == 'X') { //Corneta
      Buzzer_bit = 1;
      mseg_Buzzer = millis();
    }
  }
    if ((Buzzer_bit)) {
      if(millis() - mseg_Buzzer >= 1000 ) {
        char send = 'X';
        Serial1.write(send);
        Buzzer_bit = 0;
      }
    }
}

void ultrasonico() {
  digitalWrite (TRIG_Pin, LOW); 
  delayMicroseconds(2);
  digitalWrite (TRIG_Pin, HIGH);
  delayMicroseconds (10);
  digitalWrite (TRIG_Pin, LOW); 

  tiempo_espera = pulseIn(ECHO_Pin, HIGH, 20);

  distancia = timeToCm(tiempo_espera);

  SerialBT.print('D');  
  SerialBT.println(distancia);  
}

float timeToCm(float time) {
  return (time / 2.0) / 29.15;
}
 
