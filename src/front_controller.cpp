#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Keypad_I2C.h>

// -----------------------------------------------------------------------------
// LCD
// -----------------------------------------------------------------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);

// -----------------------------------------------------------------------------
// Keypad
// -----------------------------------------------------------------------------

constexpr byte ROWS = 4;
constexpr byte COLS = 4;

char keys[ROWS][COLS] = {
    {'D', '#', '0', '*'},
    {'C', '9', '8', '7'},
    {'B', '6', '5', '4'},
    {'A', '3', '2', '1'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad_I2C keypad(
    makeKeymap(keys),
    rowPins,
    colPins,
    ROWS,
    COLS,
    0x20
);

// -----------------------------------------------------------------------------
// Motor-controller serial link
//
// SoftwareSerial(rx, tx)
//
// Front RX = D8
// Front TX = D11
// -----------------------------------------------------------------------------

constexpr uint8_t MOTOR_RX_PIN = 8;
constexpr uint8_t MOTOR_TX_PIN = 11;

SoftwareSerial motorSerial(
    MOTOR_RX_PIN,
    MOTOR_TX_PIN
);

// -----------------------------------------------------------------------------
// UI state
// -----------------------------------------------------------------------------

String selection = "";
String motorResponse = "";

// -----------------------------------------------------------------------------
// Display helpers
// -----------------------------------------------------------------------------

void showHome() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Vending Machine");

    lcd.setCursor(0, 1);
    lcd.print("Select item...");
}

void showSelection() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Selection:");

    lcd.setCursor(0, 1);
    lcd.print(selection);
}

void showDispensing(const String& slot) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Dispensing...");

    lcd.setCursor(0, 1);
    lcd.print(slot);
}

void showDone(const String& slot) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Enjoy!");

    lcd.setCursor(0, 1);
    lcd.print(slot);
}

// -----------------------------------------------------------------------------
// Motor communication
// -----------------------------------------------------------------------------

void sendDispenseCommand(int motorNumber) {
    String command = "DISPENSE " + String(motorNumber);

    Serial.print("Sending to motor controller: ");
    Serial.println(command);

    motorSerial.println(command);
}

void handleMotorResponse(String response) {
    response.trim();

    if (response.length() == 0) {
        return;
    }

    Serial.print("Motor controller: ");
    Serial.println(response);

    if (response.startsWith("DONE ")) {
        showDone(selection);

        delay(1500);

        selection = "";
        showHome();
    }
}

void readMotorController() {
    while (motorSerial.available()) {
        char c = motorSerial.read();

        if (c == '\n' || c == '\r') {
            if (motorResponse.length() > 0) {
                handleMotorResponse(motorResponse);
                motorResponse = "";
            }
        } else {
            motorResponse += c;

            if (motorResponse.length() > 32) {
                motorResponse = "";
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Selection handling
// -----------------------------------------------------------------------------

void submitSelection() {
    if (selection.length() == 0) {
        return;
    }

    Serial.print("Selection submitted: ");
    Serial.println(selection);

    // Current test mapping:
    // A1 -> motor 1
    if (selection == "A1") {
        showDispensing(selection);
        sendDispenseCommand(1);
        return;
    }

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Invalid item");

    lcd.setCursor(0, 1);
    lcd.print(selection);

    delay(1200);

    selection = "";
    showHome();
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);

    motorSerial.begin(9600);

    Wire.begin();

    lcd.init();
    lcd.backlight();

    keypad.begin();

    showHome();

    Serial.println("front controller ready");
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------

void loop() {
    readMotorController();

    char key = keypad.getKey();

    if (!key) {
        return;
    }

    Serial.print("Key: ");
    Serial.println(key);

    // # = submit
    if (key == '#') {
        submitSelection();
        return;
    }

    // * = clear
    if (key == '*') {
        selection = "";
        showSelection();
        return;
    }

    if (selection.length() < 4) {
        selection += key;
        showSelection();
    }
}