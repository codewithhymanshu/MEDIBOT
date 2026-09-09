# 🩺 PROJECT MEDIBOT (प्रोजेक्ट मेडीबॉट)
> **ग्रामीण भारत के लिए रग्ड, ऑफलाइन-फर्स्ट, स्मार्ट IoT हेल्थकेयर कियॉस्क**  
> *Dedicated to empowering Anganwadi & ASHA Workers with real-time, non-invasive, emergency-ready healthcare.*

---

## 🌟 1. विज़न (Vision)
भारत के ग्रामीण इलाकों में स्वास्थ्य सेवा की रीढ़ हमारी **ASHA (Accredited Social Health Activist)** और **आंगनवाड़ी कार्यकर्ता** हैं। आज भी वे भारी-भरकम कागज़ी रजिस्टरों, शून्य इंटरनेट कनेक्टिविटी और सीमित मेडिकल उपकरणों के साथ काम कर रही हैं। 

**MEDIBOT** एक बेहद कम लागत वाला (₹2,500 - ₹3,000 मास प्रोडक्शन), लॉक-डाउन, पोर्टेबल मेडिकल कियॉस्क है जो:
- बिना इंटरनेट के हफ्तों तक ऑफलाइन काम करता है।
- सुई चुभाए बिना (Non-invasive) हीमोग्लोबिन (Hb) और एनीमिया की जांच करता है।
- स्मार्ट NFC हेल्थ कार्ड से 1 सेकंड में मरीज की हिस्ट्री खोलता है।
- इनबिल्ट थर्मल प्रिंटर से मरीज को तुरंत प्रिस्क्रिप्शन / वैक्सीन पर्ची देता है।
- बिना सिम कार्ड के LoRa रेडियो द्वारा 10-15 किमी दूर PHC/अस्पताल तक इमरजेंसी SOS भेजता है।

---

## 🛠️ 2. सिस्टम आर्किटेक्चर (System Architecture)

```mermaid
graph TD
    subgraph Kiosk Hardware [MEDIBOT Kiosk (ESP32-WROVER)]
        MCU[ESP32 WROVER - FreeRTOS]
        NFC[PN532 / RC522 NFC Reader] -->|SPI/I2C| MCU
        PPG[MAX30102 Optical PPG Sensor] -->|I2C| MCU
        PRINTER[2-inch Embedded Thermal Printer] -->|UART| MCU
        LORA[SX1278 LoRa Module 433/868 MHz] -->|SPI| MCU
        OLED[3.5 inch SPI Display / Keypad] -->|SPI/GPIO| MCU
        FLASH[SPIFFS / MicroSD Storage] -->|SPI| MCU
        BATTERY[Dual 18650 Li-ion + TP4056 BMS] --> MCU
    end

    subgraph Offline Operation
        MCU -->|Offline Cache Encrypted| FLASH
        MCU -->|Print Slip| PRINTER
        MCU -->|SOS Signal (No Sim/Internet)| LORA
    end

    subgraph Connectivity & Sync
        LORA -.->|10-15 km Radio Link| PHC_GATEWAY[PHC LoRa Gateway]
        PHC_GATEWAY --> AMBULANCE[Emergency Medical Dispatch]
        
        FLASH -->|Auto-sync when WiFi/Hotspot found| CLOUD[MEDIBOT Sync Server]
        CLOUD --> ABDM[ABDM / NDHM (Ayushman Bharat) M1/M2/M3]
        CLOUD --> POSHAN[Poshan Tracker API]
    end
```

---

## 📋 3. मुख्य तकनीकी विशेषताएं (Key Specifications)

| घटक (Component) | चयन (Selection) | कारण (Why chosen) |
|---|---|---|
| **माइक्रोकंट्रोलर (Brain)** | ESP32-WROVER (8MB PSRAM) | डुअल-कोर, इनबिल्ट वाई-फाई/ब्लूटूथ, बड़ा बफर (TinyML और प्रिंटर ग्राफिक्स के लिए)। |
| **नॉन-इनवेसिव Hb सेंसर** | MAX30102 (Red 660nm + IR 880nm) | ऑप्टिकल PPG से $R = \frac{AC_{660}/DC_{660}}{AC_{880}/DC_{880}}$ अनुपात और AI मॉडल द्वारा Hb (g/dL) प्रेडिक्शन। |
| **रजिस्ट्रेशन / पहचान** | NTAG213 / NTAG215 NFC Cards | ₹15-₹20 प्रति कार्ड। टैप करते ही पेशेंट ID और ऑफलाइन टोकन लोड। |
| **प्रिस्क्रिप्शन प्रिंटर** | CSN-A2 / JP-QR701 58mm Thermal Printer | नो-इंक, लो-पावर UART प्रिंटर। तुरंत प्रिस्क्रिप्शन और अगली टीका तारीख की पर्ची। |
| **इमरजेंसी SOS** | SX1276/SX1278 LoRa Transceiver (868/433 MHz) | 10-15 किमी लाइन-ऑफ़-साइट रेंज। बिना किसी सेल्युलर नेटवर्क या टेलीकॉम रिचार्ज के फ्री रेडियो अलर्ट। |
| **लोकल स्टोरेज** | MicroSD Card + Onboard Flash (SPIFFS/LittleFS) | 50,000+ मरीजों के रिकॉर्ड्स का SQLite / JSON ऑफलाइन एनक्रिप्टेड स्टोरेज। |
| **पावर सिस्टम** | 2x 18650 Li-ion (5200 mAh) + 5V/9V Boost BMS | एक बार चार्ज करने पर 24-36 घंटे का फील्ड बैकअप। |

---

## 🎯 4. Antigravity चर्चा एजेंडा के सीधे समाधान (Direct Answers to Key Points)

### 1. हार्डवेयर और सप्लाई चेन (PCB & 3D Casing)
- **कस्टम PCB**: EasyEDA या KiCAD में 2-लेयर कॉम्पैक्ट बोर्ड। JLCPCB या भारतीय फैब्रिकेटर्स (PCB Power / LionCircuits) से ₹50-₹80 प्रति बोर्ड में मैन्युफैक्चर हो सकता है।
- **रग्ड केसिंग**: फील्ड में गिरने और पानी/धूल से बचाव के लिए IP54-रेटेड रबर ग्रिप वाला 3D-प्रिंटेड ABS/PETG बॉडी डिज़ाइन।

### 2. सेंसर कैलिब्रेशन (AI/ML Hemoglobin Algorithm)
- MAX30102 सेंसर रेड (660nm) और इन्फ्रारेड (880/940nm) पल्स वेवफॉर्म देता है।
- **अल्गोरिदम**: डीसी और एसी घटकों का रेशो-ऑफ-रेशियो ($R$ value) और पल्स ट्रांजिट टाइम/वेवफॉर्म मोर्फोलॉजी से हीमोग्लोबिन प्रेडिक्ट किया जाता है।
- हमने [ml/hb_estimator.py](file:///c:/Users/Harsh/Desktop/MEDIBOT/ml/hb_estimator.py) में इसका ट्रेनर और C++ TinyML इन्फरेंस कोड तैयार किया है।

### 3. इन्क्यूबेशन और पायलट टेस्टिंग (Pilot Framework)
- 2 हफ्तों के अंदर MVP तैयार कर स्थानीय प्राथमिक स्वास्थ्य केंद्र (PHC) और जिला मुख्य चिकित्सा अधिकारी (CMO / CDPO) के साथ MoU / परमिशन के लिए ड्राफ्ट प्रपोजल।
- 50 गर्भवती महिलाओं और बच्चों पर ट्रेडिशनल पैथोलॉजी टेस्ट (Cyanmethemoglobin method) और MEDIBOT के रीडिंग्स का तुलनात्मक डेटासेट।

### 4. गवर्नमेंट API इंटीग्रेशन (ABDM & Poshan Tracker)
- **ABDM (Ayushman Bharat Digital Mission)**:
  - M1: ABHA (Ayushman Bharat Health Account) क्रिएशन / वेरिफिकेशन (NFC कार्ड पर ABHA ID स्टोर)।
  - M2: HIP (Health Information Provider) - ओपीडी और वाइटल्स को FHIR JSON फॉर्मेट में सर्वर पर अपलोड।
  - M3: HIU (Health Information User) - डॉक्टर द्वारा पूर्व हिस्ट्री देखना।
- **Poshan Tracker**: WCD (महिला एवं बाल विकास मंत्रालय) के लिए बच्चे का वजन, ऊंचाई, और एनीमिया ग्रेडिंग को बैच-सिंक (Batch-sync API) करना।

### 5. फंडिंग और सरकारी ग्रांट्स (Grants & Seed Funding)
1. **BIRAC BIG (Biotechnology Ignition Grant)**: हेल्थ-टेक और मेडिकल डिवाइसेज के लिए ₹50 लाख तक 100% इक्विटी-फ्री ग्रांट।
2. **NIDHI-PRAYAS (DST)**: प्रोटोटाइप हार्डवेयर डेवलपमेंट के लिए ₹10 लाख तक ग्रांट।
3. **Startup India Seed Fund Scheme (SISFS)**: इनक्यूबेटर के जरिए ₹20 लाख तक ग्रांट।
4. **MeitY TIDE 2.0 / NASSCOM IoT**: ₹4 लाख से ₹7 लाख तक अर्ली-स्टेज प्रोटोटाइप ग्रांट।

---

## 📁 प्रोजेक्ट डायरेक्टरी स्ट्रक्चर (Separated Hardware & Software Modules)

```
MEDIBOT/
├── README.md                      # मुख्य अवलोकन व विज़न
├── docs/                          # सरकारी ग्रांट्स और पायलट परीक्षण दस्तावेज़
│   ├── PITCH_AND_GRANTS.md        # ₹50L BIRAC BIG / NIDHI-PRAYAS ग्रांट गाइड
│   └── PILOT_TEST_PLAN.md         # PHC/आंगनवाड़ी 30-दिवसीय पायलट ट्रायल फ्रेमवर्क
│
├── hardware/                      # 🔩 [HARDWARE MODULE]
│   ├── README.md                  # हार्डवेयर आर्किटेक्चर व इंजीनियरिंग विनिर्देश
│   ├── BOM.md                     # विस्तृत बिल ऑफ मैटेरियल्स और भारतीय वेंडर्स
│   ├── PINOUT.md                  # ESP32-WROVER GPIO पिन असाइनमेंट व बस-शेयरिंग
│   ├── POWER_BUDGET.md            # बैटरी क्षमता, 55 घंटे बैकअप व BMS बूस्ट एनालिसिस
│   ├── ENCLOSURE_3D_SPECS.md      # रग्ड 3D प्रिंटेड केसिंग के तकनीकी आयाम (CAD Specs)
│   └── tests/
│       └── hardware_self_test.cpp # फैक्ट्री असेंबली के बाद हार्डवेयर डायग्नोस्टिक टेस्ट
│
└── software/                      # 💻 [SOFTWARE MODULE]
    ├── README.md                  # सॉफ्टवेयर आर्किटेक्चर व 4-स्तरीय स्टैक गाइड
    ├── firmware/                  # ESP32-WROVER FreeRTOS एम्बेडेड Kiosk OS
    │   ├── platformio.ini         # PlatformIO बोर्ड डिपेंडेंसीज
    │   └── src/
    │       ├── main.cpp           # मल्टी-टास्क स्टेट मशीन व पेरिफेरल कंट्रोलर
    │       ├── hb_algorithm.h     # ऑप्टिकल हीमोग्लोबिन प्रेडिक्शन (C++ TinyML)
    │       └── thermal_printer.h  # ESC/POS पर्ची और प्रिस्क्रिप्शन जनरेटर
    ├── edge_ml/                   # सिग्नल प्रोसेसिंग और ML कैलिब्रेशन इंजन
    │   ├── hb_estimator.js        # नोड.जेएस / पायथन PPG कैलिब्रेशन इंजन
    │   ├── hb_estimator.py        # साइकिट-लर्न / लाइटजीबीएम PPG मॉडल ट्रेनर
    │   └── ppg_dataset_mock.csv   # 600 क्लिनिकल रीडिंग्स का कैलिब्रेशन डेटासेट
    ├── backend/                   # सेंट्रलाइज्ड ABDM व गवर्नमेंट सिंक गेटवे
    │   ├── server.js              # फास्ट ऑफलाइन सिंक, FHIR R4, LoRa गेटवे
    │   └── package.json           # बैकएंड डिपेंडेंसीज
    └── kiosk_ui/                  # ASHA वर्कर टचस्क्रीन वेब ऐप / PWA (हिंदी + English)
        ├── index.html             # टचस्क्रीन कियॉस्क यूआई (NFC, Thumb Scan, Print)
        ├── style.css              # मॉडर्न रग्ड टचस्क्रीन स्टाइलिंग
        └── app.js                 # रियल-टाइम PPG पल्स ग्राफ व ऑडियो फीडबैक
```
