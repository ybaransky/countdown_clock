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
#include "countdown_clock.h"
#include "countdown_config.h"

/****************************************************************************/

#define SDA D3
#define SCL D4

CountdownClock  gCountdown;
Config          gConfig;
bool            gTestMode = false;;

std::unique_ptr<ESP8266WebServer> server; 
extern  void  handleConfig(void);
extern  void  handleConfigSave(void);

/****************************************************************************/

void handleRoot() {
  server->send(200, "text/html", "<h1>You are connected to the countdown</h1>");
}

void setup() {
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Count down clock");

  Serial.println("Loading config file");
  gConfig.loadFile();

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(gConfig._ap_name, gConfig._ap_password);

  Serial.print("AP IP address: "); Serial.println(WiFi.softAPIP());
  Serial.print("AP Name: "); Serial.println(gConfig._ap_name);
  Serial.print("AP Password: "); Serial.println(gConfig._ap_password);
  
  server.reset(new ESP8266WebServer(80));
  server->on("/",     handleConfig);
  server->on("/save", handleConfigSave);
  server->begin();
  Serial.println("HTTP server started");

  gCountdown.begin();
  gCountdown.set_time(gConfig._duration);
  gCountdown.set_message(gConfig._msg_start);
  gCountdown.displayMessage();

  delay(5000);
  gCountdown.set_message(gConfig._msg_end);
  gCountdown.clear();
}

void  doTestMode(uint32_t currtime) {
  static  bool      firstTime = true;
  static  uint32_t  starttime;
  static  int       save_time[6];

  if (firstTime) {
    firstTime = false;
    int new_time[] = {0,0,0,5,0};
    gCountdown.get_time( save_time );
    gCountdown.set_time( new_time );
    starttime = currtime;
  }
  // go back to original mode
  if (currtime - starttime > 10000) {
    firstTime = true;
    gTestMode = false;
    gCountdown.set_time( save_time );
    gCountdown.decrement( 10000 );
    gCountdown.clear();
  }
}
 
void  doCheckpoint(uint32_t currtime) {
  static  uint32_t  savetime = millis();
  if (currtime - savetime < 60000) 
    return;
  Serial.printf("%d) saving\n", int(currtime/1000));
  gCountdown.get_time( gConfig._duration );
  gConfig.saveFile();
  savetime = currtime;
}

void loop() {
  static  bool      display = true;
  static  bool      done = false;
  static  uint16_t  tick = 100;
  static  uint32_t  prevtime = millis();
  uint32_t          currtime = millis();

  server->handleClient();

  // set time to 5 seconds and then after 10, revert to original
  if (gTestMode) 
    doTestMode(currtime);

  // this is the full logic of the clock
  delay(tick);
  if (gCountdown.decrement(tick))
    gCountdown.displayClock();
  else {
    done = true;
    if (currtime - prevtime > 500) {
      if (display)
        gCountdown.displayMessage();
      else
        gCountdown.clear();
      prevtime = currtime;
      display  = !display;
    }
  }

  if (gConfig._periodic_save && !gTestMode && !done)
    doCheckpoint(currtime);
}
