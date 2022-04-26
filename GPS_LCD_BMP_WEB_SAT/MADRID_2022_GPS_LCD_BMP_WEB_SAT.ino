/*
  Modified from https://github.com/mikalhart/TinyGPSPlus
*/
//libraries and variables for web interface
#include <WiFi.h>   //wifi library
#include <HTTPClient.h> //http client
const char* ssid       = "YOUR_SSID";
const char* password   = "YOUR_PASSWORD";
const char* serverName = "YOUR_SERVER_WRITE_FILE"; // for example https://yourserver.com/iot/iotwrite.php

//libraries and global variables for BMP280
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // I2C Interface
String strTemp, strPressure;

//remember to insert the daily air pressure here
float setPressure = 1035.76;

// libraries and global variables for LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.
int lcdColumns = 20;
int lcdRows = 4;
String strLon = "not available";
String strLat = "not available";
String strAlt = "not available";
String numSats = "0";

// libraries and global variables for GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

void setup()
{
  Serial.begin(9600);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  lcd.print("LCD started.");
  delay(3000);
  lcd.clear();

  WiFi.begin(ssid, password);

  lcd.setCursor(0, 0);            // Start at top-left corner
  lcd.print("WLAN Connecting:");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);            // Start at top-left corner
    lcd.print("Connected:");
    lcd.setCursor(0, 1);            // Start at top-left corner
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();
  }

  //start BMP280 setup
  lcd.print("Booting BMP280.");

  if (!bmp.begin(0x76)) {
    lcd.print("BMP280 not found.");
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  delay(3000);
  lcd.clear();
  lcd.print("BMP280 started.");
  delay(3000);
  lcd.clear();

  //start GPS
  ss.begin(GPSBaud);
  lcd.clear();
  lcd.print("Booting GPS.");
  delay(5000);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) { //this if is needed for the GPS serial data to flow
      displayInfo();
      strLat = String(gps.location.lat(), 6);
      strLon = String(gps.location.lng(), 6);
      strAlt = String(gps.altitude.meters(), 2);
      numSats = String(gps.satellites.value());
      getTempPressure();
      write_LCD();
      UpdateWeb();
      delay(10000);
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}

void UpdateWeb() {
  //sending data to the server:

  Serial.println("add data to web");
  Serial.printf("Connecting to %s ", ssid);
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    // If you need an HTTP request with a content type: text/plain
    http.addHeader("Content-Type", "text/plain");
    String httpRequestData = strLat + "," + strLon + "," + strTemp + "," + strPressure + "," + strAlt + "," + numSats;
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println("HTTP Response code: ");
    Serial.println(httpRequestData);

    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void getTempPressure() {
  Serial.println();
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  strTemp = bmp.readTemperature();
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure() / 100); //displaying the Pressure in hPa, you can change the unit
  strPressure = bmp.readPressure() / 100;
  Serial.println(" hPa");

  //  Serial.print(F("Approx altitude = "));
  //  Serial.print(bmp.readAltitude(setPressure)); //The "1019.66" is the pressure(hPa) at sea level in day in your region
  //  strAlt = bmp.readAltitude(setPressure);
  //  Serial.println(" m");                    //If you don't know it, modify it until you get your current altitude
  //  Serial.println();
}

void write_LCD() {
  Serial.println("Write LCD");
  // insert code for writing GPS and air data to the LCD display in this function
  // set cursor to first column, first row
  // clears the display to print new message
  lcd.clear();

  // print message
  lcd.setCursor(0, 0);
  lcd.print("Lat:");
  lcd.setCursor(5, 0);
  lcd.print(strLat + " Sat:");
  lcd.setCursor(0, 1);
  lcd.print("Lon:");
  lcd.setCursor(5, 1);
  lcd.print(strLon + " " + numSats);
  lcd.setCursor(0, 2);
  lcd.print("Temp: Press:  Alt:");
  lcd.setCursor(0, 3);
  lcd.print(strTemp);
  lcd.setCursor(6, 3);
  lcd.print(strPressure);
  lcd.setCursor(14, 3);
  lcd.print(strAlt);
  delay(10);

}


void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);

  }
  else
  {
    Serial.println(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

}
