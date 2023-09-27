//
// Created by selexin on 26/3/20.
//

#include "Arduino.h"
#include "EasyDriver.h"

#define MODE_CHANGE_DELAY_MS 50

EasyDriver::EasyDriver(int pinStep, int pinDir, int pinMs1, int pinMs2, int pinEnable) {
  PIN_STEP = pinStep;
  PIN_DIR = pinDir;
  PIN_MS1 = pinMs1;
  PIN_MS2 = pinMs2;
  PIN_ENABLE = pinEnable;

  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_MS1, OUTPUT);
  pinMode(PIN_MS2, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);

  enabled = false;
  currentMode = EASYDRIVER_MODE_FULL_STEP;
  direction = EASYDRIVER_DIRECTION_FORWARDS;
  delayMicros = 1000;
}

// Reset Easy Driver pins to default states
void EasyDriver::reset() {
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_DIR, LOW);
  digitalWrite(PIN_MS1, LOW);
  digitalWrite(PIN_MS2, LOW);
  enable(false);
  setDelay(1000);
  delay(MODE_CHANGE_DELAY_MS);
}


void EasyDriver::enable(bool enabled) {
  if (enabled) {
    digitalWrite(PIN_ENABLE, HIGH);
    
  } else {
    digitalWrite(PIN_ENABLE, LOW);
  }
  this->enabled = enabled;
  delay(MODE_CHANGE_DELAY_MS);
}

void EasyDriver::setMode(int mode) {
  /** Table of Truth
    MS1  MS2  Microstep Resolution
    -----------------------------
    LOW  LOW  Full Step (2 Phase)
    HIGH LOW  Half Step
    LOW  HIGH Quarter Step
    HIGH HIGH Eigth Step
  */

  if (this->currentMode == mode) {
    return;
  }

  switch (mode) {
    case EASYDRIVER_MODE_EIGHTH_STEP:
      digitalWrite(PIN_MS1, HIGH);
      digitalWrite(PIN_MS2, HIGH);
      break;
    case EASYDRIVER_MODE_QUARTER_STEP:
      digitalWrite(PIN_MS1, LOW);
      digitalWrite(PIN_MS2, HIGH);
      break;
    case EASYDRIVER_MODE_HALF_STEP:
      digitalWrite(PIN_MS1, HIGH);
      digitalWrite(PIN_MS2, LOW);
      break;
    default: // (EASYDRIVER_MODE_FULL_STEP)
      digitalWrite(PIN_MS1, LOW);
      digitalWrite(PIN_MS2, LOW);
      break;
  }
  this->currentMode = mode;
  delay(MODE_CHANGE_DELAY_MS);
}


void EasyDriver::setDirection(int direction) {
  if (this->direction == direction) {
    return;
  }
  if (direction == EASYDRIVER_DIRECTION_FORWARDS) {
    digitalWrite(PIN_DIR, LOW);
  } else if (direction == EASYDRIVER_DIRECTION_REVERSE) {
    digitalWrite(PIN_DIR, HIGH);
  }

  this->direction = direction;
  delay(MODE_CHANGE_DELAY_MS);
}

void EasyDriver::setDelay(int _micros) {
  this->delayMicros = _micros;
}

void EasyDriver::step() {
  digitalWrite(PIN_STEP, HIGH);
  delayMicroseconds(delayMicros);
  digitalWrite(PIN_STEP, LOW);
  delayMicroseconds(delayMicros);
}

void EasyDriver::stepForward() {
  this->setDirection(EASYDRIVER_DIRECTION_FORWARDS);
  step();
}

void EasyDriver::stepReverse() {
  this->setDirection(EASYDRIVER_DIRECTION_REVERSE);
  step();
}
