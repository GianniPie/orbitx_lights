#include "defines.h"

uint8_t inputKeysRegister, oldKeysRegister, xorKeysRegister, keysRegister, keysRegisterPressed, keysRegisterReleased, keysRegisterLongPress, oldMainKeysRegister = 0;
uint8_t counterDebounce, counterLongPress, longPressCounter = 0;
uint8_t max_debounce = 5;



void inputManager(void) {

  inputsRead();

  if (bitRead(keysRegisterPressed, INPUT_DOOR)) {
    analogWrite(PIN_WHITE_LIGHT, LIGHT_OFF);
  }
  if (bitRead(keysRegisterReleased, INPUT_DOOR)) {
    analogWrite(PIN_WHITE_LIGHT, LIGHT_HIGH);
  }
  /*
  if (bitRead(keysRegisterPressed, INPUT_SEAT_FL)) {  }
  if (bitRead(keysRegisterReleased, INPUT_SEAT_FL)) {  }
  if (bitRead(keysRegisterPressed, INPUT_SEAT_FR)) {  }
  if (bitRead(keysRegisterReleased, INPUT_SEAT_FR)) {  }
  if (bitRead(keysRegisterPressed, INPUT_SEAT_RL)) {  }
  if (bitRead(keysRegisterReleased, INPUT_SEAT_RL)) {  }
  if (bitRead(keysRegisterPressed, INPUT_SEAT_RR)) {  }
  if (bitRead(keysRegisterReleased, INPUT_SEAT_RR)) {  }
  */
}


void inputsRead(void) {

  inputKeysRegister = 0b00000000;
  bitWrite(inputKeysRegister, INPUT_DOOR   , !digitalRead(PIN_DOOR   ));
  bitWrite(inputKeysRegister, INPUT_SEAT_FL, !digitalRead(PIN_SEAT_FL));
  bitWrite(inputKeysRegister, INPUT_SEAT_FR, !digitalRead(PIN_SEAT_FR));
  bitWrite(inputKeysRegister, INPUT_SEAT_RL, !digitalRead(PIN_SEAT_RL));
  bitWrite(inputKeysRegister, INPUT_SEAT_RR, !digitalRead(PIN_SEAT_RR));

  //^ = exor
  xorKeysRegister = inputKeysRegister ^ oldKeysRegister;  // 1 = input changed
  oldKeysRegister = inputKeysRegister;

  if (xorKeysRegister) {
    // at least one input has changed
    counterDebounce = 0;
  } else {
    // nothing changed
    if (counterDebounce < max_debounce) {
      counterDebounce++;
    } else {
      keysRegister = oldKeysRegister;
      counterDebounce = max_debounce;
    }
  }

  if (keysRegisterReleased) {
    keysRegisterLongPress = 0;
    counterLongPress = 0;
  }

  keysRegisterPressed = keysRegister & (~(oldMainKeysRegister));
  keysRegisterReleased = (~(keysRegister)) & oldMainKeysRegister;
  oldMainKeysRegister = keysRegister;
}