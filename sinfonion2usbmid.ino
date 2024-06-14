#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#define SYNC_BUFFER_SIZE 6
#define LED_PIN 13
#define BAUD_RATE 115200
#define RX_PIN 7

uint8_t buffer[SYNC_BUFFER_SIZE] = {0};
uint8_t buffer_index = 0;

void debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void set_root_note(uint8_t root) {
    buffer[0] = (buffer[0] & ~0x0f) | (root % 12);
}

uint8_t rootNote() {
    return buffer[0] & 0x0f;
}

void set_degree_nr(uint8_t degree_nr) {
    buffer[1] = (buffer[1] & ~0x0f) | degree_nr;
}

uint8_t degree_nr() {
    return buffer[1] & 0x0f;
}

void set_mode_nr(uint8_t mode_nr) {
    buffer[2] = (buffer[2] & ~0x0f) | mode_nr;
}

uint8_t mode_nr() {
    return buffer[2] & 0x0f;
}

void set_clock(uint8_t clock) {
    buffer[0] = (buffer[0] & ~0x70) | ((clock % 8) << 4);
}

uint8_t get_clock() {
    return (buffer[0] & 0x70) >> 4;
}

void set_transposition(int8_t trans) {
    trans = max(-64, min(63, trans - 64));
    buffer[3] = (buffer[3] & ~0x7f) | ((trans + 64) & 0x7f);
}

int8_t transposition() {
    return (buffer[3] & 0x7f) - 64;
}

void set_chaotic_detune(float detune) {
    float moded_detune = max(-1.0, min(1.0, (detune - 64) / 64.0));
    int8_t detune_int = floor(moded_detune * 63.0) + 63;
    buffer[4] = detune_int & 0x7f;
}

float chaotic_detune() {
    return (buffer[4] - 63) / 63.0;
}

void set_harmonic_shift(int8_t shift) {
    buffer[5] = (shift - 11) + 16;
}

int8_t harmonic_shift() {
    return buffer[5] - 16;
}

void set_beat(uint8_t beat) {
    buffer[1] = (buffer[1] & ~0x70) | ((beat % 8) << 4);
}

uint8_t beat() {
    return (buffer[1] & 0x70) >> 4;
}

void set_step(uint8_t step) {
    buffer[2] = (buffer[2] & ~0x70) | ((step % 8) << 4);
}

uint8_t step() {
    return (buffer[2] & 0x70) >> 4;
}

void set_reset(uint8_t reset_value) {
    buffer[5] = (buffer[5] & ~0x60) | ((reset_value % 4) << 5);
}

uint8_t reset() {
    return (buffer[5] & 0x60) >> 5;
}

void dump() {
    for (int i = 0; i < SYNC_BUFFER_SIZE; i++) {
        debug("%02x ", buffer[i]);
    }
    debug("\n");
}

void handleSerialData(byte data) {
    buffer[buffer_index] = data;
    buffer_index = (buffer_index + 1) % SYNC_BUFFER_SIZE;
    if (buffer_index == 0) {
        usbMIDI.sendProgramChange(buffer[0], 1); // Channel 1
        usbMIDI.sendProgramChange(buffer[1], 2); // Channel 2
        usbMIDI.sendProgramChange(buffer[2], 3); // Channel 3
        usbMIDI.sendProgramChange(buffer[3], 4); // Channel 4
        usbMIDI.sendProgramChange(buffer[4], 5); // Channel 5
        usbMIDI.sendProgramChange(buffer[5], 6); // Channel 6
        dump();
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial2.begin(BAUD_RATE, SERIAL_8N1_RXINV_TXINV);
    memset(buffer, 0, SYNC_BUFFER_SIZE);
}

void loop() {
    while (Serial2.available()) {
        byte data = Serial2.read();
        handleSerialData(data);
    }
}
