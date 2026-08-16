#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AccelStepper.h>

// -----------------------------------------------------------------------------
// DRV8825
// -----------------------------------------------------------------------------

constexpr uint8_t STEP_PIN = 2;
constexpr uint8_t DIR_PIN  = 3;
constexpr uint8_t EN_PIN   = 4;

constexpr long STEPS_PER_REVOLUTION = 6400;

AccelStepper stepper(
    AccelStepper::DRIVER,
    STEP_PIN,
    DIR_PIN
);

// -----------------------------------------------------------------------------
// Front-controller serial link
//
// SoftwareSerial(rx, tx)
//
// Motor RX = D10
// Motor TX = D9
// -----------------------------------------------------------------------------

constexpr uint8_t FRONT_RX_PIN = 10;
constexpr uint8_t FRONT_TX_PIN = 9;

SoftwareSerial frontSerial(
    FRONT_RX_PIN,
    FRONT_TX_PIN
);

String command = "";

// -----------------------------------------------------------------------------
// Motor functions
// -----------------------------------------------------------------------------

void dispenseMotor1() {
    Serial.println("DISPENSING 1");

    // DRV8825 ENABLE is active-low
    digitalWrite(EN_PIN, LOW);

    stepper.move(STEPS_PER_REVOLUTION);
    stepper.runToPosition();

    digitalWrite(EN_PIN, HIGH);

    Serial.println("DONE 1");

    // Tell the front Nano the vend completed
    frontSerial.println("DONE 1");
}

// -----------------------------------------------------------------------------
// Command handling
// -----------------------------------------------------------------------------

void handleCommand(String cmd) {
    cmd.trim();

    if (cmd.length() == 0) {
        return;
    }

    Serial.print("Received: ");
    Serial.println(cmd);

    if (cmd == "DISPENSE 1") {
        dispenseMotor1();
        return;
    }

    Serial.print("UNKNOWN COMMAND: ");
    Serial.println(cmd);

    frontSerial.print("ERROR UNKNOWN ");
    frontSerial.println(cmd);
}

void readFrontController() {
    while (frontSerial.available()) {
        char c = frontSerial.read();

        if (c == '\n' || c == '\r') {
            if (command.length() > 0) {
                handleCommand(command);
                command = "";
            }
        } else {
            command += c;

            if (command.length() > 32) {
                Serial.println("ERROR: command too long");
                command = "";
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);

    frontSerial.begin(9600);

    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, HIGH);

    stepper.setMaxSpeed(3000);
    stepper.setAcceleration(2000);

    Serial.println("motor controller ready");
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------

void loop() {
    readFrontController();
}