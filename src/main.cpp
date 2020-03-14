#include <Arduino.h>
#include <MIDIUSB.h>
#include "FaderController.cpp"
#include "ButtonController.cpp"
#include "EncoderController.cpp"

long start;
long stop;
long startCycle;

#define CYCLE_TIME_US 1000

#define ENCODER_BUTTON_MODE 1 // Determs if Encoder Events will be treated like Buttons

#define FADER_FILTER_FACTOR 60 //short FFF. OLDVALUE*FFF + NEWVALUE*(100 - FFF)

//***********Pin Declaration**********
const uint8_t faderPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const uint8_t numFader = sizeof(faderPins)/sizeof(faderPins[0]);
#define NUM_BUTTONS 102
#define LD_PIN 2
const uint8_t encPins[][2] = {
                          { 3, 4}, { 5, 6}, { 7, 8}, { 9,10}, {11,12},
                          {23,24}, {25,26}, {27,28}, {29,30}, {31,32},
                          {33,34}, {35,36}, {37,38}, {39,40}, {41,42},
                          {43,44}, {45,46}, {47,48}, {49,50}, {51,52}
                        };
const uint8_t numEnc = 20; // 5*4
//************************************

//***********Controller***************
FaderController faderController;
ButtonController buttonController;
EncoderController encoderController;
//************************************

//***********MIDI functions***********
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
//************************************

//***********Callbacks****************
//Fader
void valueChangedAction(uint8_t id, uint8_t val){
   /* Serial.print("New Value on Fader ");
   Serial.print(id);
   Serial.print(": ");
   Serial.println(val); */
  controlChange(id, 0x03, val);
}
void valueNotChangedAction(uint8_t id, uint8_t val){
}

//Buttons
void pressedAction(uint8_t id){
  // Serial.print("Pressed Button Number ");
  // Serial.println(id);
  noteOn(0, id, 0x7F);
}
void releasedAction(uint8_t id){
  // Serial.print("Released Button Number ");
  // Serial.println(id);
  noteOff(0, id, 0x00);
}

//Encoder
void rightAction(uint8_t id){
  // Serial.print("Turned Right Number ");
  // Serial.println(id);
  #ifdef ENCODER_BUTTON_MODE
    noteOn(1, id*2+1, 0x7F);
    delayMicroseconds(50); //Without this the Receiver misses the noteOFF
    noteOff(1, id*2+1, 0x00);
  #else
    controlChange(0, 0x09, id*2+1);
  #endif
}
void leftAction(uint8_t id){
  // Serial.print("Turned Left Number ");
  // Serial.println(id);
  #ifdef ENCODER_BUTTON_MODE
    noteOn(1, id*2, 0x7F);
    delayMicroseconds(50);//Without this the Receiver misses the noteOFF
    noteOff(1, id*2, 0x00);
  #else
    controlChange(0, 0x09, id*2);
  #endif
}
//************************************

void setup() {
  Serial.begin(115200);
  Serial.println("Here we go!");

  //***********Initialize Controller****
  faderController = FaderController(11);
  for (size_t i = 0; i < numFader; i++) {
    faderController.addFader(faderPins[i], FADER_FILTER_FACTOR);
  }

  buttonController = ButtonController(NUM_BUTTONS, LD_PIN);

  encoderController = EncoderController(numEnc);
  for (size_t i = 0; i < numEnc; i++) {
    encoderController.addEncoder(encPins[i][0], encPins[i][1]);
  }
  //************************************

  //***********Set Callbacks************
  faderController.setValueChangedActionCallback(valueChangedAction);
  faderController.setValueNotChangedActionCallback(valueNotChangedAction);

  buttonController.setButtonPressedActionCallback(pressedAction);
  buttonController.setButtonReleasedActionCallback(releasedAction);

  encoderController.setLeftTurnActionCallback(leftAction);
  encoderController.setRightTurnActionCallback(rightAction);
  //************************************
}

void loop() {
  startCycle = micros();

  // start = micros();
  faderController.processFader();
  // stop = micros();
  // Serial.print("Fader Time: ");
  // Serial.print(stop-start);
  // Serial.println(" us");

  // start = micros();
  buttonController.processButtons();
  // stop = micros();
  // Serial.print("Button Time: ");
  // Serial.print(stop-start);
  // Serial.println(" us");

  // start = micros();
  encoderController.processEncoders();
  // stop = micros();
  // Serial.print("Encoder Time: ");
  // Serial.print(stop-start);
  // Serial.println(" us");

  while(micros() - startCycle < CYCLE_TIME_US){}

}
