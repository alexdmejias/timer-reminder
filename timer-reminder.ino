#include <Bounce2.h>
#include <Process.h>
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

volatile int buttonPin = 7;
const int ledPin = 13;

boolean other = 0;
int appState = 0;
int buttonState = 0;         // current state of the button
int lastButtonState = 0; 

String startTime;

void setup() {
  Serial.begin(9600);
  Bridge.begin();
  
  pinMode(ledPin, OUTPUT);
  attachInterrupt(4, stop, RISING);
}

void stop() {
  Serial.println("STOP");
}

void loop() {

  buttonState = digitalRead(buttonPin);

  if ((buttonState != lastButtonState) && (buttonState == HIGH)) {
    appState = digitalRead(led);
    digitalWrite(led, !appState);
    if (other == false) {
      digitalWrite(led, HIGH);
      startTime = getNow();
    } else {
      digitalWrite(led, LOW);
      String stopTime = getNow();
      // addTimeGroup(startTime, stopTime);
      Serial.println(startTime);
      Serial.println(stopTime);
    }
    other = !other;
  } else {
    Serial.println("off"); 
  }
  lastButtonState = buttonState;

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