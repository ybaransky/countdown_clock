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

#include "countdown_config.h"
#include "wifi_manager.h"
#include "countdown_display.h"
#include "countdown_clock.h"

/****************************************************************************/

#define SDA D3
#define SCL D4

std::unique_ptr<ESP8266WebServer> server; 
WiFiManager       gWifiManager;
CountdownClock    gClock;
CountdownDisplay  gDisplay;
Config            gConfig;
bool              gTestMode = false;;

extern  void  handleConfigClock(void);
extern  void  handleConfigHardware(void);
extern  void  handleConfigSave(void);

/****************************************************************************/

void  setUserHandlers() {
  server->on("/",         handleConfigClock);
  server->on("/clock",    handleConfigClock);
  server->on("/hardware", handleConfigHardware);
  server->on("/save",     handleConfigSave);
}

void  doTestMode(uint32_t currtime) {
  static  bool      firstTime = true;
  static  uint32_t  starttime;
  static  int       save_time[6];

  if (firstTime) {
    firstTime = false;
    int new_time[] = {0,0,0,5,0};
    gClock.get_time( save_time );
    gClock.set_time( new_time );
    starttime = currtime;
  }
  // go back to original mode
  if (currtime - starttime > 10000) {
    firstTime = true;
    gTestMode = false;
    gClock.set_time( save_time );
    gClock.tick( 10000 );
    gDisplay.clear();
  }
}
 
void  doCheckpoint(uint32_t currtime) {
  static  uint32_t  checktime = millis();
  if ((currtime - checktime > 60000) && gConfig._periodic_save) {
    checktime = currtime;
    gClock.get_time( gConfig._time );
    gConfig.saveFile();
  }
}

bool  doCountdown(uint32_t currtime) {
  static  uint32_t  prevtime = millis();
  static  bool      done=false;

  if (currtime - prevtime > 100) {
    uint32_t delta = currtime - prevtime;
    if (!gClock.done()) {
      int time[6];
      gClock.tick(delta * gConfig._direction);
      gClock.get_time(time);
      gDisplay.displayTime(time);
    } else {
      gDisplay.displayBlinkMessage(currtime);
    }
    prevtime = currtime;
  } 
  return gClock.done();
}

//////////////////////////////////////////////////////////////////////////

void  setup() {
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Count down clock");

  Serial.println("Loading config file");
  gConfig.loadFile();
  gConfig.print("after loading");

  gConfig._periodic_save = false;

  gClock.set_time(gConfig._time);
  gClock.print("initial");

  gDisplay.set_message(gConfig._msg_end);
  gDisplay.begin();
  Serial.println("about to Wifi");

/*
  gDisplay.set_message(gConfig._msg_start);

  gCountdown.displayMessage();
  delay(4000);
  gCountdown.set_message(gConfig._msg_end);
  gCountdown.clear();
*/
  /*
  * start the WiFi Manager
  */
//  WiFi.disconnect();  // this should erase credentials, causes some issue!!!
  Serial.println("after disconnect");
  gWifiManager.setUserHandlers( setUserHandlers ); 
  Serial.println("after install hadlers");
  if (!gWifiManager.autoConnect(gConfig._ap_name, gConfig._ap_password)) {
    Serial.println("failed to connect and hit timeout");
    delay(1000);
    ESP.reset();
    delay(1000);
  }
  Serial.printf("connected to %s\n",WiFi.SSID().c_str());
  Serial.print("IP Addr: "); Serial.println(WiFi.localIP());
}

void loop() {
  uint32_t currtime = millis();

  server->handleClient();

  // set clock time to 5 seconds and then after 10seconds, revert to original
  if (gTestMode) 
    doTestMode(currtime);

  // this is the full logic of the clock
  bool done = doCountdown(currtime);

  // checkpoint whenever its possible
  if (!gTestMode && !done)
    doCheckpoint(currtime);
}
