/*
 * ============================================================
 *  Remote Control Cloner — ESP32 Firmware
 *  by EngrPanda — uses IRremote v4.7.1 (same library as Arduino)
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
  #error "Wrong board! Select ESP32 Dev Module. Use IR_Cloner_Arduino.ino for Uno/Nano."
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
