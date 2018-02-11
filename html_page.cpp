#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "countdown_config.h"
#include "countdown_display.h"
#include "countdown_clock.h"

extern  std::unique_ptr<ESP8266WebServer> server; 
static  const String     NL("\r\n");
static  const String     EMPTY("");
static  const char  *choices[] = {
  "0-1-2", 
  "0-2-1", 
  "1-0-2", 
  "1-2-0", 
  "2-0-1", 
  "2-1-0"
};

static  const char STYLE_BUTTON[] PROGMEM = \
"button {border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:40%;}" ;

static  const char  STYLE_HEAD[] PROGMEM = \
"<head>\r\n" \
"<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>\r\n" \
"<title>Countdown Setup</title>\r\n" \
"<style type='text/css'>\r\n" \
"table       {text-align:center; border-collapse:collapse; width=90%; margin=0px auto;}\r\n" \
"th,td       {width:50%; padding-left: 5px; padding-right: 5px; border:1px solid blue}\r\n" \
"td.noborder {border: 0px;}\r\n" \
"td.bold     {font-weight:bold;}\r\n" \
"td.left     {text-align:left;}\r\n" \
"td.right    {text-align:right;}\r\n" \
"td.grey     {background-color:#f2f2f2;}\r\n" \
"button {border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:40%;}\r\n" \
"input[type='text'], input[type='number'] {font-size:100%; border:2px solid red}\r\n" \
"</style></head>\r\n";

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
  // have to put in a hidden field because of the idiocy of the checkbxx
  // behavior, namely, only changes from unchecked to checked is sent down
  // // everytig else is not
  String input = NL;
  input += "<input ";
  input += "type='hidden' ";
  input += "id='"          + String(id)   + "' ";
  input += "name='"        + String(id)   + "' ";
  input += "value='false'";
  input += ">";
  input += "<input ";
  input += "type='checkbox' ";
  input += "id='"          + String(id)   + "' ";
  input += "name='"        + String(id)   + "' ";
  input += "value='true'";
  if (checked)
    input += "checked";
  input += ">";
  return input;
}

static 
String configPageInputNumber(const char* id, int value, int isize=3, int imin=0, int imax=-99999) 
{
  String input = NL;
  input += "<input ";
  input += "type='number' ";
  input += "id='"          + String(id)    + "' ";
  input += "name='"        + String(id)    + "' ";
  input += "placeholder='" + String(value) + "' ";
//  input += "maxlength='"   + String(isize) + "' "; // not supported for number
  input += "size='"        + String(isize) + "' ";
  input += "min='"         + String(imin) + "' ";
  if (imax > imin)
    input += "max='"       + String(imax) + "' ";
  input += ">";
  return input;
}

static 
String configPageInputCombobox(const char* id, const char* choices[], const char *choice, int n) {
  // what is the default
  int i;
  int ichoice = 0;
  for (i=0; i<n; i++)  {
    if (!strcmp(choices[i],choice)) {
      ichoice = i;
      break;
    }
  }

  String input = NL;
  input += "<select name='" + String(id) + "'>";
  for(i=0;i<n;i++) {
    input += "<option value='" + String(choices[i]) + "'";
    if (i==ichoice)
      input += " selected";
    input += ">" + String(choices[i]) + "</option>";
  }
  return input;
}

void handleConfigClock(void) {
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page += FPSTR(STYLE_HEAD);
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:center; min-width:260px;'>";
  
  // 
  // title of the pag
  //
  page += "<h3 style='text-align:center; font-weight:bold'>Counting to<br>Something Important<br><hr></h3>" + NL;
  
  //
  // wrap this in a form
  //
  page += "<form method='get' action='save'>" + NL;
    
  String field, field2;
  int time[5];
  gClock.get_time(time);

  /*
   * clock table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Clock Settings</caption>" + NL;

  field  = configPageInputNumber("dd", time[DAYS], 4, 0, 9999); 
  field2 = configPageInputCheckbox("ddchk", gConfig._visible[DAYS]) + "Visible";
  page  += configPageInputRow("Days (0-9999)", field + field2);

  field  = configPageInputNumber("hh", time[HOURS], 4, 0, 23); 
  field2 = configPageInputCheckbox("hhchk", gConfig._visible[HOURS]) + "Visible";
  page  += configPageInputRow("Hours (0-23)", field + field2);

  field  = configPageInputNumber("mm", time[MINUTES], 4, 0, 59); 
  field2 = configPageInputCheckbox("mmchk", gConfig._visible[MINUTES]) + "Visible";
  page  += configPageInputRow("Minutes (0-59)", field + field2);

  field  = configPageInputNumber("ss", time[SECONDS], 4, 0, 59); 
  field2 = configPageInputCheckbox("sschk", gConfig._visible[SECONDS]) + "Visible";
  page  += configPageInputRow("Seconds (0-59)", field + field2);

  field2 = configPageInputCheckbox("uuchk", gConfig._visible[MILLIS]) + "Visible";
  page  += configPageInputRow("Tenths", field2);

  page += "</table><br>" + NL;

  /*
   * msg table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Message Settings</caption>" + NL;

  field = configPageInputText("msg0", gConfig._msg_start, 13); 
  page += configPageInputRow("Start Message (0-12)", field);

  field = configPageInputText("msg1", gConfig._msg_end, 13); 
  page += configPageInputRow("End Message (0-12)", field);

  page += "</table><br>" + NL;

  /*
   * hardware-ish/ AP network config
   */
  page += "<a href='/hardware' align=center><b>Hardware Settings</b></a><br><br>" + NL;

  /*
   * external wifi link
   */
  page += "<a href='/wifi' align=center><b>Network Settings</b></a>" + NL;

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

void handleConfigHardware(void) {
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page += FPSTR(STYLE_HEAD);
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:center; min-width:260px;'>";
  
  // 
  // title of the pag
  //
  page += "<h3 style='text-align:center; font-weight:bold'>Counting to<br>Something Important<br><hr></h3>" + NL;
  
  //
  // wrap this in a form
  //
  page += "<form method='get' action='save'>" + NL;
  
  /*
   * clock table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Hardware Settings</caption>" + NL;
  
  String field,field1,field2;
  field  = configPageInputNumber("brt", gConfig._brightness, 2, 0, 15); 
  page  += configPageInputRow("Brightness (0-15)", field);
  
  const char* directions[] = {"Countdown","Countup"};
  const char* direction = gConfig._direction == 1 ? directions[1] : directions[0];
  field  = configPageInputCombobox("dir", directions, direction , 2); 
  page += configPageInputRow("Direction", field);
  
  field  = configPageInputCombobox("addr", choices, gConfig._address , 6 ); 
  page += configPageInputRow("Address", field);
  
  field = configPageInputCheckbox("pschk", gConfig._periodic_save); 
  page += configPageInputRow("Periodic Save", field + " Every Minute");
  
  page += "</table><br>" + NL;
  
  /*
   * access point table
   */
  page += "<table align='center'>" + NL;
  page += "<caption>Access Point Wifi Settings</caption>" + NL;
  
  field = configPageInputText("apn", gConfig._ap_name, 16); 
  page += configPageInputRow("AP Name", field);
  
  field = configPageInputText("app", gConfig._ap_password, 16); 
  page += configPageInputRow("AP Password", field);
  page += "</table><br>" + NL;
  
  /*
   * back to clock
   */
  page += "<a href='/' align=center><b>Clock Settings</b></a><br><br>" + NL;
  
  /*
   * external wifi link
   */
  page += "<a href='/wifi' align=center><b>Network Settings</b></a>" + NL;
  
  page += "<br></br>";
  page += "<button type='submit' name='btn' value='save' >Save</button>"  + NL;
  page += "</form>" + NL;
    
  //
  // the end
  //
  page +=   "</div>";
  page += "</body></html>";
  server->send(200, "text/html", page.c_str());
}
  
void handleConfigSave(void) {
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page +=     "<head>" + NL;
  page +=       "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>" + NL;
  page +=       "<title>Countdown Setup</title>" + NL;
  page +=       "<style type='text/css'>" + NL;
  page +=         STYLE_BUTTON + NL;
  page +=       "</style>" + NL;
  page +=     "</head>" + NL;
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:left; min-width:260px;'>";

  page += "<h3>" + NL;
  for(int i=0; i<server->args();i++) 
    page += server->argName(i) + "='" + server->arg(i) + "'<br>" + NL;
  page += "</h3>" + NL;

  page += "<br><form action='/' method='get'><button>Home</button></form>\n";
  page +=  "</div></body></html>";
  Serial.println(page);
  server->send(200, "text/html", page);
      
  int       time[N_ELEMENTS];
  bool      visible[N_ELEMENTS] = {false,false,false,false,false};;
  bool      changed = false;
  bool      changedTime = false;
  bool      periodic_save = false;
  gClock.get_time(time);

  for(int i=0; i<server->args();i++) {
    if (server->arg(i).length()) {
      changed = true;

      if (server->argName(i) == "dd") {
        changedTime = true;
        time[DAYS] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "ddchk") {
        if (server->arg(i).equals("true"))
          visible[DAYS] = true;
      }

      else if (server->argName(i) == "hh") {
        changedTime = true;
        time[HOURS] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "hhchk") {
        if (server->arg(i).equals("true"))
          visible[HOURS] = true;
      }

      else if (server->argName(i) == "mm") {
        changedTime = true;
        time[MINUTES] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "mmchk") {
        if (server->arg(i).equals("true"))
          visible[MINUTES] = true;
      }

      else if (server->argName(i) == "ss") {
        changedTime = true;
        time[SECONDS] = server->arg(i).toInt();
      }
      else if (server->argName(i) == "sschk") {
        if (server->arg(i).equals("true"))
          visible[SECONDS] = true;
      }

      else if (server->argName(i) == "uuchk") {
        if (server->arg(i).equals("true"))
          visible[MILLIS] = true;
      }

      if (server->argName(i) == "msg0") 
        strlcpy( gConfig._msg_start, server->arg(i).c_str(), sizeof(gConfig._msg_start));

      else if (server->argName(i) == "msg1") {
        strlcpy(gConfig._msg_end, server->arg(i).c_str(), sizeof(gConfig._msg_end));
        gDisplay.set_message(gConfig._msg_end);
      }

      // hardare page
      else if (server->argName(i) == "brt") 
        gConfig._brightness = server->arg(i).toInt();

      else if (server->argName(i) == "dir") {
        gConfig._direction = server->arg(i).equals("Countdown") ? -1 : 1;
        changed = true;
      }
      else if (server->argName(i) == "addr") {
        strlcpy(gConfig._address, server->arg(i).c_str(), sizeof(gConfig._address));
        changed = true;
      }
      else if (server->argName(i) == "pschk")  
        if (server->arg(i).equals("true"))
          periodic_save = true;
       

      else if (server->argName(i) == "apn") 
        strlcpy(gConfig._ap_name, server->arg(i).c_str(), sizeof(gConfig._ap_name));

      else if (server->argName(i) == "app") 
        strlcpy(gConfig._ap_password, server->arg(i).c_str(), sizeof(gConfig._ap_password));

    } 
  }
  //
  // checkbox's set to false are not sent down to the server
  if (gConfig._periodic_save != periodic_save) {
    changed = true;
    gConfig._periodic_save = periodic_save;
  }
  //
  // visibility
  Serial.println("");
  for (int i=0;i < sizeof(visible)/sizeof(visible[0]);i++) {
    Serial.printf("%d) config=%d html=%d\n",i,gConfig._visible[i],visible[i]);
    if (gConfig._visible[i] != visible[i]) {
      gConfig._visible[i] = visible[i];
      changed = true;
    }
  }

  if (changedTime) {
    gClock.set_time(time);
    gConfig.set_time(time);
  }

  if (changed) 
    gConfig.saveFile();

  if (server->arg("btn").equals("test")) {
    extern bool gTestMode;
    gTestMode = true;
  }
}

