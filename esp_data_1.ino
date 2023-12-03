// Include required libraries
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "Arduino.h"
#include "FSR.h"

// Pin Definitions
#define FSR_1_PIN_1 A10
#define FSR_2_PIN_1 A0
#define FSR_3_PIN_1 A3
#define FSR_4_PIN_1 A13
#define FSR_5_PIN_1 A11
#define FSR_6_PIN_1 A7

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;
// WiFi credentials
const char *ssid = "SKYNET 4G";       // change SSID
const char *password = "jobitjos";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbx1Pu6eWQTe_IWIVRtlX2mq4vwl7k48hpic--05GHx1scr1Cf6J_G04s4wUmZUx-gAj"; // ESP_data sheet Gscript ID
int count = 0;

// Global variables and defines
String blehm10Str = "";
// Object initialization
HardwareSerial &blehm10 = Serial1;
FSR fsr_1(FSR_1_PIN_1);
FSR fsr_2(FSR_2_PIN_1);
FSR fsr_3(FSR_3_PIN_1);
FSR fsr_4(FSR_4_PIN_1);
FSR fsr_5(FSR_5_PIN_1);
FSR fsr_6(FSR_6_PIN_1);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; // 50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time: ");
    Serial.println(asString);

    float fsr_1Force = fsr_1.getForce();
    float fsr_2Force = fsr_2.getForce();
    float fsr_3Force = fsr_3.getForce();
    float fsr_4Force = fsr_4.getForce();
    float fsr_5Force = fsr_5.getForce();
    float fsr_6Force = fsr_6.getForce();

    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" +
                      "time=" + asString +
                      "&fsr_1=" + String(fsr_1Force) +
                      "&fsr_2=" + String(fsr_2Force) +
                      "&fsr_3=" + String(fsr_3Force) +
                      "&fsr_4=" + String(fsr_4Force) +
                      "&fsr_5=" + String(fsr_5Force) +
                      "&fsr_6=" + String(fsr_6Force);
    Serial.print("POST data to spreadsheet: ");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    // Getting response from Google Sheet
    String payload;
    if (httpCode > 0)
    {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;
  delay(1000);
}