#include <Adafruit_NeoPixel.h>
#include <Process.h>
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

volatile int interrupt = 4;
const int ledPin = 13;

int appState = LOW; 

String startTime;

const int numOfLights = 24;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numOfLights, ledPin, NEO_GRB + NEO_KHZ800);



void setup() {
  Serial.begin(9600);
  Bridge.begin();
  
  pinMode(ledPin, OUTPUT);
  attachInterrupt(interrupt, buttonPress, RISING);
  Serial.println("setup");
}

void buttonPress() {
  if (appState == LOW) {
    Serial.println("on");
    startTime = getNow();
  } else {
    Serial.println("off");
    String endTime = getNow();

    Serial.println(startTime + " - " + endTime);
  }

  appState = !appState;
}

void setAllLights(uint32_t color) {
  for(int i = 0; i < numOfLights; i++) {
    strip.setPixelColor(i, color);
    strip.show();
  }
}

void setAllLightsRed(int brightness) {
  setAllLights(strip.Color(brightness, 0 ,0));
}

int brightness = 0;
int breathingSteps = 5;
const int interval = 50;
unsigned long previousMillis = 0;        // will store last time LED was updated

void breathe() {

  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    setAllLightsRed(brightness);

    brightness = brightness + breathingSteps;

    if (brightness == 0 || brightness == 125) {
      breathingSteps = -breathingSteps;
    }
    
  }

}

void loop() {
  setAllLightsRed(255);
  Serial.println("WASD");
  if (appState == LOW) {
    setAllLights(strip.Color(0, 0, 0));
  } else {
    breathe();
  }
}

String getNow() {
  Process date;

  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%m/%d/%Y %T");
    date.run();
  }

  String now = date.readString();

  return now;
}

void addTimeGroup(String start, String stop) {
      
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
    Serial.print(c);
  }
  AppendRowChoreo.close();
}
