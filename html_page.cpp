#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include "countdown_config.h"
#include "countdown_display.h"
#include "countdown_clock.h"

extern  std::unique_ptr<ESP8266WebServer> server; 
static  const String     NL("\r\n");
static  const String     EMPTY("");

static  const char STYLE_BUTTON[] PROGMEM = \
"button {border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:40%;}" ;

static  const char  STYLE_HEAD[] PROGMEM = \
"<head>\r\n" \
"<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>\r\n" \
"<title>Countdown Setup</title>\r\n" \
"<style type='text/css'>\r\n" \
"table       {text-align:center; border-collapse:collapse; width=90%; margin=0px auto;}\r\n" \
"th,td       {padding-left: 5px; padding-right: 5px; border:1px solid blue}\r\n" \
"td.noborder {border: 0px;}\r\n" \
"td.bold     {font-weight:bold;}\r\n" \
"td.left     {text-align:left;}\r\n" \
"td.center   {text-align:center;}\r\n" \
"td.right    {text-align:right;}\r\n" \
"td.grey     {background-color:#f2f2f2;}\r\n" \
"caption     {font-weight:bold;}\r\n" \
"button {border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:40%;}\r\n" \
"input[type='text'], input[type='number'] {font-size:100%; border:2px solid red}\r\n" \
"</style></head>\r\n";

//"th,td   {width:33%; padding-left: 5px; padding-right: 5px; border:1px solid blue}\r\n" 
//
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
  row += "<td class='right border grey'>" + String(desc) + "</td><td class='left border'>" + value + "</td>";
  row += "</tr>" + NL;
  return row;
}

static 
String configPageInputRow3(const char* desc, const String& value) {
  String row("");
  row += "<tr>";
  row += "<td class='right border grey'>" + String(desc) + "</td>";
  row += "<td colspan='3' class='center border'>" + value + "</td>";
  row += "</tr>" + NL;
  return row;
}

static 
String configPageInputRow(const char* desc, const String& v1, const String& v2) {
  String row("");
  row += "<tr>";
  row += "<td class='right border grey'>" + String(desc) + "</td>";
  row += "<td class='center border'>" + v1 + "</td>";
  row += "<td class='left border'>" + v2 + "</td>";
  row += "</tr>" + NL;
  return row;
}

static 
String configPageInputRow2Cols(const char* desc, 
    const String& v1, const String& v2) {
  String row("");
  row += "<tr>";
  row += "<td class='right border grey'>" + String(desc) + "</td>";
  row += "<td class='center border'>" + v1 + "</td>";
  row += "<td class='center border'>" + v2 + "</td>";
  row += "</tr>" + NL;
  return row;
}

static 
String configPageInputRow3Cols(const char* desc, 
    const String& v1, const String& v2, const String& v3) {
  String row("");
  row += "<tr>";
  row += "<td class='right border grey'>" + String(desc) + "</td>";
  row += "<td class='center border'>" + v1 + "</td>";
  row += "<td class='center border'>" + v2 + "</td>";
  row += "<td class='center border'>" + v3 + "</td>";
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
String configPageInputCombobox(const char* id, const char* choices[], int ichoice, int n) {
  String input = NL;
  if (ichoice <  0) ichoice = 0;
  if (ichoice >= n) ichoice = 0;
//  input += "<div style='height:100%'>";
  input += "<select name='" + String(id) + "'>";
  for(int i=0;i<n;i++) {
    input += "<option value='" + String(i) + "'";
    if (i==ichoice)
      input += " selected";
    input += ">" + String(choices[i]) + "</option>";
  }
//  input += "</div>";
  return input;
}

static 
String configPageInputRadio(
  const char* id, const char* choices[], const int* values, int choice, int n) {
  String input = "";
  for(int i=0;i<n;i++) {
    input += "<input type='radio' name='" + String(id) + "'";
    input += " value='" + String(values[i]) + "'";
    if (i==choice)
      input += " checked";
    input += ">" + String(choices[i]) + "  ";
  }
  return input;
}

void handleConfigClock(void) {
  String TABLE = "<table width='95%' align='center'>" + NL;
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
  
  /*
   * start form
   */
  page += "<form method='get' action='save'>" + NL;
    
  String field, field0, field1, field2, empty("");
  int time[5];
  gClock.get_time(time);

  /*
   * clock table
   */
  page += TABLE;
  page += "<caption>Clock Setup</caption>" + NL;
  page += "<col width='50%'><col width='50%'>" + NL;
  page += "<tr><th>Time</th><th>Value</th></tr>" + NL;
  field = configPageInputNumber("dd", time[DAYS], 4, 0, 9999); 
  page += configPageInputRow("Days (0-9999)", field);
  field = configPageInputNumber("hh", time[HOURS], 4, 0, 23); 
  page += configPageInputRow("Hours (0-23)", field);
  field = configPageInputNumber("mm", time[MINUTES], 4, 0, 59); 
  page += configPageInputRow("Minutes (0-59)", field);
  field = configPageInputNumber("ss", time[SECONDS], 4, 0, 59); 
  page += configPageInputRow("Seconds (0-59)", field);

  const int values[] = {-1,1};
  const char* directions[] = {"Down","Up"};
  const int   direction = gConfig._direction == -1 ? 0 : 1;
  field  = configPageInputRadio("dir", directions, values, direction , 2); 
  page  += configPageInputRow("Clock Dirction", field);

  const char  *modes[] = {
   "dd D | hh:mm |  ss.u",
   "dd D | hh:mm |    ss",
   "dd D | hh  H | mm:ss",
   "dd D | hh  H |  mm N",
   "  dd | hh:mm |  ss.u",
   "  dd | hh:mm |    ss",
   "  dd |    hh | mm:ss",
   "  dd |    hh |    mm"
  };

  int imode = 0;
  field = configPageInputCombobox("mode", modes, 
      gConfig._display_mode, sizeof(modes)/sizeof(modes[0]));
  page  += configPageInputRow("Display Mode", field);

  page += "</table><br>" + NL;

  /*
   * msg table
   */
  page += TABLE;
  page += "<caption>Start/Stop Messages</caption>" + NL;
  page += "<col width='50%'><col width='50%'>" + NL;
  page += "<tr><th>Message</th><th>Text</th></tr>" + NL;
  field = configPageInputText("msg0", gConfig._msg_start, 13); 
  page += configPageInputRow("Start (0-12)", field);
  field = configPageInputText("msg1", gConfig._msg_end, 13); 
  page += configPageInputRow("End (0-12)", field);
  page += "</table><br>" + NL;

  /*
   * buttons
   */
  page += "<br>";
  page += "<button type='submit' name='btn' value='save' >Save</button>"  + NL;
  page += "<button type='submit' name='btn' value='test' >Test</button>"  + NL;
  page += "<br><br><br>";
  page += "<p><p>";

  /*
   * hardware table
   */
  page += TABLE;
  page += "<caption>Hardware Settings</caption>" + NL;
  page += "<col width='55%'><col width='15%'><col width='15%'><col width='15%'>" + NL;
  page += "<tr><th>Description</th><th>Addr</th><th>Bright</th><th>Show</th></tr>" + NL;
  for(int i=0;i<N_SEGMENTS; i++) {
    char sv[32], brt[32],addr[32],desc[32];
    Segment&  segment = gConfig._segments[i];
    sprintf(sv,   "svchk%d",i);
    sprintf(brt,  "brt%d",i);
    sprintf(addr, "addr%d",i);
    sprintf(desc, "Segment %4s", segment._name);
    field0 = configPageInputNumber(addr, segment._address,    1, 0, 2); 
    field1 = configPageInputNumber(brt , segment._brightness, 2, 0, 15); 
    field2 = configPageInputCheckbox(sv, segment._visible); 
    page  += configPageInputRow3Cols(desc , field0, field1, field2);
  }


  field = configPageInputCheckbox("pschk", gConfig._periodic_save); 
  page += configPageInputRow3("Save Config", field + " Every Minute");
  page  += "</table><br>" + NL;
  
  /*
   * access point table
   */
  page += TABLE;
  page += "<caption>Access Point Wifi Settings</caption>" + NL;
  field = configPageInputText("apn", gConfig._ap_name, 16); 
  page += configPageInputRow("AP Name", field);
  field = configPageInputText("app", gConfig._ap_password, 16); 
  page += configPageInputRow("AP Password", field);
  page += "</table><br>" + NL;

  /*
   * external links
   */
  page += "<a href='/view' align=center><b>View Config File</b></a>" + NL;
  page += "<p>";
  page += "<a href='/delete' align=center><b>Delete Config File</b></a>" + NL;
  page += "<p>";
  page += "<a href='/wifi' align=center><b>Network Settings</b></a>" + NL;
  page += "<br>";

  /*
   * end form
   */
  page += "</form>" + NL;
  
  //
  // the end
  //
  page +=   "</div>";
  page += "</body></html>";
  server->send(200, "text/html", page.c_str());
}

void  handleConfigView(void) {
  if (SPIFFS.exists(gConfig._filename)) {
    String  context = "text/json";
    File file = SPIFFS.open(gConfig._filename, "r");
    size_t sent = server->streamFile(file, "text/json");
    file.close();
  } else {
    String page("<!DOCTYPE htm>\n");
    page += "<html lang='en'><body>\n";
    page += "<h2>File '" + gConfig._filename + "' Not Found</h2>";
    page += "</body></html>\n";
    server->send(200, "text/html", page);
  }
  return;
}

void  handleConfigReboot(void) {
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page += FPSTR(STYLE_HEAD);
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:center; min-width:260px;'>";
  page += "<h2 style='text-align:center; font-weight:bold'>Rebooting in 2 second</h2>";
  page += "<p><p>";
  page += "<form method='get' action='/'><button type='submit'>Home</button></form>";
  page += "</div></body></html>";
  server->send(200, "text/html", page); 
  extern  void  reboot(void);
  reboot();
  return;
}

void  handleConfigDelete(void) {
  const String&  path = gConfig._filename;
  String page = "";
  page += "<!doctype html>"    + NL;;
  page +=   "<html lang='en'>" + NL;;
  page += FPSTR(STYLE_HEAD);
  page +=     "<body>"  + NL;  
  page +=     "<div style='text-align:center; min-width:260px;'>";
  
  String msg = "file '" + path + "'";
  if (SPIFFS.exists(path)) {
    SPIFFS.remove(path);
    msg += " removed";
  } else {
    msg += " not found!";
  }
  page += "<h2 style='text-align:center; font-weight:bold'>" + msg + "</h2>";
  page += "<p><p>";
  page += "<form method='get' action='/'><button type='submit'>Home</button></form>";
  page += "</div></body></html>";
  server->send(200, "text/html", page); 
  return;
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

  page += "<h4>" + NL;
  for(int i=0; i<server->args();i++) 
    page += server->argName(i) + "='" + server->arg(i) + "'<br>" + NL;
  page += "</h4>" + NL;
  page += "</div>";

  //page += "<div style='text-align:center; min-width:260px;'><br>";
  page += "<div style='text-align:center'><br>";
//  page += "<form style='display:inline-block' action='/' method='get'>";
  page += "<form action='/' method='get'>";
  page += "  <button>Home</button>";
  page += "<p>";
  page += "</form>";
//  page += "<form style='display:inline-block' action='/reboot' method='get'>";
  page += "<form action='/reboot' method='get'>";
  page += "  <button>Reboot</button>";
  page +=  "</form>\n";
  page +=  "</div></body></html>";
//  Serial.println(page);
  server->send(200, "text/html", page);
      
  int       time[N_ELEMENTS];
  bool      visible[N_SEGMENTS] = {false,false,false};
  bool      changed = false;
  bool      changedTime = false;
  bool      periodic_save = false;
  bool      changedBrightness = false;
  gClock.get_time(time);

  for(int i=0; i<server->args();i++) {
    if (server->arg(i).length()) {
      changed = true;

      if (server->argName(i) == "dd") {
        changedTime = true;
        time[DAYS] = server->arg(i).toInt();
      }

      else if (server->argName(i) == "hh") {
        changedTime = true;
        time[HOURS] = server->arg(i).toInt();
      }

      else if (server->argName(i) == "mm") {
        changedTime = true;
        time[MINUTES] = server->arg(i).toInt();
      }

      else if (server->argName(i) == "ss") {
        changedTime = true;
        time[SECONDS] = server->arg(i).toInt();
      }

      else if (server->argName(i) == "mode") {
        gConfig._display_mode = server->arg(i).toInt();
      }

      else if (server->argName(i) == "msg0") {
        strlcpy( gConfig._msg_start, server->arg(i).c_str(), sizeof(gConfig._msg_start));
      }

      else if (server->argName(i) == "msg1") {
        strlcpy(gConfig._msg_end, server->arg(i).c_str(), sizeof(gConfig._msg_end));
        gDisplay.set_message(gConfig._msg_end);
      }

      // hardware page
      else if (server->argName(i) == "addr0") 
        gConfig._segments[0]._address = server->arg(i).toInt();

      else if (server->argName(i) == "addr1") 
        gConfig._segments[1]._address = server->arg(i).toInt();

      else if (server->argName(i) == "addr2") 
        gConfig._segments[2]._address = server->arg(i).toInt();

      else if (server->argName(i) == "brt0") {
        gConfig._segments[0]._brightness = server->arg(i).toInt();
        changedBrightness = true;
      }

      else if (server->argName(i) == "brt1") {
        gConfig._segments[1]._brightness = server->arg(i).toInt();
        changedBrightness = true;
      }

      else if (server->argName(i) == "brt2") {
        gConfig._segments[2]._brightness = server->arg(i).toInt();
        changedBrightness = true;
      }

      else if (server->argName(i) == "svchk0") {
        if (server->arg(i).equals("true"))
          visible[0] = true;
      }

      else if (server->argName(i) == "svchk1") {
        if (server->arg(i).equals("true"))
          visible[1] = true;
      }

      else if (server->argName(i) == "svchk2") {
        if (server->arg(i).equals("true"))
          visible[2] = true;
      }

      else if (server->argName(i) == "dir") 
        gConfig._direction = server->arg(i).toInt();

      else if (server->argName(i) == "pschk") {
        if (server->arg(i).equals("true"))
          periodic_save = true;
      }
       
      // wifi ap settings
      else if (server->argName(i) == "apn") {
        strlcpy(gConfig._ap_name, server->arg(i).c_str(), sizeof(gConfig._ap_name));
      }

      else if (server->argName(i) == "app")  {
        strlcpy(gConfig._ap_password, server->arg(i).c_str(), sizeof(gConfig._ap_password));
      }
    } 
  }
  //
  // checkbox's set to false are not sent down to the server
  if (gConfig._periodic_save != periodic_save) {
    gConfig._periodic_save = periodic_save;
    changed = true;
  }

  for(int i=0; i<N_SEGMENTS; i++) {
    if (gConfig._segments[i]._visible != visible[i]) {
      gConfig._segments[i]._visible = visible[i];
      changed = true;
    }
  }

  if (changedTime) {
    gClock.set_time(time);
    gConfig.set_time(time);
  }

  if (changedBrightness) {
    gDisplay.set_brightness();
    gDisplay.writeDisplay(true);
    Serial.println("forcing new brightness");
  }

  if (changed) 
    gConfig.saveFile();

  if (server->arg("btn").equals("test")) {
    extern bool gTestMode;
    gTestMode = true;
  }
}

