/*
 * Http Alarm Server for TP-Link NVR esp8266 and esp32 with SD
 *
 * AUTHOR:  Dougal Plummer
 *
*               First Very Rough Hack
 *
 *Shared SPI pins
Function	ESP32	Notes
SCK	18	shared
MOSI	23	shared
MISO	19	only SD uses this
GND	GND	shared
3.3V	3.3V	shared


ST7735S display (128×170)
Signal	ESP32 GPIO
CS	5
DC	2
RST	4
BL	3.3V or PWM pin
SCK	18
MOSI	23
MISO	unused

SD Card module (SPI mode)
Signal	ESP32 GPIO
CS	15
SCK	18
MOSI	23
MISO	19
VCC	3.3V (NOT 5V unless level-shifted!)

 *
 */
#define BUILD_TIME (__DATE__ " " __TIME__)
#define MYVER 0x12435688         // change this if you change the structures that hold data that way it will force a "backinthebox" to get safe and sane values from eeprom
#define MYVER_NEW 0x12345680     // change this if you change the structures that hold data that way it will force a "backinthebox" to get safe and sane values from eeprom
const int PROG_BASE = 320 ;      // where the irrigation valve setup and program information starts in eeprom
#define ESP32_BUILTIN_LED 35
const byte MAX_WIFI_TRIES = 45 ;

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Display pins
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

// SD card chip select
#define SD_CS   15

#include "Wire.h"
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined ESP32
#include <WiFi.h>
//#include "lwip/apps/sntp.h"
//#include "SPIFFS.h"
#include "time.h"
#endif
#include <EEPROM.h>            // Internal chip eeprom
#include "StaticPages.h"

#include <WiFi.h>              // All creature great and small as long as they are WiFi based 
#include <WiFiClient.h>        // from the "standard book of spells" (Arduino lib manager)
#include <WebServer.h>         // what it says on the box
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#include <WiFiUDP.h>           // UDP protocol driver
#include <Update.h>            // this is the OTA update code ?

//#include "SPI.h"
//#include "SdFat.h"
//SdFat sd;
//#include <MultiFTPServer.h>

//#include <U8g2lib.h>


#define MIN_REBOOT  720             //   720   12 hours normally      10 min for testing
#define BUFF_MAX 64
char buff[BUFF_MAX]; 

byte rtc_sec = 0 ;
byte rtc_min = 0 ;
byte rtc_hour = 0  ;
uint8_t rtc_status ;
float rtc_temp ;
long lScanCtr = 0 ;
long lScanLast = 0 ;
long lMinUpTime = 0 ; 
long lRebootCode = 0 ;
bool bPrevConnectionStatus = false;
unsigned long lTimeNext = 600000 ;           // next network retry

char Toleo[10] = {"Ver 1.1\0"}  ;
char cssid[32] = {"FTP_RELAY_XXXXXXXX\0"} ;
char *host = "FTP_RELAY_00000000\0";                // overwrite these later with correct chip ID

IPAddress MyIP ;
IPAddress MyIPC  ;


typedef struct __attribute__((__packed__)) {     // eeprom stuff
  unsigned int localPort = 2390;          // 2 local port to listen for NTP UDP packets
  unsigned int localPortCtrl = 8666;      // 4 local port to listen for Control UDP packets
  unsigned int RemotePortCtrl = 8664;     // 6 local port to listen for Control UDP packets
  long lNodeAddress ;                     // 22

  long  gmtOffset_sec = 10*3600;          // Adjust for your timezone offset in seconds
  int   daylightOffset_sec = 3600;        // Daylight saving offset in seconds
  IPAddress RCIP ;                         // (192,168,4,1)  30
  char NodeName[32] ;                     // 46
  char nssid[24] ;                        // 62
  char npassword[32] ;                    // 78
  uint8_t cpufreq ;                       //    240 160 80   not flash at 26
  uint8_t lDisplayOptions  ;              // 83
  uint8_t lNetworkOptions  ;              // 84
  uint8_t lShowRelays ;                   // 85    value between 1 and max_relay
  uint8_t lSpare2  ;                      // 86
  char timeServer[32] ;                   // 110   = {"au.pool.ntp.org\0"}
  char cssid[24] ;                        // 62
  char cpassword[32] ;                    // 126
  long lVersion  ;                        // 130
  IPAddress IPStatic ;                    // (192,168,0,123)   134
  IPAddress IPGateway ;                   // (192,168,0,1)     138 
  IPAddress IPMask ;                      // (255,255,255,0)   142
  IPAddress IPDNS ;                       // (192,168,0,15)    146
 } general_housekeeping_stuff_t ;         // computer says it's    ??? is my maths crap ????
general_housekeeping_stuff_t ghks ;       // 252 bytes 


#define MAX_RELAY 2
typedef struct __attribute__((__packed__)) {     // eeprom stuff
  long RebootInterval ;                   // intervale in minutes between reboots
  long MinRecycleTime ;                   // Minimum interval in minutes between Reboots
  long ReBootOffTime ;                    // Off time in seconds of the reboot  
  uint8_t RelayPin[MAX_RELAY]  ;
  uint8_t ActiveValue[MAX_RELAY]  ;
  uint32_t MaxTimer[MAX_RELAY];
  int  ReBootOption ;
  byte ReBoot_sec   ;
  byte ReBoot_min   ;
  byte ReBoot_hour   ;
  byte ReBoot_wdays   ;
  long lRebootTimeDay;
  IPAddress IPPing  ;
  long PingMax ;
  long PingFreq ;
  long SelfReBoot ;
  int  SMTP_Port;
  char SMTP_Server[48] ;
  char SMTP_User[48] ;
  char SMTP_Password[48] ;
  char SMTP_FROM[48] ;
  char SMTP_TO[48] ;
  char SMTP_CC[48] ;
  char SMTP_BCC[48] ;
  bool SMTP_bSecure ;
  char SMTP_Message[64] ;
  char SMTP_Subject[64] ;
  bool bUseEmail ;
  bool bSPARE ;   
  char dyndns_user[32];
  char dyndns_pass[32];
  char dyndns_host[32];
} tplf_e_t ;

tplf_e_t tplf ;


const int button = 0;    //Push Button address / used to wake up wifi on remote nodes
const int MAX_EEPROM = 4000 ;
char dayarray[8] = {'S','M','T','W','T','F','S','E'} ;

WebServer server(80);
//FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

struct tm timeinfo;
uint64_t chipid = 0 ;
int iUploadPos = 0 ;
int iValveLogTTG = 60 ;
long  MyCheckSum ;
long  MyTestSum ;
uint32_t RelayTimers[MAX_RELAY];
long lTD ;
bool bSNTPState = true ;
String strLastAlarm ="";
String strLastDate ="";

/*
void _callback(FtpOperation ftpOperation, uint32_t freeSpace, uint32_t totalSpace){
  switch (ftpOperation) {
    case FTP_CONNECT:
      Serial.println(F("FTP: Connected!"));
      break;
    case FTP_DISCONNECT:
      Serial.println(F("FTP: Disconnected!"));
      break;
    case FTP_FREE_SPACE_CHANGE:
      Serial.printf("FTP: Free space change, free %u of %u!\n", freeSpace, totalSpace);
      break;
    default:
      break;
  }
};

void _transferCallback(FtpTransferOperation ftpOperation, const char* name, uint32_t transferredSize){
  switch (ftpOperation) {
    case FTP_UPLOAD_START:
      Serial.println(F("FTP: Upload start!"));
      break;
    case FTP_UPLOAD:
      Serial.printf("FTP: Upload of file %s byte %u\n", name, transferredSize);
      break;
    case FTP_TRANSFER_STOP:
      Serial.println(F("FTP: Finish transfer!"));
      break;
    case FTP_TRANSFER_ERROR:
      Serial.println(F("FTP: Transfer error!"));
      break;
    default:
      break;
  }
 */ 
 /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */	
//};

void setup(void){


  lRebootCode = random(1,+2147483640) ;   // want to change it straight away
  chipid=ESP.getEfuseMac();               //The chip ID is essentially its MAC address(length: 6 bytes).

  Serial.begin(115200);
  EEPROM.begin(MAX_EEPROM);
  LoadParamsFromEEPROM(true);
  
  SPI.begin(18, 19, 23);   // SCK, MISO, MOSI (VSPI defaults)

   if ( MYVER != ghks.lVersion ) {
//  if ( false ) {
    Serial.println("Mem Ver" + String(ghks.lVersion,HEX)+" Firmware Ver " + String(MYVER,HEX)+" Loading memory defaults...");
    BackInTheBoxMemory();         // load defaults if blank memory detected but dont save user can still restore from eeprom
    ghks.lVersion = MYVER  ; 
    delay(200);
  }

  for (int i ; i < MAX_RELAY ; i++ ){     // configure and set relays ASAP
    pinMode(tplf.RelayPin[i], OUTPUT);
    digitalWrite(tplf.RelayPin[i],!tplf.ActiveValue[i]);
    RelayTimers[i] = 0 ; 
  }

  // Initialize display
  tft.initR(INITR_BLACKTAB);// yep it has a green tab IRL
//  tft._colstart = 1;
//  tft._rowstart = 1;   // fixes 1-pixel white top line
  tft.setCursor(0, 0);
  tft.setRotation(2);   // Portrait orientation
  tft.fillScreen(ST77XX_BLACK);

  // Simple message to confirm display is working
  tft.setCursor(0, 150);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.println("Display OK");
  delay(100);
  StartWiFi();
 
  /* Endpoint to receive alarms
  server.on("/alarm", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    Serial.println("---- ALARM RECEIVED ----");

    // Print raw POST data
    String body;
    for (size_t i = 0; i < len; i++) body += (char)data[i];
    Serial.println(body);

    // TODO: parse JSON here (see below)

    Serial.println("-------------------------");
  });*/
  server.on("alarm", HTTP_POST, handleAlarm); //
  server.on("/alarm", HTTP_POST, handleAlarm); //
  server.on("/", handleRoot);
  server.on("/setup", handleSetup);
  server.on("/scan", i2cScan);
  server.on("/stime", handleRoot);
  server.on("/info", handleInfo);
  server.on("/eeprom", DisplayEEPROM);
  server.on("/backup", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_GET , handleBackup);
  server.on("/backup.txt", HTTP_POST,  handleRoot, handleFileUpload); 
  server.on("/login", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
    Serial.printf("Display Login Page");
  });
  server.on("/update", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", updatePage);
  });
  server.on("/update", HTTP_POST, []() {   //handling uploading firmware file
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {   // flashing firmware to ESP
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  server.onNotFound(handleNotFound);  
 // server.setTimeout(200);
  server.begin();
  Serial.println("HTTP server started");


  // Initialize NTP
  if (WiFi.isConnected()){
  // Set update every 24 hours

    configTime(ghks.gmtOffset_sec, ghks.daylightOffset_sec, ghks.timeServer);

    // Wait for time to be set
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }else{
      // Print current time
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
  }

  /////FTP Setup, ensure SPIFFS is started before ftp;  /////////

/*
  /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
  // Initialize SD card
  if (!sd.begin(SD_CS, SD_SCK_MHZ(5))) {   //, SPI  SD
    Serial.println("SD card initialization failed!");
    tft.setCursor(10, 40);
    tft.setTextColor(ST77XX_RED);
    tft.println("SD Init Fail");
  } else {
    Serial.println("SD card initialized.");
    tft.setCursor(10, 40);
    tft.setTextColor(ST77XX_GREEN);
    tft.println("SD OK");

//    ftpSrv.setCallback(_callback);
//    ftpSrv.setTransferCallback(_transferCallback);
//    ftpSrv.begin("","","TPLink Relay");    //username, password for ftp.   (default 21, 50009 for PASV
    
  }
*/
}

void loop(void){
long lRet ;
long lTime = millis() ;

  getLocalTime(&timeinfo);
//  ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!
  server.handleClient();
  lScanCtr++ ;

  WiFiClient client = server.client();
  if (client) {
      if (!client.connected() || !client.available()) {
          // Stale or half-open connection
          client.stop();
      }
  }

  if ( rtc_sec != timeinfo.tm_sec){
    tft.setCursor(0, 0);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_GREEN);
    for (int i = 0  ; i < MAX_RELAY ; i++ ){
      if (RelayTimers[i]> 0 ){
        RelayTimers[i]-- ;
        if (RelayTimers[i]== 0 ){   // now zero
          digitalWrite(tplf.RelayPin[i],!tplf.ActiveValue[i]);
        }
      }
      else{

      }
      tft.setCursor(0, 80+(i*15));
      tft.println("Rly "+ String(i) + " Val " + digitalRead(tplf.RelayPin[i]) +  " Tmr " + String(RelayTimers[i]) );
    }
    tft.setTextColor(ST77XX_BLUE);
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday , timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    tft.setCursor(0, 60);
    tft.println(buff);
    tft.setCursor(0, 70);
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);            
    tft.println(buff);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(0, 0);
    tft.println(strLastAlarm);
    tft.setCursor(0, 15);
    tft.println(strLastDate);

    if (WiFi.isConnected())  {
      digitalWrite(ESP32_BUILTIN_LED,!digitalRead(ESP32_BUILTIN_LED));
    }else{
      if (( rtc_sec % 4 ) == 0 )    {
        digitalWrite(ESP32_BUILTIN_LED,true);        
      }else{
        digitalWrite(ESP32_BUILTIN_LED,false);                
      }
    }
    rtc_sec = timeinfo.tm_sec ;
  }



  lScanLast = lScanCtr ;
  lScanCtr = 0 ;

  if (rtc_min != timeinfo.tm_min){
    lMinUpTime++;
    rtc_min = timeinfo.tm_min ;
  }


  if (rtc_hour != timeinfo.tm_hour){

    rtc_hour = timeinfo.tm_hour;
  }

  if ((lMinUpTime > 10  )){
    snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday , timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    if ( !bPrevConnectionStatus && WiFi.isConnected() ){
       Serial.println(String(buff )+ " WiFi Reconnected OK...");  
       MyIP =  WiFi.localIP() ;
       snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);            
       Serial.println(buff);      
    }
    if (!WiFi.isConnected())  {
      lTD = (long)lTimeNext-(long) millis() ;
      if (( abs(lTD)>40000)||(bPrevConnectionStatus)){ // trying to get roll over protection and a 30 second retry
        lTimeNext = millis() - 1 ;
      }
      bPrevConnectionStatus = false;
      if ( lTimeNext < millis() ){
        Serial.println(String(buff )+ " Trying to reconnect WiFi ");
        WiFi.disconnect(false);
  //      Serial.println("Connecting to WiFi...");
        WiFi.mode(WIFI_AP_STA);
        if ( ghks.lNetworkOptions != 0 ) {            // use ixed IP
          WiFi.config(ghks.IPStatic, ghks.IPGateway, ghks.IPMask, ghks.IPDNS );
        }
        if ( ghks.npassword[0] == 0 ) {
          WiFi.begin((char*)ghks.nssid);                    // connect to unencrypted access point
        } else {
          WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);  // connect to access point with encryption
        }
        lTimeNext = millis() + 30000 ;
      }
    }else{
      bPrevConnectionStatus = true ;
    }  
  }

}


void SetSelectedSpeed(void){
  switch(ghks.cpufreq){
    case 13:
      setCpuFrequencyMhz(13);
    break;
    case 26:
      setCpuFrequencyMhz(26);
    break;
    case 80:
      setCpuFrequencyMhz(80);
    break;
    case 160:
      setCpuFrequencyMhz(160);
    break;
    default:
      setCpuFrequencyMhz(240);
    break;    
  }
//  Serial.println("CPU commanded " +String(ghks.cpufreq) + " actual " + String(getCpuFrequencyMhz())+ " MHz");
//  Serial.println("CPU commanded " +String(ghks.cpufreq)+ " MHz");
}

bool StartWiFi(void){
  int i , k , j = 0; 
  
  WiFi.disconnect();
  Serial.println("Configuring soft access point...");
  WiFi.setTxPower(WIFI_POWER_19_5dBm);    // Set WiFi RF power output to highest level  // 
  WiFi.mode(WIFI_AP_STA);  // we are having our cake and eating it eee har
  if ( cssid[0] == 0 || cssid[1] == 0 ){   // pick a default setup ssid if none
    sprintf(ghks.cpassword,"\0");
    sprintf(ghks.cssid,"GunSmoke_%08X\0",chipid);
  }
//  MyIPC =  IPAddress (192, 168, 4 , 1);   //5 +(chipid & 0x7f )  
  MyIPC =  IPAddress (ghks.RCIP[0], ghks.RCIP[1], ghks.RCIP[2] , ghks.RCIP[3]);   //5 +(chipid & 0x7f )  ghks.RCIP ; //
//  MyIPC = ghks.RCIP ; 
  Serial.print("Asking for Soft AP on address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);      
  Serial.println(buff);
  WiFi.softAPConfig(MyIPC,MyIPC,IPAddress (255, 255, 255 , 0));  
  Serial.println("Starting access point...");
  Serial.print("Host SSID: ");
  Serial.println(ghks.cssid);

  if (( ghks.cpassword[0] == 0 ) || ( ghks.cpassword[0] == 0xff)  || ( strlen((char*)ghks.cpassword) < 8 )) {
    Serial.println("OPEN AP (password invalid)");
    WiFi.softAP((char*)ghks.cssid);                   // no passowrd
  }else{
    WiFi.softAP((char*)ghks.cssid,(char*) ghks.cpassword);
    Serial.print("Password: >");
    Serial.println(String(ghks.cpassword) + "<");
  }
  MyIPC = WiFi.softAPIP();  // get back the address to verify what happened
  Serial.print("Soft AP IP Started on address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);      
  Serial.println(buff);
  
  if ( ghks.lNetworkOptions != 0 ) {
    WiFi.config(ghks.IPStatic,ghks.IPGateway,ghks.IPMask,ghks.IPDNS ); 
  }  
  Serial.print("Attaching to Client SSID: ");
  Serial.println(ghks.nssid);
  Serial.print("Password: >");
  Serial.print(ghks.npassword);
  Serial.println("< ");

  if ( ghks.npassword[0] == 0 ){
    WiFi.begin((char*)ghks.nssid);                            // connect to unencrypted access point      
  }else{
    WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);     // connect to access point with encryption
  }
  while (( WiFi.status() != WL_CONNECTED ) && ( j < MAX_WIFI_TRIES )) {
    j = j + 1 ;
    delay(500);
    Serial.print(".");

    tft.setTextWrap(false);
    tft.setTextSize(1);    
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_BLUE);
    tft.setCursor(0, 0);
    tft.println("RSSI  -> " + String(WiFi.RSSI()));
    tft.setCursor(0, 9);
    tft.println("SSID: "+String(ghks.nssid));
    tft.setCursor(0, 18);
    tft.println("Password: "+String(ghks.npassword));

    tft.setCursor(j*4,27);
    tft.print(">") ;
    tft.setCursor(0, 36 );
    tft.print(String(1.0*j/2) + String(" (s)" ));   
    snprintf(buff, BUFF_MAX, ">>  IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);            
    tft.setCursor(0, 54);
    tft.println(buff);


    digitalWrite(ESP32_BUILTIN_LED,!digitalRead(ESP32_BUILTIN_LED));
  } 
  if ( j >= MAX_WIFI_TRIES ) {
     WiFi.disconnect();
     Serial.println("");
     Serial.println("WiFi failed to connect");  
     tft.setCursor(0, 83);
     tft.println("WiFi failed to connect...");
  }else{
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.print("IP address: ");
    MyIP =  WiFi.localIP() ;
    snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);            
    Serial.println(buff);
    tft.setCursor(0, 53);
    tft.println(buff);
    tft.setCursor(0, 83);
    tft.println("Connected...");
    bPrevConnectionStatus = true ;
  }
  return(true);
}


bool StopWiFi(void){
  if (lMinUpTime > 60 ) {
    WiFi.disconnect(true);
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi OFF");  
  }else{
    Serial.println("WiFi Wants to go OFF");      
  }
}
