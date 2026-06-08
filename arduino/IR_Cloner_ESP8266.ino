/*
 * ============================================================
 *  Remote Control Cloner — ESP8266 Firmware (NodeMCU / Wemos D1)
 *  by EngrPanda — uses IRremote v4.7.1 (same library as Arduino)
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
 *
 *  DIAGRAM:
 *    [VS1838B]       [NodeMCU]               [HC-05/HC-06]
 *    OUT ──────────► D5 (GPIO14)
 *    GND ──────────► GND ◄───────────────── GND
 *    VCC ──────────► 3V3                     VCC ←── VIN
 *                    D1 (GPIO5) ◄────────── TXD
 *                    D2 (GPIO4) ─[1kΩ]────► RXD
 *                                  └[2kΩ]── GND
 * ============================================================
 */

#if !defined(ESP8266)
  #error "Wrong board! Select NodeMCU 1.0. Use IR_Cloner_ESP32.ino for ESP32."
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
