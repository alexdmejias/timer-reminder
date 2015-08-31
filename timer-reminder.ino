#include <Bounce2.h>
#include <Process.h>
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

const int buttonPin = 2;
const int ledPin = 13;

String startTime;

int ledState = LOW;
boolean appActive = false;

Bounce bouncer = Bounce();
int lastButtonState = LOW;

void setup() {
  Serial.begin(9600);
  Bridge.begin();
  
  pinMode(ledPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  bouncer.attach(buttonPin);
  bouncer.interval(5);
}

void loop() {

  bouncer.update();

  int currentButtonState = bouncer.read();

  if (bouncer.rose()) {
    if (appActive == false) {
      digitalWrite(ledPin, HIGH);
      startTime = getNow();
    } else {
      digitalWrite(ledPin, LOW);
      String stopTime = getNow();
      addTimeGroup(startTime, stopTime);
    }

    appActive = !appActive;
  }

  if (appActive == false ) {
    // do reminder stuff here;
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