// Business card
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>
#include "time.h"

#define LED_PIN 10
#define BUTTON_PIN 6

#define LED_COUNT 47

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiManager wm;

const char* ntpServer = "0.ro.pool.ntp.org";
const long  gmtOffset_sec = 7200;
int   daylightOffset_sec = 3600;
bool secOn = false;
volatile bool btnPush = false;
volatile int cntPush = 0;
//variables to keep track of the timing of recent interrupts
volatile unsigned long button_time = 0;
volatile unsigned long last_button_time = 0;

uint32_t color;
struct tm timeinfo;

void IRAM_ATTR buttonPressed() {
  button_time = millis();
  if (button_time - last_button_time > 250) {
    if (cntPush == 6) {
      cntPush = 0;
    }
    else {
      cntPush++;
    }
    btnPush = true;
    last_button_time = button_time;
  }
}

void setup() {
  // put your setup code here, to run once:
  // initialize strip
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip.setPixelColor(0, strip.Color(255,  127,  63));
  strip.show();
  // initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // initialize WiFi
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP 
  // initalize WiFi Manager
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu);
  wm.setClass("invert"); // dark mode
  wm.setConnectTimeout(120);
  wm.setConfigPortalTimeout(120);
  bool res;
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(3000);
    if (digitalRead(BUTTON_PIN) == LOW) {
      wm.resetSettings();
      ESP.restart();
    }
    wm.setConfigPortalTimeout(120);
    if (!wm.startConfigPortal("OnDemandAP","password")) {
      delay(3000);
      //ESP.restart();
    } else {
      //if you get here you have connected to the WiFi
      // get NTP time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
  }
  else {
    res = wm.autoConnect("AutoConnectAP","password");
    if (!res) {
      //ESP.restart();
    }
    else {
      //if you get here you have connected to the WiFi
      // get NTP time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
  }
  attachInterrupt(BUTTON_PIN, buttonPressed, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  // check if button was pressed
  if (btnPush) {
    // set color
    switch (cntPush) {
      case 0:
        color = strip.Color(127, 127, 127);
        break;
      case 1:
        color = strip.Color(127, 0, 0);
        break;
      case 2:
        color = strip.Color(0, 127, 0);
        break;
      case 3:
        color = strip.Color(0, 0, 127);
        break;
      case 4:
        color = strip.Color(127, 127, 0);
        break;
      case 5:
        color = strip.Color(127, 0, 127);
        break;
      case 6:
        color = strip.Color(0, 127, 127);
        break;
      default:
        //default to white
        color = strip.Color(127, 127, 127);
        break;
    }
    btnPush = false;
  }
  // actual app
  if(!getLocalTime(&timeinfo)){
    // failed to get time
  }
  else {
    secOn = !secOn;
    // set PM indicator
    if (timeinfo.tm_hour >= 12) {
      strip.setPixelColor(0, color);
    }
    else {
      strip.setPixelColor(0, strip.Color(0, 0, 0));
    }
    // hour tens - 1 or none
    if ((timeinfo.tm_hour == 0) || (timeinfo.tm_hour == 10) || (timeinfo.tm_hour == 11) || (timeinfo.tm_hour == 12) || (timeinfo.tm_hour == 22) || (timeinfo.tm_hour == 23)) {
      for (int i=1; i<=5; i++) {
        strip.setPixelColor(i, color);
      }
    }
    else {
      for (int i=1; i<=5; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    // hour units - 0
    if ((timeinfo.tm_hour == 22) || (timeinfo.tm_hour == 10)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 1
    else if ((timeinfo.tm_hour == 23) || (timeinfo.tm_hour == 11) || (timeinfo.tm_hour == 13) || (timeinfo.tm_hour == 1)) {
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 2
    else if ((timeinfo.tm_hour == 0) || (timeinfo.tm_hour == 12) || (timeinfo.tm_hour == 14) || (timeinfo.tm_hour == 2)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 3
    else if ((timeinfo.tm_hour == 3) || (timeinfo.tm_hour == 15)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 4
    else if ((timeinfo.tm_hour == 4) || (timeinfo.tm_hour == 16)) {
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 5
    else if ((timeinfo.tm_hour == 5) || (timeinfo.tm_hour == 17)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 6
    else if ((timeinfo.tm_hour == 6) || (timeinfo.tm_hour == 18)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 7
    else if ((timeinfo.tm_hour == 7) || (timeinfo.tm_hour == 19)) {
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 8
    else if ((timeinfo.tm_hour == 8) || (timeinfo.tm_hour == 20)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    // hour units - 9
    else if ((timeinfo.tm_hour == 9) || (timeinfo.tm_hour == 21)) {
      strip.setPixelColor(6, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(12, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(14, color);
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
    }
    if (secOn) {
      strip.setPixelColor(19, color);
      strip.setPixelColor(20, color);
    }
    else {
      strip.setPixelColor(19, strip.Color(0, 0, 0));
      strip.setPixelColor(20, strip.Color(0, 0, 0));
    }
    // min tens is 0
    if (timeinfo.tm_min < 10) {
      strip.setPixelColor(21, color);
      strip.setPixelColor(22, color);
      strip.setPixelColor(23, color);
      strip.setPixelColor(24, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(26, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(33, color);
    }
    // min tens is 1
    else if ((timeinfo.tm_min >= 10) && (timeinfo.tm_min < 20)) {
      strip.setPixelColor(27, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(33, color);
    }
    // min tens is 2
    else if ((timeinfo.tm_min >= 20) && (timeinfo.tm_min < 30)) {
      strip.setPixelColor(21, color);
      strip.setPixelColor(22, color);
      strip.setPixelColor(23, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(26, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(29, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(33, color);
    }
    // min tens is 3
    else if ((timeinfo.tm_min >= 30) && (timeinfo.tm_min < 40)) {
      strip.setPixelColor(21, color);
      strip.setPixelColor(23, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(26, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(29, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(33, color);
    }
    // min tens is 4
    else if ((timeinfo.tm_min >= 40) && (timeinfo.tm_min < 50)) {
      strip.setPixelColor(23, color);
      strip.setPixelColor(24, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(29, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(33, color);
    }
    // min tens is 5
    else if ((timeinfo.tm_min >= 50) && (timeinfo.tm_min < 60)) {
      strip.setPixelColor(21, color);
      strip.setPixelColor(23, color);
      strip.setPixelColor(24, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(26, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(29, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(33, color);
    }
    // min units is 0
    if ((timeinfo.tm_min == 0) || (timeinfo.tm_min == 10) || (timeinfo.tm_min == 20) || (timeinfo.tm_min == 30) || (timeinfo.tm_min == 40) || (timeinfo.tm_min == 50)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 1
    else if ((timeinfo.tm_min == 1) || (timeinfo.tm_min == 11) || (timeinfo.tm_min == 21) || (timeinfo.tm_min == 31) || (timeinfo.tm_min == 41) || (timeinfo.tm_min == 51)) {
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(46, color);
    }
    // min units is 2
    else if ((timeinfo.tm_min == 2) || (timeinfo.tm_min == 12) || (timeinfo.tm_min == 22) || (timeinfo.tm_min == 32) || (timeinfo.tm_min == 42) || (timeinfo.tm_min == 52)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 3
    else if ((timeinfo.tm_min == 3) || (timeinfo.tm_min == 13) || (timeinfo.tm_min == 23) || (timeinfo.tm_min == 33) || (timeinfo.tm_min == 43) || (timeinfo.tm_min == 53)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 4
    else if ((timeinfo.tm_min == 4) || (timeinfo.tm_min == 14) || (timeinfo.tm_min == 24) || (timeinfo.tm_min == 34) || (timeinfo.tm_min == 44) || (timeinfo.tm_min == 54)) {
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(46, color);
    }
    // min units is 5
    else if ((timeinfo.tm_min == 5) || (timeinfo.tm_min == 15) || (timeinfo.tm_min == 25) || (timeinfo.tm_min == 35) || (timeinfo.tm_min == 45) || (timeinfo.tm_min == 55)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 6
    else if ((timeinfo.tm_min == 6) || (timeinfo.tm_min == 16) || (timeinfo.tm_min == 26) || (timeinfo.tm_min == 36) || (timeinfo.tm_min == 46) || (timeinfo.tm_min == 56)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 7
    else if ((timeinfo.tm_min == 7) || (timeinfo.tm_min == 17) || (timeinfo.tm_min == 27) || (timeinfo.tm_min == 37) || (timeinfo.tm_min == 47) || (timeinfo.tm_min == 57)) {
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(46, color);
    }
    // min units is 8
    else if ((timeinfo.tm_min == 8) || (timeinfo.tm_min == 18) || (timeinfo.tm_min == 28) || (timeinfo.tm_min == 38) || (timeinfo.tm_min == 48) || (timeinfo.tm_min == 58)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    // min units is 9
    else if ((timeinfo.tm_min == 9) || (timeinfo.tm_min == 19) || (timeinfo.tm_min == 29) || (timeinfo.tm_min == 39) || (timeinfo.tm_min == 49) || (timeinfo.tm_min == 59)) {
      strip.setPixelColor(34, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(38, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(44, color);
      strip.setPixelColor(45, color);
      strip.setPixelColor(46, color);
    }
    strip.show();
  }
  delay(1000);
}

//strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//strip.show();                          //  Update strip to match
//strip.Color(255,   0,   0)