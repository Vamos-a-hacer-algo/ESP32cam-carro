#include <Arduino.h>
#include "BluetoothSerial.h"
#include <HardwareSerial.h>

BluetoothSerial SerialBT;

//Pines libres 4

const int MR1_Pin = 13; //ESP32 pins (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)
const int MR2_Pin = 12; 
const int ML1_Pin = 15;
const int ML2_Pin = 14;
const int TRIG_Pin = 16;      // Pines del para el sensor ultrasonico
const int ECHO_Pin = 2;
const int INFRAIZQ_Pin = 32; //Pines para los sensores
const int INFRADER_Pin = 33;
const int RXD1 = 3;
const int TXD1 = 1;

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


//Control ruedas
void Forward();
void Backward();
void Left();
void Right();
void Stop();
void ForwardLeft();
void ForwardRight();
void BackwardLeft();
void BackwardRight();
//Rutina de lectura del ultrasonico

void ultrasonico(); 
float timeToCm(float time);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32 CAR"); //Nombre del dispositivo Bluetooth
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1); //Inicio de la comunicación serial con el pico
  pinMode(MR1_Pin, OUTPUT); 
  pinMode(MR2_Pin, OUTPUT);
  pinMode(ML1_Pin, OUTPUT);
  pinMode(ML2_Pin, OUTPUT);
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
      Forward();  
    } else if(!INFRAIZQ_bit && INFRADER_bit) {
      Right();
    } else if(INFRAIZQ_bit && !INFRADER_bit) {
      Left();
    } else {
      Stop();
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
        Stop();
      }
      else {
        modo = 1;
      } 
    }
    
    if (!modo) { //Manual
      if(receivedChar == 'F') {
        Forward();
      }
      if(receivedChar == 'G') {
        Backward(); 
      }         
      if(receivedChar == 'L') {
        Left();
      }        
      if(receivedChar == 'R') {
        Right(); 
      }
      if(receivedChar == 'Q') {
        ForwardLeft();
      }
      if(receivedChar == 'E') {
        ForwardRight();
      }
      if(receivedChar == 'Z') {
        BackwardLeft();
      }
      if(receivedChar == 'C') {
        BackwardRight();
      }
      if(receivedChar == 'S') {
        Stop();
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

void Forward() {
  //RIGHT MOTOR
  digitalWrite(MR1_Pin,HIGH);
  digitalWrite(MR2_Pin,LOW);
  //LEFT MOTOR
  digitalWrite(ML1_Pin,HIGH);
  digitalWrite(ML2_Pin,LOW);
}
void Backward() {
  digitalWrite(MR1_Pin,LOW);
  digitalWrite(MR2_Pin,HIGH);
  digitalWrite(ML1_Pin,LOW);
  digitalWrite(ML2_Pin,HIGH);
}
void Left() {
  digitalWrite(MR1_Pin,HIGH);
  digitalWrite(MR2_Pin,LOW);
  digitalWrite(ML1_Pin,LOW);
  digitalWrite(ML2_Pin,HIGH);
}
void Right() {
  digitalWrite(MR1_Pin,LOW);
  digitalWrite(MR2_Pin,HIGH);
  digitalWrite(ML1_Pin,HIGH);
  digitalWrite(ML2_Pin,LOW);
}
void Stop() {
  digitalWrite(MR1_Pin,LOW); 
  digitalWrite(MR2_Pin,LOW);
  digitalWrite(ML1_Pin,LOW); 
  digitalWrite(ML2_Pin,LOW); 
}
void ForwardLeft() {
  digitalWrite(MR1_Pin,HIGH); 
  digitalWrite(MR2_Pin,LOW);
  digitalWrite(ML1_Pin,LOW); 
  digitalWrite(ML2_Pin,LOW);
}
void ForwardRight() {
  digitalWrite(MR1_Pin,LOW); 
  digitalWrite(MR2_Pin,LOW);
  digitalWrite(ML1_Pin,HIGH); 
  digitalWrite(ML2_Pin,LOW);
}
void BackwardLeft() {
  digitalWrite(MR1_Pin,LOW); 
  digitalWrite(MR2_Pin,HIGH);
  digitalWrite(ML1_Pin,LOW); 
  digitalWrite(ML2_Pin,LOW);
}
void BackwardRight() {
  digitalWrite(MR1_Pin,LOW); 
  digitalWrite(MR2_Pin,LOW);
  digitalWrite(ML1_Pin,LOW); 
  digitalWrite(ML2_Pin,HIGH);
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
 