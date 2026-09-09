// MEDIBOT Embedded Mini 58mm Thermal Printer Driver (UART)
// Generates physical paper slips for rural patients (Prescriptions, Vitals & Next Vaccine Date)
#ifndef THERMAL_PRINTER_H
#define THERMAL_PRINTER_H

#include <Arduino.h>
#include "hb_algorithm.h"

class ThermalPrinter {
private:
    HardwareSerial* _serial;

    void sendCommand(const uint8_t* cmd, size_t len) {
        _serial->write(cmd, len);
    }

public:
    ThermalPrinter(HardwareSerial* serialPort) : _serial(serialPort) {}

    void begin(unsigned long baudRate = 9600) {
        _serial->begin(baudRate, SERIAL_8N1, 16, 17); // RX2 = GPIO 16, TX2 = GPIO 17
        reset();
    }

    void reset() {
        uint8_t initCmd[] = { 0x1B, 0x40 }; // ESC @
        sendCommand(initCmd, sizeof(initCmd));
    }

    void setBold(bool enable) {
        uint8_t boldCmd[] = { 0x1B, 0x45, (uint8_t)(enable ? 1 : 0) }; // ESC E n
        sendCommand(boldCmd, sizeof(boldCmd));
    }

    void setAlignCenter() {
        uint8_t alignCenter[] = { 0x1B, 0x61, 0x01 }; // ESC a 1
        sendCommand(alignCenter, sizeof(alignCenter));
    }

    void setAlignLeft() {
        uint8_t alignLeft[] = { 0x1B, 0x61, 0x00 }; // ESC a 0
        sendCommand(alignLeft, sizeof(alignLeft));
    }

    void feed(uint8_t lines = 2) {
        uint8_t feedCmd[] = { 0x1B, 0x64, lines }; // ESC d n
        sendCommand(feedCmd, sizeof(feedCmd));
    }

    void printPatientSlip(const char* patientId, const char* name, int age, const char* gender,
                          const HbResult& hbData, const char* nextVaccineDate, const char* ashaWorkerId) {
        reset();
        
        // Header
        setAlignCenter();
        setBold(true);
        _serial->println("================================");
        _serial->println("      PROJECT MEDIBOT KIOSK     ");
        _serial->println("  RURAL HEALTH CHECKUP SLIP     ");
        _serial->println("  National Health Mission / ASHA");
        _serial->println("================================");
        setBold(false);

        // Patient Details
        setAlignLeft();
        _serial->printf("Patient ID  : %s\n", patientId);
        _serial->printf("Name        : %s\n", name);
        _serial->printf("Age/Gender  : %d Yrs / %s\n", age, gender);
        _serial->printf("ASHA Center : %s\n", ashaWorkerId);
        _serial->println("--------------------------------");

        // Vitals & Non-Invasive Lab Report
        setBold(true);
        _serial->println("[ VITAL HEALTH REPORT ]");
        setBold(false);
        _serial->printf("Hemoglobin (Hb) : %.1f g/dL\n", hbData.hemoglobin_g_dl);
        _serial->printf("Anemia Status   : %s\n", getAnemiaGradeHindi(hbData.anemia_grade));
        _serial->printf("Blood Oxygen    : %.1f %% SpO2\n", hbData.spo2_percent);
        _serial->printf("Pulse Rate      : %.0f BPM\n", hbData.heart_rate_bpm);
        _serial->println("--------------------------------");

        // Guidance & Vaccine
        setBold(true);
        _serial->println("[ NEXT APPOINTMENT / VACCINE ]");
        setBold(false);
        _serial->printf("Due Date    : %s\n", nextVaccineDate);
        _serial->println("Note: Carry this slip to PHC.");
        if (hbData.anemia_grade >= ANEMIA_MODERATE) {
            setBold(true);
            _serial->println("! ALERT: Doctor consultation required for Iron/Folic Acid!");
            setBold(false);
        }
        _serial->println("================================");

        feed(4); // Paper feed so user can tear off slip
    }
};

#endif // THERMAL_PRINTER_H
