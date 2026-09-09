# 🔌 ESP32-WROVER Pinout & Wiring Configuration

ESP32-WROVER के पास 38 पिन्स हैं। चूंकि हमें एक साथ कई परिधीय उपकरण (NFC, Display, Thermal Printer, LoRa, MAX30102, SOS Button) जोड़ने हैं, इसलिए बस-शेयरिंग और GPIO असाइनमेंट बहुत सावधानी से किया गया है:

---

## 🗺️ 1. GPIO पिन असाइनमेंट मैट्रिक्स (Pin Assignment Matrix)

```
                       +-------------------------+
                       |      ESP32-WROVER       |
                       +-------------------------+
       [3.3V] -------- | 3V3                 GND | -------- [GND]
       [RESET BUTTON]- | EN                 GPIO23 | -------- [SPI MOSI (LoRa, Display, SD)]
 [MAX30102 I2C SDA] -- | GPIO21             GPIO22 | -------- [MAX30102 I2C SCL]
 [THERMAL PRINTER TX]- | GPIO17 (TX2)       GPIO16 | -------- [THERMAL PRINTER RX (RX2)]
     [LORA NSS / CS] - | GPIO5              GPIO18 | -------- [SPI SCK (Shared)]
     [LORA DIO0 INT] - | GPIO26             GPIO19 | -------- [SPI MISO (Shared)]
    [LORA RST RESET] - | GPIO27             GPIO4  | -------- [NFC SS / CS]
  [TFT DISPLAY CS] --- | GPIO15             GPIO2  | -------- [TFT DC / RS]
   [TFT DISPLAY RST] - | GPIO0              GPIO14 | -------- [SD CARD CS]
   [NFC IRQ INT] ----- | GPIO32             GPIO33 | -------- [STATUS LED / BUZZER]
   [PHYSICAL SOS BTN]- | GPIO34 (Input only)GPIO35 | -------- [BATTERY VOLTAGE ADC]
                       +-------------------------+
```

---

## 🚌 2. बस आर्किटेक्चर (Bus Sharing)

### A. I2C Bus (Pins: SDA=21, SCL=22, Frequency: 400kHz Fast Mode)
- **MAX30102 PPG Sensor** (Address: `0x57`)
- *वैकल्पिक: DS3231 Real-Time Clock RTC (Address: `0x68`) यदि नेटवर्क के बिना सटीक समय चाहिए।*

### B. High-Speed SPI Bus (Pins: SCK=18, MOSI=23, MISO=19)
- **SX1278 LoRa Module**: Chip Select (`CS = GPIO 5`), Interrupt (`DIO0 = GPIO 26`)
- **NFC Reader (PN532/RC522)**: Chip Select (`CS = GPIO 4`)
- **TFT 3.5" Display**: Chip Select (`CS = GPIO 15`), Data/Command (`DC = GPIO 2`)
- **MicroSD Card**: Chip Select (`CS = GPIO 14`)
*(नोट: SPI में मल्टीपल डिवाइसेज अपने-अपने CS पिन के ज़रिए बस शेयर करती हैं।)*

### C. Hardware UART2 (Pins: TX2=17, RX2=16, Baud: 9600 / 19200)
- **58mm Thermal Printer**:
  - `ESP32 TX2 (GPIO 17)` ➡️ `Printer RX`
  - `ESP32 RX2 (GPIO 16)` ⬅️ `Printer TX` (पेपर स्टेटस और एरर डिटेक्शन)

### D. Dedicated Interrupts & Analog Inputs
- **Emergency SOS Push Button**: `GPIO 34` (Internal/External Pull-up, Falling Edge Trigger Interrupt).
- **Battery Monitor**: `GPIO 35` (ADC1 Channel 7, Voltage divider 100kΩ / 100kΩ) से सीधे बैटरी का % नापा जाता है।
- **Buzzer & Beep Alert**: `GPIO 33` (PWM tone feedback जब कार्ड स्कैन हो या टेस्ट पूरा हो)।

---

## ⚡ 3. पावर डिस्ट्रीब्यूशन और लेवल शिफ्टिंग
- ESP32, MAX30102, SX1278, और NFC सभी **3.3V लॉजिक** पर सुरक्षित काम करते हैं।
- **थर्मल प्रिंटर**: 5V - 9V पावर सप्लाई पर काम करता है। इसे सीधे 2-सेल बैटरी पैक (7.4V) या 3.7V बैटरी से 7.4V स्टेप-अप बूस्टर से पावर दी जाती है ताकि प्रिंटिंग डार्क और तेज़ हो।
