#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#define SYNC_BUFFER_SIZE 12
#define LED_PIN 13
#define RX_PIN 7
#define BAUD_RATE 115200

uint8_t buffer[SYNC_BUFFER_SIZE] = {0};
uint8_t previous_buffer[SYNC_BUFFER_SIZE] = {0};
uint8_t buffer_index = 0;

bool buffers_differ(uint8_t* buf1, uint8_t* buf2, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (buf1[i] != buf2[i]) {
            return true;
        }
    }
    return false;
}

void handleSerialData(byte data) {
    buffer[buffer_index] = data;
    buffer_index = (buffer_index + 1) % SYNC_BUFFER_SIZE;
    if (buffer_index == 0) {
        if (buffers_differ(buffer, previous_buffer, SYNC_BUFFER_SIZE)) {
            for (int i = 0; i < 10; i++) {
                usbMIDI.sendProgramChange(buffer[i], i + 1); // Channels 1 to 10
            }
            memcpy(previous_buffer, buffer, SYNC_BUFFER_SIZE);
        }
        memset(buffer, 0, SYNC_BUFFER_SIZE);
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(RX_PIN, INPUT_PULLUP);
    Serial2.begin(BAUD_RATE, SERIAL_8N1_RXINV_TXINV);

    memset(buffer, 0, SYNC_BUFFER_SIZE);
    memset(previous_buffer, 0, SYNC_BUFFER_SIZE);
}

void loop() {
    while (Serial2.available()) {
        byte data = Serial2.read();
        handleSerialData(data);
    }
}
