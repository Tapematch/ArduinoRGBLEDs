/*
  RGBLED.cpp - Library for controlling RGBLEDs without blocking the rest of the code
  Created by Phil Taubert, 04.08.2018
  Released into the public domain.
*/

#include "Arduino.h"
#include "RGBLED.h"

/* STATE:
 * 0 -> off
 * 1 -> setColor
 * 2 -> fadeToColor
 * 3 -> pulse
 * 4 -> blink
 * 5 -> rainbow
 */

RGBLED::RGBLED(uint8_t redpin, uint8_t greenpin, uint8_t bluepin) {
    pinMode(redpin, OUTPUT);
    pinMode(greenpin, OUTPUT);
    pinMode(bluepin, OUTPUT);
    _redpins[0] = redpin;
    _greenpins[0] = greenpin;
    _bluepins[0] = bluepin;
}

void RGBLED::addLED(uint8_t redpin, uint8_t greenpin, uint8_t bluepin) {
    pinMode(redpin, OUTPUT);
    pinMode(greenpin, OUTPUT);
    pinMode(bluepin, OUTPUT);
    for(int i = 0; i < 10; i++){
        if(_redpins[i] == 99 || _greenpins[i] == 99 || _bluepins[i] == 99) {
            _redpins[i] = redpin;
            _greenpins[i] = greenpin;
            _bluepins[i] = bluepin;
            break;
        }
    }
}

void RGBLED::writeCurrentValues(uint8_t red, uint8_t green, uint8_t blue) {
    _currentred = red;
    _currentgreen = green;
    _currentblue = blue;
    for(int i = 0; i < 10; i++){
        uint8_t redpin = _redpins[i];
        uint8_t greenpin = _greenpins[i];
        uint8_t bluepin = _bluepins[i];
        if(redpin != 99 && greenpin != 99 && bluepin != 99) {
            analogWrite(redpin, _currentred);
            analogWrite(greenpin, _currentgreen);
            analogWrite(bluepin, _currentblue);
        } else {
            break;
        }
    }
}

void RGBLED::setStartValues() {
    starttime = millis();
    _startred = _currentred;
    _startgreen = _currentgreen;
    _startblue = _currentblue;
}

void RGBLED::update() {
    unsigned long currentMillis = millis();
    switch (state) {
        case 2:
            if (currentMillis >= starttime + _time) {
                writeCurrentValues(_targetred, _targetgreen, _targetblue);
            } else {
                writeCurrentValues(
                        map(currentMillis, starttime, starttime + _time, _startred, _targetred),
                        map(currentMillis, starttime, starttime + _time, _startgreen, _targetgreen),
                        map(currentMillis, starttime, starttime + _time, _startblue, _targetblue)
                );
            }
            break;
        case 3:
            if (currentMillis > starttime + _time) {
                if (_targetred != 0 || _targetgreen != 0 || _targetblue != 0) {
                    _startred = _targetred;
                    _startgreen = _targetgreen;
                    _startblue = _targetblue;
                    _targetred = 0;
                    _targetgreen = 0;
                    _targetblue = 0;
                } else {
                    _targetred = _startred;
                    _targetgreen = _startgreen;
                    _targetblue = _startblue;
                    _startred = 0;
                    _startgreen = 0;
                    _startblue = 0;
                }
                starttime = millis();
            }
            writeCurrentValues(
                    map(currentMillis, starttime, starttime + _time, _startred, _targetred),
                    map(currentMillis, starttime, starttime + _time, _startgreen, _targetgreen),
                    map(currentMillis, starttime, starttime + _time, _startblue, _targetblue)
            );
            break;
        case 4:
            if (currentMillis - previousMillis >= _time) {
                // save the last time you blinked the LED
                previousMillis = currentMillis;

                // if the LED is off turn it on and vice-versa:
                if (_currentred == 0 && _currentgreen == 0 && _currentblue == 0) {
                    writeCurrentValues(_targetred, _targetgreen, _targetblue);
                } else {
                    writeCurrentValues(0, 0, 0);
                }
            }
            break;
        case 5: {
            if (currentMillis > starttime + _time) {
                starttime = millis();
            }
            int k = map(currentMillis, starttime, starttime + _time, 0, 360);
            writeCurrentValues(lights[(k + 120) % 360], lights[k], lights[(k + 240) % 360]);
        }
            break;
        default:
            break;
    }
}

void RGBLED::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    state = 1;
    _targetred = red;
    _targetgreen = green;
    _targetblue = blue;
    writeCurrentValues(_targetred, _targetgreen, _targetblue);
    setStartValues();
}


void RGBLED::fadeToColor(uint8_t red, uint8_t green, uint8_t blue, unsigned long time) {
    state = 2;
    _time = time;
    _targetred = red;
    _targetgreen = green;
    _targetblue = blue;
    setStartValues();
}

void RGBLED::pulse(uint8_t red, uint8_t green, uint8_t blue, unsigned long time) {
    state = 3;
    _time = time;
    _targetred = red;
    _targetgreen = green;
    _targetblue = blue;
    setStartValues();
}

void RGBLED::blink(uint8_t red, uint8_t green, uint8_t blue, unsigned long time) {
    state = 4;
    _time = time;
    _targetred = red;
    _targetgreen = green;
    _targetblue = blue;
    setStartValues();
}

void RGBLED::rainbow(unsigned long time) {
    state = 5;
    _time = time;
    setStartValues();
}

void RGBLED::off() {
    state = 0;
    writeCurrentValues(0, 0, 0);
    setStartValues();
}

int RGBLED::getCurrentRed() {
    return _currentred;
}

int RGBLED::getCurrentGreen() {
    return _currentgreen;
}

int RGBLED::getCurrentBlue() {
    return _currentblue;
}