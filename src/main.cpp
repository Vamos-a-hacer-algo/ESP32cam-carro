#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

char receivedChar;// received value will be stored as CHAR in this variable

const int MR1 = 15; //ESP32 pins (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)
const int MR2 = 14; 
const int ML1 = 12;
const int ML2 = 13;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("CARRITO"); //You can change your Bluetooth device name
  pinMode(MR1, OUTPUT); 
  pinMode(MR2, OUTPUT);
  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
}

void Forward() {
      //RIGHT MOTOR
      digitalWrite(MR1,HIGH);
      digitalWrite(MR2,LOW);
      //LEFT MOTOR
      digitalWrite(ML1,HIGH);
      digitalWrite(ML2,LOW);
}
void Backward() {
      digitalWrite(MR1,LOW);
      digitalWrite(MR2,HIGH);
      digitalWrite(ML1,LOW);
      digitalWrite(ML2,HIGH);
}
void Left() {
      digitalWrite(MR1,HIGH);
      digitalWrite(MR2,LOW);
      digitalWrite(ML1,LOW);
      digitalWrite(ML2,HIGH);
}
void Right() {
      digitalWrite(MR1,LOW);
      digitalWrite(MR2,HIGH);
      digitalWrite(ML1,HIGH);
      digitalWrite(ML2,LOW);
}
void Stop() {
      digitalWrite(MR1,LOW); 
      digitalWrite(MR2,LOW);
      digitalWrite(ML1,LOW); 
      digitalWrite(ML2,LOW); 
}
void ForwardLeft() {
  digitalWrite(MR1,HIGH); 
  digitalWrite(MR2,LOW);
  digitalWrite(ML1,LOW); 
  digitalWrite(ML2,LOW);
}
void ForwardRight() {
  digitalWrite(MR1,LOW); 
  digitalWrite(MR2,LOW);
  digitalWrite(ML1,HIGH); 
  digitalWrite(ML2,LOW);
}
void BackwardLeft() {
  digitalWrite(MR1,LOW); 
  digitalWrite(MR2,HIGH);
  digitalWrite(ML1,LOW); 
  digitalWrite(ML2,LOW);
}
void BackwardRight() {
  digitalWrite(MR1,LOW); 
  digitalWrite(MR2,LOW);
  digitalWrite(ML1,LOW); 
  digitalWrite(ML2,HIGH);
}
void loop() {
  receivedChar =(char)SerialBT.read();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {
     
    Serial.print ("Received:");//print on serial monitor
    Serial.println(receivedChar);//print on serial monitor    
    
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

  delay(20);
}
 