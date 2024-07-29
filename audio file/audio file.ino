#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = D2; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = D3; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

//const int wirePin = 4; 
//const int endPin = 5; 

// Create the Player object
DFRobotDFPlayerMini player;

void setup() {

//pinMode(wirePin, INPUT);
//pinMode(endPin, INPUT);

  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Play the first MP3 file on the SD card
    player.play(3);
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {

 
   }
