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

    // In IRremote v4.7.x the raw buffer is a CLASS MEMBER of IRrecv:
    //   IrReceiver.irparams.rawlen    = number of entries (index 0 = gap)
    //   IrReceiver.irparams.rawbuf[i] = tick counts (1 tick = MICROS_PER_TICK µs)
    uint16_t rawlen = IrReceiver.irparams.rawlen;

    if (rawlen > 3) {
      String csv = "";
      for (uint16_t i = 1; i < rawlen; i++) {  // skip index 0 (idle gap)
        uint32_t us = (uint32_t)IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
        if (i > 1) csv += ",";
        csv += String(us);
      }
      emit("CAP:" + String(CARRIER_HZ) + ":" + csv);
    }

    IrReceiver.resume();
  }
}
