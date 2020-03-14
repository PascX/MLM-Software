/*

*/
#include <Arduino.h>

class Encoder{

private:
  uint8_t pinA;
  uint8_t pinB;

  bool pinStateNow_A;
  bool pinStateNow_B;
  bool pinStateLast_A;
  bool pinStateLast_B;

  int8_t counter;
  int8_t state;

  void updatePinState(){
    pinStateNow_A = digitalRead(pinA);
    pinStateNow_B = digitalRead(pinB);
  }
  void new2OldState(){
    pinStateLast_A = pinStateNow_A;
    pinStateLast_B = pinStateNow_B;
  }
public:
  Encoder(){}
  Encoder(uint8_t A, uint8_t B){
    //pass Pin Numbers
    pinA = A;
    pinB = B;

    //init Pins
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);

    //read current Pin-State
    updatePinState();
    new2OldState();

    //init default values
    state = 0;
    counter = 0;
  }


  /*
  State Table: (Only Valid Statess -> any other State is invalid; + means right turn)
  LAST:   NOW:      VAL:    State:
  A:  B:  A:  B:
  0   0   0   1     1       -1
  0   0   1   0     2       +1
  0   1   0   0     4       +1
  0   1   1   1     7       -1
  1   0   0   0     8       -1
  1   0   1   1     11      +1
  1   1   0   1     13      +1
  1   1   1   0     14      -1
  */// Maybe we need the states with +/-2
  int8_t updateState(){
    updatePinState();

    uint8_t binaryState = 0;
    binaryState |= pinStateLast_A * 8;
    binaryState |= pinStateLast_B * 4;
    binaryState |= pinStateNow_A * 2;
    binaryState |= pinStateNow_B * 1;

    switch (binaryState) {
      case 2: case 4: case 11: case 13:
        //Right turn
        counter++;
        break;

      case 1: case 7: case 8: case 14:
        //Left turn
        counter--;
        break;
      default:
        //Maybe some Error handling here. If it's necessary.
        break;
    }

    //4 Ticks are one in/decrement.
    switch (counter) {
      case 4:
        state = 1;
        counter = 0;
        break;
      case -4:
        state = -1;
        counter = 0;
        break;
      default:
        state = 0;
    }

    new2OldState();

    return state;

  }


};
