

void SerialOutParams(){
String message ;
   
  message = "Web Request URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  Serial.println(message);
  message = "";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  Serial.println(message);
}

void handleRoot() {
  boolean currentLineIsBlank = true;
  long  i = 0 ;
  int ii  ;
  int iProgNum = 0;
  int j ;
  int k , kk , iTmp ;
  boolean bExtraValve = false ;
  uint8_t iPage = 1 ;
  boolean bDefault = true ;
//  int td[6];
  long lTmp ; 
  String MyCheck , MyColor , MyNum  ;
  byte mac[6];
  String message ;
  uint32_t pcol ;
  uint32_t xcol ;
  uint8_t r, g, b ;
  
  Serial.println(F("Process WEB request"));  
  
  for (int j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
          LoadParamsFromEEPROM(true);
        break;
        case 2: // Save values
          LoadParamsFromEEPROM(false);
        break;
        case 3: // 
        break;
        case 4: // 
        break;
        case 5: // 
        break;
        case 8: //  Cold Reboot
//          ESP.reset() ;
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 667: // wipe the memory to factory default
          BackInTheBoxMemory();
        break;
        case 665:
//          sendNTPpacket(ghks.timeServer); // send an NTP packet to a time server  once and hour  
        break;
        case 668:
          for (int i = 0  ; i < MAX_RELAY ; i++ ){
            digitalWrite(tplf.RelayPin[i],tplf.ActiveValue[i]);
            RelayTimers[i]= 60 ;     
          }
        break;
        case 669:
          for (int i = 0  ; i < MAX_RELAY ; i++ ){
            digitalWrite(tplf.RelayPin[i],!tplf.ActiveValue[i]);    
            RelayTimers[i]= 0 ;     
          }
        break;
      }  
    }
    
    i = String(server.argName(j)).indexOf("rlyt0");
    if (i != -1){  // 
      tplf.MaxTimer[0] = String(server.arg(j)).toInt() ;
    }      
    i = String(server.argName(j)).indexOf("rlyt1");
    if (i != -1){  // 
      tplf.MaxTimer[1] = String(server.arg(j)).toInt() ;    
    }      

    i = String(server.argName(j)).indexOf("stime");
    if (i != -1){  // 
      timeinfo.tm_year = (String(server.arg(j)).substring(0,4).toInt()-1970) ;
      timeinfo.tm_mon =(String(server.arg(j)).substring(5,7).toInt()) ;
      timeinfo.tm_mday = (String(server.arg(j)).substring(8,10).toInt()) ;
      timeinfo.tm_hour =(String(server.arg(j)).substring(11,13).toInt()) ;
      timeinfo.tm_min = (String(server.arg(j)).substring(14,16).toInt()) ;
      timeinfo.tm_sec = 0 ;
      time_t t = mktime(&timeinfo);   // Convert struct tm → epoch seconds
      struct timeval now = {
          .tv_sec = t,
          .tv_usec = 0
      };
      settimeofday(&now, NULL);      
    }        


                  
  }          

  SendHTTPHeader();
  
  message = F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>") ;         
  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday , timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  message += "<b>"+ String(buff) ; 
  if ( timeinfo.tm_year < 124 ) {
    message +=F("--- CLOCK NOT SET ---") ;
  }
  message +=F("</b><br>") ;  

  if (String(server.uri()).indexOf("stime")>0) {  // setup of the time
    bDefault = false ;
    snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday , timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    message += "<br><br><form method=get action=" + server.uri() + "><br>Set Current Time: <input type='text' name='stime' value='"+ String(buff) + "' size=12>";
    message +=F("<input type='submit' value='SET'><br><br></form>");
  }
  server.sendContent(message);
  message = "" ; 
     
  if (bDefault) {     // default valve control and setup
    message +=F("<br><center><b>Alarm Relay Setup</b><br>");
    message +=F("<table border=1 title='Alarm Relay Setup'>");
    message +=F("<tr><td>Relay No</td><td>TTG<br>(s)</td><td>Trigger<br>Time (min)</td><td>Active<br>Level</td><td>GPIO<br>PIN</td></tr>");
    for (int i = 0 ; i < MAX_RELAY ; i++){
      message += "<form method=post action=" + server.uri() + "><tr><td>"+String(i)+"</td><td>"+String(RelayTimers[i])+"</td><td align=center>" ; 
      message += "<input type='text' name='rlyt"+String(i)+"' value='" + String(tplf.MaxTimer[i]) + "' size=12></td><td>"+String(tplf.ActiveValue[i])+"</td><td>"+String(tplf.RelayPin[i])+"</td><td><input type='submit' value='SET'></form></td></tr>\r\n";
    }
    message +=F("</table><br><br>");
    if (strLastAlarm.length() > 0 )
      message += "Last Alarm <b>" + strLastAlarm + "</b><br>Time of Alarm <b>"+strLastDate + "</b><br><br>" ;  
    server.sendContent(message);
    message = "" ; 
  }
  SendHTTPPageFooter();
}

void handleNotFound(){
  String message = F("Seriously - No way DUDE\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
  Serial.print(message);
}

void SendHTTPHeader(){
  server.sendHeader(F("Server"),F("ESP32-night-crows"),false);
  server.sendHeader(F("X-Powered-by"),F("Dougal-2.0"),false);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  String message = F("<!DOCTYPE HTML>");
  message += F("<link rel='icon' href='data:,'></head><body><html><center><h2>HTTP Alarm Relay Link for TP-Link VIGI cameras</h2>");
  message += F("<a href='/'>Refresh</a><br><br>") ;         
  message += F("<meta name=viewport content='width=320, auto inital-scale=1'>");
  server.sendContent(message) ;  
  message = "" ;     
}

void SendHTTPPageFooter(){
  String message =  F("<br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br><br><a href='/?command=668'>Relay Test</a><br><a href='/?command=669'>Reset Relay</a><br><br><a href='/?command=665'>Sync UTP Time</a><br><a href='/stime'>Manual Time Set</a><br><a href='/scan'>I2C Scan</a><br>") ;     
  message += "<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>";
  message += F("<a href='/eeprom'>EEPROM Memory Contents</a><br>");
  message += F("<a href='/setup'>Node Setup</a><br>");
  message += F("<a href='/info'>Node Infomation</a><br>");
  if (!WiFi.isConnected()) {
    snprintf(buff, BUFF_MAX, ">%u.%u.%u.%u<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);
  }else{
    snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  }
  message += "<a href='http://" + String(buff) + ":81/update'>OTA Firmware Update</a><br>";  
  message += "<a href='https://github.com/Dougal121'>Source at GitHub</a><br>";  
  message += "<a href='http://" + String(buff) + "/backup'>Backup / Restore Settings</a><br><br>";  
  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  message += "WatchDog Uptime <b>"+String(buff)+"</b> (day:hr:min) <br>" ;
  
  message += F("</body></html>\r\n");
  server.sendContent(message) ;  
  message = "" ;     
}
