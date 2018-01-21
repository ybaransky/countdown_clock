#include <Arduino.h>
#include "countdown_clock.h"
#include <ESP8266WebServer.h>
extern  std::unique_ptr<ESP8266WebServer> server; 
extern  CountdownClock gCountdown;

static const String NL("\r\n");
static const String EMPTY("");
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
  //row += "<td style='text-align:right; font-weight:bold'>" + String(name) + "</td><td style='text-align:left'>" + value + "</td>";
  row += "<td class='right border bold grey'>" + String(desc) + "</td><td class='left border'>" + value + "</td>";
  row += "</tr>" + NL;
  return row;
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
  page += "<h3 style='text-align:center; font-weight:bold'>Countdown to<br>Yurij\'s Termination<br><hr></h3>" + NL;

  //
  // wrap this in a form
  //
  page += "<form method='get' action='save'>" + NL;

  //
  // the table
  //
  //page += "<table width='100%' style='margin: 0px'>" + NL;
  page += "<table align='center'>" + NL;

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

  page += "</table>" + NL;
  page += "<br><br>";
  page += "<button type='submit' value='Save' >Save</button>"  + NL;
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
  String page = "<style type='text/css'>" + NL;
  page +=         STYLE_BUTTON + NL;
  page +=       "</style>" + NL;
    
  page += String("<h1>") + 
    "Days="    + server->arg("dd") + "<br>\n" +
    "Hours="   + server->arg("hh") + "<br>\n" +
    "Minutes=" + server->arg("mm") + "<br>\n" +
    "Seconds=" + server->arg("ss") + "<br>\n" +
    "</h1>\n";
      
  /*
  String ipaddr = "192.168.4.1/"; // networkAddressStr();
  String script = "<script>";
  script += "var timer = setTimeout(function() { window.location=";
  script += "'http://" + ipaddr + "'}, 3000);";
  script += "</script>" + NL;
  page += script;
 */
  
  int time[6];
  gCountdown.get_time(time);
  for(int i=0; i<server->args();i++) {
    if (server->arg(i).length()) {
       if (server->argName(i) == "dd") 
        time[0] = server->arg(i).toInt();
       else if (server->argName(i) == "hh") 
        time[1] = server->arg(i).toInt();
       else if (server->argName(i) == "mm") 
        time[2] = server->arg(i).toInt();
       else if (server->argName(i) == "ss") 
        time[3] = server->arg(i).toInt();
    } 
  } 

  page += "<br><form action='/home' method='get'><button>Home</button></form>\n";
  
//    gConfig.print();
//  gConfig.save();
  gCountdown.set_time(time);
  server->send(200, "text/html", page);
}
