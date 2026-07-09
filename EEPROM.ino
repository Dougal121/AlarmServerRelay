

void LoadParamsFromEEPROM(bool bLoad){
long lTmp ;  
int i ;
int j ;
int bofs ,ofs ;
int eeAddress ;
  if ( bLoad ) {
    EEPROM.get(0,ghks);
    eeAddress = sizeof(ghks) ;
    Serial.println("read - ghks structure size " +String(eeAddress));   
    EEPROM.get(eeAddress,tplf);
    eeAddress += sizeof(tplf) ;
    Serial.println("read - tplf structure size " +String(sizeof(tplf)));   

    ghks.lNodeAddress = constrain(ghks.lNodeAddress,0,32768);
    ghks.localPort = constrain(ghks.localPort,1,65535);
    ghks.localPortCtrl = constrain(ghks.localPortCtrl,1,65535);
    ghks.RemotePortCtrl = constrain(ghks.RemotePortCtrl,1,65535);

    ghks.lDisplayOptions = constrain(ghks.lDisplayOptions,0,1);

    eeAddress = PROG_BASE ;   // move us up so ghks can have wiggle room in future versions
    Serial.println("read - total size " +String(eeAddress));   
    
  }else{
    ghks.lVersion  = MYVER ;
    EEPROM.put(0,ghks);
    eeAddress = sizeof(ghks) ;
    Serial.println("write - ghks structure size " +String(eeAddress));   

    EEPROM.put(eeAddress,tplf);
    eeAddress += sizeof(tplf) ;
    Serial.println("write - tplf structure size " +String(sizeof(tplf)));   

    eeAddress = PROG_BASE ; // move us up so ghks can have wiggle room in future versions
    Serial.println("write - total size " +String(eeAddress));   


    EEPROM.commit();                                                       // save changes in one go ???
  }
}
