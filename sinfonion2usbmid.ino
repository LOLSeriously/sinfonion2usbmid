// send data from sinfoinion out1 as master to Teensy RX pin, send to usbMIDI to be processed by Droid

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#define SYNC_BUFFER_SIZE 12
#define LED_PIN 13

uint8_t buffer[SYNC_BUFFER_SIZE] = {0};
uint8_t previous_buffer[SYNC_BUFFER_SIZE] = {0};
unsigned long last_interrupt = 0;
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
    // Assuming the data follows the same format as the original program
    buffer[buffer_index] = data;
    buffer_index = (buffer_index + 1) % SYNC_BUFFER_SIZE;
    if (buffer_index == 0) {
        // All buffer data received, process the buffer
        if (buffers_differ(buffer, previous_buffer, SYNC_BUFFER_SIZE)) {
            // Send USB MIDI only if the buffer data has changed
            usbMIDI.sendProgramChange(buffer[0], 1); // Channel 1
            usbMIDI.sendProgramChange(buffer[1], 2); // Channel 2
            usbMIDI.sendProgramChange(buffer[2], 3); // Channel 3
            usbMIDI.sendProgramChange(buffer[3], 4); // Channel 4
            usbMIDI.sendProgramChange(buffer[4], 5); // Channel 5
            usbMIDI.sendProgramChange(buffer[5], 6); // Channel 6
            usbMIDI.sendProgramChange(buffer[6], 7); // Channel 7
            usbMIDI.sendProgramChange(buffer[7], 8); // Channel 8                        
            usbMIDI.sendProgramChange(buffer[8], 9); // Channel 9
            usbMIDI.sendProgramChange(buffer[9], 10); // Channel 10

            // Update previous buffer
            for (int i = 0; i < SYNC_BUFFER_SIZE; i++) {
                previous_buffer[i] = buffer[i];
            }
        }

        // Clear buffer for next data
        for (int i = 0; i < SYNC_BUFFER_SIZE; i++) {
            buffer[i] = 0;
        }
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(7, INPUT_PULLUP); // Pin 7 is RX2

    Serial2.begin(115200, SERIAL_8N1_RXINV_TXINV);
    // Initialize buffer
    for (int i = 0; i < SYNC_BUFFER_SIZE; i++) {
        buffer[i] = 0;
        previous_buffer[i] = 0;
    }
}

void loop() {
    while (Serial2.available()) {
        byte data = Serial2.read();
        handleSerialData(data);
    }
}
