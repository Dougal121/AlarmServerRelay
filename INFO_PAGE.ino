void handleInfo(){
  int i , ii , iTmp , iX ;
  uint8_t j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];


  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
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
  }
  
  SendHTTPHeader();

  server.sendContent(F("<br><center><b>Node Info</b><br>"));
  server.sendContent(F("<table border=1 title='Device Info'>"));

  server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String((uint32_t)chipid, HEX) + "</td><td align=center>"+String((uint32_t)chipid)+"</td></tr>" ) ; 
  MyIP =  WiFi.localIP() ;
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  server.sendContent("<tr><td>Network Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  MyIPC = WiFi.softAPIP();  // get back the address to verify what happened  
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);
  server.sendContent("<tr><td>Config Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Time Server</td><td align=center>" + String(ghks.timeServer) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Network SSID</td><td align=center>" + String(ghks.nssid) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Network Password</td><td align=center>" + String(ghks.npassword) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Configure SSID</td><td align=center>" + String(cssid) + "</td><td>.</td></tr>" ) ; 
  server.sendContent("<tr><td>Configure Password</td><td align=center>" + String(ghks.cpassword) + "</td><td></td></tr>" ) ; 
  
  server.sendContent("<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>" ) ; 
  WiFi.macAddress(mac);      
  snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
 
  server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanLast) + "</td><td>(per second)</td></tr>" ) ;    

  server.sendContent("<tr><td>ESP Core Version</td><td align=center>" + String(ESP.getCoreVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>ESP Chp Revision</td><td align=center>" + String(ESP.getChipRevision()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>SDK Version</td><td align=center>" + String(ESP.getSdkVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Frequecy</td><td align=center>" + String(ESP.getCpuFreqMHz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>XTAL Frequecy</td><td align=center>" + String(getXtalFrequencyMhz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>APB Frequecy</td><td align=center>" + String(getApbFrequency()/1000000) + "</td><td>(MHz)</td></tr>" ) ;    

  snprintf(buff, BUFF_MAX, "%d:%02d:%02d",(lMinUpTime/1440),((lMinUpTime/60)%24),(lMinUpTime%60));
  server.sendContent("<tr><td>Computer Uptime</td><td align=center>"+String(buff)+"</td><td>(day:hr:min)</td></tr>" ) ;

  server.sendContent(F("</table><br>"));    
  SendHTTPPageFooter();
}


void handleSetup(){
  int i , ii , iTmp , iX ;
  uint8_t j , k , kk ;
  String message = "" ;  
  long lTmp ;
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  String MyCheck ;
  byte mac[6];
  String strSelected = "";
  String strOption = "" ; 


//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 2: // Save values
          LoadParamsFromEEPROM(false);
        break;
      }
    }
    
        
    i = String(server.argName(j)).indexOf("ndadd");
    if (i != -1){  // 
      ghks.lNodeAddress = String(server.arg(j)).toInt() ;
      ghks.lNodeAddress = constrain(ghks.lNodeAddress,0,32768);
    }        
    i = String(server.argName(j)).indexOf("disop");
    if (i != -1){  // 
      ghks.lDisplayOptions = String(server.arg(j)).toInt() ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }  
    i = String(server.argName(j)).indexOf("disro");
    if (i != -1){  // 
      ii =(( 0x01 & String(server.arg(j)).toInt()) << 1 ) ;
      ghks.lDisplayOptions = ( ghks.lDisplayOptions & 0xFD ) | ii ;
      ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,255);
    }  
    i = String(server.argName(j)).indexOf("cpufr");
    if (i != -1){  // 
      ghks.cpufreq = String(server.arg(j)).toInt() ;
    }  
 

    
    i = String(server.argName(j)).indexOf("lpntp");
    if (i != -1){  // 
      ghks.localPort = String(server.arg(j)).toInt() ;
      ghks.localPort = constrain(ghks.localPort,1,65535);
    }        
    i = String(server.argName(j)).indexOf("lpctr");
    if (i != -1){  // 
      ghks.localPortCtrl = String(server.arg(j)).toInt() ;
      ghks.localPortCtrl = constrain(ghks.localPortCtrl,1,65535);
    }        
    i = String(server.argName(j)).indexOf("rpctr");
    if (i != -1){  // 
      ghks.RemotePortCtrl = String(server.arg(j)).toInt() ;
      ghks.RemotePortCtrl = constrain(ghks.RemotePortCtrl,1,65535);
    }        

    i = String(server.argName(j)).indexOf("cname");
    if (i != -1){  // have a request to request a time update
     String(server.arg(j)).toCharArray( ghks.NodeName , sizeof(ghks.NodeName)) ;
    }
    i = String(server.argName(j)).indexOf("rpcip");
    if (i != -1){  // have a request to request an IP address
      ghks.RCIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.RCIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.RCIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.RCIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("staip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPStatic[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPStatic[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPStatic[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPStatic[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("gatip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPGateway[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPGateway[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPGateway[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPGateway[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("mskip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPMask[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPMask[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPMask[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPMask[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("dnsip");
    if (i != -1){  // have a request to request an IP address
      ghks.IPDNS[0] = String(server.arg(j)).substring(0,3).toInt() ;
      ghks.IPDNS[1] =String(server.arg(j)).substring(4,7).toInt() ;
      ghks.IPDNS[2] = String(server.arg(j)).substring(8,11).toInt() ;
      ghks.IPDNS[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }
    i = String(server.argName(j)).indexOf("nssid");
    if (i != -1){                                    // SSID
 //    Serial.println("SookyLala 1 ") ;
     String(server.arg(j)).toCharArray( ghks.nssid , sizeof(ghks.nssid)) ;
    }
    
    i = String(server.argName(j)).indexOf("npass");
    if (i != -1){                                    // Password
     String(server.arg(j)).toCharArray( ghks.npassword , sizeof(ghks.npassword)) ;
    }
    
    i = String(server.argName(j)).indexOf("cpass");
    if (i != -1){                                    // Password
     String(server.arg(j)).toCharArray( ghks.cpassword , sizeof(ghks.cpassword)) ;
    }
    
    i = String(server.argName(j)).indexOf("timsv");
    if (i != -1){                                    // timesvr
     String(server.arg(j)).toCharArray( ghks.timeServer , sizeof(ghks.timeServer)) ;
    }
    i = String(server.argName(j)).indexOf("srbt");
    if (i != -1){  // 
      tplf.SelfReBoot =  String(server.arg(j)).toInt()  ;
      if ((tplf.SelfReBoot < MIN_REBOOT )){
        if (tplf.SelfReBoot <= 0 ) {
          tplf.SelfReBoot = 0 ;                  
        }else{
          tplf.SelfReBoot = MIN_REBOOT ;
        }
      }
    }  
    i = String(server.argName(j)).indexOf("lrtd");
    if (i != -1){  // 
      lTmp =  String(server.arg(j)).toInt()  ;
      tplf.lRebootTimeDay = lTmp & 0xfff ;
    }        
    for ( k = 0 ; k < 8 ; k++){  // handle all the valve control commands for any and all valves
      i = String(server.argName(j)).indexOf( "dw" + String(k) );
      if (i != -1){  // 
        tplf.lRebootTimeDay |= ( 0x1000 << k ) ;
      }              
    }    




      
             
  }
  
  SendHTTPHeader();
  server.sendContent(F("<a href='/setup?command=2'>Save Parameters to EEPROM</a><br>")) ;     

  message += "<form method=post action=" + server.uri() + "><table border=1 title='Node Settings'>";
  message += F("<tr><th>Parameter</th><th>Value</th><th><input type='submit' value='SET'></th></tr>\r\n");

  message += F("<tr><td>Controler Name</td><td align=center>") ; 
  message += "<input type='text' name='cname' value='"+String(ghks.NodeName)+"' maxlength=15 size=12></td><td></td></tr>\r\n";


  message += F("<tr><td>Node Address</td><td align=center>") ; 
  message += "<input type='text' name='ndadd' value='" + String(ghks.lNodeAddress) + "' size=12></td><td>"+String(ghks.lNodeAddress & 0xff)+"</td></tr>\r\n";

  message += F("<tr><td>Self Reboot Timer</td><td align=center>") ; 
  message += "<input type='text' name='srbt' value='" + String( tplf.SelfReBoot) + "' size=8 maxlength=8></td><td>(min)</td></tr>";

  message += F("<tr><td>Reboot Time of Day</td><td align=center>") ; 
  message += "<input type='text' name='lrtd' value='" + String( tplf.lRebootTimeDay & 0xfff ) + "' size=8 maxlength=8></td><td>(HHMM)</td></tr>";
  message += "<tr><td>Reboot Days</td><td colspan=2 align=center>";
  for (k = 0 ; k < 8 ; k++){      
    MyColor =  ""   ;  
    if ( ( tplf.lRebootTimeDay & (0x1000 << k)) != 0 ){
      MyCheck = F("CHECKED")  ;  
    }else{
      MyCheck = "" ;      
    }
    message += String(dayarray[k])+ "<input type='checkbox' name='dw" + String(k)+"' "+String(MyCheck)+ ">";    
  }
  message += F("</td></tr></form>\r\n<tr><td colspan=3></td></tr>");

  message += F("</td><td></td></tr>\r\n");



  server.sendContent(message) ;
  message = "<form method=post action=" + server.uri() + ">" ;
      
  message += F("<tr><td>CPU Frequency</td><td align=center>") ; 
  message += F("<select name='cpufr'>") ;
  for ( j = 0 ; j <= 4 ; j++ ) {
    strOption = CPUSpeedTextLUT(j,&ii);
    if ( ghks.cpufreq == ii ){
      strSelected = "SELECTED" ;
    }else{
      strSelected = "" ;
    }
    message += "<option value='"+String(ii)+"' " + strSelected + ">" + strOption + "\r\n"; 
  }
  message += F("</select></td><td>(MHz)<input type='submit' value='SET'></td></tr>\r\n");

  message += F("<tr><td>Display Option</td><td align=center>") ; 
  message += F("<select name='disop'>") ;
  if ((ghks.lDisplayOptions & 0x01 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - Normal"); 
    message += F("<option value='1'>1 - Invert"); 
  }else{
    message += F("<option value='0'>0 - Normal"); 
    message += F("<option value='1' SELECTED>1 - Invert"); 
  }
  message += F("</select></td><td></td></tr>\r\n");

  message += F("<tr><td>Display Power Option</td><td align=center>") ; 
  message += F("<select name='disro'>") ;
  if (((ghks.lDisplayOptions & 0x02 ) >> 1 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - Always On"); 
    message += F("<option value='1'>1 - Go off with WiFi"); 
  }else{
    message += F("<option value='0'>0 - Always On"); 
    message += F("<option value='1' SELECTED>1 - Go off with WiFi"); 
  }
  message += F("</select></td><td></td></tr>\r\n");

   server.sendContent(message) ;
  message = "" ;
  
  message += "<form method=post action=" + server.uri() + "><tr><td colspan=3></td></tr>\r\n" ; 



  message += "<form method=post action=" + server.uri() + "><tr><td></td><td></td><td></td></tr>" ; 

  message += F("<tr><td>Local UDP Port NTP</td><td align=center>") ; 
  message += "<input type='text' name='lpntp' value='" + String(ghks.localPort) + "' size=12></td><td><input type='submit' value='SET'></td></tr>\r\n";

  message += F("<tr><td>Local UDP Port Control</td><td align=center>") ; 
  message += "<input type='text' name='lpctr' value='" + String(ghks.localPortCtrl) + "' size=12></td><td></td></tr>\r\n";

  message += F("<tr><td>Remote UDP Port Control</td><td align=center>") ; 
  message += "<input type='text' name='rpctr' value='" + String(ghks.RemotePortCtrl) + "' size=12></td><td></td></tr>\r\n";

  message += F("<tr><td>Network SSID</td><td align=center>") ; 
  message += "<input type='text' name='nssid' value='" + String(ghks.nssid) + "' maxlength=15 size=12></td><td></td></tr>\r\n";

  message += F("<tr><td>Network Password</td><td align=center>") ; 
  message += "<input type='text' name='npass' value='" + String(ghks.npassword) + "' maxlength=15 size=12></td><td></td></tr>\r\n";

  message += F("<tr><td>Configure Password</td><td align=center>") ; 
  message += "<input type='text' name='cpass' value='" + String(ghks.cpassword) + "' maxlength=15 size=12></td><td>Min 8 Chars</td></tr>\r\n";

  message += F("<tr><td>Time Server</td><td align=center>") ; 
  message += "<input type='text' name='timsv' value='" + String(ghks.timeServer) + "' maxlength=23 size=12></td><td></td></tr>\r\n";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.RCIP[0],ghks.RCIP[1],ghks.RCIP[2],ghks.RCIP[3]);
  message += F("<tr><td>Configure AP</td><td align=center > ") ; 
  message += "<input type='text' name='rpcip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr></form>";
  server.sendContent(message) ;
  message = "" ;
  message += F("<tr><td colspan=3></td></tr>\r\n") ;




//  message += "<tr><td colspan=3 align='center'><form method=post action=" + server.uri() + "><input type='hidden' name='command' value='369'><input type='submit' value='Reset LoRa Parameters'></form></td></tr>\r\n" ; 
  
  server.sendContent(message) ;
  message = "" ;

  message += "<form method=postaction=" + server.uri() + "><tr><td colspan=3></td></tr>\r\n" ; 

  message += F("<tr><td><b>Network Options</b></td><td align=center>") ; 
  message += F("<select name='netop'>") ;
  if ((ghks.lNetworkOptions & 0x01 ) == 0 ){
    message += F("<option value='0' SELECTED>0 - DHCP"); 
    message += F("<option value='1'>1 - Static"); 
  }else{
    message += F("<option value='0'>0 - DHCP"); 
    message += F("<option value='1' SELECTED>1 - Static IP"); 
  }
  message += F("</select></td><td><input type='submit' value='SET'></td></tr>");
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPStatic[0],ghks.IPStatic[1],ghks.IPStatic[2],ghks.IPStatic[3]);
  message += F("<tr><td>Static IP Address</td><td align=center>") ; 
  message += "<input type='text' name='staip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPGateway[0],ghks.IPGateway[1],ghks.IPGateway[2],ghks.IPGateway[3]);
  message += F("<tr><td>Gateway IP Address</td><td align=center>") ; 
  message += "<input type='text' name='gatip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPMask[0],ghks.IPMask[1],ghks.IPMask[2],ghks.IPMask[3]);
  message += F("<tr><td>IP Mask</td><td align=center>") ; 
  message += "<input type='text' name='mskip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>";

  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", ghks.IPDNS[0],ghks.IPDNS[1],ghks.IPDNS[2],ghks.IPDNS[3]);
  message += F("<tr><td>DNS IP Address</td><td align=center>") ; 
  message += "<input type='text' name='dnsip' value='" + String(buff) + "' maxlength=16 size=12></td><td></td></tr>";

  message += "<tr><td>Last Scan Speed</td><td align=center>" + String(lScanLast) + "</td><td>(per second)</td></tr>"  ;    
  message += F("</form></table>");
  server.sendContent(message) ;
  message = "" ;

  SendHTTPPageFooter();
}


void IndicateReboot(void){
    tft.setTextSize(2);    
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(30, 32);
    tft.println("AUTO");
    tft.setCursor(50, 32);
    tft.println("REBOOT");
    tft.fillRect((int16_t)0, (int16_t)0, (int16_t)128, (int16_t)11,ST77XX_RED);
    tft.fillRect(0, 80, 128, 11, ST77XX_RED);
    ESP.restart() ;
}


String CPUSpeedTextLUT(int Option , int *speedvalue){
String tmpStr = "" ;

  switch(Option){
    case 0:
      tmpStr = "13 Mhz - Glacial - Danger Will Robinson" ;
      *speedvalue = 13 ;
    break;    
    case 1:
      tmpStr = "26 Mhz - Sloooow - Danger Will Robinson" ;
      *speedvalue = 26 ;
    break;    
    case 2:
      tmpStr = "80 Mhz - Slow - Low Power" ;
      *speedvalue = 80 ;
    break;    
    case 3:
      tmpStr = "160MHz" ;
      *speedvalue = 160 ;     
    break;  
    case 4:  
    default:
      tmpStr = "240MHz - Fast - High Power" ;
      *speedvalue = 240 ;
    break;    
  }
  return(tmpStr);  
}

String BandWidthText(uint8_t bw_no){
String tmpStr = "" ;  
  switch(bw_no){
    case 0:
      tmpStr = "7.8" ;
    break;
    case 1:
      tmpStr = "10.4" ;
    break;
    case 2:
      tmpStr = "15.6" ;
    break;
    case 3:
      tmpStr = "20.8" ;
    break;
    case 4:
      tmpStr = "31.25" ;
    break;
    case 5: 
      tmpStr = "41.7" ;
    break;
    case 6:
      tmpStr = "62.5" ;
    break;
    case 7:
      tmpStr = "125" ;
    break;
    case 8:
      tmpStr = "250" ;
    break;
    default:
      tmpStr = "500" ;
    break;
  }
  return(tmpStr);
}


