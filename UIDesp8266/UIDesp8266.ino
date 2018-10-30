#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <string.h>

constexpr uint8_t RST_PIN = D3;     
constexpr uint8_t SS_PIN = D4;   

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;

const char *ssid = "*********";  //ENTER YOUR WIFI SETTINGS
const char *password = "*******";
const char *host = "192.168.0.109";  

String content= "";

void ConnectToWiFi(void);
void ReadRFID(void);

void setup(){
  Serial.begin(115200);   // Initiate a serial communication
  ConnectToWiFi();
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
}

void loop(){

  String Link="";
  
  HTTPClient http;    //Declare object of class HTTPClient
  content= "";
  ReadRFID();
  Link = "http://192.168.0.109:8080/add_voltagevalues?rfidvalues=";
  Link += String(content.substring(0));
  Serial.println(Link);

  http.begin(Link);              //Specify request destination
  int httpCode = http.GET();   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  //Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
  delay(1000);  //Post Data at every 3 seconds
} 

void ConnectToWiFi(void){
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void ReadRFID(void){
// Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return ;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return ;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
}


