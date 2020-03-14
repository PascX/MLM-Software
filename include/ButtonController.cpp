/*

*/
#include <Arduino.h>
#include "_74xx165.cpp"


class ButtonController{

private:
  _74xx165 pisoChip;
  uint8_t _numButtons;
  uint8_t *buttonValsNow;
  uint8_t *buttonValsLast;


  //Template for Callback: https://forum.arduino.cc/index.php?topic=65095.0
  void (*_buttonPressedAction)(uint8_t id);
  void (*_buttonReleasedAction)(uint8_t id);

public:
  ButtonController(){};
  ButtonController(uint8_t numButtons, uint8_t loadPin){
    _numButtons = numButtons;

    uint8_t chipCount = 0;
    if (_numButtons % 8 == 0) {
      chipCount = _numButtons / 8;
    } else{
      chipCount = (_numButtons / 8) + 1;
    }

    pisoChip = _74xx165(loadPin, chipCount, SPISettings(10e3, LSBFIRST, SPI_MODE2));

    buttonValsNow = new uint8_t[_numButtons]; //initializing the array
    buttonValsLast = new uint8_t[_numButtons]; //initializing the array
    if(buttonValsNow == nullptr || buttonValsLast == nullptr){
      Serial.println("Schwerer Fehler!! Code 02");
    }

    pisoChip.read(&buttonValsNow[0]); //initial reading of the values
    memcpy(buttonValsLast, buttonValsNow, _numButtons);

  }

  void setButtonPressedActionCallback( void (*buttonPressedAction)(uint8_t id) ){
    _buttonPressedAction = buttonPressedAction;
  }
  void setButtonReleasedActionCallback( void (*buttonReleasedAction)(uint8_t id) ){
    _buttonReleasedAction = buttonReleasedAction;
  }
  /*
  State Table:
  LAST:   NOW:      VAL:    State:
  0       0         0       No Change
  0       1         1       Pressed
  1       0         2       Released
  1       1         3       No Change
  */
  void processButtons(){
    pisoChip.read(&buttonValsNow[0]); //Read Inputs
    for (size_t i = 0; i < _numButtons; i++) {
      uint8_t chipNumber = i/8; //Results in the chipNumber i.e. 7/8 = 0; 8/8 = 1; ...
      //Serial.println("New Byte "+String(chipNumber)+": "+String(buttonValsNow[chipNumber]));
      //Serial.println("Last Byte "+String(chipNumber)+": "+String(buttonValsLast[chipNumber]));
      uint8_t j = i%8;
      //Serial.println("J: "+String(j));
      uint8_t state = (buttonValsNow[chipNumber] >> (7-j)) & 1;
      //Serial.println("state 1: "+String(state));
      state |= ( (buttonValsLast[chipNumber] >> (7-j)) & 1 ) * 2;
      //Serial.println("state 2: "+String(state));

      switch (state) {
        case 1:
          _buttonPressedAction(i);
          break;
        case 2:
          _buttonReleasedAction(i);
          break;
        default:
          break;
      }
    }

    memcpy(buttonValsLast, buttonValsNow, _numButtons);//OLD = NEW
  }


};
