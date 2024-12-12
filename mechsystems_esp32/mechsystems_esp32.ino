#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Esp32test"
#define WIFI_PASSWORD "yess1234"
#define API_KEY "AIzaSyAi7m5RWfiDAvIHr3786ryhX-UHvTIXEPY"
#define DATABASE_URL "https://mechasystem-7394a-default-rtdb.europe-west1.firebasedatabase.app/"

#define LDR_PIN 36

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int ldrData =0;
float voltage =0.0;

void setup() {
Serial.begin(115200);
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

  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis ==0)){
    sendDataPrevMillis= millis();
    ldrData = analogRead(LDR_PIN);
    ldrData = random(200);
    voltage = (float)analogReadMilliVolts(LDR_PIN)/1000;
    voltage = random(200);
    
    if(Firebase.RTDB.setInt(&fbdo, "Sensor/ldr_data", ldrData)){
      Serial.println();
      Serial.print(ldrData);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ") ");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
    if(Firebase.RTDB.setFloat(&fbdo, "Sensor/Voltage", voltage)){
      Serial.println();
      Serial.print(voltage);
      voltage = random(200);
      Serial.print("- succesfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
  }
  
}