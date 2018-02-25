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
bool              gTestMode = false; // set via HTML

/****************************************************************************/

void  setUserHandlers() {
  extern  void  handleConfigClock(void);
  extern  void  handleConfigSave(void);
  extern  void  handleConfigView(void);
  extern  void  handleConfigDelete(void);
  extern  void  handleConfigReboot(void);

  server->on("/",       handleConfigClock);
  server->on("/clock",  handleConfigClock);
  server->on("/save",   handleConfigSave);
  server->on("/view",   handleConfigView);
  server->on("/delete", handleConfigDelete);
  server->on("/reboot", handleConfigReboot);
}

void  doTestMode(uint32_t currtime) {
  static  bool      inTestMode = false;
  static  uint32_t  starttime;
  static  int       save_time[6];

  if (!inTestMode) {
    int new_time[] = {0,5,0,0,0}; // uu=0,ss,mm,hh,dd 
    gDisplay.clear();
    gClock.get_time( save_time );
    gClock.set_time( new_time );
    starttime = currtime;
    inTestMode = true;
  }
  // go back to original mode, after 10 seconds
  if (inTestMode && (currtime - starttime > 10000)) {
    gClock.set_time( save_time );
    gClock.tick( 10000 );
    gDisplay.clear();
    inTestMode = false;
    gTestMode = false;
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
    gClock.tick(delta * gConfig._direction);
    if (gClock.done()) {
      gDisplay.displayBlinkMessage();
    } else {
      int time[6];
      gClock.get_time(time);
      gDisplay.displayTime(time);
    }
    prevtime = currtime;
  } 
  return gClock.done();
}

void  reboot(void) {
//  delay(2000);
  WiFi.forceSleepBegin();
  ESP.restart();
  //digitalWrite(D0, LOW);
}

//////////////////////////////////////////////////////////////////////////

void  setup() {
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Count down clock");

  // we we can do a soft reboot
  pinMode(D0, OUTPUT);
  digitalWrite(D0,HIGH);

  Serial.println("Loading config file");
  gConfig.loadFile();

  gClock.set_time(gConfig._time);

  gDisplay.begin();
  gDisplay.set_brightness();
  gDisplay.set_message(gConfig._msg_start);
  gDisplay.displayMessage();
  delay(2*1000);
  gDisplay.set_message(gConfig._msg_end);
  gDisplay.clear();

  /*
  for(int i=32;i<32+96;i++) {
    char buf[8];
    sprintf(buf,"%c",i);
    String  str(buf);
    gDisplay.set_message(buf);
    gDisplay.displayMessage();
    delay(1000);
  }
  */

  /*
  * start the WiFi Manager
  */
  Serial.println("about to Wifi");
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
