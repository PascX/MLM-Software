/*
Version with only SPI
*/
#include "Arduino.h"
#include <SPI.h>

class _74xx165{

private:
  uint8_t _loadPin;
  uint8_t _numChip;
  SPISettings _mySettings;

public:
  _74xx165(){}
  _74xx165(uint8_t loadPin, uint8_t numChip){
    _loadPin = loadPin;
    _numChip = numChip;

    pinMode(loadPin, OUTPUT);
    digitalWrite(_loadPin, HIGH);

    SPI.begin();

    _mySettings = SPISettings(10e6, LSBFIRST, SPI_MODE2);
  }
  _74xx165(uint8_t loadPin, uint8_t numChip, SPISettings mySettings){
    _loadPin = loadPin;
    _numChip = numChip;

    pinMode(loadPin, OUTPUT);
    digitalWrite(_loadPin, HIGH);

    SPI.begin();

    _mySettings = mySettings;
  }

  void read(uint8_t* result){
    //Array mit 0 initialisieren
    for (size_t i = 0; i < _numChip; i++) { result[i] = 0; }
    //SPI aktivieren
    SPI.beginTransaction(_mySettings);

    //load parallel Inputs
    digitalWrite(_loadPin, LOW);
    digitalWrite(_loadPin, HIGH);

    for (size_t j = 0; j < _numChip; j++) {
      result[j] = ~SPI.transfer(0x00);
    }
  }

};
