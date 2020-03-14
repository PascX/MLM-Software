/*

*/
#include <Arduino.h>
#define FACTOR 100

class Fader{
private:

  uint8_t _pin;
  uint32_t _outputValue;
  uint32_t _holdValue;
  uint8_t _outputValueLast;
  uint8_t _filterFactor; // In %
  uint8_t _state; //1=value changed; 0=value not changed


public:
  Fader(){}
  Fader(uint8_t pin){
    _pin = pin;

    _holdValue = 0;
    _filterFactor = 40;
    _outputValue = analogRead(_pin);
    _state = 1;
  }
  Fader(uint8_t pin, uint8_t filterFactor){
    _pin = pin;

    _holdValue = 0;
    _filterFactor = filterFactor;
    _outputValue = analogRead(_pin);
    _state = 1;
  }

  void updateValue(){
    _outputValueLast = _outputValue;
    /*
    IIR-Lowpassfilter
    _filterFactor = exp(-w_0 * T_A) in %
    with:
    -w_0: cut-off frequency in rad/s
    T_A: samplerate
    */
    // uint16_t val = analogRead(_pin);
    // uint32_t tmp1 = _outputValue*_filterFactor;
    // uint32_t tmp2 = val * (FACTOR - _filterFactor);
/*      Serial.print("Val:");
     Serial.println(val); */
    // Serial.print("Tmp2:");
    // Serial.println(tmp2);
    //val = val >> 2;
    _holdValue = ( (_holdValue*_filterFactor) + ((analogRead(_pin) << 8) * (FACTOR - _filterFactor)) ) / FACTOR; //Linksshift um Genauigkeit zu erhöhen
    _outputValue = _holdValue >> (3+8);
     /* Serial.print("HOLD:");
     Serial.println(_holdValue); */
    // _outputValue = (tmp1 + tmp2) / FACTOR;
    // Serial.print("NOW2:");
    // Serial.println(_outputValue);

    if (_outputValue == _outputValueLast){
      _state = 0; //Value not changed
    } else{
      _state = 1; //value changed
    }

  }

  uint8_t getValue(){
    return _outputValue;
  }

  float getFilterFactor(){
    return _filterFactor;
  }

  uint8_t getState(){
    return _state;
  }

  uint8_t getPin(){
    return _pin;
  }


};
