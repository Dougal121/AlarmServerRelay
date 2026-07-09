

void handleAlarm() {
  Serial.println("---- ALARM RECEIVED ----");

  // Read raw POST body
  String json = server.arg("event");
  if (json.length() < 10 )
    json = server.arg("plain");
  Serial.println(json);
/*  String message = F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  Serial.println(message);*/
  // Send response
  server.sendHeader("Connection", "close");   // <-- here
  server.send(200, "text/plain", "OK");
  Serial.println("-------------------------");

  int subtype = extractSubType(json);
  Serial.println("Subtype " + String(subtype) + " " + subtypeToString(subtype));
  strLastAlarm = String(subtype) + " " + subtypeToString(subtype);
  snprintf(buff, BUFF_MAX, "%04d/%02d/%02d %02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday , timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  strLastDate = String(buff) ; 
  if (subtype>=21){
    if (RelayTimers[0]== 0){
      digitalWrite(tplf.RelayPin[0],tplf.ActiveValue[0]);
      RelayTimers[0]= tplf.MaxTimer[0]*60;
    }
    else{
      digitalWrite(tplf.RelayPin[1],tplf.ActiveValue[1]);
      digitalWrite(tplf.RelayPin[0],tplf.ActiveValue[0]);
      RelayTimers[0]= tplf.MaxTimer[0]*60;
      RelayTimers[1]= tplf.MaxTimer[1]*60;
    }
  }

}

int extractSubType(const String& json)
{
    int start = json.indexOf("\"sub_type\"");
    if (start < 0) return -1;

    start = json.indexOf('[', start);
    if (start < 0) return -1;

    start++; // move past '['

    int end = json.indexOf(']', start);
    if (end < 0) return -1;

    return json.substring(start, end).toInt();
}


String subtypeToString(int subtype)
{
    switch (subtype)
    {
        case 2:   return "Motion Detected";
        case 4:   return "Line Crossing";
        case 6:   return "Region Entering";
        case 7:   return "Region Exiting";
        case 14:  return "Abnormal Sound";
        case 21:  return "Human Identification";
        case 22:  return "Vehicle Identification";
        case 101: return "Human (classification)";
        case 102: return "Vehicle (classification)";

        default:  return "Unknown Event";
    }
}


const char* subtypeToText(int subtype)
{
    switch (subtype)
    {
        case 2:   return "Motion Detected";
        case 4:   return "Line Crossing";
        case 6:   return "Region Entering";
        case 7:   return "Region Exiting";
        case 14:  return "Abnormal Sound";
        case 21:  return "Human Identification";
        case 22:  return "Vehicle Identification";
        case 101: return "Human (classification)";
        case 102: return "Vehicle (classification)";

        default:  return "Unknown Event";
    }
}
