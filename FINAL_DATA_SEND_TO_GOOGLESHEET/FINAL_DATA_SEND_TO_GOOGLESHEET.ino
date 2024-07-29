
#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

static const uint8_t PIN_MP3_TX = D2; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = D3; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

//-----------------------------------------------------------
const String web_app_url = "https://script.google.com/macros/s/AKfycbznoaGUds5arMOB-Aw3XYSKAZOcFb9fTk1_g-NQxrHXbXuHpkLVeWrgT3VG8qYjIiATqA/exec";
//-----------------------------------------------------------
#define WIFI_SSID "Aniket"
#define WIFI_PASSWORD "12345678"
//-----------------------------------------------------------
int blocks[] = {4,5,6};
#define blocks_len  (sizeof(blocks) / sizeof(blocks[0]))
//-----------------------------------------------------------
//GPIO 0 --> D3
//GPIO 2 --> D4
//GPIO 4 --> D2
#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D0
String x ;


//-----------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status; 

//-----------------------------------------
/* Be aware of Sector Trailer Blocks */
int blockNum = 2; 
byte block_data[16]; 
/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
//-----------------------------------------
// Fingerprint for demo URL, expires on ‎Monday, ‎May ‎2, ‎2022 7:20:58 AM, needs to be updated well before this date
//const uint8_t fingerprint[20] = {0xD9, 0x13, 0xA9, 0x50, 0x67, 0xDE, 0xAF, 0x83, 0x0D, 0xCE, 0xB5, 0x0C, 0x19, 0xA7, 0xC0, 0xD3, 0x14, 0xE6, 0xC7, 0x5E};
//9a 87 9b 82 e9 19 7e 63 8a db 67 ed a7 09 d9 2f 30 de e7 3c





DFRobotDFPlayerMini player;
/****************************************************************************************************
 * setup() function
 ****************************************************************************************************/
void setup()
{
  //--------------------------------------------------
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  //Serial.setDebugOutput(true);
  //--------------------------------------------------
  //WiFi Connectivity
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(BUZZER, OUTPUT);
  SPI.begin();
  //--------------------------------------------------
}
      





/****************************************************************************************************
 * loop() function
 ****************************************************************************************************/
 void loop()
{
  //------------------------------------------------------------------------
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
  /* Read data from the same block */
  Serial.println();
  Serial.println(F("Reading last data from RFID..."));  
  //------------------------------------------------------------------------
  String fullURL = "", temp;
  for (byte i = 0; i < blocks_len; i++) {
    ReadDataFromBlock(blocks[i], readBlockData);
    if(i == 0){
      temp = String((char*)readBlockData);
      Serial.print("Temp = "); Serial.println(temp);
      temp.trim();
      x = temp;
      fullURL = "data" + String(i) + "=" + temp;
    }
    else{
      temp = String((char*)readBlockData);
      temp.trim();
      fullURL += "&data" + String(i) + "=" + temp;
    }
  }

  //Serial.println(fullURL);
  fullURL.trim();
  fullURL = web_app_url + "?" + fullURL;
  fullURL.trim();
  Serial.println(fullURL);
  //------------------------------------------------------------------------
  if(x == "1150"){
        Serial.println("This is first card");
        Serial.println(x);
        if (player.begin(softwareSerial)) {
           Serial.println("OK");

           // Set volume to maximum (0 to 30).
             player.volume(30);
            // Play the first MP3 file on the SD card
             player.play(1);
          } 
          else {
            Serial.println("Connecting to DFPlayer Mini failed!");
           }
      }

   if(x == "1650"){
        Serial.println("This is second card");
        Serial.println(x);
        if (player.begin(softwareSerial)) {
           Serial.println("OK");

           // Set volume to maximum (0 to 30).
             player.volume(30);
            // Play the first MP3 file on the SD card
             player.play(2);
          } 
          else {
            Serial.println("Connecting to DFPlayer Mini failed!");
           }
      }    

  
  
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(3000);
  //------------------------------------------------------------------------

  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if (WiFi.status() == WL_CONNECTED) {
    //-------------------------------------------------------------------------------
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    //-------------------------------------------------------------------------------
    //uncomment following line, if you want to use the SSL certificate
    // client->setFingerprint(fingerprint);
    //or uncomment following line, if you want to ignore the SSL certificate
    client->setInsecure();
    //-------------------------------------------------------------------------------
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));
    //-------------------------------------------------------------------------------

    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    if (https.begin(*client, (String)fullURL)){
      //-----------------------------------------------------------------
      // HTTP
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
      //-----------------------------------------------------------------
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
      }
      //-----------------------------------------------------------------
      else 
      {Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());}
      //-----------------------------------------------------------------
      https.end();
      delay(1000);
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else {
      Serial.printf("[HTTPS} Unable to connect\n");
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}




/****************************************************************************************************
 * ReadDataFromBlock() function
 ****************************************************************************************************/
void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 
  //----------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //----------------------------------------------------------------------------
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  //----------------------------------------------------------------------------s
  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  //----------------------------------------------------------------------------
  else {
    Serial.println("Authentication success");
  }
  //----------------------------------------------------------------------------
  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    readBlockData[16] = ' ';
    readBlockData[17] = ' ';
    Serial.println("Block was read successfully");  
  }
  //----------------------------------------------------------------------------
}
