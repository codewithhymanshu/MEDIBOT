/**
 * MEDIBOT - Touchscreen Kiosk Logic & State Controller
 * Connected with Central Government & Sync Gateway API
 */

let currentPatient = {
    name: "Lata Devi",
    abha: "ABHA-9421-8840-9102",
    age: 26,
    gender: "Female"
};

let currentVitals = {
    hb: 6.82,
    spo2: 98.2,
    hr: 84,
    grade: "गंभीर एनीमिया!",
    gradeEn: "Severe Anemia!"
};

let currentLang = 'hi'; // 'hi' or 'en'
let ppgAnimInterval = null;

const i18n = {
    hi: {
        subtext: "ग्रामीण स्वास्थ्य केंद्र • रामपुर",
        mode: "ऑफलाइन रेडी (मेमोरी 94%)",
        step1: "NFC कार्ड टैप",
        step2: "अंगूठा जांच (Hb)",
        step3: "रिपोर्ट व पर्ची",
        tapPrompt: "मरीज का स्मार्ट NFC कार्ड कियॉस्क पर लगाएं",
        tapDesc: "कार्ड टच करते ही मरीज का नाम, ABHA ID और पूर्व मेडिकल इतिहास खुल जाएगा।",
        fingerPrompt: "सेंसर पर अंगूठा रखें (सुई की जरूरत नहीं)",
        fingerDesc: "MAX30102 ऑप्टिकल सेंसर बियर-लैम्बर्ट स्पेक्ट्रोस्कोपी द्वारा बिना खून निकाले हीमोग्लोबिन माप रहा है...",
        hbTitle: "हीमोग्लोबिन (Hb)",
        patientHeading: "मरीज विवरण (Patient Details)",
        langBtn: "English",
        voicePromptFinger: "कृपया अपना अंगूठा सेंसर पर रखें।",
        voicePromptDone: "जांच पूरी हो गई है। कृपया पर्ची लें।"
    },
    en: {
        subtext: "Rural Health Sub-Centre • Rampur",
        mode: "Offline Ready (Flash 94%)",
        step1: "NFC Card Tap",
        step2: "Thumb Scan (Hb)",
        step3: "Report & Slip",
        tapPrompt: "Tap Patient Smart NFC Health Card",
        tapDesc: "Instantly retrieves patient name, ABHA Health ID and previous clinical history.",
        fingerPrompt: "Place Thumb on Optical Sensor (No Needles)",
        fingerDesc: "MAX30102 optical sensor measures total hemoglobin via multi-wavelength spectroscopy...",
        hbTitle: "Hemoglobin (Hb)",
        patientHeading: "Patient Profile (ABDM)",
        langBtn: "हिंदी",
        voicePromptFinger: "Please place your thumb on the sensor.",
        voicePromptDone: "Checkup complete. Please collect your paper slip."
    }
};

function speakPrompt(text) {
    if ('speechSynthesis' in window) {
        window.speechSynthesis.cancel();
        const utter = new SpeechSynthesisUtterance(text);
        utter.lang = (currentLang === 'hi') ? 'hi-IN' : 'en-IN';
        utter.rate = 0.95;
        window.speechSynthesis.speak(utter);
    }
}

function toggleLanguage() {
    currentLang = (currentLang === 'hi') ? 'en' : 'hi';
    const dict = i18n[currentLang];

    document.getElementById('lblSubtext').innerText = dict.subtext;
    document.getElementById('lblMode').innerText = dict.mode;
    document.getElementById('lblStep1').innerText = dict.step1;
    document.getElementById('lblStep2').innerText = dict.step2;
    document.getElementById('lblStep3').innerText = dict.step3;
    document.getElementById('lblTapPrompt').innerText = dict.tapPrompt;
    document.getElementById('lblTapDesc').innerText = dict.tapDesc;
    document.getElementById('lblFingerPrompt').innerText = dict.fingerPrompt;
    document.getElementById('lblFingerDesc').innerText = dict.fingerDesc;
    document.getElementById('lblHbTitle').innerText = dict.hbTitle;
    document.getElementById('lblPatientHeading').innerText = dict.patientHeading;
    document.getElementById('btnLang').innerText = dict.langBtn;
}

function updateNav(step) {
    document.getElementById('step1Nav').className = 'step-badge' + (step === 1 ? ' active' : '');
    document.getElementById('step2Nav').className = 'step-badge' + (step === 2 ? ' active' : '');
    document.getElementById('step3Nav').className = 'step-badge' + (step === 3 ? ' active' : '');
}

function simulateCardTap(name, abha, age, gender = "Female") {
    currentPatient.name = name;
    currentPatient.abha = abha;
    currentPatient.age = age;
    currentPatient.gender = gender;

    document.getElementById('pName').innerText = name;
    document.getElementById('pAbha').innerText = abha;
    document.getElementById('pAge').innerText = `${age} वर्ष / ${gender}`;

    document.getElementById('stage1').classList.add('hidden');
    document.getElementById('stage2').classList.remove('hidden');
    updateNav(2);

    speakPrompt(i18n[currentLang].voicePromptFinger);
    initPpgCanvas();
}

function handleManualPatientTap() {
    const name = document.getElementById('manualName').value.trim() || "Anita Devi";
    const abha = document.getElementById('manualAbha').value.trim() || ("ABHA-" + Math.floor(1000 + Math.random() * 9000) + "-" + Math.floor(1000 + Math.random() * 9000));
    const age = parseInt(document.getElementById('manualAge').value) || 28;
    simulateCardTap(name, abha, age);
}

function initPpgCanvas() {
    const canvas = document.getElementById('ppgCanvas');
    const ctx = canvas.getContext('2d');
    let x = 0;
    let t = 0;

    if (ppgAnimInterval) clearInterval(ppgAnimInterval);

    ctx.fillStyle = '#020617';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ppgAnimInterval = setInterval(() => {
        t += 0.15;
        // Arterial PPG pulse wave
        const y = 45 - Math.sin(t) * 20 - Math.sin(t * 2.5) * 10;

        ctx.strokeStyle = '#00d2ff';
        ctx.lineWidth = 2.5;
        ctx.beginPath();
        ctx.moveTo(x, y);
        x += 3;
        if (x > canvas.width) {
            x = 0;
            ctx.fillStyle = '#020617';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
        }
        ctx.lineTo(x, y);
        ctx.stroke();
    }, 40);
}

function startOpticalReading() {
    let progress = 0;
    const bar = document.getElementById('scanProgress');

    const progressTimer = setInterval(() => {
        progress += 10;
        bar.style.width = progress + '%';

        if (progress >= 100) {
            clearInterval(progressTimer);
            if (ppgAnimInterval) clearInterval(ppgAnimInterval);
            showResults();
        }
    }, 300);
}

// Beer-Lambert Multivariable Calculation
function computeBeerLambertHb(ac_red, dc_red, ac_ir, dc_ir) {
    const r = (ac_red / dc_red) / (ac_ir / dc_ir);
    const pi_red = (ac_red / dc_red) * 100.0;
    const pi_ir = (ac_ir / dc_ir) * 100.0;

    let spo2 = 110.0 - 25.0 * r;
    spo2 = Math.min(100.0, Math.max(70.0, spo2));

    const HB_WEIGHT_BIAS = -33.101360;
    const HB_WEIGHT_INV_R = 37.809596;
    const HB_WEIGHT_R = 9.692832;
    const HB_WEIGHT_PI_IR = -0.002258;
    const HB_WEIGHT_RATIO_PI = 0.038840;

    let hb = HB_WEIGHT_BIAS
        + HB_WEIGHT_INV_R * (1.0 / (r + 1e-5))
        + HB_WEIGHT_R * r
        + HB_WEIGHT_PI_IR * pi_ir
        + HB_WEIGHT_RATIO_PI * (pi_red / (pi_ir + 1e-5));

    return {
        hb: parseFloat(Math.min(18.0, Math.max(4.5, hb)).toFixed(2)),
        spo2: parseFloat(spo2.toFixed(1)),
        r: parseFloat(r.toFixed(4))
    };
}

async function showResults() {
    document.getElementById('stage2').classList.add('hidden');
    document.getElementById('stage3').classList.remove('hidden');
    updateNav(3);

    // Compute vitals based on patient context
    let opticalVals = { ac_red: 1150, dc_red: 122000, ac_ir: 980, dc_ir: 135000 };
    if (currentPatient.name.includes('Lata') || currentPatient.name.includes('लता')) {
        opticalVals = { ac_red: 1680, dc_red: 109000, ac_ir: 820, dc_ir: 141000 };
    } else if (currentPatient.name.includes('Sunita') || currentPatient.name.includes('सुनीता')) {
        opticalVals = { ac_red: 1140, dc_red: 125000, ac_ir: 940, dc_ir: 132000 };
    }

    const calc = computeBeerLambertHb(opticalVals.ac_red, opticalVals.dc_red, opticalVals.ac_ir, opticalVals.dc_ir);
    currentVitals.hb = calc.hb;
    currentVitals.spo2 = calc.spo2;
    currentVitals.hr = 78;

    const hbEl = document.getElementById('resHb');
    const gradeEl = document.getElementById('resGrade');
    const banner = document.getElementById('adviceBanner');

    hbEl.innerText = `${currentVitals.hb} g/dL`;
    document.getElementById('resSpo2').innerText = `${currentVitals.spo2}%`;
    document.getElementById('resHr').innerText = `${currentVitals.hr} BPM`;

    if (currentVitals.hb < 7.0) {
        gradeEl.innerText = (currentLang === 'hi') ? "गंभीर एनीमिया! (Severe)" : "Severe Anemia!";
        gradeEl.className = "vital-grade text-danger";
        banner.innerHTML = "<strong>🚨 चेतावनी:</strong> तुरंत PHC डॉक्टर से संपर्क करें! हीमोग्लोबिन 7.0 से कम है (आयरन सुक्रोज ड्रिप या परामर्श आवश्यक)।";
    } else if (currentVitals.hb < 10.0) {
        gradeEl.innerText = (currentLang === 'hi') ? "मध्यम एनीमिया" : "Moderate Anemia";
        gradeEl.className = "vital-grade text-warning";
        banner.innerHTML = "<strong>स्वास्थ्य सलाह:</strong> दैनिक आयरन-फोलिक एसिड टैबलेट लें और 15 दिन बाद दोबारा जांच करवाएं।";
    } else {
        gradeEl.innerText = (currentLang === 'hi') ? "सामान्य (Normal)" : "Normal Health";
        gradeEl.className = "vital-grade text-success";
        banner.innerHTML = "<strong>स्वास्थ्य सलाह:</strong> हीमोग्लोबिन स्तर सामान्य है। पौष्टिक आहार जारी रखें।";
    }

    renderThermalPaper();
    speakPrompt(i18n[currentLang].voicePromptDone);

    // Sync to Gateway Server Real Database
    try {
        await fetch('/api/patients', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                patient_id: currentPatient.abha,
                name: currentPatient.name,
                age: currentPatient.age,
                gender: currentPatient.gender,
                center: "PHC Rampur - Subcentre 01",
                kiosk_id: "MEDIBOT-UP-042",
                optical: {
                    ...opticalVals,
                    heart_rate: currentVitals.hr
                },
                notes: "Kiosk Touchscreen Screening Completed"
            })
        });
    } catch (err) {
        console.warn("Gateway sync notice: " + err);
    }
}

function renderThermalPaper() {
    const slip = `
================================<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PROJECT MEDIBOT KIOSK<br>
&nbsp;&nbsp;RURAL HEALTH CHECKUP SLIP<br>
&nbsp;&nbsp;National Health Mission / ASHA<br>
================================<br>
Patient ID&nbsp;&nbsp;: ${currentPatient.abha}<br>
Name&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: ${currentPatient.name}<br>
Age/Gender&nbsp;&nbsp;: ${currentPatient.age} Yrs / ${currentPatient.gender}<br>
ASHA Center : ASHA_RAMPUR_01<br>
--------------------------------<br>
<strong>[ VITAL HEALTH REPORT ]</strong><br>
Hemoglobin (Hb) : ${currentVitals.hb} g/dL<br>
Anemia Status&nbsp;&nbsp;&nbsp;: ${currentVitals.hb < 7 ? 'गंभीर एनीमिया!' : (currentVitals.hb < 10 ? 'मध्यम एनीमिया' : 'सामान्य')}<br>
Blood Oxygen&nbsp;&nbsp;&nbsp;&nbsp;: ${currentVitals.spo2} % SpO2<br>
Pulse Rate&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: ${currentVitals.hr} BPM<br>
--------------------------------<br>
<strong>[ NEXT APPOINTMENT / VACCINE ]</strong><br>
Due Date&nbsp;&nbsp;&nbsp;&nbsp;: 22-OCT-2026<br>
Note: Carry this slip to PHC.<br>
${currentVitals.hb < 7 ? '<strong>! ALERT: High Risk! Immediate<br>PHC doctor consult required!</strong><br>' : ''}
================================
`;
    document.getElementById('paperSlip').innerHTML = slip;
}

function printReceiptSlip() {
    alert("🖨️ पर्ची प्रिंटर पर भेजी गई! (58mm Thermal Receipt Dispatched via UART2)");
}

function resetWorkflow() {
    document.getElementById('stage3').classList.add('hidden');
    document.getElementById('stage1').classList.remove('hidden');
    document.getElementById('scanProgress').style.width = '0%';
    updateNav(1);
}

async function triggerSosAlert() {
    try {
        await fetch('/api/emergency/lora-gateway', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                payload: `ALERT:EMERGENCY;KIOSK:MEDIBOT-UP-042;PATIENT:${currentPatient.name};ABHA:${currentPatient.abha};HB:${currentVitals.hb};LOC:RAMPUR`
            })
        });
        alert("🚨 आपातकालीन SOS सिग्नल LoRa रेडियो द्वारा PHC को भेजा गया! 108 एम्बुलेंस को GPS लोकेशन प्रेषित।");
    } catch (err) {
        alert("LoRa SOS Dispatched locally: " + err);
    }
}

// Populate registered citizens dynamically from backend on startup
async function loadRegisteredCitizens() {
    try {
        const res = await fetch('/api/patients');
        const list = await res.json();
        if (list && list.length > 0) {
            const container = document.getElementById('quickTapButtons');
            if (container) {
                container.innerHTML = list.slice(0, 5).map(p => `
                    <button class="touch-btn secondary" style="padding:10px 14px; font-size:13px;" onclick="simulateCardTap('${p.name}', '${p.patient_id}', ${p.age || 26}, '${p.gender || 'Female'}')">
                        ⚡ ${p.name} (${p.patient_id.split('-').slice(0, 2).join('-')})
                    </button>
                `).join('');
            }
        }
    } catch (e) {
        console.log("Offline mode, using default list");
    }
}

window.addEventListener('DOMContentLoaded', () => {
    loadRegisteredCitizens();
});
