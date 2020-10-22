
/*
This code has used parts from the following:

1. DFPlayer - A Mini MP3 Player For Arduino
   <https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299>
   This example shows the basic function of library for DFPlayer.
   Created 2016-12-07 By [Angelo qiao](Angelo.qiao@dfrobot.com)
   GNU Lesser General Public License.
   
2. Calibration - Demonstrates one technique for calibrating sensor input.
   <http://www.arduino.cc/en/Tutorial/Calibration>
   created 29 Oct 2008 by David A Mellis modified 30 Aug 2011 by Tom Igoe.
   This example code is in the public domain.

*/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

const int sensorPin = A0; // photoresistor input
const int speaker = 11; // speaker output
int sensorValue = 0;  // declare sensoValue 
int sensorMin = 1023; // minimum sensor value
int sensorMax = 0; // maximum sensor value
int mapMin = 50; // min mapping
int mapMax = 1000; // max mapping

//Initialize serial for pins 10 & 11 for mp3 player
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  mySoftwareSerial.begin(9600); // Initialize mp3 module serial
  Serial.begin(115200); // Initialize Arduino serial

  // ***** Start Sensor Calibration *****
  pinMode(13, OUTPUT);  // Assign LED for calibration indicator
  digitalWrite(13, HIGH);  // turn on LED to signal the start of the calibration period
  
  // calibrate during the first ten seconds
  while (millis() < 10000) {
    sensorValue = analogRead(sensorPin);
    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }
    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }
  
  digitalWrite(13, LOW); // Turn off LED to signal the end of the calibration period
  // ***** Sensor Calibration finished *****
  
  // ***** Start mp3 module check  *****
  //Use softwareSerial to communicate with mp3 player.
  if (!myDFPlayer.begin(mySoftwareSerial)) {  
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  // ***** mp3 module check finished  ****
  
  // Initial parametres and start playback
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(15); // Set volume at value 10 (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); // EQ normal
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD); // We use SD as default
  // myDFPlayer.loop(1);  //Loop the first mp3
  myDFPlayer.enableLoopAll(); //loop all mp3 files.
  delay(10);

  // ****** Read mp3 player info ******
  Serial.println("DFPlayer state: ");
  Serial.println(myDFPlayer.readState()); //read mp3 state
  Serial.println("DFPlayer volume: ");
  Serial.println(myDFPlayer.readVolume()); //read current volume
  Serial.println("DFPlayer EQ: ");
  Serial.println(myDFPlayer.readEQ()); //read EQ setting
  Serial.println("DFPlayer files in card: ");
  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.println("DFPlayer current play file number: ");
  Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
 
}

void loop() {
  
  sensorValue = analogRead(sensorPin); // read sensor value
  // apply the calibration to the sensor reading
  sensorValue = map(sensorValue, sensorMin, sensorMax, mapMin, mapMax);
  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, mapMin, mapMax);


  if (sensorValue > mapMin + 70) {
    myDFPlayer.pause();  // pause playback if something blocks the light source
    Serial.print(F("sensorValue:"));
    Serial.print(sensorValue);
    Serial.println(F(" Play paused!"));
  } 
   else {
    myDFPlayer.start();  // resume playback if the light source is unblocked
    Serial.print(F("sensorValue:"));
    Serial.print(sensorValue);
    Serial.println(F(" Play begin!"));
  }
  
  // print the results to the serial monitor:
  Serial.print("sensor = ");
  Serial.println(sensorValue);

  delay(500); // delay 500  milliseconds for next sensor value
}
