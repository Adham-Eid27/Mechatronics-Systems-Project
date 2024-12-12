#include <wifi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDPHelper.h"

#define WIFI_SSID "AASTGUEST"
#define WIFI_PASSWORD ""
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
serial.begin(115200);
wifi.begin(WIFI_SSID, WIFI_PASSWORD);
serial.print ("CONNECTING  TO WI-FI");
serial.println(wifi.localIP());
serial.println();

config .api_key=API_KEY;
config.database_url=DATABASE_URL;
if(Firebase.signUp(&config,&auth, "", ""))
}
void loop() {
  
}