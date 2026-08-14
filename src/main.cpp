#include <Arduino.h>

// Hardware smoke test: blink the built-in LED and print to serial.
// No vending-machine logic yet -- this only proves upload + serial work.

static const unsigned long BLINK_INTERVAL_MS = 1000;
static const unsigned long PRINT_INTERVAL_MS = 2000;

static unsigned long lastBlinkMs = 0;
static unsigned long lastPrintMs = 0;
static bool ledOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("vending-machine boot"));
}

void loop() {
  const unsigned long now = millis();

  if (now - lastBlinkMs >= BLINK_INTERVAL_MS) {
    lastBlinkMs = now;
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
  }

  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    Serial.println(F("vending-machine alive"));
  }
}
