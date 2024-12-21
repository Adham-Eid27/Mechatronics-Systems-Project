#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>
HardwareSerial SerialPort(2);

int slave_1_id = 1;

const int Enable =  15;

void setup() {
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, 16, 17); 
  //SerialPort.setTimeout(250);
  pinMode(Enable, OUTPUT);
  pinMode(2,OUTPUT);
  digitalWrite(Enable, LOW);
}

void loop() {
  int randomm =random(200);
    delay(500);
    digitalWrite(Enable, HIGH);
    SerialPort.print(slave_1_id);
    SerialPort.print(randomm);
    Serial.println(randomm);
    //SerialPort.flush();
    digitalWrite(Enable, LOW);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2,LOW);
      
  }