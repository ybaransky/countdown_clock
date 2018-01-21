/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.

#define SDA D3
#define SCL D4

/* Set these to your desired credentials. */
const char *ssid = "YurijCountdown";
const char *password = "thereisnospoon";
int start_time[] = {60, 12, 30, 30, 0}; // dd,hh,mm,ss,uu

std::unique_ptr<ESP8266WebServer> server; 
extern  void  handleConfig(void);
extern  void  handleConfigSave(void);

#include "countdown_clock.h"
CountdownClock  gCountdown;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server->send(200, "text/html", "<h1>You are connected to the countdown</h1>");
}

void setup() {
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Count down clock");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.reset(new ESP8266WebServer(80));
  server->on("/",     handleConfig);
  server->on("/save", handleConfigSave);
  server->begin();
  Serial.println("HTTP server started");

  gCountdown.set_time(start_time);
  gCountdown.begin();
  gCountdown.print();
  gCountdown.display();
}

void loop() {
  static uint32  prevtime = millis();
  static uint16_t  tick = 100;
  static bool display = true;

  server->handleClient();

  delay(tick);
  if (gCountdown.decrement(tick))
    gCountdown.display();
  else {
    if (millis() - prevtime > 500) {
      gCountdown.done_message(display);
      prevtime = millis();
      display  = !display;
    }
  }
}
