#include <Arduino.h>

constexpr uint8_t COIN_PIN = 2;

// CH-926 sends a burst of pulses for each accepted coin.
// We count falling edges in the ISR, then process the burst
// after no new pulse has arrived for a short period.
volatile uint8_t pulseCount = 0;
volatile unsigned long lastPulseMs = 0;

constexpr unsigned long PULSE_DEBOUNCE_MS = 20;
constexpr unsigned long COIN_BURST_TIMEOUT_MS = 300;

uint32_t creditCents = 0;

void onCoinPulse()
{
    const unsigned long now = millis();

    if (now - lastPulseMs >= PULSE_DEBOUNCE_MS) {
        pulseCount++;
        lastPulseMs = now;
    }
}

uint16_t centsFromPulseCount(uint8_t pulses)
{
    switch (pulses) {
        case 1:
            return 5;   // nickel

        case 2:
            return 10;  // dime

        case 5:
            return 25;  // quarter

        default:
            return 0;   // unknown / malformed pulse burst
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(COIN_PIN, INPUT_PULLUP);
    attachInterrupt(
        digitalPinToInterrupt(COIN_PIN),
        onCoinPulse,
        FALLING
    );

    Serial.println("coin controller ready");
}

void loop()
{
    uint8_t completedPulseCount = 0;

    noInterrupts();

    if (
        pulseCount > 0 &&
        millis() - lastPulseMs >= COIN_BURST_TIMEOUT_MS
    ) {
        completedPulseCount = pulseCount;
        pulseCount = 0;
    }

    interrupts();

    if (completedPulseCount > 0) {
        const uint16_t coinValue = centsFromPulseCount(completedPulseCount);

        if (coinValue > 0) {
            creditCents += coinValue;

            Serial.print("Coin: $");
            Serial.print(coinValue / 100);
            Serial.print(".");
            if ((coinValue % 100) < 10) {
                Serial.print("0");
            }
            Serial.println(coinValue % 100);

            Serial.print("Credit: $");
            Serial.print(creditCents / 100);
            Serial.print(".");
            if ((creditCents % 100) < 10) {
                Serial.print("0");
            }
            Serial.println(creditCents % 100);
        } else {
            Serial.print("Unknown coin pulse count: ");
            Serial.println(completedPulseCount);
        }
    }
}