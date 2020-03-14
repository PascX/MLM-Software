/*

*/
#include <Arduino.h>
#include "Encoder.cpp"

class EncoderController{

private:
  Encoder *encoderPool; //Pointer to an array. the array will be initialized later!

  uint8_t _numEncoder;
  uint8_t _numFree; //Number of NOT used array elements
  uint8_t _nextFreeElement; //Index of the next free element

  //Template for Callback: https://forum.arduino.cc/index.php?topic=65095.0
  void (*_rightTurnAction)(uint8_t id);
  void (*_leftTurnAction)(uint8_t id);

public:
  EncoderController(){}
  EncoderController(uint8_t numEncoder){
    encoderPool = new Encoder[numEncoder]; //initializing the array
    if(encoderPool == nullptr){Serial.println("Schwerer Fehler!! Code 01");}

    _numEncoder = numEncoder;
    _numFree = _numEncoder;
    _nextFreeElement = 0;
  }
  /*
  Returns the position of the Encoder in the array
  If there are no free elements it returns -1
  */
  int8_t addEncoder(uint8_t A, uint8_t B){ // Returns the position of the Encoder in the array; if
    if (_numFree > 0){
      encoderPool[_nextFreeElement] = Encoder(A, B);
      _numFree--; //decrement free counter
      _nextFreeElement++;
      return _nextFreeElement - 1;
    } else{
      return -1;
    }
  }

  void setRightTurnActionCallback( void (*rightTurnAction)(uint8_t id) ){
    _rightTurnAction = rightTurnAction;
  }
  void setLeftTurnActionCallback( void (*leftTurnAction)(uint8_t id) ){
    _leftTurnAction = leftTurnAction;
  }

  void processEncoders(){
    for (uint8_t i = 0; i < _numEncoder-_numFree; i++) {
      switch (encoderPool[i].updateState()) {
        case 1:
          _rightTurnAction(i);
          break;
        case -1:
          _leftTurnAction(i);
          break;
        default:
          break;
      }

    }
  }


};
