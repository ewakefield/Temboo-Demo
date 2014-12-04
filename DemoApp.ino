/*
 * There are places in the code that held passwords and vital information that is unique to each user.
 * Please read through the code and place your own passwords and tokens. All places to replace or change are denoted by XXXX's
 * Depending on use, you might have to change a few other things as well. Thanks! Enjoy!
 *
 * DemoApp.ino
 *
 *      Created on: Nov 6, 2014
 *      Author: a0221660@ti.com
 *
 *      Copyright (c) 2014, Evan Wakefield
 *		All rights reserved.
 *
 *		Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *		1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 *		2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *                 documentation and/or other materials provided with the distribution.
 *
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *		EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *		OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 *		SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *		OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *		HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *		(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *		EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;
int readValue = 0;   //value that is read from analogRead() function
int emailSent = 0;   //used to ensure email doesn't get sent more than once
int textSent = 0;    //used to ensure text doesn't get sent more than once
int dataLogged = 0;  //used to ensure data doesn't get logged more than once

void setup() {
  Serial.begin(9600);
  pinMode(24, INPUT);      //sets the pin the sensor is connected to as an INPUT. 
  connectToInternet();    //connects to the internet
  Serial.println("Setup complete.\n");
}

void loop() {

  readValue = analogRead(24);          //reads the pin the sensor is connected to

  Serial.print("Data value read = ");
  Serial.println(readValue);
  if((1000 < readValue) && (readValue < 2000) && (dataLogged == 0)) {
    logData();
  }
  else if((2000 < readValue) && (readValue < 3000)&&(emailSent == 0)){ 
    sendEmail();
  }
  else if((3000 < readValue) && (textSent == 0)){
    sendSMS();
  }
    delay(300);
}

void connectToInternet(){
    int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present.
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here.
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network.
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }
}

void sendSMS(){
  Serial.println("Running SendSMS");

    TembooChoreo SendSMSChoreo(client);

    // Invoke the Temboo client
    SendSMSChoreo.begin();

    // Set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set profile to use for execution
    SendSMSChoreo.setProfile("XXXXXXXXXXXXXX");

    // Set Choreo inputs
    String BodyValue = "An event has been triggered!";
    SendSMSChoreo.addInput("Body", BodyValue);

    // Identify the Choreo to run
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // Run the Choreo; when results are available, print them to serial
    SendSMSChoreo.run();

    while(SendSMSChoreo.available()) {
      char c = SendSMSChoreo.read();
      Serial.print(c);
    }
    SendSMSChoreo.close();
    
    textSent = 1;
}

void logData()
{
  Serial.println("Running AppendRow to Google Spreadsheet");

  TembooChoreo AppendRowChoreo(client);
  
  // Invoke the Temboo client
  AppendRowChoreo.begin();
  
  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Set Choreo inputs
  String PasswordValue = "XXXXXXXXXXXXXXXXXXXXXXXX";
  AppendRowChoreo.addInput("Password", PasswordValue);
  String UsernameValue = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX@gmail.com";
  AppendRowChoreo.addInput("Username", UsernameValue);
  String RowDataValue = "Threshold Exceeded" + String(readValue);
  AppendRowChoreo.addInput("RowData", RowDataValue);
  String SpreadsheetTitleValue = "DemoApp";
  AppendRowChoreo.addInput("SpreadsheetTitle", SpreadsheetTitleValue);
  
  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
  
  // Run the Choreo; when results are available, print them to serial
  AppendRowChoreo.run();
  
  while(AppendRowChoreo.available()) {
    char c = AppendRowChoreo.read();
    Serial.print(c);
  }
  AppendRowChoreo.close();
  dataLogged = 1;
}

void sendEmail(){
  Serial.println("Running SendEmail");

    TembooChoreo SendEmailChoreo(client);

    // Invoke the Temboo client
    SendEmailChoreo.begin();

    // Set Temboo account credentials
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String MessageBodyValue = "Threshold exceeded!";
    SendEmailChoreo.addInput("MessageBody", MessageBodyValue);
    String SubjectValue = "WARNING!";
    SendEmailChoreo.addInput("Subject", SubjectValue);
    String PasswordValue = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    SendEmailChoreo.addInput("Password", PasswordValue);
    String UsernameValue = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX@gmail.com";
    SendEmailChoreo.addInput("Username", UsernameValue);
    String FromAddressValue = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX@gmail.com";
    SendEmailChoreo.addInput("FromAddress", FromAddressValue);
    String ToAddressValue = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX@gmail.com";
    SendEmailChoreo.addInput("ToAddress", ToAddressValue);

    // Identify the Choreo to run
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");

    // Run the Choreo; when results are available, print them to serial
    SendEmailChoreo.run();

    while(SendEmailChoreo.available()) {
      char c = SendEmailChoreo.read();
      Serial.print(c);
    }
    SendEmailChoreo.close();
  
  emailSent = 1;
}
  
