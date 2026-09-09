# 🔩 MEDIBOT Hardware Bill of Materials (BOM)

इस दस्तावेज़ में प्रोटोटाइप (Prototype) और मास प्रोडक्शन (Mass Production - 1,000+ यूनिट्स) के लिए सभी कॉम्पोनेंट्स, भारतीय वेंडर्स, और अनुमानित लागत का विवरण है।

---

## 📊 1. कॉम्पोनेंट लागत तालिका (Component Cost Sheet)

| # | कॉम्पोनेंट का नाम | स्पेसिफिकेशन | प्रोटोटाइप लागत (₹) | स्केल लागत (₹ - 1k+ Qty) | सुझाया गया सप्लायर |
|---|---|---|---|---|---|
| 1 | **ESP32-WROVER-E** | 8MB PSRAM, 16MB Flash, Dual-core 240MHz, WiFi + BLE | ₹420 | ₹240 | Robu.in / LCSC |
| 2 | **MAX30102 PPG Sensor** | Red (660nm) + IR (880nm) High-sensitivity Pulse Oximeter | ₹220 | ₹95 | Robu / Quartz Components |
| 3 | **Embedded Thermal Printer** | 58mm Mini Embedded Receipt Printer (UART, 5V-9V, QR Code support) | ₹1,850 | ₹1,100 | Sharvi Electronics / Alibaba |
| 4 | **LoRa Module (SX1278)** | 433MHz / 868MHz SPI Transceiver, +20dBm, 10-15km range | ₹350 | ₹190 | Robu.in / Sunrom |
| 5 | **NFC Module (PN532 / RC522)** | 13.56MHz RFID/NFC Reader (SPI/I2C/UART), ISO14443A | ₹180 | ₹90 | Robu.in |
| 6 | **NFC Cards (100 pcs)** | NTAG213 / NTAG215 Rewriteable PVC Cards | ₹15 / कार्ड | ₹8 / कार्ड | IndiaMART / Robu |
| 7 | **Display (3.5" TFT or 128x64 OLED)** | ST7796 3.5" Color SPI Display (with Touch) | ₹650 | ₹380 | Robu / LCSC |
| 8 | **MicroSD Card Module** | SPI MicroSD Slot + 16GB Industrial Class 10 Card | ₹280 | ₹160 | Local Market / SanDisk |
| 9 | **Battery Pack** | 2x 18650 3.7V 2600mAh Li-ion (Total: 5200mAh 7.4V/3.7V) | ₹320 | ₹180 | Robu / EV Battery Vendors |
| 10 | **Power Management (BMS + Boost)** | TP4056 Type-C Charger + MT3608 5V/9V Step-up Converter | ₹90 | ₹45 | Robu / Quartz |
| 11 | **Physical SOS Button & Enclosure** | Heavy-duty Industrial Red Push Button + 3D Printed Case | ₹350 | ₹140 (Injection Mold) | Local 3D Print / Make3D |
| 12 | **Custom PCB + Passive Parts** | 2-Layer FR4 PCB, Resistors, Capacitors, Connectors | ₹450 (Proto Batch) | ₹65 | JLCPCB / LionCircuits |
| | **कुल अनुमानित लागत (Total Estimated Cost)** | | **₹5,175 - ₹5,400** | **₹2,693 - ₹2,900** | |

---

## ⚡ 2. पावर बजट और बैटरी बैकअप (Power Consumption Analysis)

गांवों में बिजली 8-12 घंटे ही उपलब्ध रहती है, इसलिए MEDIBOT को कम से कम 24 घंटे लगातार काम करने के लिए डिज़ाइन किया गया है:

| मोड | सक्रिय कॉम्पोनेंट | करंट की खपत (mA @ 3.7V) | प्रतिशत समय |
|---|---|---|---|
| **Deep Sleep / Standby** | ESP32 Sleep + RTC + NFC Wakeup interrupt | ~3 - 5 mA | 70% समय |
| **पेशेंट चेकअप (वाइटल्स जांच)** | Display ON + MAX30102 + ESP32 Active | ~120 - 150 mA | 20% समय |
| **पर्ची प्रिंटिंग (Peak Load)** | Thermal Print Head Heating (2-3 सेकंड प्रति पर्ची) | ~1,200 - 1,800 mA (Peak) | 2% समय |
| **LoRa SOS ट्रांसमिशन** | SX1278 +20dBm Transmission | ~120 mA (1-2 सेकंड) | 1% समय |
| **WiFi Sync (जब नेटवर्क मिले)** | ESP32 WiFi TX/RX | ~180 - 240 mA | 7% समय |

- **औसत खपत (Average Current)**: ~85 - 110 mA
- **बैटरी क्षमता**: 5,200 mAh (2x 18650 Cells)
- **अनुमानित फील्ड बैकअप**: **45 से 55 घंटे** (सामान्य उपयोग में 3-4 दिन बिना चार्जिंग के)।
- **चार्जिंग पोर्ट**: USB Type-C (5V 2A सामान्य मोबाइल चार्जर से 3.5 घंटे में फुल चार्ज)।

---

## 🏭 3. सप्लाई चेन और मैन्युफैक्चरिंग पार्टनरशिप्स

1. **कस्टम PCB फैब्रिकेशन**:
   - प्रोटोटाइपिंग के लिए: [JLCPCB](https://jlcpcb.com) या भारतीय वेंडर [LionCircuits](https://lioncircuits.com) (बेंगलुरु)।
   - एसएमटी असेंबली (SMT Assembly): JLCPCB SMT Service द्वारा सभी SMD पार्ट्स (ESP32, कैपेसिटर्स, वोल्टेज रेगुलेटर्स) पहले से असेंबल होकर आएंगे।
2. **3D प्रिंटिंग और केसिंग**:
   - प्रोटोटाइप: PETG / ABS मटेरियल में 20% इन्फिल के साथ (रग्ड, गिरकर टूटने से सुरक्षित)।
   - मास स्केल: स्थानीय मोल्ड निर्माताओं (अहमदाबाद/पुणे) से एल्युमिनियम टूलिंग द्वारा इंजेक्शन मोल्डेड पॉलीकार्बोनेट केस।
3. **सेंसर और प्रिंटर वेंडर एग्रीमेंट्स**:
   - सीधे OEM निर्माताओं (जैसे Xiamen Cashino या EPST Thermal Printers) से 1,000 पीस का बल्क आर्डर करने पर थर्मल प्रिंटर ₹900-₹1,000 में उपलब्ध हो जाता है।
