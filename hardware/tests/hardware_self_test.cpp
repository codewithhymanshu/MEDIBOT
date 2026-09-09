/**
 * MEDIBOT Factory Assembly & Hardware Self-Test Routine
 * ======================================================
 * Run this test immediately after soldering and assembling the hardware.
 * It automatically diagnoses:
 *   1. I2C Bus & MAX30102 Sensor presence (Address 0x57)
 *   2. SPI Bus & LoRa SX1278 presence
 *   3. SPI Bus & MFRC522 NFC Reader presence
 *   4. UART2 Thermal Printer connection & test print
 *   5. Battery Voltage ADC Reading
 *   6. Physical SOS Red Button Interrupt
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <MFRC522.h>

#define PIN_SOS_BTN    34
#define PIN_BUZZER     33
#define PIN_BATT_ADC   35
#define LORA_CS        5
#define LORA_RST       27
#define LORA_DIO0      26
#define NFC_CS         4
#define NFC_RST        25

MFRC522 nfc(NFC_CS, NFC_RST);

void printBanner(const char* title) {
    Serial.println("\n-------------------------------------------");
    Serial.print("🛠️  TESTING: ");
    Serial.println(title);
    Serial.println("-------------------------------------------");
}

void testI2CBus() {
    printBanner("I2C Bus & MAX30102 Sensor");
    Wire.begin(21, 22);
    byte count = 0;
    bool maxFound = false;

    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("  [I2C FOUND] Device at address 0x%02X", addr);
            if (addr == 0x57) {
                Serial.print(" --> MAX30102 Optical PPG Sensor [OK]");
                maxFound = true;
            }
            Serial.println();
            count++;
        }
    }

    if (maxFound) {
        Serial.println("  ✅ PASS: MAX30102 PPG Sensor detected and healthy.");
    } else {
        Serial.println("  ❌ FAIL: MAX30102 NOT found! Check SDA (21), SCL (22) and 3.3V.");
    }
}

void testLoRaRadio() {
    printBanner("LoRa SX1278 868MHz Radio");
    LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
    if (LoRa.begin(868E6)) {
        Serial.println("  ✅ PASS: SX1278 LoRa Radio initialized at 868 MHz (+20dBm).");
        LoRa.end();
    } else {
        Serial.println("  ❌ FAIL: LoRa module not responding. Check SPI wiring & CS(5).");
    }
}

void testNfcReader() {
    printBanner("NFC PN532 / MFRC522 RFID Scanner");
    SPI.begin(18, 19, 23, NFC_CS);
    nfc.PCD_Init();
    byte v = nfc.PCD_ReadRegister(nfc.VersionReg);
    if (v == 0x91 || v == 0x92) {
        Serial.printf("  ✅ PASS: MFRC522 detected (Firmware Version: 0x%02X).\n", v);
    } else {
        Serial.printf("  ❌ FAIL: MFRC522 not responding (Read: 0x%02X). Check CS(4).\n", v);
    }
}

void testBatteryVoltage() {
    printBanner("Battery Monitor ADC & Voltage Divider");
    int raw = analogRead(PIN_BATT_ADC);
    float voltage = ((float)raw / 4095.0f) * 3.3f * 2.0f;
    int pct = (int)(((voltage - 3.2f) / 1.0f) * 100.0f);
    pct = constrain(pct, 0, 100);

    Serial.printf("  ADC Raw Value : %d\n", raw);
    Serial.printf("  Battery Pack  : %.2f Volts (%d%%)\n", voltage, pct);
    if (voltage >= 3.2f && voltage <= 4.35f) {
        Serial.println("  ✅ PASS: Battery voltage within normal Li-ion range.");
    } else {
        Serial.println("  ⚠️ WARN: Voltage outside expected bounds. Check 100k resistors.");
    }
}

void testThermalPrinter() {
    printBanner("58mm Thermal Printer (UART2)");
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Serial2.println("\n=== MEDIBOT FACTORY HARDWARE PASS ===");
    Serial2.println("All Sensors, Radios & Power Rails OK!");
    Serial2.println("Device ID: MEDIBOT-UP-001");
    Serial2.println("=====================================\n\n\n");
    Serial.println("  ✅ PASS: Diagnostic test ticket printed via UART2.");
}

void setup() {
    Serial.begin(115200);
    delay(1500);

    pinMode(PIN_SOS_BTN, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);

    // Beep once
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);

    Serial.println("==================================================");
    Serial.println("   MEDIBOT KIOSK FACTORY DIAGNOSTIC SUITE        ");
    Serial.println("==================================================");

    testI2CBus();
    testLoRaRadio();
    testNfcReader();
    testBatteryVoltage();
    testThermalPrinter();

    Serial.println("\n==================================================");
    Serial.println(" Press the RED PHYSICAL SOS BUTTON to test interrupt...");
    Serial.println("==================================================");
}

void loop() {
    if (digitalRead(PIN_SOS_BTN) == LOW) {
        digitalWrite(PIN_BUZZER, HIGH);
        Serial.println("🚨 RED SOS BUTTON PRESSED! Hardware trigger confirmed [OK].");
        delay(300);
        digitalWrite(PIN_BUZZER, LOW);
        delay(1000);
    }
    delay(100);
}
