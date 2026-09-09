# 🛠️ MEDIBOT - Hardware Engineering Module

यह डायरेक्टरी MEDIBOT पोर्टेबल कियॉस्क के संपूर्ण हार्डवेयर डिज़ाइन, सर्किट स्कीमेटिक्स, 3D केसिंग CAD विनिर्देशों, और पावर मैनेजमेंट को समर्पित है।

---

## 📂 हार्डवेयर डायरेक्टरी स्ट्रक्चर (Hardware Directory Map)

```
hardware/
├── README.md                 # हार्डवेयर मॉड्यूल अवलोकन और विनिर्देश
├── BOM.md                    # विस्तृत बिल ऑफ मैटेरियल्स और भारतीय वेंडर्स
├── PINOUT.md                 # ESP32 GPIO पिन असाइनमेंट और बस-शेयरिंग
├── POWER_BUDGET.md           # बैटरी क्षमता, बूस्ट कन्वर्टर और चार्जिंग एनालिसिस
├── ENCLOSURE_3D_SPECS.md     # रग्ड 3D प्रिंटेड केसिंग के तकनीकी आयाम (CAD Specs)
├── SCHEMATIC_GUIDE.md        # PCB लेआउट गाइड, कपैसिटर्स, और लेवल शिफ्टिंग
└── tests/
    └── hardware_self_test.cpp # फैक्ट्री असेंबली के बाद हार्डवेयर डायग्नोस्टिक टेस्ट
```

---

## ⚡ 1. सिस्टम ब्लॉक डायग्राम (Hardware Interconnect)

```mermaid
graph TD
    subgraph Power Architecture [Power Distribution]
        BATT[2x 18650 3.7V 5200mAh] --> BMS[TP4056 Dual BMS + Type-C 5V 2A]
        BMS --> REG_3V3[AMS1117-3.3V LDO]
        BMS --> BOOST_7V4[MT3608 Boost to 7.4V]
        REG_3V3 -->|3.3V VCC| MCU[ESP32-WROVER 8MB PSRAM]
        BOOST_7V4 -->|7.4V High Current| PRINTER[58mm Thermal Printer]
    end

    subgraph Peripherals & Sensors
        MCU -->|I2C 400kHz (GPIO 21, 22)| MAX30102[MAX30102 Optical PPG Sensor]
        MCU -->|Hardware UART2 (GPIO 16, 17)| PRINTER
        MCU -->|SPI Shared (GPIO 18, 19, 23)| LORA[SX1278 LoRa 868MHz]
        MCU -->|SPI Shared| NFC[PN532 / RC522 RFID]
        MCU -->|SPI Shared| SD[MicroSD Card Socket]
        MCU -->|SPI / Parallel| TFT[3.5 inch Touch Display]
        MCU <--|GPIO 34 Interrupt| SOS[Physical Red Industrial Button]
        MCU <--|GPIO 35 ADC1| VDIV[Battery Voltage Divider]
    end
```

---

## 🎯 2. हार्डवेयर की मुख्य विशेषताएं
1. **पोर्टेबल और रग्ड**: 1.2 किग्रा कुल वजन, हैंडहेल्ड ग्रिप के साथ।
2. **धूल व पानी से बचाव**: IP54 ग्रेड रबर सील व सिलिकॉन पोर्ट कवर्स।
3. **कम लागत में मास मैन्युफैक्चरिंग**: 1,000 यूनिट्स पर प्रति डिवाइस लागत **₹2,693 - ₹2,900**।
4. **ऑल-इन-वन डायग्नोस्टिक फर्मवेयर**: हार्डवेयर जोड़ने के तुरंत बाद `hardware/tests/hardware_self_test.cpp` चलाकर सभी सेंसर और पेरिफेरल्स की जांच की जा सकती है।
