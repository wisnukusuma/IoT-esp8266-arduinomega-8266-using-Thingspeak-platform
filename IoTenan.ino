
/* Arduino mega+esp8266 thingSpeak example  
 * Example name = "Write temperature and humidity to Thingspeak channel"
 * Created by Ilias Lamprou
 * Updated Oct 30 2016
 * 
 * Download latest Virtuino android app from the link:https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl
 * Video tutorial link: https://youtu.be/4XEe0HY0j6k
 * Contact address for questions or comments: iliaslampr@gmail.com
 */

//==================== Upload sensor values to ThingSpeak server with Arduino MEGA and ESP8266 module
#include <LiquidCrystal.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 7
const int rs = 22, en = 26, d4 = 28, d5 = 30, d6 = 32, d7 = 34;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String apiKey = "your WRITE API thingspeak";     // replace with your channel's thingspeak WRITE API key
String ssid="your ssid router";    // Wifi network SSID
String password ="your ssid password";  // Wifi network password
String tmp,hmd;
boolean DEBUG=true;

//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (Serial1.available()){
        c=Serial1.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value1, float value2){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  Serial1.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(Serial1.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  // getStr +="&field3=";
  // getStr += String(value3);
  // ...
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial1.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(Serial1.find(">")){
    Serial1.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    Serial1.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}
//================================================================================ setup
void setup() {                
    pinMode(24, OUTPUT);
  digitalWrite(24, LOW);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  DEBUG=true; // enable debug serial
  lcd.clear();
  lcd.print("Loading.");
  Serial.begin(9600); 
  Serial1.begin(115200);   // Your esp8266 module's speed is probably at 115200. 
  //Serial1.println("AT+UART_CUR=9600,8,1,0,0");         // Enable this line only if you want to change the ESP8266 baud rate to 9600
  //showResponse(1000);
  //Serial1.println("AT+RST");         // reset esp8266
  //showResponse(1000);
  Serial1.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);
  lcd.clear();
  lcd.print("Loading..");
  Serial1.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);
  lcd.clear();
  lcd.print("Loading...");
  delay(500);
  if (DEBUG)  Serial.println("Setup completed");
  lcd.clear();
  lcd.print("Conected");
  delay(1000);
}
// ====================================================================== loop
void loop() {
  // Read sensor values
  lcd.clear();
  int chk = DHT.read11(DHT11_PIN);
  float t = DHT.temperature;
  float h = DHT.humidity;
  tmp = DHT.temperature;
  hmd = DHT.humidity;
  Serial.print("Temperature = ");
  Serial.println(t);
  Serial.print("Humidity = ");
  Serial.println(h);
  lcd.print("temperature="+tmp);
  lcd.setCursor(0, 1);
  lcd.print("humidity="+hmd);      
  thingSpeakWrite(t,h);
  // Write values to thingspeak 
  // thingspeak needs 15 sec delay between updates,     
  delay(15000);  
}
