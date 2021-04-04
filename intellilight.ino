#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

// Keypad rows and columns
#define ROWS 4
#define COLS 4

// Photoresistor
#define PHOTOR A0

// Modes
#define TIME 0
#define LIGHT 1

// LCD width and height
#define LCD_WIDTH 16
#define LCD_HEIGHT 2

// LCD
LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

// Keypad
const char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}};

const byte rowPins[ROWS] = {9, 8, 7, 6};
const byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Mode
int mode = LIGHT;
int timeout = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  displayStatus();
}

void loop() {
  char key;
  static bool selecting_mode;
  static bool setting_time;

  key = keypad.getKey();
  if (key != NO_KEY) {
    switch (key) {
    // Select mode
    case 'A':
      selecting_mode = true;
      lcd.clear();
      lcd.print("Mode:");
      lcd.setCursor(0, 1);
      lcd.print("*: Time #: Light");
      break;

    // Time mode
    case '*':
      if (selecting_mode) {
        mode = TIME;

        // Reset variables
        selecting_mode = false;
        setting_time = true;
        timeout = 0;

        lcd.clear();
        lcd.print("Press B to end.");
        lcd.setCursor(0, 1);
        lcd.print("Time (min): ");
      }
      break;

    // Light mode
    case '#':
      if (selecting_mode) {
        mode = LIGHT;
        selecting_mode = false;
        displayStatus();
      }
      break;

    // End time input
    case 'B':
      setting_time = false;
      displayStatus();
      break;

    // Unused keys
    case 'C':
    case 'D':
      break;

    // 0-9
    default:
      if (setting_time) {
        lcd.print(key);
        timeout *= 10;
        timeout += key - '0';
      }
    }
  }

  if (mode == TIME) {
    // TODO
  } else if (mode == LIGHT) {
    // TODO
  }
}

// Display the status of the device
void displayStatus() {
  char str[LCD_WIDTH + 1];

  if (mode == TIME) {
    sprintf(str, "Mode: Time %dm", timeout);
  } else if (mode == LIGHT) {
    sprintf(str, "Mode: Light");
  }
  lcd.clear();
  lcd.print(str);
}

void doTime() {
  // TODO
}

void doLight() {
  // TODO
}