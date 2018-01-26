#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "countdown_clock.h"
#include "countdown_config.h"

extern  std::unique_ptr<ESP8266WebServer> server; 
extern  CountdownClock  gCountdown;
extern  Config          gConfig;
static const String     NL("\r\n");
static const String     EMPTY("");

static const char STYLE_BUTTON[] PROGMEM = "button {border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:40%;}" ;

static 
String configPageRow(const char* desc, const String& value) {
  String row("");
  row += "<tr>";
  //row += "<td style='text-align:right; font-weight:bold'>" + String(name) + "</td><td style='text-align:left'>" + value + "</td>";
  row += "<td class='right bold'>" + String(desc) + "</td><td class='left grey'>" + value + "</td>";
  row += "</tr>" + NL;
  return row;
}

static 
String configPageInputRow(const char* desc, const String& value) {
  String row("");
  row += "<tr>";
  row += "<td class='right border bold grey'>" + String(desc) + "</td><td class='left border'>" + value + "</td>";
  row += "</tr>" + NL;
  return row;
}

static
String configPageInputText(const char* id, const char* text, int size=0) 
{
  String input = NL;
  input += "<input ";
  input += "type='text' ";
  input += "id='"          + String(id)   + "' ";
  input += "name='"        + String(id)   + "' ";
  input += "placeholder='" + String(text) + "' ";
  if (size) {
    input += "maxlength="    + String(size) + " ";
    input += "size="         + String(size) + " ";
  }
  input += ">";
  return input;
}

static
String configPageInputCheckbox(const char* id, bool checked=false) 
{
  String input = NL;
  input += "<input ";
  input += "type='checkbox' ";
  input += "id='"          + String(id)   + "' ";
  input += "name='"        + String(id)   + "' ";
  input += "value='true'";
  if (checked)
    input += "checked='checked'";
  input += "> Every minute";
  return input;
}

static 
String configPageInputNumber(const char* id, int value, int imin, int imax=-99999) 
{
  String input = NL;
  int size = 5;
  input += "<input ";
  input += "type='number' ";
  input += "id='"          + String(id)    + "' ";
  input += "name='"        + String(id)    + "' ";
  input += "placeholder='" + String(value) + "' ";
  input += "maxlength="    + String(size) + " ";
  input += "size="         + String(size) + " ";
  input += "min="          + String(imin) + " ";
  if (imax > imin)
    input += "max="          + String(imax) + " ";
  input += ">";
  return input;
}

void handleConfig(void) {
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page +=     "<head>" + NL;
  page +=       "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>" + NL;
  page +=       "<title>Countdown Setup</title>" + NL;
  page +=       "<style type='text/css'>" + NL;
  page +=         "table       {text-align:center; border-collapse:collapse; width=90%; margin=0px auto;}" + NL;
  page +=         "th,td       {width:50%; padding-left: 5px; padding-right: 5px; border:1px solid blue}" + NL;
  page +=         "td.noborder {border: 0px;           }" + NL;
  page +=         "td.bold     {font-weight:bold;}" + NL;
  page +=         "td.left     {text-align:left;}"  + NL;
  page +=         "td.right    {text-align:right;}" + NL;
  page +=         "td.grey     {background-color:#f2f2f2;}" + NL;
  page +=         STYLE_BUTTON + NL;
  page +=         "input[type='text'], input[type='number'] {font-size:100%; border:2px solid red}"  + NL; 
  page +=       "</style>" + NL;
  page +=     "</head>" + NL;
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:center; min-width:260px;'>";

  // 
  // title of the pag
  //
  page += "<h3 style='text-align:center; font-weight:bold'>Countdown to<br>Something Important<br><hr></h3>" + NL;

  //
  // wrap this in a form
  //
  page += "<form method='get' action='save'>" + NL;

  /*
   * clock table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Clock Settings</caption>" + NL;

  String input_row;
  int time[6];
  gCountdown.get_time(time);

  input_row = configPageInputNumber("dd", time[0], 0, 100); 
  page     += configPageInputRow("Days", input_row);

  input_row = configPageInputNumber("hh", time[1], 0, 23); 
  page += configPageInputRow("Hours (0-23)", input_row);

  input_row = configPageInputNumber("mm", time[2], 0, 59); 
  page += configPageInputRow("Minutes (0-59)", input_row);

  input_row = configPageInputNumber("ss", time[3], 0, 59); 
  page += configPageInputRow("Seconds (0-59)", input_row);

  input_row = configPageInputText("msg0", gConfig._msg_start, 13); 
  page += configPageInputRow("Start Message (0-12)", input_row);

  input_row = configPageInputText("msg1", gConfig._msg_end, 13); 
  page += configPageInputRow("End Message (0-12)", input_row);

  input_row = configPageInputCheckbox("chk", gConfig._periodic_save); 
  page += configPageInputRow("Periodic Save", input_row);

  page += "</table><br>" + NL;

  /*
   * access point table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Access Point Wifi Settings</caption>" + NL;
  input_row = configPageInputText("apn", gConfig._ap_name, 16); 
  page += configPageInputRow("AP Name", input_row);
  input_row = configPageInputText("app", gConfig._ap_password, 16); 
  page += configPageInputRow("AP Password", input_row);
  page += "</table><br>" + NL;

  /*
   * external wifi link
   */
  page += "<a href='/wifi' align=center><b>Local Network Wifi Settings</b></a>" + NL;

  page += "<br></br>";
  page += "<button type='submit' name='btn' value='save' >Save</button>"  + NL;
  page += "<button type='submit' name='btn' value='test' >Test</button>"  + NL;
  page += "</form>" + NL;
    
  //
  // the end
  //
  page +=   "</div>";
  page += "</body></html>";
  server->send(200, "text/html", page.c_str());
}

void handleConfigSave(void) 
{
  String page("");
  page += "<style type='text/css'>" + NL;
  page += STYLE_BUTTON + NL;
  page += "</style>" + NL;
  page += String("<h1>") + NL;
  for(int i=0; i<server->args();i++) 
    page += server->argName(i) + "='" + server->arg(i) + "'<br>" + NL;
  page += "</h1>" + NL;

  page += "<br><form action='/' method='get'><button>Home</button></form>\n";
      
  int       time[6];
  bool      changed = false;
  bool      changedMsg = false;
  bool      changedTime = false;
  bool      periodic_save = false;
  for(int i=0; i<server->args();i++) {
    if (server->arg(i).length()) {
      changed = true;
      if (server->argName(i) == "msg0`") 
        strlcpy( gConfig._msg_start, server->arg(i).c_str(), sizeof(gConfig._msg_start));
      else if (server->argName(i) == "msg1`") {
        changedMsg = true;
        strlcpy(gConfig._msg_end, server->arg(i).c_str(), sizeof(gConfig._msg_end));
      }
      else if (server->argName(i) == "apn`") 
        strlcpy(gConfig._ap_name, server->arg(i).c_str(), sizeof(gConfig._ap_name));
      else if (server->argName(i) == "app`") 
        strlcpy(gConfig._ap_password, server->arg(i).c_str(), sizeof(gConfig._ap_password));
      else if (server->argName(i) == "chk") {
        periodic_save = true;
      }
      else if (server->argName(i) == "dd") {

        changedTime = true;
        gConfig._duration[0] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "hh") {
        changedTime = true;
        gConfig._duration[1] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "mm") {
        changedTime = true;
        gConfig._duration[2] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "ss") {
        changedTime = true;
        gConfig._duration[3] = server->arg(i).toInt();
      }
    } 
  }
  if (gConfig._periodic_save != periodic_save) {
    changed = true;
    gConfig._periodic_save = periodic_save;
  }

  if (changedTime)
    gCountdown.set_time(gConfig._duration);

  if (changedMsg)
    gCountdown.set_message(gConfig._msg_end);


  if (server->arg("btn").equals("test")) {
    extern bool gTestMode;
    gTestMode = true;
  }

  if (changed) 
    gConfig.saveFile();

  Serial.println(page);
  server->send(200, "text/html", page);
}
