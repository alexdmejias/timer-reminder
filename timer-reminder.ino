#include <Adafruit_NeoPixel.h>
#include <Process.h>
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

int interruptIndex = 4;
volatile int appState = LOW;
volatile boolean actedOnStateChange = true;

String startTime;
String endTime;
Process date;

const int ledPin = 6;
const int numOfLights = 24;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numOfLights, ledPin, NEO_GRB + NEO_KHZ800);

// Breathing related variables
int brightness = 0;  // will store the brightness of the lights
int breathingSteps = 1; // how much brightness to add on each step
const int breathingInterval = 10; // amount of milliseconds per step
const int breathingMaxBrightness = 200; // maximum brightness
unsigned long breathingPreviousMillis = 0; // will store last time LED was updated

void setup() {
  Serial.begin(9600);
  Bridge.begin();
  
  pinMode(ledPin, OUTPUT);
  attachInterrupt(interruptIndex, onInterrupt, RISING);
}

void onInterrupt() {
  actedOnStateChange = false;
  appState = !appState; // toggle the app state
}

void loop() {
  if (actedOnStateChange == false) {
    if (startTime.length() < 1 && appState == HIGH) {
      startTime = getNow();
    } 

    if (startTime.length() > 1 && appState == LOW) {
      endTime = getNow();
    }

    if (startTime.length() > 1 && endTime.length() > 1 && appState == LOW) {
      addTimeGroup(startTime, endTime);
    }
    actedOnStateChange = true;
  }

  if (appState == LOW) { 
    setAllLightsRed(0);
  } else {
    breathe();
  }
  
}

void setAllLights(uint32_t color) {
  for(int i = 0; i < numOfLights; i++) {
    strip.setPixelColor(i, color);
    strip.show();
  }
}

void setAllLightsRed(int brightness) {
  setAllLights(strip.Color(brightness, 0, 0));
}

void breathe() {
  unsigned long currentMillis = millis();
  if (currentMillis - breathingPreviousMillis >= breathingInterval) {
    breathingPreviousMillis = currentMillis;   
    setAllLightsRed(brightness);
    brightness = brightness + breathingSteps;
    if (brightness == 0 || brightness == breathingMaxBrightness) {
      breathingSteps = -breathingSteps;
    }
  }
}

String getNow() {
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%m/%d/%Y %T");
    date.run();
  }

  String now;
  if (date.available() > 0) {
    now = date.readString();
  } else {
    now = "no date";
  }

  Serial.println("returning:" + now);
  return now;
}

void addTimeGroup(String start, String stop) {
  Serial.println(start);
  String times = start + "," + stop;
  TembooChoreo AppendRowChoreo;

  // Invoke the Temboo client
  AppendRowChoreo.begin();

  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Set Choreo inputs
  AppendRowChoreo.addInput("ClientSecret", "SeLpeYgt4IhTN9-6m7Bv9pUr");
  AppendRowChoreo.addInput("RefreshToken", "1/Uzqnw135kVmiXxMwYl-dLesOq9CXCqO1NYRZBhx2k58");
  AppendRowChoreo.addInput("RowData", times);
  AppendRowChoreo.addInput("SpreadsheetTitle", "temboo-test");
  AppendRowChoreo.addInput("ClientID", "780256191844-t2hp4ibf3j6be7d8ecnhkmliavlmcgam.apps.googleusercontent.com");
  
  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
  
  // Run the Choreo; when results are available, print them to serial
  AppendRowChoreo.run();
  
  while(AppendRowChoreo.available()) {
    char c = AppendRowChoreo.read();
    Serial.println(c);
  }

  startTime = "";
  endTime = "";
  AppendRowChoreo.close();
}





