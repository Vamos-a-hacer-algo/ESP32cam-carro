#include <Arduino.h>
#include "BluetoothSerial.h"
#include <HardwareSerial.h>

BluetoothSerial SerialBT;

// Pines seguros CAM: 2, 12, 13, 14, 15, 16
// Nombre de los pines de los motores
// (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)

// ********** DECLARACIÓN DE PINES **********

// Pines para el sensor ultrasonico

const int TRIG_Pin = 19;
const int ECHO_Pin = 21;

// Pines para los infrarrojos
const int INFRAIZQ_Pin = 18; //Pines para los sensores
const int INFRADER_Pin = 5;

// Pines de comunicación con el Pico 
// (U2 en el ESP32, para que no colisione
// con la comunicación por U1 con la computadora
// para debugging)

const int RXD1 = 16;
const int TXD1 = 17;

// **************** VARIABLES ****************
char receivedChar;           // Caracter recibido por la aplicación
int Luces_bit = 0;           // Estado de las luces (0 == OFF; 1 == ON)
int Buzzer_bit = 0;          // Estado del buzzer (0 == OFF; 1 == ON)

// Estado de los infrarrojos
int INFRAIZQ_bit;
int INFRADER_bit;
// (0 == detecta; 1 == no detecta)

float tiempo_espera;         //Salida sensor ultrasonico
float distancia;             //Distancia medida por el ultrasonico  (Luego de convertir tiempo a distancia)
int modo = 0;                //Modo de operación del carrito. 0 es manual y 1 es automático

uint32_t mseg_Ultrasonico = 0;  // Momento entre la última medida del ultrasónico y la próxima
uint32_t mseg_Buzzer = 0;       // Momento desde el encendido del buzzer

// ***************** RUTINAS *****************

void ultrasonico();         // Rutina de lectura del ultrasónico
float timeToCm(float time); // Rutina auxiliar para el ultrasónico

void setup() {
  Serial.begin(115200);     // Inicio de la comunicación serial para debugging
  SerialBT.begin("ESP32 CAR"); // Nombre del dispositivo Bluetooth
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1); //Inicio de la comunicación serial con el pico
  
  // Inicialización de los pines

  pinMode(ECHO_Pin, INPUT);
  pinMode(TRIG_Pin, OUTPUT);
  pinMode(INFRAIZQ_Pin, INPUT);
  pinMode(INFRADER_Pin, INPUT);
}

void loop() {
  // Rutina de lectura del ultrasónico
  if(millis() - mseg_Ultrasonico >= 700) { // Si pasa el tiempo apropiado desde la última lectura...
    ultrasonico();                        // Haz la rutina del ultrasónico
    mseg_Ultrasonico = millis();          // Actualiza el momento de la última lectura
  }
  
  // Rutina de lectura de los infrarrojos
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
  // Este if sirve para enviar comandos desde la computadora hacia la aplicación
  if (Serial.available()) {        // Si se envían datos desde la computadora...
    SerialBT.write(Serial.read()); // Se reenvían directo a la app
  }

  // Rutina de lectura de los comandos enviados desde la app
  if (SerialBT.available()) { // Si se envía un caracter desde la app
    
    receivedChar =(char)SerialBT.read();
    //Serial1.write(receivedChar); // Envio DIRECTO al Pico ## DA PROBLEMAS ##

    // Los caracteres de la app tienen al final un salto de línea
    // El objetivo de este if es que la información del debugging
    // Se vea lo más bonita posible
    if(receivedChar != '\n') {
      Serial.print ("Received:");//print on serial monitor
      Serial.println(receivedChar);//print on serial monitor 
      }

    if(receivedChar == 'M') {  // Luces
      Serial1.write('M');
    }
    
    if(receivedChar == 'Y') { //Modo del carro
      if (modo) {
        modo = 0;
        Serial1.write('S'); // Si el carro pasa de automático a manual, que se detenga
      }
      else {
        modo = 1;
      } 
    }

    // Corneta
    //
    // Se puso la condición "!Buzzer_bit" porque el buzzer
    // Es temporizado, y no se quiere que se envíe otro caracter X
    // Cuando el buzzer ya está sonando

    if (receivedChar == 'X' && !Buzzer_bit) { //Corneta
      Serial1.write('X');
      Buzzer_bit = 1;
      mseg_Buzzer = millis();
    }
    // En este if se manejan todos los caracteres
    // que se refieren a movimiento de las ruedas

    if (!modo) { // Si el modo es manual...
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
  }
   // Aquí se controla el tiempo de encendido del buzzer
  if ((Buzzer_bit)) { // Si el buzzer está encendido
    if(millis() - mseg_Buzzer >= 1000 ) { // Si pasa el tiempo suficiente desde el encendido del buzzer...
      char send = 'X'; 
      Serial1.write(send); // Apagalo
      Buzzer_bit = 0; // Y actualiza la bandera de estado correspondiente
    }
  }
  delay(20);
}

void ultrasonico() {
  // Se manda el pulso al ultrasónico
  digitalWrite (TRIG_Pin, LOW); 
  delayMicroseconds(2);
  digitalWrite (TRIG_Pin, HIGH);
  delayMicroseconds (10);
  digitalWrite (TRIG_Pin, LOW); 

  // Se mide la respuesta del ultrasónico
  tiempo_espera = pulseIn(ECHO_Pin, HIGH, 20);

  // Se transforma la respuesta a unidad de distancia
  distancia = timeToCm(tiempo_espera);
  
  // Se manda la información a la app
  SerialBT.print('D');  
  SerialBT.println(distancia);  
}

float timeToCm(float time) {
  return (time / 2.0) / 29.15;
}
 
