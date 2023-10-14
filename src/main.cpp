#include <Arduino.h>
#include "BluetoothSerial.h"
//#include "esp_camera.h"
//#include <WiFi.h>
//#include "esp_timer.h"
//#include "img_converters.h"
//#include "fb_gfx.h"
//#include "soc/soc.h"            //disable brownout problems
//#include "soc/rtc_cntl_reg.h"   //disable brownout problems
//#include "esp_http_server.h"

BluetoothSerial SerialBT;

char caracterRecibido;  //Caracter enviado desde la app
int automatico = 0;     //Modo de operacion (seguidor de linea o manual)
int luces_bit = 0;      //Control de luces
float tiempo_espera;    //Salida sensor ultrasonico
float distancia;        //Distancia medida por el ultrasonico

const int MRP = 15;     //ESP32 pins (MR=Right Motor) (ML=Left Motor) (P=Positivo) (N=Negativo)
const int MRN = 14; 
const int MLP = 12;
const int MLN = 13;

const int LUCES = 2;  //Pin para controlar luces 
const int BUZZER = 4; //Pin para controlar buzzer

const int TRIG = 16; // Pines del para el sensor ultrasonico
const int ECHO = 0;

const int INFRAIZQ = 3; //Pines para los sensores
const int INFRADER = 1;

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

void ultrasonico(); //Rutina de lectura del ultrasonico
float timeToCm(float time);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("Carrito ESP32 Normal"); //Nombre del dispositivo Bluetooth

  pinMode(MRP, OUTPUT); 
  pinMode(MRN, OUTPUT);
  pinMode(MLP, OUTPUT);
  pinMode(MLN, OUTPUT);
  pinMode(LUCES, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(INFRAIZQ, INPUT);
  pinMode(INFRADER, INPUT);
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());  //Manda info desde el terminal del VScode hasta la app
  }

  if (SerialBT.available()) {
    caracterRecibido = (char)SerialBT.read(); //Lee informacion de la app
  }

  Serial.print("Received:");           //print on serial monitor
  Serial.println(caracterRecibido);    //print on serial monitor    

  if(caracterRecibido == 'Y') {
    automatico = (automatico == 0 ? 1 : 0); //automatico = 0, el carro está en manual. automatico = 1, el carro está en automatico
  }

  if(caracterRecibido == 'M') {
    luces_bit = (luces_bit == 0 ? 1 : 0); // cambia el bit de luces cada vez que sea recibido el caracter M
  }

  if(caracterRecibido == 'X') {
    //buzzer_bit = (buzzer_bit == 0 ? 1 : 0);
    digitalWrite(BUZZER,HIGH);
    delay(500);
    digitalWrite(BUZZER,LOW);
  }

  if(automatico) {  //Si esta en modo seguidor de linea (automatico)
    if(!INFRAIZQ && !INFRADER) {
      Forward();  
    } else if(!INFRAIZQ && INFRADER) {
      Right();
    } else if(INFRAIZQ && !INFRADER) {
      Left();
    } else {
      Stop();
    }
  } else {  //Si esta en modo manual
    if(caracterRecibido == 'F') {
      Forward();
    }
    if(caracterRecibido == 'G') {
      Backward(); 
    }         
    if(caracterRecibido == 'L') {
      Left();
    }        
    if(caracterRecibido == 'R') {
      Right(); 
    }
    if(caracterRecibido == 'Q') {
      ForwardLeft();
    }
    if(caracterRecibido == 'E') {
      ForwardRight();
    }
    if(caracterRecibido == 'Z') {
      BackwardLeft();
    }
    if(caracterRecibido == 'C') {
      BackwardRight();
    }
    if(caracterRecibido == 'S') {
      Stop();
    }
  }

  digitalWrite(LUCES, luces_bit ? HIGH : LOW);

  //digitalWrite(BUZZER, buzzer_bit ? HIGH : LOW);

  ultrasonico();

  delay(50);
}

void Forward() {
  //RIGHT MOTORS
  digitalWrite(MRP,HIGH);
  digitalWrite(MRN,LOW);
  //LEFT MOTORS
  digitalWrite(MLP,HIGH);
  digitalWrite(MLN,LOW);
}

void Backward() {
  digitalWrite(MRP,LOW);
  digitalWrite(MRN,HIGH);
  digitalWrite(MLP,LOW);
  digitalWrite(MLN,HIGH);
}

void Left() {
  digitalWrite(MRP,HIGH);
  digitalWrite(MRN,LOW);
  digitalWrite(MLP,LOW);
  digitalWrite(MLN,HIGH);
}

void Right() {
  digitalWrite(MRP,LOW);
  digitalWrite(MRN,HIGH);
  digitalWrite(MLP,HIGH);
  digitalWrite(MLN,LOW);
}

void Stop() {
  digitalWrite(MRP,LOW); 
  digitalWrite(MRN,LOW);
  digitalWrite(MLP,LOW); 
  digitalWrite(MLN,LOW); 
}

void ForwardLeft() {
  digitalWrite(MRP,HIGH); 
  digitalWrite(MRN,LOW);
  digitalWrite(MLP,LOW); 
  digitalWrite(MLN,LOW);
}

void ForwardRight() {
  digitalWrite(MRP,LOW); 
  digitalWrite(MRN,LOW);
  digitalWrite(MLP,HIGH); 
  digitalWrite(MLN,LOW);
}

void BackwardLeft() {
  digitalWrite(MRP,LOW); 
  digitalWrite(MRN,HIGH);
  digitalWrite(MLP,LOW); 
  digitalWrite(MLN,LOW);
}

void BackwardRight() {
  digitalWrite(MRP,LOW); 
  digitalWrite(MRN,LOW);
  digitalWrite(MLP,LOW); 
  digitalWrite(MLN,HIGH);
}

void ultrasonico() {
  digitalWrite (TRIG, LOW); 
  delayMicroseconds(2);
  digitalWrite (TRIG, HIGH);
  delayMicroseconds (10);
  digitalWrite (TRIG, LOW); 

  tiempo_espera = pulseIn(ECHO, HIGH);

  distancia = timeToCm(tiempo_espera);

  SerialBT.print('D');  
  SerialBT.println(distancia);  
}

float timeToCm(float time) {
  return (time / 2.0) / 29.15;
}

 