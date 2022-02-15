/**************************************************************************

   WiFi Internet clock (NTP) with ESP8266 NodeMCU (ESP-12E) board and
     ST7735 TFT display.
   This is a free software with NO WARRANTY.
   https://simple-circuit.com/

 *************************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>        // include NTPClient library
#include <TimeLib.h>          // Include Arduino time library
#include <Adafruit_GFX.h>     // include Adafruit graphics library
#include <Adafruit_ST7735.h>  // include Adafruit ST7735 TFT library

//---------- wifi ----------
#define HOSTPREFIX "CGM-"   //18 chars max
#include "ESP8266WiFi.h"    //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>
char macBuffer[24];         //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];          //Holds hostNamePrefix + the last three bytes of the MAC address.


//---------- TFT module connections ----------
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D3 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D2 (GPIO4)
// initialize ST7735 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// set WiFi network SSID and password
///const char *ssid     = "kaywinnet";
///const char *password = "806194edb8";

WiFiUDP ntpUDP;

// 'time.nist.gov' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval
NTPClient timeClient(ntpUDP, "time.nist.gov", 3600, 60000);
unsigned long unix_epoch;

void setup(void)
{
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);     // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);  // fill screen with black color
  tft.drawFastHLine(0, 44,  tft.width(), ST7735_BLUE);    // draw horizontal blue line at position (0, 44)
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setTextSize(1);                 // text size = 1
  tft.setCursor(19, 10);              // move cursor to position (43, 10) pixel
  tft.print("ESP8266 NodeMCU");
  tft.setCursor(4, 27);               // move cursor to position (4, 27) pixel
  tft.print("Wi-Fi Internet Clock");
  Serial.println(F("Wi-Fi Internet Clock"));


  setup_wifi();
  /*WiFi.begin(ssid, password);
    Serial.print("Connecting.");
    tft.setCursor(0, 54);              // move cursor to position (4, 27) pixel
    tft.print("Connecting..");
    while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
    }*/
  Serial.println("connected");
  tft.print("connected");
  delay(2000);

  tft.fillRect(0, 54, tft.width(), 8, ST7735_BLACK);

  tft.drawFastHLine(0, 102, tft.width(), ST7735_BLUE);  // draw horizontal blue line at position (0, 102)
  tft.setTextSize(2);                                   // text size = 2
  tft.setTextColor(ST7735_MAGENTA, ST7735_BLACK);       // set text color to magenta and black background
  tft.setCursor(37, 112);                               // move cursor to position (37, 112) pixel
  tft.print("TIME:");
  Serial.println(F("TIME: "));

  timeClient.begin();
}

void RTC_display()
{
  char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
  byte x_pos[7] = {29, 29, 23, 11, 17, 29, 17};
  static byte previous_dow = 0;
  // print day of the week

  if ( previous_dow != weekday(unix_epoch) ) {
    previous_dow = weekday(unix_epoch);
    tft.fillRect(11, 55, 108, 14, ST7735_BLACK);     // draw rectangle (erase day from the display)
    tft.setCursor(x_pos[previous_dow - 1], 55);
    tft.setTextColor(ST7735_CYAN, ST7735_BLACK);     // set text color to cyan and black background
    tft.print( dow_matrix[previous_dow - 1] );
    ///  Serial.println( dow_matrix[previous_dow - 1] );
  }


  // print date
  tft.setCursor(4, 79);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);     // set text color to yellow and black background
  tft.printf( "%02u-%02u-%04u", day(unix_epoch), month(unix_epoch), year(unix_epoch) );
  Serial.printf( "%02u-%02u-%04u ", day(unix_epoch), month(unix_epoch), year(unix_epoch) );

  // print time
  tft.setCursor(16, 136);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);     // set text color to green and black background
  tft.printf( "%02u:%02u:%02u", hour(unix_epoch), minute(unix_epoch), second(unix_epoch) );
  Serial.printf( "%02u:%02u:%02u\n", hour(unix_epoch), minute(unix_epoch), second(unix_epoch) );
}

// main loop
void loop()
{
  timeClient.update();
  unix_epoch = timeClient.getEpochTime();   // get UNIX Epoch time

  RTC_display();
  delay(1000);    // wait 200ms

}

// end of code.
