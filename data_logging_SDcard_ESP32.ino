/* 
This code logs the UART and voltage data from a ZE15-CO sensor using the ESP32 version of the SD library.
Tested on a Xiao ESP32 S3
*/

#include "FS.h"
#include <SPI.h>     //for the SD card module
#include <SD.h>      // for the SD card
#include <RTClib.h>  // for the RTC
#include <ZE15_CO.h>
#include <SoftwareSerial.h>
//Software serial pins used for board in use.
#define Softserial_RX_pin D7
#define Softserial_TX_Pin D6

//Create a Software Serial Port.
SoftwareSerial COSerial(Softserial_RX_pin, Softserial_TX_Pin);

//Refer the Software Serial port to the ZE15_CO library.
ZE15_CO ZE15_CO(&COSerial);

float concentration = 0;  //CO concentration value in ppm.
bool read_flag = false;   //Toggles when UART data is received
bool last_read_flag=true;
const int chipSelect = 4;

// Create a file to store the data
File myFile;
// an object to store to clock data
DateTime now;
// RTC
RTC_DS3231 rtc;
// file name
int voltageVal = 0;
#define REASSIGN_PINS
int sck = 7;
int miso = 8;
int mosi = 9;
int cs = 4;


void writeFile(fs::FS &fs, const char *path, const char *message) {
  //Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    //Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    //Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  COSerial.begin(9600);
  delay(1000);
  //LED pin to be used for indicating UART data reception.
  pinMode(LED_BUILTIN, OUTPUT);
  // setup for the RTC
  while (!Serial)
    ;
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  } /*else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }*/


#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
#else
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  writeFile(SD, "/experiment_2.txt", "Date,Time,Analog,UART,\n");
  delay(1000);
  //Serial.println("Setup done");
}



void loop() {
  now = rtc.now();
  int sec = now.second();  // since I use this value alot
  if (sec == 0 || sec == 30) {
    String logTime = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + "," + String(now.hour()) + ":" + String(now.minute()) + ":" + String(sec) + ",";
    appendFile(SD, "/experiment_2.txt", logTime.c_str());
    logVoltage();
    logUART();
Serial.println(logTime+String(voltageVal));

  } else if (sec == 15 || sec == 45) {
    // One ping sent every 0, 15th, 30th, and 45th second to prevent the sensor automatically entering the initiative
    // upload mode
    ZE15_CO.Request_CO_data();
    delay(100);  // Allow some time for the sensor to respond
    ZE15_CO.update_data(concentration, read_flag);
    Serial.print("Read flag: ");
    Serial.println(read_flag);
    Serial.print("CO concentration: ");
    Serial.println(concentration);
  }
  delay(1000);
}

void logVoltage() {
  voltageVal = analogRead(A0);
  if (isnan(voltageVal)) {
    Serial.println("Failed to read from gas sensor!");
    return;
  }
  String logVolt = String(voltageVal) + ",";
  appendFile(SD, "/experiment_2.txt", logVolt.c_str());
}
void logUART() {
  ZE15_CO.Request_CO_data();
  delay(100);  // Allow some time for the sensor to respond
  while (COSerial.available() > 0) {
    ZE15_CO.update_data(concentration, read_flag);
  }
  String strConc = String(concentration) + String(",\n");
  appendFile(SD, "/experiment_2.txt", strConc.c_str());
  read_flag = false; // Reset read flag after processing
}
void Show_dataready()
{
 if(read_flag) 
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else 
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }  
}
