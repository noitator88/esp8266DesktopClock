#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define WIFISSID "Noitator88"
#define PASSWD "gracefulriver243"

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 15, /* reset=*/ 16); // Feather HUZZAH ESP8266, E=clock=14, RW=data=13, RS=CS

unsigned long lastTime = 0;
unsigned long timerDelay = 1510;

HTTPClient http;
WiFiClient wclient;
//Ticker blinker;

String jsonBuffer;

String httpGETRequest(const char* serverName) {    
  // Your IP address with path or Domain name with URL path 
  http.begin(wclient, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void setup(void) {
  // LCD setup
  u8g2.begin(); 
  u8g2.setDisplayRotation(U8G2_R2);
  u8g2.setFont(u8g2_font_wqy14_t_gb2312);
  // wifi setup
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop(void) { 
//  String city, weather, temp1, temp2;
  const char* disp_to = " ~ ";

  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      // fetch the weather information
      String serverPath = "http://www.weather.com.cn/data/cityinfo/101270101.html";
      Serial.println(serverPath);
      jsonBuffer = httpGETRequest( serverPath.c_str() );
      Serial.println(jsonBuffer);
      // parse the json data
      JSONVar myObject = JSON.parse(jsonBuffer);
      Serial.print("JSON object = ");
      Serial.println(myObject);
      // city name
      const char* city = (const char*) myObject["weatherinfo"]["city"];
      Serial.println(city);
      // temp1 ~ temp2
      const char* temp1 = (const char*) myObject["weatherinfo"]["temp1"];
      const char* temp2 = (const char*) myObject["weatherinfo"]["temp2"];
      Serial.println(temp1);
      Serial.println(temp2);
      // weather
      const char* weather = (const char*) myObject["weatherinfo"]["weather"];
      Serial.println(weather);
      // display in the LCD
      u8g2.firstPage();
      do {
        u8g2.drawUTF8(0, 12, temp1 );
        u8g2.drawUTF8(28, 12, "~"); 
        u8g2.drawUTF8(38, 12, temp2 );
        u8g2.drawUTF8(70, 12, weather );
      } while ( u8g2.nextPage() );
    }
    lastTime = millis();
   }
    
}
