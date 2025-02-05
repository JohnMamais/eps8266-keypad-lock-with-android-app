//
// Created by Fedon on 04.01.2025
//

#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x) 
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#include "mysetup.h"


// Create an LCD instance
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// Create an instance of the web server on port 443 (HTTPS)
ESP8266WebServer server(80);

// Create a keypad instance
Keypad_I2C kpd(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574);

// declaration of some variables
int lockState;
int lockPassword;

// declaring the pin for the relay that opens and closes the door
const int door_pin = 13;

// Callback function type
typedef void (*KeyPressCallback)(char key);

KeyPressCallback onKeyPress = NULL;  // Initialize callback as NULL

//password string
String keypadInput = "";

// 
const char* respType PROGMEM = "application/json";

void keyPressedCallBack(char);

void EEPROMFallback();

void setLockPassword(int);

void setLockState(int);

void getLockState();

void getLockPassword();

void handlePostRequest();

String lockStateToString(int);

void EEPROMFallback();

void setup(){
  Serial.begin(9600);

  debugln();

  //EEPROM SETUP
  EEPROM.begin(EEPROM_SIZE);  // Initialize EEPROM

  // Fallback values
  EEPROMFallback();

  pinMode(door_pin, OUTPUT);  // Set pin 13 as an output for the door

  //lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();  // Make sure backlight is on

  //keypad
  Wire.begin();
  kpd.begin(makeKeymap(keys));

  //keypad
  // Assign the callback function
  onKeyPress = keyPressedCallback;

  debugln("ESP Board MAC Address:  ");
  debugln(WiFi.macAddress());

  // Configure the Static IP
  WiFi.softAPConfig(local_IP, gateway, subnet);

  // Start the AP
  WiFi.softAP(ssid, password, 1, hideAP); // 1 is the channel

  debug("AP started. \nIP: ");
  debugln(WiFi.softAPIP());

  server.begin();

  // Define endpoint to handle POST requests
  server.on("/post", HTTP_POST, handlePostRequest);

  // Define a not found page
  server.onNotFound([] () {
    debugln("Wrong endpoint");
    server.send(400, "text/plain", "Error! We don't have what you're looking for.");
  });

  Serial.println("Setup complete.");
}

void loop(){

  server.handleClient();

  lcd.setCursor(0, 0);  // Set the cursor to char 0, line 1
  lcd.print("Lock: ");
  lcd.print(lockStateToString(lockState));

  if (keypadEnabled) {
    char key = kpd.getKey();  // Check if any key is pressed

    if (key) {
      // If a key is pressed, call the callback function
      if (onKeyPress != NULL) {
        onKeyPress(key);  // Call the callback function
      }
    }
  }

  switch (lockState) {
    case LOCK_OFF:
      {  // code to handle unlocked state
        lcd.setCursor(0, 1);
        lcd.print("PIN Disabled");
        digitalWrite(door_pin, HIGH);  //idle lock state
        break;
      }
    case LOCK_AUTO:
      {
        lcd.setCursor(0, 1);
        lcd.print("PIN: ");
        if (keypadInput.length() > 0) {
          for (int i = 0; i < keypadInput.length(); i++) {
            lcd.print("*");
          }
        }
        keypadEnabled = true;
        digitalWrite(door_pin, LOW);  //idle lock state
        break;
      }
    case LOCK_ON:
      {
        lcd.setCursor(0, 1);
        lcd.print("PIN Disabled");
        digitalWrite(door_pin, LOW);  //idle lock state
        break;
      }
  }
}

void EEPROMFallback() {
  //fallback values if the EEPROM is empty
  int pass, st;
  bool flag = false;

  EEPROM.get(passwordAddress, pass);
  if (pass == -1) {
    flag = true;
    lockPassword = 254254;
    EEPROM.put(passwordAddress, 254254);
  } else {
    lockPassword = pass;
  }
  EEPROM.get(stateAddress, st);
  if (st == -1) {
    flag = true;
    lockState = LOCK_AUTO;
    EEPROM.put(stateAddress, LOCK_AUTO);
  } else {
    lockState = st;
  }

  if(flag){
    EEPROM.commit();
    debugln("EEPROM_ERROR");
  } else {
    debugln("EEPROM OK");
  }
}

void keyPressedCallback(char key) {
  //debugln("Key Pressed: ");
  //debugln(key);
  // Do something when a key is pressed
  // For example, stop some ongoing task or trigger a change in state
  debugln(key);
  if (key == '#') {
    // clear password
    debugln("pswd reset");
    keypadInput = "";
    lcd.clear();
  } else if (key == '*') {
    // check password
    debugln("Input is: ");
    debugln(keypadInput);

    // maybe don't get the password from EEPROM everytime it needs to check it
    // EEPROM.get(passwordAddress, lockPassword);

    if (keypadInput == String(lockPassword)) {
      lcd.setCursor(0, 1);
      lcd.print("UNLOCKED      ");

      debugln("correct pswd");

      digitalWrite(door_pin, HIGH);
      delay(5000);
      digitalWrite(door_pin, LOW);
      lcd.clear();

    } else {
      debugln("pswd atmpt: ");
      debugln(keypadInput);

      lcd.setCursor(0, 1);
      debugln("correct pswd");

      lcd.print("INCORRECT       ");
      delay(5000);
      lcd.clear();

    }

    lcd.clear();
    debugln("pswd rst.");
    keypadInput = "";
  } else if (key != 'A' && key != 'B' && key != 'C' && key != 'D'){
    //append to password
    keypadInput += key;
  }
}

void setLockPassword(int newPassword) {
  if (String(abs(newPassword)).length() >= 5) {
    EEPROM.put(passwordAddress, newPassword);
    EEPROM.commit();
    lockPassword=newPassword;
    server.send(200, respType, "{\"success\": \"Password Changed\"}");
    debugln("Password Changed.");
  } else {
    server.send(400, respType, "{\"error\": \"Invalid Password\"}");
    debugln("Password Invalid. Didn't change password.");
  }
}

void setLockState(int state) {
  debugln("Lock state was: ");
  debugln(lockState);
  if (state >= 0 && state <= 2) {
    // Clear lcd
    lcd.clear();
    
    //put new state in EEPROM
    EEPROM.put(stateAddress, state);
    lockState = state;
    EEPROM.commit();

    // inform serial of changes
    debugln("Changed lock state to: ");
    debugln(lockState);

    // disable keypad if lock is in any state other than auto
    if (lockState != LOCK_AUTO) {
      keypadEnabled = false;
      debugln("Disabled Keypad.");
    }

    server.send(200, respType, "{\"success\": \"state changed\" }");
  } else {
    server.send(400, respType, "{\"error\": \"Argument out of bounds\"}");
    debugln("Argument value out of bounds, didn't change lock state.");
  }
}

void getLockState() {
  debugln("Got lockstate");

  server.send(200, respType, "{\"state\": " + String(lockState) + "}");
}

void getLockPassword() {
  debugln("Got lockpassword");
  server.send(200, respType, "{\"password\": " + String(lockPassword) + "}");
}

void handlePostRequest() {

  if (server.hasArg("plain")) {
    String postData = server.arg("plain");
    debugln("Received JSON data:");
    debugln(postData);

    // Set CORS headers
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST");

    DynamicJsonDocument doc(512);  // You can increase this size if needed

    // Deserialize document, returns true if failure arguments(destination, original)
    if (deserializeJson(doc, postData)) {
      debugln("Invalid JSON received.");
      server.send(400, respType, "{\"error\": \"Invalid JSON\"}");
      return;
    }

    if (doc.containsKey("action") && doc.containsKey("argument")) {  // Access the JSON object fields
      String action = doc["action"];
      int argument = doc["argument"];

      if (action == "setLockState") {
        setLockState(argument);
      } else if (action == "setLockPassword") {
        setLockPassword(argument);
      } else if (action == "getLockPassword") {
        getLockPassword();
      } else if (action == "getLockState") {
        getLockState();
      } else {
        server.send(400, respType, "{\"error\": \"Invalid Action\"}");
        return; // Ensure no further processing if action is invalid
      }

      // Send a response back in JSON format
      //String response = "{\"message\": \"Action executed successfully\", \"action\": \"" + action + "\", \"argument\": " + String(argument) + "}";
      //server.send(200, respType, response);
    } else {
      server.send(400, respType, "{\"error\": \"Invalid JSON\"}");
    }

  } else {
    server.send(400, respType, "{\"error\": \"No data received\"}");
  }
}

String lockStateToString(int state) {
  switch (state) {
    case LOCK_OFF:
      return "OFF";
    case LOCK_AUTO:
      return "AUTO";
    case LOCK_ON:
      return "ON";
    default:
      return "Err";
  }
}

