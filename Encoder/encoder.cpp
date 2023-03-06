#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#define PIN_A 28
#define PIN_B 25


volatile int32_t position = 0; // current position of the encoder
volatile int8_t state = 0; // current state of the encoder

void isr() {
    int8_t new_state = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);
    int8_t delta = 0;


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

    position += delta;
    state = new_state;
}

int main() {
    wiringPiSetup();
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);
    wiringPiISR(PIN_A, INT_EDGE_BOTH, isr);
    wiringPiISR(PIN_B, INT_EDGE_BOTH, isr);

    while (true) {
        printf("Position: %d\n", position/4);
        delay(100);
    }
    return 0;
}