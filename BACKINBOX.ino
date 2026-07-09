void BackInTheBoxMemory(){
int i  ;  
  sprintf(ghks.nssid,"************\0");  // put your default credentials in here if you wish
  sprintf(ghks.npassword,"********\0");  // put your default credentials in here if you wish

  
  ghks.lNodeAddress = chipid & 0xff ;
  ghks.localPort = 2390 ;
  ghks.localPortCtrl = 8088 ;
  ghks.RemotePortCtrl= 8089 ;
  ghks.lVersion = MYVER ;
  ghks.lShowRelays = 1 ;
  ghks.gmtOffset_sec = 10*3600; 
  ghks.daylightOffset_sec = 3600; 
  strncpy(ghks.timeServer, "au.pool.ntp.org", sizeof(ghks.timeServer));
  ghks.timeServer[sizeof(ghks.timeServer) - 1] = '\0';  // Ensure null termination
//  sprintf(ghks.timeServer ,"au.pool.ntp.org\0"); 
   tplf.SelfReBoot = 0 ;
   tplf.lRebootTimeDay = 0 ;
    
 // sprintf(ghks.RCIP ,"192.168.004.001\0"); 
  
  ghks.RCIP[0] = 192 ;
  ghks.RCIP[1] = 168 ;
  ghks.RCIP[2] = 4 ;
  ghks.RCIP[3] = 1 ;


  ghks.lNetworkOptions = 0 ;     // DHCP 
  ghks.IPStatic[0] = 192 ;
  ghks.IPStatic[1] = 168 ;
  ghks.IPStatic[2] = 1 ;
  ghks.IPStatic[3] = 123 ;

  ghks.IPGateway[0] = 192 ;
  ghks.IPGateway[1] = 168 ;
  ghks.IPGateway[2] = 1 ;
  ghks.IPGateway[3] = 1 ;

  ghks.IPDNS = ghks.IPGateway ;

  ghks.IPMask[0] = 255 ;
  ghks.IPMask[1] = 255 ;
  ghks.IPMask[2] = 255 ;
  ghks.IPMask[3] = 0 ;

  tplf.RelayPin[0] = 17 ;
  tplf.RelayPin[1] = 16 ;
  tplf.ActiveValue[0] = 1  ;
  tplf.ActiveValue[1] = 1  ;
  tplf.MaxTimer[0] = 30 ; 
  tplf.MaxTimer[1] = 30 ; 
  
}
