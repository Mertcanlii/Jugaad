#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <chrono>
#define PIN_A 28
#define PIN_B 25


volatile int32_t encoder_ticks = 0; // current position of the encoder
volatile double rps = 0; //revolution per second
volatile int8_t state = 0; // current state of the encoder
const int PPR = 500; // Pulse per revolution


void isr() {
    int8_t new_state = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);
    int8_t delta = 0; //If the increment increases, this value also increases, which adds to the position

//clockwise rotation is represented by the chain of states 11 → 01 → 00 → 10 → 11 and counterclockwise rotation is represented by the chain of states 11 → 10 → 00 → 01 → 11.
// ref: https://www.pinteric.com/rotary.html
    if (new_state == 0b00 && state == 0b01)
        delta = 1;
    else if (new_state == 0b01 && state == 0b11)
        delta = 1;
    else if (new_state == 0b11 && state == 0b10) 
        delta = 1;
    else if (new_state == 0b10 && state == 0b00) 
        delta = 1;
    else if (new_state == 0b00 && state == 0b10)
        delta = -1;
    else if (new_state == 0b10 && state == 0b11)
        delta = -1;
    else if (new_state == 0b11 && state == 0b01)
        delta = -1;
    else if (new_state == 0b01 && state == 0b00)
        delta = -1;
    encoder_ticks += delta;
    state = new_state;
}

int main() {
    wiringPiSetup();
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);
    wiringPiISR(PIN_A, INT_EDGE_BOTH, isr);
    wiringPiISR(PIN_B, INT_EDGE_BOTH, isr);

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    int prev_position = 0;
    while (true) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        int delta_position = encoder_ticks - prev_position;
        prev_position = encoder_ticks;
        double rpm = static_cast<double>(delta_position/4) * 60.0 / static_cast<double>(PPR * elapsed_time) * 1000000.0;
        std::cout << "RPM: " << rpm << std::endl;
        start = end;
        delay(1000);
    }
    return 0;
}   