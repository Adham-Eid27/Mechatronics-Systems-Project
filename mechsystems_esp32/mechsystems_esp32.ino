#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <DHT.h>
#include <arduino.h>
#include <HardwareSerial.h>

#define WIFI_SSID "Esp32test"
#define WIFI_PASSWORD "yess1234"
#define API_KEY "AIzaSyAi7m5RWfiDAvIHr3786ryhX-UHvTIXEPY"
#define DATABASE_URL "https://mechasystem-7394a-default-rtdb.europe-west1.firebasedatabase.app/"

HardwareSerial SerialPort(2);
const int Enable =  2;
const int SlaveNumber = 1;
int Slave;

int LDR_PIN=34;
int DHTpin=33;
int Line_follower=32;
int RelayPin=27;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int ldrData =0;
int line =0;

float temp=0;
float humid=0;

DHT dht(DHTpin, DHT11);

void setup() {

SerialPort.begin(115200, SERIAL_8N1, 16, 17); 
  SerialPort.setTimeout(250);
  pinMode(Enable, OUTPUT);

pinMode(RelayPin, OUTPUT);
digitalWrite(RelayPin, LOW);

Serial.begin(115200);

dht.begin();

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print ("CONNECTING  TO WI-FI");
while(WiFi.status() != WL_CONNECTED){
  Serial.print("..."); delay(300);
}
Serial.println();
Serial.print("Connected with IP: ");
Serial.println(WiFi.localIP());
Serial.println();

config.api_key=API_KEY;
config.database_url=DATABASE_URL;
if(Firebase.signUp(&config, &auth, "", "")){
  Serial.println("SignUP OK");
signupOK = true;
}else{
  Serial.printf("%s\n", config.signer.signupError.message.c_str());
}
config.token_status_callback = tokenStatusCallback;
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
}
void loop() {

  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 100 || sendDataPrevMillis ==0)){
    sendDataPrevMillis= millis();
    //LDR
    ldrData = analogRead(LDR_PIN);
    //DHT11
    temp = dht.readTemperature();
    humid = dht.readHumidity();
    //Line Follower Analog input
    line = analogRead(Line_follower);
    

    //LDR
    if(Firebase.RTDB.setInt(&fbdo, "/Sensor/LDR", ldrData)){
      Serial.println();
      //ldrData = random(200);
      Serial.print(ldrData);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ") ");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    //DHT11 temperature
    if(Firebase.RTDB.setFloat(&fbdo, F("/Sensor/DHTTemperature"), temp)){
      Serial.println();
      //temp = random(200);
      Serial.print(temp);
      Serial.print(" - succesfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ") ");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    //DHT11 humid
    if(Firebase.RTDB.setFloat(&fbdo, F("/Sensor/DHTHumidity"), humid)){
      Serial.println();
      //humid = random(200);
      Serial.print(humid);
      Serial.print(" - succesfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    //Line follower
    if(Firebase.RTDB.setInt(&fbdo, "/Sensor/LineFollower", line)){
      Serial.println();
      //Line_follower = random(200);
      Serial.print(line);
      Serial.print(" - succesfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    //relay
    int Relay;
    if(Firebase.RTDB.getInt(&fbdo, "/Sensor/RELAY", &Relay)){
      digitalWrite(RelayPin, Relay);
      
      Serial.println();
      Serial.print(Relay);
      Serial.print(" - succesfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");

    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    digitalWrite(Enable, LOW); 
    Slave = SerialPort.read();
      Serial.println(Slave);
  if(SerialPort.available())
  {
    delay(10000);
      Serial.println("It reached here");
      Slave = SerialPort.read();
      Serial.println(Slave);
      if(Slave == SlaveNumber)
      {   
        String command = SerialPort.readString(); 
          Serial.println("pls"); 
          Serial.println(command);
           Serial.println("loop");
        }

  }
  }
}