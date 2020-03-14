/*

*/
#include <Arduino.h>
#include "Fader.cpp"

class FaderController{

private:
  Fader *faderPool;

  uint8_t _numFader;
  uint8_t _numFree; //Number of NOT used array elements
  uint8_t _nextFreeElement; //Number of the next free element

  //Template for Callback: https://forum.arduino.cc/index.php?topic=65095.0
  void (*_valueChangedAction)(uint8_t id, uint8_t val);
  void (*_valueNotChangedAction)(uint8_t id, uint8_t val);

public:
  FaderController(){}
  FaderController(uint8_t numFader){
    _numFader = numFader;
    _numFree = _numFader;
    _nextFreeElement = 0;

    faderPool = new Fader[_numFader];
  }

  /*
  Returns the position of the Fader in the array
  If there are no free elements it returns -1
  */
  //int8_t addFader(uint8_t pin, float cutOffFreq_Hz, float samplerate){ // Returns the position of the Fader in the array; if
  int8_t addFader(uint8_t pin, uint8_t filterFactor){ // Returns the position of the Fader in the array; if
    if (_numFree > 0){
      faderPool[_nextFreeElement] = Fader(pin, filterFactor);
      _numFree--; //decrement free counter
      _nextFreeElement++;
      return _nextFreeElement - 1;
    } else{
      return -1;
    }
  }

  void setValueChangedActionCallback(   void (*valueChangedAction)(uint8_t id, uint8_t val) ){
    _valueChangedAction = valueChangedAction;
  }
  void setValueNotChangedActionCallback(   void (*valueNotChangedAction)(uint8_t id, uint8_t val) ){
    _valueNotChangedAction = valueNotChangedAction;
  }

  void processFader(){
    for (uint8_t i = 0; i < _numFader-_numFree; i++) {
      // Serial.print("FF=");
      // Serial.println(faderPool[i].getFilterFactor(),10);
      faderPool[i].updateValue();
      switch (faderPool[i].getState()) {
        case 0: //Fadervalue hasn't changed
          _valueNotChangedAction(i, faderPool[i].getValue());
          break;
        case 1:
          _valueChangedAction(i, faderPool[i].getValue());
          break;
        default:
          break;
      }
    }
  }

};
