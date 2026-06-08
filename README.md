<div align="center">



# Remote Control Cloner

**Clone any IR remote with your Android phone.**
Capture IR codes via Arduino/ESP + IR receiver over Bluetooth,
then transmit from your phone's built-in IR blaster.

[![Android](https://img.shields.io/badge/Platform-Android%205.0%2B-3DDC84?style=flat-square&logo=android&logoColor=white)](https://github.com/engrpanda/RemoteControl_Cloner)
[![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)](https://github.com/engrpanda/RemoteControl_Cloner/blob/main/LICENSE)
[![IRremote](https://img.shields.io/badge/IRremote-v4.7.1-orange?style=flat-square)](https://github.com/Arduino-IRremote/Arduino-IRremote)
[![Firebase](https://img.shields.io/badge/Firebase-Firestore-FFCA28?style=flat-square&logo=firebase&logoColor=black)](https://firebase.google.com/)
[![Made by](https://img.shields.io/badge/Made%20by-EngrPanda-9b59b6?style=flat-square)](https://engrpanda.github.io/)

[🌐 Website](https://engrpanda.github.io/) · [⭐ Star this repo](https://github.com/engrpanda/RemoteControl_Cloner) · [🐛 Report a Bug](https://github.com/engrpanda/RemoteControl_Cloner/issues) · [💡 Request a Feature](https://github.com/engrpanda/RemoteControl_Cloner/issues)

</div>

---

<div align="center">

<img src="https://github.com/engrpanda/RemoteControl_Cloner/blob/main/sc/sc.png?raw=true" />

</div>

---

## ✨ Features

- 📡 **Capture** — Grab IR codes from any physical remote using Arduino/ESP + VS1838B
- 📲 **Transmit** — Fire codes from your phone's built-in IR blaster
- 🌍 **Community Library** — Share and browse remotes via Firebase Firestore (no account needed)
- 🗄️ **Online Database** — Thousands of codes from Flipper-IRDB and irdb, fetched live
- 🎨 **Customizable Buttons** — Labels, colours, shapes, sizes, drag-to-reorder
- 📦 **Export / Import** — Share remotes as `.json` via WhatsApp, Drive, etc.
- ✏️ **Manual IR Edit** — Paste Pronto Hex or raw µs timings directly
- 🔌 **External IR Dongle** — 3.5mm / USB-C audio IR dongles supported

---

---

## Download

📦 **[Download latest release →](https://github.com/engrpanda/RemoteControl_Cloner/releases/latest)**

## Table of Contents

1. [What You Need](#what-you-need)
2. [Hardware - ESP32](#hardware-esp32)
3. [Hardware - ESP8266 + HC-05/HC-06](#hardware-esp8266)
4. [Hardware - Arduino Uno / Nano + HC-05/HC-06](#hardware-arduino)
5. [Upload Firmware](#upload-firmware)
6. [Pair Bluetooth](#pair-bluetooth)
7. [Clone a Remote](#clone-a-remote)
8. [Using a Remote](#using-a-remote)
9. [My Remotes](#my-remotes)
10. [Community Remotes](#community-remotes)
11. [Online Database](#online-database)
12. [Export and Import](#export-and-import)
13. [Edit IR Codes](#edit-ir-codes)
14. [Settings](#settings)
15. [External IR Dongle](#external-ir-dongle)
16. [Build from Source](#build-from-source)
17. [Troubleshooting](#troubleshooting)
18. [Credits](#credits)

---

## What You Need

### To capture IR codes (required for cloning your own remote)

| Part | Notes | Est. Cost (₱) |
|---|---|---|
| **VS1838B** or **TSOP38238** | IR receiver module — 3-pin, both work identically | ₱20–₱50 |
| **ESP32** dev board | ⭐ Recommended — built-in Bluetooth, simplest setup | ₱150–₱300 |
| — OR — **ESP8266** (NodeMCU) | Needs HC-05/06 Bluetooth module | ₱80–₱150 |
| — OR — **Arduino Uno / Nano** | Needs HC-05/06 Bluetooth module | ₱100–₱200 |
| **HC-05** or **HC-06** | Only for ESP8266 and Arduino builds | ₱80–₱150 |
| Jumper wires | Male-to-male and male-to-female | ₱30 |
| USB cable | For flashing firmware | — |

### To transmit IR (to use the phone as a remote)

- Android phone with a **built-in IR blaster** (most Xiaomi/Redmi, many Samsung, some OPPO/vivo), **OR**
- A **3.5mm or USB-C audio IR dongle** — see [Section 15](#15-external-ir-dongle)

> **Tip:** The Home screen of the app tells you immediately whether your phone has an IR blaster.

### IR Receiver Pinout (VS1838B / TSOP38238)

Both modules are pin-compatible — same wiring for either.

```
  Hold the receiver flat/lens face toward you:

        ┌─────────┐
        │  ( ● )  │   ← lens / flat face
        └─┬──┬──┬─┘
          │  │  │
         OUT GND VCC
         (1) (2) (3)

  Pin 1 = OUT / Signal
  Pin 2 = GND
  Pin 3 = VCC → 3.3V for ESP32/ESP8266 | 5V for Arduino
```

---

## Hardware ESP32

ESP32 has **built-in Bluetooth Classic** — no extra module needed. Just three wires.

### Wiring

```
  VS1838B / TSOP38238          ESP32 Dev Board
  ───────────────────          ───────────────
  Pin 1  OUT  ─────────────►  GPIO15
  Pin 2  GND  ─────────────►  GND
  Pin 3  VCC  ─────────────►  3V3
```

> ⚠️ Use **3V3**, never 5V on ESP32. IR modules work fine at 3.3V.

> 💡 GPIO15 is the default in the sketch. You can change `IR_RECEIVE_PIN` to any free GPIO if needed.

### Firmware Sketch

```cpp
/*
 * ============================================================
 *  Remote Control Cloner — ESP32 Firmware
 *  by EngrPanda — uses IRremote v4.7.1
 * ============================================================
 *
 *  LIBRARY: "IRremote" by shirriff/ArminJo, version 4.x
 *           (Arduino Library Manager → search "IRremote")
 *  BOARD:   Tools → Board → ESP32 Arduino → ESP32 Dev Module
 *
 *  WIRING — IR Receiver (VS1838B or TSOP38238)
 *    OUT → GPIO15  |  GND → GND  |  VCC → 3V3
 *
 *  No extra Bluetooth module needed — ESP32 has built-in Bluetooth.
 *  The device appears as "RC_Cloner" when pairing.
 * ============================================================
 */

#if !defined(ESP32)
  #error "Wrong board! Select ESP32 Dev Module."
#endif

#include <Arduino.h>
#include <BluetoothSerial.h>

#define IR_RECEIVE_PIN 15
#include <IRremote.hpp>

#define CARRIER_HZ     38000
#define BT_NAME        "RC_Cloner"

BluetoothSerial SerialBT;

void emit(const String &line) {
  Serial.println(line);
  SerialBT.println(line);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  SerialBT.begin(BT_NAME);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  emit("INFO:Bluetooth ready as '" + String(BT_NAME) + "'. Pair and connect in app.");
  emit("INFO:IR receiver on GPIO15. Point your remote and press a button.");
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t rawlen = IrReceiver.irparams.rawlen;
    if (rawlen > 3) {
      String csv = "";
      for (uint16_t i = 1; i < rawlen; i++) {
        uint32_t us = (uint32_t)IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
        if (i > 1) csv += ",";
        csv += String(us);
      }
      emit("CAP:" + String(CARRIER_HZ) + ":" + csv);
    }
    IrReceiver.resume();
  }
}
```

### Arduino IDE Setup for ESP32

1. Open **Arduino IDE 2.x**
2. **File → Preferences → Additional boards manager URLs** — add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools → Board → Boards Manager** → search `esp32` → install **esp32 by Espressif Systems**
4. **Tools → Manage Libraries** → search `IRremote` → install **IRremote by shirriff, z3t0, ArminJo** (version **4.x**)
5. Paste the sketch above into a new `.ino` file (or open `IR_Cloner_ESP32.ino`)
6. **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
7. **Tools → Port** → select your COM port
8. Click **Upload ▶**

### Verify

Open **Serial Monitor** at **115200 baud**. Expected output:
```
INFO:Bluetooth ready as 'RC_Cloner'. Pair and connect in app.
INFO:IR receiver on GPIO15. Point your remote and press a button.
```
Point any remote at the VS1838B and press a button — you should see:
```
CAP:38000:9000,4500,560,1690,560,560,...
```

---

## Hardware ESP8266

ESP8266 has **no built-in Bluetooth Classic**, so an HC-05 or HC-06 module is used as a serial Bluetooth bridge.

### Parts

- NodeMCU v1.0 (ESP-12E) or Wemos D1 Mini
- HC-05 **or** HC-06 module (either works — HC-06 is simpler, slave-only)
- VS1838B or TSOP38238

### Wiring — IR Receiver

```
  VS1838B / TSOP38238          NodeMCU (ESP8266)
  ───────────────────          ─────────────────
  Pin 1  OUT  ─────────────►  D5  (GPIO14)
  Pin 2  GND  ─────────────►  GND
  Pin 3  VCC  ─────────────►  3V3
```

### Wiring — HC-05 / HC-06

```
  HC-05 / HC-06                NodeMCU (ESP8266)
  ─────────────                ─────────────────
  VCC  ────────────────────►  VIN   (5V from USB)
  GND  ────────────────────►  GND
  TXD  ────────────────────►  D1   (GPIO5) — SoftSerial RX
  RXD  ──[1kΩ]──────────────► D2   (GPIO4) — SoftSerial TX
               └──[2kΩ]────►  GND
```

> ⚠️ The voltage divider (1kΩ + 2kΩ) is important — it steps the ESP8266's 3.3V TX down to ~2.2V-safe for the HC-05 RXD pin. You can substitute 2.2kΩ + 3.3kΩ or any similar ratio.

### Full Diagram

```
  [VS1838B]       [NodeMCU]               [HC-05/HC-06]
  OUT ──────────► D5 (GPIO14)
  GND ──────────► GND ◄───────────────── GND
  VCC ──────────► 3V3                     VCC ←── VIN
                  D1 (GPIO5) ◄────────── TXD
                  D2 (GPIO4) ─[1kΩ]────► RXD
                                └[2kΩ]── GND
```

### Firmware Sketch

```cpp
/*
 * ============================================================
 *  Remote Control Cloner — ESP8266 Firmware (NodeMCU / Wemos D1)
 *  by EngrPanda — uses IRremote v4.7.1
 * ============================================================
 *
 *  LIBRARY: "IRremote" by shirriff/ArminJo, version 4.x
 *           (Arduino Library Manager → search "IRremote")
 *  BOARD:   Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)
 *
 *  WIRING — IR Receiver (VS1838B or TSOP38238)
 *    OUT → D5 (GPIO14)  |  GND → GND  |  VCC → 3V3
 *
 *  WIRING — HC-05 / HC-06 Bluetooth Module
 *    VCC → VIN (5V)  |  GND → GND
 *    TXD → D1 (GPIO5)   [SoftwareSerial RX]
 *    RXD → D2 (GPIO4)   [SoftwareSerial TX, use voltage divider:
 *                         D2 → 1kΩ → HC-05 RXD → 2kΩ → GND]
 * ============================================================
 */

#if !defined(ESP8266)
  #error "Wrong board! Select NodeMCU 1.0."
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>

#define IR_RECEIVE_PIN 14     // D5 = GPIO14
#include <IRremote.hpp>

#define BT_RX_PIN   5        // D1 = GPIO5 ← HC-05 TXD
#define BT_TX_PIN   4        // D2 = GPIO4 → HC-05 RXD (via voltage divider)
#define BT_BAUD     9600
#define CARRIER_HZ  38000

SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void emit(const String &line) {
  Serial.println(line);
  BTSerial.println(line);
}

void setup() {
  Serial.begin(115200);
  BTSerial.begin(BT_BAUD);
  delay(300);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  emit("INFO:ESP8266 ready. Pair HC-05/HC-06 (PIN 1234) then connect in app.");
  emit("INFO:IR receiver on D5 (GPIO14). Point your remote and press a button.");
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t rawlen = IrReceiver.irparams.rawlen;
    if (rawlen > 3) {
      String csv = "";
      for (uint16_t i = 1; i < rawlen; i++) {
        uint32_t us = (uint32_t)IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
        if (i > 1) csv += ",";
        csv += String(us);
      }
      emit("CAP:" + String(CARRIER_HZ) + ":" + csv);
    }
    IrReceiver.resume();
  }
}
```

### Arduino IDE Setup for ESP8266

1. Open **Arduino IDE 2.x**
2. **File → Preferences → Additional boards manager URLs** — add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. **Tools → Board → Boards Manager** → search `esp8266` → install **esp8266 by ESP8266 Community**
4. **Tools → Manage Libraries** → search `IRremote` → install **IRremote by shirriff, z3t0, ArminJo** (version **4.x**)
5. Paste the sketch above or open `IR_Cloner_ESP8266.ino`
6. **Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)**
7. **Tools → Port** → select your COM port
8. Click **Upload ▶**

### Verify

Open **Serial Monitor** at **115200 baud**:
```
INFO:ESP8266 ready. Pair HC-05/HC-06 (PIN 1234) then connect in app.
INFO:IR receiver on D5 (GPIO14). Point your remote and press a button.
```

### First-Time HC-05 / HC-06 Bluetooth Setup

No AT commands needed. Just pair in Android:
- **Settings → Bluetooth → Scan**
- Select **HC-05** or **HC-06**
- Enter PIN: **`1234`**

---

## Hardware Arduino

The Arduino build uses the same **IRremote v4.x** library as the ESP builds. SoftwareSerial handles the Bluetooth module.

### Parts

- Arduino Uno **or** Arduino Nano
- HC-05 **or** HC-06 Bluetooth module
- VS1838B or TSOP38238

### Wiring — IR Receiver

```
  VS1838B / TSOP38238          Arduino Uno / Nano
  ───────────────────          ──────────────────
  Pin 1  OUT  ─────────────►  D2
  Pin 2  GND  ─────────────►  GND
  Pin 3  VCC  ─────────────►  5V
```

> ✅ Arduino outputs 5V on the 5V pin. Both VS1838B and TSOP38238 are 5V-rated.

### Wiring — HC-05 / HC-06

```
  HC-05 / HC-06                Arduino Uno / Nano
  ─────────────                ──────────────────
  VCC  ────────────────────►  5V
  GND  ────────────────────►  GND
  TXD  ────────────────────►  D10   (SoftwareSerial RX)
  RXD  ────────────────────►  D11   (SoftwareSerial TX)
```

> ℹ️ Direct connection to D11 is fine here — most HC-05/HC-06 breakout boards have 5V-tolerant RXD pins, and the Arduino TX output is 5V compatible.

### Full Diagram

```
  [VS1838B]        [Arduino Uno/Nano]    [HC-05 / HC-06]
  OUT ───────────► D2
  GND ───────────► GND ──────────────── GND
  VCC ───────────► 5V  ──────────────── VCC
                   D10 (RX) ◄────────── TXD
                   D11 (TX) ───────────► RXD
```

### Firmware Sketch

```cpp
/*
 * ============================================================
 *  Remote Control Cloner — Arduino Uno / Nano Firmware
 *  by EngrPanda — verified against IRremote v4.7.1
 * ============================================================
 *
 *  LIBRARY: "IRremote" by shirriff/ArminJo, version 4.x
 *  BOARD:   Tools → Board → Arduino AVR Boards → Arduino Uno (or Nano)
 *
 *  WIRING — IR Receiver (VS1838B or TSOP38238)
 *    OUT → D2  |  GND → GND  |  VCC → 5V
 *
 *  WIRING — HC-05 / HC-06 Bluetooth
 *    VCC → 5V  |  GND → GND  |  TXD → D10  |  RXD → D11
 * ============================================================
 */

#if defined(ESP8266) || defined(ESP32)
  #error "Wrong board! Use IR_Cloner_ESP8266.ino or IR_Cloner_ESP32.ino instead."
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>

#define IR_RECEIVE_PIN 2
#include <IRremote.hpp>

#define BT_RX_PIN   10
#define BT_TX_PIN   11
#define BT_BAUD     9600
#define CARRIER_HZ  38000

SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void emit(const String &line) {
  Serial.println(line);
  BTSerial.println(line);
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(BT_BAUD);
  delay(300);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  emit("INFO:Arduino ready. Pair HC-05/HC-06 (PIN 1234) then connect in app.");
  emit("INFO:IR receiver on D2. Point your remote and press a button.");
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t rawlen = IrReceiver.irparams.rawlen;
    if (rawlen > 3) {
      String csv = "";
      for (uint16_t i = 1; i < rawlen; i++) {
        uint32_t us = (uint32_t)IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
        if (i > 1) csv += ",";
        csv += String(us);
      }
      emit("CAP:" + String(CARRIER_HZ) + ":" + csv);
    }
    IrReceiver.resume();
  }
}
```

### Arduino IDE Setup for Uno / Nano

1. Open **Arduino IDE 2.x** — no extra board package needed (Uno/Nano is built-in)
2. **Tools → Manage Libraries** → search `IRremote` → install **IRremote by shirriff, z3t0, ArminJo**
   - ⚠️ You **must** use version **4.x** — older 2.x/3.x will not compile
3. Paste the sketch above or open `IR_Cloner_Arduino.ino`
4. **Tools → Board → Arduino AVR Boards → Arduino Uno** (or **Arduino Nano**)
   - For Nano clone: if upload fails, try **Tools → Processor → ATmega328P (Old Bootloader)**
5. **Tools → Port** → select your COM port
6. Click **Upload ▶**

> ✅ You do **not** need to disconnect the HC-05 from D10/D11 before uploading. SoftwareSerial pins never conflict with the USB upload process. Only D0/D1 (the hardware UART) would conflict.

### Verify

Open **Serial Monitor** at **9600 baud** (important — not 115200 like the ESP boards):
```
INFO:Arduino ready. Pair HC-05/HC-06 (PIN 1234) then connect in app.
INFO:IR receiver on D2. Point your remote and press a button.
```

---

## Upload Firmware

Quick-reference upload checklist for all three boards:

| Board | Baud Rate (Serial Monitor) | Library | Bluetooth Name |
|---|---|---|---|
| ESP32 | 115200 | IRremote 4.x | RC_Cloner |
| ESP8266 (NodeMCU) | 115200 | IRremote 4.x | HC-05 or HC-06 |
| Arduino Uno / Nano | 9600 | IRremote 4.x | HC-05 or HC-06 |

**Common upload issues:**

- **Port not found** → install the correct USB-to-Serial driver. ESP32/ESP8266 NodeMCU boards typically use **CH340** or **CP2102** drivers. Arduino Uno uses **CH340** on clones, native USB on genuine boards.
- **Nano won't upload** → try **Tools → Processor → ATmega328P (Old Bootloader)**
- **"Wrong board!" compile error** → the sketch has a guard — make sure the board selected in Tools matches the `.ino` file you opened
- **IRremote 2.x/3.x API errors** → update IRremote to 4.x in Library Manager

---

## Pair Bluetooth

Do this **once** before using the app for the first time:

1. Power on your capture device (ESP32 / Arduino with HC-05)
2. Android: **Settings → Bluetooth → turn on Bluetooth**
3. Tap **Pair new device** (or **Scan**)
4. Select:
   - ESP32 → **RC_Cloner**
   - With HC-05 module → **HC-05**
   - With HC-06 module → **HC-06**
5. PIN prompt → type **`1234`** (HC-05/HC-06) — or just tap Pair (ESP32, no PIN)
6. Wait for **"Paired"** ✓

> On **Android 12+**: go to **Settings → Apps → Remote Control Cloner → Permissions** and grant **Nearby devices** (Bluetooth permission). The app will also prompt you on first launch.

---

## Clone a Remote

### Step 1 — Open Clone a Remote

Tap **Clone a Remote** on the home screen or the bottom navigation bar.

### Step 2 — Connect the capture device

Tap **Connect capture device** → your paired device appears in the list → tap it.
The status bar turns green and shows **Connected to RC_Cloner** (or HC-05/HC-06).

> If the device doesn't appear in the list, check the Bluetooth pairing in Section 6.

### Step 3 — Create your buttons

Two ways:

- Tap **Create buttons** → enter a number (e.g. `15`) → OK — creates that many blank button cards at once. Best if you know how many buttons the original remote has.
- Tap **Add button** — adds one card at a time. Good for adding a few more later.

### Step 4 — Style each button (optional but recommended)

Tap the **✏️ pencil / palette icon** on a button card to open the config dialog:

| Field | Description |
|---|---|
| **Label** | Text shown on the button — e.g. `Power`, `Vol +`, `CH 1` |
| **Colour** | Pick from colour chips |
| **Shape** | Rounded / Circle / Pill / Square |
| **Size** | Small / Medium / Large |
| **Type** | Standard / Power / D-Pad / Number / Touchpad |

Tap **Save**. You can style before or after capturing codes, or not at all.

> 💡 **Drag to reorder:** Long-press any button card and drag it to rearrange the grid.

### Step 5 — Capture each IR code

1. Tap **Register code** on a button card
2. The app shows "Waiting for capture…"
3. Point the physical remote **2–5 cm** directly at the VS1838B
4. Press **one button** on the physical remote — hold it for ~0.5 seconds
5. The card updates to show ✔ (code captured)
6. Repeat for every button

> 💡 **Capture tips:**
> - If the card stays on "Waiting…" — move the remote closer and press more firmly
> - If you get a very short code (< 5 timings) — that's a partial capture, try again
> - Bright sunlight and fluorescent lights can interfere with the IR receiver — capture indoors or shade the receiver
> - You don't need to capture every button in one session — save first, then come back via **My Remotes → long-press → Capture / edit buttons**

### Step 6 — Save the remote

Scroll down and tap the green **Save remote** button. Fill in:

| Field | Example |
|---|---|
| Remote name | Samsung Living Room TV |
| Brand | Samsung |
| Device type | TV |
| Creator name | EngrPanda |

Toggle **Share to Community** ON to share it with all app users via the community library.

Tap **Save** — the remote is immediately available in **My Remotes**.

---

## Using a Remote

Tap any saved remote card to open it. All buttons are shown in a **4-column grid**.

| Action | What it does |
|---|---|
| **Tap** a button | Transmits the IR code once |
| **Hold** a button | Repeats the IR code continuously — useful for Volume, Channel, etc. |

Vibration and sound feedback per tap can be toggled in **Settings**.

> ⚠️ Your phone needs an IR blaster (or external dongle) to transmit. The app Home screen shows whether your device is capable.

---

## My Remotes

All locally saved remotes are here, displayed as a 2-column grid.

| Action | How |
|---|---|
| **Open** | Tap the card |
| **Long-press menu** | Long-press the card |
| **Search** | Type in the search bar |
| **Sort** | Tap sort chips: Name / Brand / Device / Date / Creator / Stars |
| **Import** | Tap **Import** button — pick a `.json` file |
| **Export all** | Tap **Export All** — saves a single `.json` with everything |

**Long-press actions:**
- **Open** — use it right now
- **Capture / edit buttons** — re-open the Clone builder with this remote loaded (add more buttons, re-capture a code, restyle)
- **Edit details** — change name, brand, device, creator
- **Export this remote** — share as `.json` via WhatsApp, Gmail, Drive, etc.
- **Delete** — permanently remove from your phone

---

## Community Remotes

A shared live library of remotes from all app users, powered by Firebase Firestore. No account needed.

### Browse

- Tap **World Remotes** bottom tab → **Community** tab
- Tap a card → opens the remote immediately
- Long-press → **Open / Add to My Remotes / Mark working ★**

### Share your remote

When saving a cloned remote, toggle **Share to Community** ON. It appears for all users within seconds.

### The ★ star system

Tapping ★ on a community remote does three things:
1. Saves a local copy to **My Remotes**
2. Marks it as "works on my device" locally
3. Adds +1 to the public star count visible to everyone

The star count is the community's signal that a remote actually works on real hardware.

---

## Online Database

Thousands of IR codes from two open-source databases, fetched live (nothing is bundled in the app).

### Browse

1. **World Remotes** → **Online database** tab
2. Tap a category (TV, Receiver, DVD, Projector, Audio…)
3. Tap a remote card → downloads codes and opens it instantly
4. Tap the **← breadcrumb** to go back to categories

### Search

Type in the search bar — filters across all categories in real time.

### Tap ★ on an online remote

- Downloads the full remote
- Saves it to **My Remotes**
- Marks it as "works for me" locally

> ℹ️ Online database remotes cover NEC, Samsung, Sony SIRC, and RC5 protocols. The ★ system helps identify which codes actually work on your device.

---

## Export and Import

Share remotes with friends, back them up, or move between phones.

**Export one remote:** My Remotes → long-press → **Export this remote** → share via any app

**Export all:** My Remotes → **Export All** → share the file

**Import:** My Remotes → **Import** → pick a `.json` file → all valid remotes inside are added

The `.json` format is human-readable and contains button labels, colours, shapes, and raw IR timings.

---

## Edit IR Codes

Every button stores a carrier frequency (e.g. 38000 Hz) and raw on/off timing in microseconds. You can view, copy, or replace any button's code manually.

**Pronto Hex** is the standard copy-paste format. Example:
```
0000 006D 0022 0002 0157 00AC 0016 0041 0016 0016 0016 0041
```
Pronto Hex codes can be found on the LIRC database, IrScrutinizer exports, and many IR code sharing sites.

**How to edit a button's code:**

1. Clone a Remote screen (or edit an existing remote) → tap **✏️ pencil** on a button card
2. Scroll to the **IR Code** section at the bottom of the dialog
3. The current code is shown in Pronto Hex — copy it to share or back up
4. Paste any Pronto Hex code, or enter raw microseconds:
   - Pronto Hex: `0000 006D 0022 0002 ...`
   - Raw µs (38 kHz assumed): `9000 4500 560 1690 560 560 ...`
   - Raw with explicit frequency: `38000: 9000 4500 560 ...`
5. A red error means invalid format — fix or clear the field
6. Clearing the field entirely removes the code from this button
7. Tap **Save**

---

## Settings

**≡ drawer → Appearance & Feedback**

| Setting | Options |
|---|---|
| Theme | System default / Light / Dark |
| Sound on press | On / Off — small beep per button tap |
| Vibrate on press | On / Off — short vibration per tap |
| IR output | Auto / Built-in blaster / External dongle (3.5mm or USB-C audio) |

Tap **Save settings** to apply.

---

## External IR Dongle

If your phone has no built-in IR blaster, a **3.5mm headphone-jack IR dongle** may work.

### Setup

1. Plug the dongle into the headphone jack (or USB-C audio adapter)
2. **Settings → IR output → External dongle (3.5mm / USB-C audio)**
3. Set **media volume to maximum** — signal strength depends directly on volume
4. Test via **≡ drawer → IR Blaster Check**

### Requirements and Limitations

- Phone DAC needs **≥ 96 kHz sample rate** for clean 38 kHz IR output. Phones limited to 48 kHz DAC output usually won't work
- Range is shorter than a built-in blaster
- USB-C **data protocol** IR devices (e.g. some Xiaomi USB-C IR dongles) are **not** supported — audio-based dongles only

---

## Build from Source

> For developers only. Skip this section if you just want to use the APK.

### Requirements

| Tool | Version |
|---|---|
| Android Studio | Hedgehog or newer |
| JDK | 17 (bundled with Android Studio) |
| AGP | 8.2 |
| Gradle | 8.2 |
| Kotlin | 1.9 |
| Min Android | 5.0 (API 21) |
| Target Android | 14 (API 34) |

### Steps

1. Clone or download the repo: `https://github.com/engrpanda/RemoteControl_Cloner`
2. **File → Open** → select the `RemoteControlCloner/` folder
3. Wait for Gradle sync to complete
4. Connect your Android phone with USB Debugging enabled
5. Press **▶ Run**

### Firebase

The `app/google-services.json` file is included in the repo. The Community library (World Remotes → Community tab) connects to Firebase Firestore automatically — no setup needed.

### Gradle Build Tips

- If sync fails with "Plugin not found" → **File → Sync Project with Gradle Files**
- If that doesn't help → **Build → Clean Project → Rebuild Project**
- Check the **Build** output panel for the exact file and line number of any error
- Make sure you're on JDK 17 (check **File → Project Structure → SDK Location → JDK Location**)

---

## Troubleshooting

### Bluetooth device not in list inside the app

- Complete the pairing in Android Settings first (Section 6) — the app can only connect to already-paired devices
- On Android 12+: grant **Nearby devices** permission to the app
- Unplug and re-power the capture device, wait 5 seconds, then try connecting again

### "Waiting for capture…" — nothing happens

- Move the remote **closer**: aim it 2–5 cm directly at the VS1838B / TSOP38238 flat face
- Press the remote button **firmly** for about 0.5 seconds
- Open **Serial Monitor** — you should see `CAP:38000:...` lines. If you see nothing at all, check wiring: OUT pin → correct board pin, VCC → correct voltage (3.3V for ESP, 5V for Arduino)
- Sunlight and flickering fluorescents can saturate the receiver — try a darker room or shade the sensor

### Captured code looks wrong / too short

- That's a partial capture — tap **Register code** again and try once more
- Aim more directly at the lens of the VS1838B

### Remote transmits but device doesn't respond

- Try different distances from the phone to the device (1 m is a good starting point)
- Check **Settings → IR output** is set correctly
- Try another code from the Online Database for the same brand
- Use the ★ system — starred remotes are ones other users confirmed working

### "This phone has no IR blaster" on Home screen

- Your phone physically has no IR blaster — use an external dongle (Section 15) or browse/clone remotes without transmitting

### HC-05 / HC-06 won't pair

- Default PIN is **1234** — try **0000** on older modules
- Make sure the power LED on the HC-05/HC-06 is blinking (fast blink = not paired = discoverable)
- If already paired from a previous device, un-pair it in Android Bluetooth settings first

### Arduino Nano won't upload

- Try **Tools → Processor → ATmega328P (Old Bootloader)** — most cheap Nano clones ship with the old bootloader
- Make sure no HC-05 wires are on D0 or D1 (hardware UART pins) — D10/D11 (SoftwareSerial) are fine

### IRremote compile errors

- Check Library Manager — you need version **4.x**, not 2.x or 3.x
- `irparams` / `rawbuf` errors mean you're on an old version — update to 4.7.1 or newer

---

## Credits

**Firmware libraries:**
- [IRremote](https://github.com/Arduino-IRremote/Arduino-IRremote) by shirriff / ArminJo (MIT) — used for ESP32, ESP8266, and Arduino Uno/Nano
- [BluetoothSerial](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial) — included with ESP32 Arduino core

**Online IR databases** (fetched at runtime, not bundled in the app):
- [Flipper-IRDB](https://github.com/Lucaslhm/Flipper-IRDB) by Lucaslhm and contributors — CC0 license
- [irdb](https://github.com/probonopd/irdb) by Simon Peter and contributors — used under permission

**Android libraries:** Firebase Firestore, OkHttp, Gson, Material 3

---

*Built by [EngrPanda](https://engrpanda.github.io/) · [GitHub Repo](https://github.com/engrpanda/RemoteControl_Cloner)*

*Have fun, and please don't blast your neighbor's TV. 📺*
