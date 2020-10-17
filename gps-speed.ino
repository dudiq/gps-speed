#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

static const int RXPin = 4, TXPin = 3;

// GPS RX -> D3
// GPS TX -> D4

// SCREEN SDA - A4
// SCREEN SCL - A5


static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// For stats that happen every 5 seconds
unsigned long last = 0UL;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

int fontWidth = 0;

unsigned int DEFAULT_TIME = 200;

int cnt = 0;
int dx = 10;
unsigned oldSpeedInt = 0;

void updateSpeed() {
    oled.set2X();
    oled.setCursor(0, 0);
    oled.print("    ");
    //oled.clearToEOL();

    cnt = cnt + dx;
    if (cnt > 180 + dx) dx = -10;
    if (cnt < -dx) dx = 10;

    int speedInt = int(gps.speed.kmph());
    if (speedInt == 0 || speedInt == oldSpeedInt) return;
    oldSpeedInt = speedInt;
    String speed = String(speedInt);
    int len = speed.length();
    int newX = (5 * fontWidth - (len * fontWidth));

    oled.setCursor(newX, 0);
    oled.print(speed);

//    oled.set1X();
//    oled.setCursor(50, 1);   
//    oled.print("km/h");
}

void updateSatellites() {
    if (!gps.satellites.isUpdated()) return;
    oled.set1X();
    oled.setCursor(1, 3);
    oled.clearToEOL();
    oled.print("sat: " + String(gps.satellites.value()));
}

void setup() {
    delay(4000);
    
    Serial.begin(115200);
    ss.begin(GPSBaud);
    Wire.begin();
    Wire.setClock(400000L);

#if RST_PIN >= 0
    oled.begin(&Adafruit128x32, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
    oled.begin(&Adafruit128x32, I2C_ADDRESS);
#endif // RST_PIN >= 0
    oled.setFont(font5x7);
    fontWidth = oled.fontWidth();

    oled.setCursor(20, 1);
    oled.print("GPS speedometer");
    delay(4000);
    oled.set2X();
    oled.clear();

    oled.set1X();
    oled.setCursor(50, 1);
    oled.print("km/h");
}

void loop() {

    // Dispatch incoming characters

    while (ss.available() > 0)
        gps.encode(ss.read());

    updateSpeed();
    updateSatellites();
    delay(DEFAULT_TIME);
}
