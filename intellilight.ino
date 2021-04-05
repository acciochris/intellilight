#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Keypad rows and columns
#define ROWS 4
#define COLS 4

// Photoresistor
#define PHOTOR A0
#define VMIN 400
#define VMAX 800

// Modes
#define TIME 0
#define LIGHT 1

// LCD width and height
#define LCD_WIDTH 16
#define LCD_HEIGHT 2

// Lights
#define RED 9
#define GREEN 10
#define BLUE 11

// LCD
LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

// Keypad
const char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}};

const byte rowPins[ROWS] = {12, 8, 7, 6};
const byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Mode
int mode = LIGHT;
unsigned long timeout = 0;
unsigned long start_time;

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  lcd.init();
  lcd.backlight();
  displayStatus();
}

void loop() {
  char key;
  static bool selecting_mode;
  static bool setting_time;
  static bool is_on;

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
        timeout = 0UL;

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
      if (setting_time) {
        is_on = true;
        setting_time = false;
        start_time = millis();
        colorRGB(255, 255, 255);
        displayStatus();
      }
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

  if (is_on && (mode == TIME)) {
    is_on = doTime();
  } else if (mode == LIGHT) {
    doLight();
  }
}

// Display the status of the device
void displayStatus() {
  char str[LCD_WIDTH + 1];

  lcd.clear();
  if (mode == TIME) {
    lcd.print("Mode: Time");
  } else if (mode == LIGHT) {
    lcd.print("Mode: Light");
  }
  lcd.setCursor(0, 1);
  lcd.print("Press A to chmod");
}

bool doTime() {
  if (millis() - start_time > timeout * 60000UL) {
    Serial.println("timeout in doTime ...");
    colorRGB(0, 0, 0);
    return false;
  }
  return true;
}

void doLight() {
  static unsigned long last;
  unsigned long current;
  int val;

  if ((current = millis()) - last > 1000) {
    Serial.println("doLight ...");
    last = current;
    val = analogRead(PHOTOR);
    Serial.println(val);
    int brightness = map(val, VMIN, VMAX, 0, 255);
    colorRGB(brightness, brightness, brightness);
  }
}

void colorRGB(int red, int green, int blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}
