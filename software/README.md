# 💻 MEDIBOT - Software Architecture & Engineering Module

यह डायरेक्टरी MEDIBOT के संपूर्ण सॉफ़्टवेयर स्टैक को समर्पित है, जो एम्बेडेड फर्मवेयर (ESP32), ऑन-डिवाइस Edge AI/TinyML, क्लाउड सिंक गेटवे, और ASHA वर्कर टचस्क्रीन UI को समाहित करता है।

---

## 📂 सॉफ़्टवेयर डायरेक्टरी स्ट्रक्चर (Software Directory Map)

```
software/
├── README.md                 # सॉफ्टवेयर मॉड्यूल अवलोकन एवं गाइड
├── firmware/                 # ESP32-WROVER FreeRTOS एम्बेडेड Kiosk OS
│   ├── platformio.ini        # PlatformIO बोर्ड डिपेंडेंसीज
│   └── src/
│       ├── main.cpp          # मल्टी-टास्क स्टेट मशीन व पेरिफेरल कंट्रोलर
│       ├── hb_algorithm.h    # ऑप्टिकल हीमोग्लोबिन प्रेडिक्शन (C++ TinyML)
│       └── thermal_printer.h # ESC/POS पर्ची और प्रिस्क्रिप्शन जनरेटर
├── edge_ml/                  # सिग्नल प्रोसेसिंग और ML कैलिब्रेशन इंजन
│   ├── hb_estimator.js       # नोड.जेएस / पायथन PPG कैलिब्रेशन इंजन
│   ├── hb_estimator.py       # साइकिट-लर्न / लाइटजीबीएम PPG मॉडल ट्रेनर
│   └── ppg_dataset_mock.csv  # 600 क्लिनिकल रीडिंग्स का कैलिब्रेशन डेटासेट
├── backend/                  # सेंट्रलाइज्ड ABDM व गवर्नमेंट सिंक गेटवे
│   ├── server.js             # फास्ट ऑफलाइन सिंक, FHIR R4, LoRa गेटवे
│   └── package.json          # बैकएंड डिपेंडेंसीज
└── kiosk_ui/                 # ASHA वर्कर टचस्क्रीन वेब ऐप / PWA
    ├── index.html            # टचस्क्रीन कियॉस्क यूआई (हिंदी + English)
    ├── style.css             # मॉडर्न रग्ड ग्लास-मॉर्फिक डिज़ाइन
    └── app.js                # ऑफलाइन-फर्स्ट स्टेट, वाइटल्स ग्राफ, पर्ची प्रिंट
```

---

## ⚙️ 1. चार-स्तरीय सॉफ़्टवेयर आर्किटेक्चर (4-Tier Software Stack)

```mermaid
graph TD
    subgraph Layer 1: Embedded OS [Tier 1: ESP32 FreeRTOS Firmware]
        CORE1[Core 1: Sensor Sampling & TinyML Inference]
        CORE0[Core 0: LoRa SOS Radio & Offline LittleFS Queue]
    end

    subgraph Layer 2: Edge AI [Tier 2: On-Device Machine Learning]
        PPG_FILTER[Butterworth Bandpass Filter] --> RATIO_ENGINE[Beer-Lambert R Ratio Engine]
        RATIO_ENGINE --> EMBEDDED_MODEL[Trained Multivariable Model (hb_algorithm.h)]
        EMBEDDED_MODEL --> ANEMIA_GRADE[Anemia WHO Severity Classification]
    end

    subgraph Layer 3: Kiosk UX [Tier 3: Bilingual ASHA Kiosk Interface]
        UI[Touchscreen PWA UI in Hindi/English]
        AUDIO[Hindi Voice Prompts for illiterate patients]
    end

    subgraph Layer 4: Cloud & ABDM [Tier 4: National Health Gateway]
        FHIR_GEN[FHIR R4 Observation Bundle Builder]
        ABHA_M1[M1: ABHA ID NFC Mapping]
        HIP_M2[M2: Health Information Provider Upload]
        POSHAN_API[Poshan Tracker Batch API]
    end

    CORE1 --> Layer 2
    CORE0 --> Layer 4
    Layer 2 --> Layer 3
```

---

## 🔒 2. ऑफलाइन-फर्स्ट सुरक्षा और डेटा अखंडता (Security & Offline Integrity)
1. **AES-128 एन्क्रिप्शन**: जब इंटरनेट नहीं होता, मरीज का डेटा ESP32 के इंटरनल LittleFS फ़्लैश में एन्क्रिप्टेड फॉर्मेट में स्टोर रहता है।
2. **शून्य डेटा लॉस**: प्रत्येक चेकअप रिकॉर्ड में एक मोनोटोनिक सीक्वेंस नंबर होता है। इंटरनेट आने पर सर्वर केवल अन-सिंक किए गए रिकॉर्ड्स को लेता है और डुप्लीकेट प्रविष्टियों को रोकता है।
3. **ABDM FHIR R4 कंप्लायंस**: डेटा सीधा LOINC कोड `718-7` (Hemoglobin [Mass/volume] in Blood) के अंतर्गत मैप होकर सरकार के आयुष्मान भारत नेटवर्क से जुड़ता है।
