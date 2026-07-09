
bool IsValidRelayPin(uint8_t RelayPin){
  bool bRet = false ;
#if defined(ESP8266) 
  switch (RelayPin){
    case 0: 
    case 2: 
    case 4: 
    case 5: 
    case 12: 
    case 13: 
    case 15: 
    case 16: bRet = true ; break ;
    default:  
    break;
  }
#endif  

#if defined(ESP32)
  switch (RelayPin){
    case 12: 
    case 13: 
    case 14: 
    case 16:          // left
    case 17:          // right
    case 18: 
    case 19: 
    case 2:            // analog or digital 
    case 4: 
    case 34: 
    case 35: 
    case 36: 
    case 39: 
    case 23: 
    case 25: 
    case 26: 
    case 27: bRet = true ; break ;
    default:  
    break;
  }
#endif  
  return(bRet);
}
