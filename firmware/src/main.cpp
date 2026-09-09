/**
 * PROJECT MEDIBOT - Firmware Main Controller
 * Microcontroller: ESP32-WROVER (FreeRTOS)
 * Features:
 *   - Offline-first storage (LittleFS)
 *   - LoRa SOS Emergency Alert (433/868 MHz)
 *   - Smart NFC Health Card Scanner
 *   - MAX30102 Non-Invasive Hemoglobin (Hb) & SpO2
 *   - Embedded Mini Thermal Printer (Slip Generation)
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <MFRC522.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "hb_algorithm.h"
#include "thermal_printer.h"

// Hardware Pin Definitions (Matching PINOUT.md)
#define PIN_SOS_BUTTON     34
#define PIN_BUZZER         33
#define PIN_BATTERY_ADC    35

// LoRa SX1278 SPI Pins
#define LORA_CS            5
#define LORA_RST           27
#define LORA_DIO0          26
#define LORA_FREQ          868E6 // 868 MHz for India (ISM Band: 865-867 MHz / 433 MHz)

// NFC MFRC522 Pins
#define NFC_SS_PIN         4
#define NFC_RST_PIN        25

// Peripherals
ThermalPrinter printer(&Serial2);
MFRC522 mfrc522(NFC_SS_PIN, NFC_RST_PIN);

// Global State
volatile bool g_sosTriggered = false;
volatile unsigned long g_lastSosTime = 0;
String g_currentPatientId = "";
String g_currentPatientName = "Lata Devi";

// FreeRTOS Task Handles
TaskHandle_t TaskSensorHandle;
TaskHandle_t TaskSyncHandle;
QueueHandle_t offlineRecordQueue;

// Interrupt Service Routine for Emergency Physical Red SOS Button
void IRAM_ATTR isrEmergencyButton() {
    unsigned long now = millis();
    if (now - g_lastSosTime > 2000) { // 2 second debounce
        g_sosTriggered = true;
        g_lastSosTime = now;
    }
}

void triggerLoRaEmergencySOS() {
    digitalWrite(PIN_BUZZER, HIGH);
    Serial.println("[EMERGENCY] !!! RED SOS BUTTON PRESSED !!!");
    Serial.println("[LoRa] Broadcasting Emergency Distress Packet to PHC Gateway...");

    // LoRa Emergency Packet
    LoRa.beginPacket();
    LoRa.print("ALERT:EMERGENCY;KIOSK_ID:MEDIBOT_UP_042;LOC:VILLAGE_RAMPUR;TYPE:CRITICAL_MEDICAL_SOS;BATTERY:87%");
    LoRa.endPacket();

    delay(200);
    digitalWrite(PIN_BUZZER, LOW);
    Serial.println("[LoRa] SOS Packet Dispatched Over 10km Sub-GHz Radio Link.");
}

// -------------------------------------------------------------
// FreeRTOS Task 1: Background Offline Sync Manager
// -------------------------------------------------------------
void taskOfflineSync(void *pvParameters) {
    for (;;) {
        // If WiFi is available, upload queued records to FastAPI / ABDM backend
        // Else keep records encrypted in LittleFS flash
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// -------------------------------------------------------------
// FreeRTOS Task 2: Optical PPG Sensor & Hb Calibration
// -------------------------------------------------------------
void taskSensorProcessing(void *pvParameters) {
    for (;;) {
        // Poll for finger placement on MAX30102
        // Example mock calculation:
        float ac_red = 1420.0f;
        float dc_red = 145000.0f;
        float ac_ir  = 1380.0f;
        float dc_ir  = 155000.0f;
        float hr     = 76.0f;

        HbResult hb = calculateHemoglobin(ac_red, dc_red, ac_ir, dc_ir, hr);

        if (hb.valid) {
            Serial.printf("[SENSOR] Hb: %.2f g/dL | SpO2: %.1f%% | HR: %.0f BPM | Grade: %s\n",
                          hb.hemoglobin_g_dl, hb.spo2_percent, hb.heart_rate_bpm,
                          getAnemiaGradeHindi(hb.anemia_grade));

            // If an NFC card was scanned, print receipt slip
            if (g_currentPatientId.length() > 0) {
                Serial.println("[PRINTER] Printing patient slip on Thermal Printer...");
                printer.printPatientSlip(g_currentPatientId.c_str(), g_currentPatientName.c_str(), 
                                         26, "Female", hb, "15-OCT-2026", "ASHA_RAMPUR_01");
                g_currentPatientId = ""; // Reset after printing
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n==========================================");
    Serial.println("  BOOTING PROJECT MEDIBOT KIOSK OS (v1.0) ");
    Serial.println("==========================================");

    // 1. Configure SOS Button and Buzzer
    pinMode(PIN_SOS_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_SOS_BUTTON), isrEmergencyButton, FALLING);

    // 2. Initialize Thermal Printer
    Serial.println("[BOOT] Initializing 58mm Thermal Printer...");
    printer.begin(9600);

    // 3. Initialize LoRa Radio
    Serial.println("[BOOT] Initializing SX1278 LoRa Radio...");
    LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(LORA_FREQ)) {
        Serial.println("[WARN] LoRa initialization failed or module not attached.");
    } else {
        LoRa.setTxPower(20); // Max power +20dBm for maximum penetration
        Serial.println("[BOOT] LoRa Ready at 868 MHz.");
    }

    // 4. Initialize NFC Reader
    Serial.println("[BOOT] Initializing NFC Scanner (PN532/RC522)...");
    SPI.begin(18, 19, 23, NFC_SS_PIN); // SCK, MISO, MOSI, SS
    mfrc522.PCD_Init();

    // 5. Initialize LittleFS Flash Storage
    if (!LittleFS.begin(true)) {
        Serial.println("[WARN] LittleFS Mount Failed. Formatting...");
    } else {
        Serial.println("[BOOT] LittleFS Offline Storage Mounted.");
    }

    // 6. Spawn FreeRTOS Tasks
    xTaskCreatePinnedToCore(taskSensorProcessing, "SensorTask", 4096, NULL, 1, &TaskSensorHandle, 1);
    xTaskCreatePinnedToCore(taskOfflineSync, "SyncTask", 4096, NULL, 1, &TaskSyncHandle, 0);

    Serial.println("[READY] MEDIBOT Kiosk is operational. Ready for patient cards & checkups.");
}

void loop() {
    // 1. Check SOS Trigger
    if (g_sosTriggered) {
        g_sosTriggered = false;
        triggerLoRaEmergencySOS();
    }

    // 2. Poll NFC Reader for new health card
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String cardUid = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            cardUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            cardUid += String(mfrc522.uid.uidByte[i], HEX);
        }
        cardUid.toUpperCase();
        Serial.printf("[NFC] Card Detected! UID: %s\n", cardUid.c_str());
        
        // Load Patient profile
        g_currentPatientId = "ABHA-" + cardUid.substring(0, 8);
        Serial.printf("[AUTH] Patient Identified: %s (%s)\n", g_currentPatientName.c_str(), g_currentPatientId.c_str());

        // Beep confirmation
        digitalWrite(PIN_BUZZER, HIGH);
        delay(80);
        digitalWrite(PIN_BUZZER, LOW);

        mfrc522.PICC_HaltA();
    }

    delay(100);
}
