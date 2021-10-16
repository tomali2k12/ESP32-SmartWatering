#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Oled Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Wifi Setup
WiFiMulti WiFiMulti;

// Time Settings
const long utcOffsetInSeconds = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Endpoint
String serverName = "http://10.20.30.56:3000/users";
void setup()
{
    setup_serial();
    setup_oled();
    delay(5000);
    setup_wifi();
    delay(5000);
    setup_time();
}
void loop()
{
    write_oled("Hello");
    delay(2000);
    post_values_server(15, 80, 123, 125);
    Serial.print(getCurrentTime());
    Serial.println();
}

void setup_serial()
{
    Serial.begin(115200);
    while (!Serial)
        ;
}

void setup_time()
{
    timeClient.begin();
}

String getCurrentTime()
{
  timeClient.update();
  return timeClient.getFormattedDate();
  
}
void setup_wifi(){
    WiFiMulti.addAP("smart-water", "password");
    
    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");
    write_oled("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        write_oled(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    write_oled("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void post_values_server(float temp, float humm, int analog_reading, int moist)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverName);

        http.addHeader("Content-Type", "application/json");

        StaticJsonDocument<200> doc;

        doc["sensor"] = "A0";
        doc["analog_reading"] = analog_reading;
        doc["time_read"] = getCurrentTime();
        doc["temperature"] = temp;
        doc["humidity"] = humm;
        doc["moisture"] = moist;

        String json_body;
        serializeJson(doc, json_body);

        int httpResponseCode = http.POST(json_body);
        Serial.print("HTTP Response code: ");
        Serial.println(json_body);
        
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}
void setup_oled()
{
    Serial.println("Setting up Oled");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    delay(2000);
    display.clearDisplay();
    //display.invertDisplay(true);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Booting....");
    display.display();
}

void write_oled(String oled_message){
    display.println(oled_message);
    display.display();
}
/*
void oled_shiftRows(String new_line, String *original_lines[]){
    String *new_lines[] = {"","","",""};
    display.setCursor(0, 0);
    display.println("Booting....");
    for (int i = 0; i < 3; i++) {
        new_lines[i] = original_lines[i+1];
    }
    original_lines[4] = new_line;
    display.display();
}
*/