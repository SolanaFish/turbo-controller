#include <ClickEncoder.h>
#include <TimerOne.h>
#include <Wire.h>

#define ENCODERS    4

#define ENCODER0_PINA     2
#define ENCODER0_PINB     3
#define ENCODER0_BTN      4

#define ENCODER1_PINA     5
#define ENCODER1_PINB     6
#define ENCODER1_BTN      7

#define ENCODER2_PINA     8
#define ENCODER2_PINB     9
#define ENCODER2_BTN      10

#define ENCODER3_PINA     11
#define ENCODER3_PINB     12
#define ENCODER3_BTN      13

ClickEncoder encoder0 = ClickEncoder(
  ENCODER0_PINA,
  ENCODER0_PINB,
  ENCODER0_BTN,
  4
);

ClickEncoder encoder1 = ClickEncoder(
  ENCODER1_PINA,
  ENCODER1_PINB,
  ENCODER1_BTN,
  2
);

ClickEncoder encoder2 = ClickEncoder(
  ENCODER2_PINA,
  ENCODER2_PINB,
  ENCODER2_BTN,
  2
);

ClickEncoder encoder3 = ClickEncoder(
  ENCODER3_PINA,
  ENCODER3_PINB,
  ENCODER3_BTN,
  2
);

ClickEncoder *encoders[4];

void timerIsr() {
  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    encoders[i] -> service();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  encoders[0] = &encoder0;
  encoders[1] = &encoder1;
  encoders[2] = &encoder2;
  encoders[3] = &encoder3;

  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    encoders[i] -> setButtonHeldEnabled(false);
    encoders[i] -> setDoubleClickEnabled(true);

    // Enable the button to be on pin 0.  Normally pin 0 is not recognized as a valid pin for a button,
    // this is to maintain backward compatibility with an old version of the library
    // This version can have the button on pin zero, and this call enables the feature.
    // in this version best to use pin -1 instead of 0 to disable button functions
    encoders[i] -> setButtonOnPinZeroEnabled(false);
  }
}

void loop() {
  static int_fast16_t value[ENCODERS];
  static int_fast16_t last[ENCODERS];
  ClickEncoder::Button button;

  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    value[i] += encoders[i] -> getValue();
  }

  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    // turn handle
    if (value[i] != last[i]) {
      Wire.beginTransmission(1);
      Wire.write((byte)i);
      Wire.write((byte)1);
      Wire.endTransmission();
      switch(i) {
        default: {
          Serial.print("Encoder ");
          Serial.print(i);
          Serial.print(" : ");
          Serial.println(value[i]);
        } break;
      }

      last[i] = value[i];
    }

    // button handle
    button = encoders[i] -> getButton();
    if (button != ClickEncoder::Open) {
      Serial.print("Button ");
      Serial.print(i);
      #define VERBOSECASE(label) case label: Serial.println(#label); break;
      switch (button) {
        VERBOSECASE(ClickEncoder::Pressed)
        VERBOSECASE(ClickEncoder::Held)
        VERBOSECASE(ClickEncoder::Released)
        VERBOSECASE(ClickEncoder::Clicked)
        VERBOSECASE(ClickEncoder::DoubleClicked)
      }

      switch(i) {
        default: {

        } break;
      }
    }
  }
}
