// Original file from 1nformatica "https://www.youtube.com/watch?v=oTkA9l9l8mw&lc=" (M5Stack Data Logger Voltage Graph Tutorial ESP32)
// Other parts learned from Random Nerd Tutorials

#include <M5Stack.h>
#include <SPI.h>
#include <Wire.h>

// Libraries for SD card
#include "FS.h"
#include "SD.h"

// Libraries to get time from NTP Server
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
// Library for Dallas 18B20 tem sensors
#include <DallasTemperature.h>

// Data wire is connected to GPIO17
#define ONE_WIRE_BUS 17
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

//You can add more sensors over here
DeviceAddress sensor1 = { 0x28, 0xAA, 0xE4, 0x1, 0x58, 0x14, 0x1, 0x10 };
DeviceAddress sensor2 = { 0x28, 0xAA, 0x5E, 0x2, 0x53, 0x14, 0x1, 0xAB };

// Replace with your network credentials
const char* ssid     = "Your SSID";
const char* password = "Password";

// Define CS pin for the SD card module
const int chipSelect = 4;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String dataMessage;
//int Voltage; 


void setup(){
   Serial.begin(115200);
   sensors.begin();
   M5.begin();
   
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
 // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7200);

  Serial.print("Initializing SD card...");
 
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

   // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data_temp.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data_temp.txt", "Date, Time, Temp1, Temp2 \r\n"); // If you use more sensors put them in here to log
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  
   M5.Lcd.begin();
   M5.Lcd.setCursor(10, 10);
   M5.Lcd.setTextColor(GREEN);
   M5.Lcd.setTextSize(3);
   M5.Lcd.printf("Temp Logger");
   M5.Lcd.setCursor(10, 40);
   M5.Lcd.printf("Temp 1 = ");
   M5.Lcd.setCursor(10, 70);
   M5.Lcd.printf("Temp 2 = ");
   // Add more sensors on the screen
   M5.Lcd.setCursor(10, 100);
   M5.Lcd.printf("Temp 3 = ");
   //M5.Lcd.setCursor(10, 130);
   //M5.Lcd.printf("Temp 4 = ");
 }

 
void loop(){
  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("Sensor 1(*C): ");
  Serial.println(sensors.getTempC(sensor1));
  Serial.print("Sensor 2(*C): ");
  Serial.println(sensors.getTempC(sensor2));
  Serial.println()
  ; 
  // If you use more sensors
  //Serial.print("Sensor 3(*C): ");
  //Serial.println(sensors.getTempC(sensor2));
  //Serial.println();
     
   M5.Lcd.setCursor(170, 40);
   M5.Lcd.setTextColor(GREEN, BLACK);
   M5.Lcd.print(sensors.getTempC(sensor1));
   M5.Lcd.setCursor(170, 70);
   M5.Lcd.print(sensors.getTempC(sensor2));
   // If you use more sensors
   //M5.Lcd.setCursor(170, 100);
   //M5.Lcd.print(sensors.getTempC(sensor3));
   
   getTimeStamp();
   dataMessage = String(dayStamp) + "," + String(timeStamp) + 
                         "," + String(sensors.getTempC(sensor1)) + "," + String(sensors.getTempC(sensor2)) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data_temp.txt", dataMessage.c_str());

   delay(5000);                    //Take samples every 5 seconds
   }

// Function to get date and time from NTPClient
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  //Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
 // Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  //Serial.println(timeStamp);
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
