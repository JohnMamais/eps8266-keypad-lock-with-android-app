//
// Created by Fedon on 04.01.2025
//

#ifndef MY_SETUP_H
#define MY_SETUP_H

#include <Arduino.h>

//EEPROM SETUP
int passwordAddress = 0;
int stateAddress = 10;


// Access point credentials
const char* ssid = "esp_ap_01";
const char* password = "esp_ap_01";

// Set up board's static ip
IPAddress local_IP(192, 168, 1, 10);

// Default gateway
IPAddress gateway(192, 168, 1, 1);

// Subnet mask
IPAddress subnet(255, 255, 255, 0);

//KEYPAD SETUP
#define I2CADDR 0x20
//OTHER SETUP
#define LOCK_OFF 0   //Lock off, door always open
#define LOCK_AUTO 1  //Auto, door unlocks with keypad
#define LOCK_ON 2    //Lock on, door always locked

bool keypadEnabled = true;

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //four columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Digitran keypad, bit numbers of PCF8574 i/o port
//byte rowPins[ROWS] = { 0, 1, 2, 3 };  //connect to the row pinouts of the keypad
//byte colPins[COLS] = { 4, 5, 6, 7 };  //connect to the column pinouts of the keypad
byte rowPins[ROWS] = {3, 2, 1, 0};
byte colPins[COLS] = {7, 6, 5, 4};

#endif