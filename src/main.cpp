#include <Arduino.h>
#include "BluetoothSerial.h"
#include <SoftwareSerial.h>


BluetoothSerial SerialBT;
SoftwareSerial mySerial(12, 13); // RX, TX

//Hola x3

//Pines libres 4, 13, 12, 15, 14

//Pico pins (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)

const int TRIG_Pin = 19;      // Pines del para el sensor ultrasonico
const int ECHO_Pin = 21;
const int INFRAIZQ_Pin = 18; //Pines para los sensores
const int INFRADER_Pin = 5;
const int RXD1 = 16;
const int TXD1 = 17;

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
  mySerial.begin(4800, SWSERIAL_7E2);
  SerialBT.begin("ESP32cam CAR"); //Nombre del dispositivo Bluetooth
  //mySerial.begin(115200, SERIAL_8N1, RXD1, TXD1); //Inicio de la comunicación serial con el pico
  
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
      mySerial.write('F');
    } else if(!INFRAIZQ_bit && INFRADER_bit) {
      mySerial.write('R');
    } else if(INFRAIZQ_bit && !INFRADER_bit) {
      mySerial.write('L');
    } else {
      mySerial.write('S');
    }
  }

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {
    
    receivedChar =(char)SerialBT.read();
    //mySerial.write(receivedChar); //Envio a RasberryPi Pico

    if(receivedChar != '\n') {
      Serial.print ("Received:");//print on serial monitor
      Serial.println(receivedChar);//print on serial monitor 
      }

     if(receivedChar == 'M') {
      mySerial.write('M');
     }
    
    if(receivedChar == 'Y') { //Modo del carro
      if (modo) {
        modo = 0;
        mySerial.write('S');
      }
      else {
        modo = 1;
      } 
    }
    
    if (!modo) { //Manual
      if(receivedChar == 'F') {
        mySerial.write('F');
      }
      if(receivedChar == 'G') {
        mySerial.write('G');
      }         
      if(receivedChar == 'L') {
        mySerial.write('L');
      }        
      if(receivedChar == 'R') {
        mySerial.write('R');
      }
      if(receivedChar == 'Q') {
        mySerial.write('Q');
      }
      if(receivedChar == 'E') {
        mySerial.write('E');
      }
      if(receivedChar == 'Z') {
        mySerial.write('Z');
      }
      if(receivedChar == 'C') {
        mySerial.write('C');
      }
      if(receivedChar == 'S') {
        mySerial.write('S');
      }
    }
    
    if (receivedChar == 'X' && !Buzzer_bit) { //Corneta
      mySerial.write('X');
      Buzzer_bit = 1;
      mseg_Buzzer = millis();
    }
  }
    if ((Buzzer_bit)) {
      if(millis() - mseg_Buzzer >= 1000 ) {
        char send = 'X';
        mySerial.write(send);
        Buzzer_bit = 0;
      }
    }
  delay(20);
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
 
