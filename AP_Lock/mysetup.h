//
// Created by Fedon on 04.01.2025
//

#ifndef MY_SETUP_H
#define MY_SETUP_H

#include <Arduino.h>

//EEPROM SETUP
#define EEPROM_SIZE 10 // eeprom size in bytes | we need to store 2 ints (2 bytes eache)
int passwordAddress = 0; // address to store an int (2bytes)
int stateAddress = 5;


// Access point credentials
const char* ssid = "esp_ap_02";
const char* password = "esp_ap_02";

//
const bool hideAP = false;

// Set up board's static ip
IPAddress local_IP(192, 168, 1, 10);

// Default gateway
IPAddress gateway(192, 168, 1, 1);

// Subnet mask
IPAddress subnet(255, 255, 255, 0);

// OTHER SETUP
#define LOCK_OFF 0   //Lock off, door always open
#define LOCK_AUTO 1  //Auto, door unlocks with keypad
#define LOCK_ON 2    //Lock on, door always locked

// KEYPAD SETUP
#define I2CADDR 0x20

bool keypadEnabled = true;

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //four columns
const char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// keypad mapping bit numbers of PCF8574 i/o port
byte rowPins[ROWS] = {3, 2, 1, 0};
byte colPins[COLS] = {7, 6, 5, 4};

#endif