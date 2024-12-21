#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <DHT.h>
#include <arduino.h>
//transmitter
#include <ModbusMaster.h>

ModbusMaster node;

#define WIFI_SSID "Esp32test"
#define WIFI_PASSWORD "yess1234"
#define API_KEY "AIzaSyAi7m5RWfiDAvIHr3786ryhX-UHvTIXEPY"
#define DATABASE_URL "https://mechasystem-7394a-default-rtdb.europe-west1.firebasedatabase.app/"

int LDR_PIN=34;
int DHTpin=33;
int Line_follower=32;
int RelayPin=27;

#define MAX485_DE_RE 4

void preTransmission() {
digitalWrite(MAX485_DE_RE, HIGH); // Enable transmitter
}

void postTransmission() {
digitalWrite(MAX485_DE_RE, LOW); // Disable transmitter, enable receiver
}

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
pinMode(RelayPin, OUTPUT);
digitalWrite(RelayPin, LOW);

Serial.begin(115200);

dht.begin();

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print ("CONNECTING  TO WI-FI");
while(WiFi.status() != WL_CONNECTED){
  Serial.print("..."); delay(300);


  // Initialize DE/RE pin
pinMode(MAX485_DE_RE, OUTPUT);
digitalWrite(MAX485_DE_RE, LOW); // Start with receiver enabled

// Initialize Serial for debugging
Serial.begin(9600);

// Initialize Modbus communication baud rate
Serial2.begin(9600, SERIAL_8N1, 16, 17); // Using UART2 (pins 16 and 17)

// Modbus communication settings
node.begin(1, Serial2); // Slave ID: 1
node.preTransmission(preTransmission);
node.postTransmission(postTransmission);
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

    uint8_t result;
uint16_t data;

// Read holding register 0x0001
result = node.readHoldingRegisters(0x0001, 1);

if (result == node.ku8MBSuccess) {
data = node.getResponseBuffer(0x00);
Serial.print("Register value: ");
Serial.println(data);
} else {
Serial.println("Communication error");
}

delay(1000);

  }
  
}