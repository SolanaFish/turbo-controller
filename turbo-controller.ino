#include <ClickEncoder.h>
#include <TimerOne.h>

#define ENCODERS    4

#define ENCODER0_PINA     2
#define ENCODER0_PINB     4
#define ENCODER0_BTN      7

#define ENCODER1_PINA     8
#define ENCODER1_PINB     12
#define ENCODER1_BTN      13

#define ENCODER2_PINA     A0
#define ENCODER2_PINB     A1
#define ENCODER2_BTN      A2

#define ENCODER3_PINA     A3
#define ENCODER3_PINB     A4
#define ENCODER3_BTN      A5

#define strip1A    3
#define strip1B    5

#define strip2A    6
#define strip2B    9

#define strip3    10

#define ENCODER_STEPS_PER_NOTCH    4   // Change this depending on which encoder is used

ClickEncoder encoder0 = ClickEncoder(
  ENCODER0_PINA,
  ENCODER0_PINB,
  ENCODER0_BTN,
  ENCODER_STEPS_PER_NOTCH
);

ClickEncoder encoder1 = ClickEncoder(
  ENCODER1_PINA,
  ENCODER1_PINB,
  ENCODER1_BTN,
  ENCODER_STEPS_PER_NOTCH
);

ClickEncoder encoder2 = ClickEncoder(
  ENCODER2_PINA,
  ENCODER2_PINB,
  ENCODER2_BTN,
  ENCODER_STEPS_PER_NOTCH
);

ClickEncoder encoder3 = ClickEncoder(
  ENCODER3_PINA,
  ENCODER3_PINB,
  ENCODER3_BTN,
  ENCODER_STEPS_PER_NOTCH
);

ClickEncoder *encoders[4];

class LedStrip {
protected:
  int_fast16_t brightness;
  bool on;

public:
  ledStrip(int_fast16_t brightness = 255, bool on = false) {
    this -> brightness = brightness;
    this -> on = on;
  }

  void setBrightness(int_fast16_t i) {
    if (i > 255) {
      i = 255;
    }
    if (i < 0) {
      i = 0;
    }

    brightness = i;
  }

  void brighter (int_fast16_t i = 1) {
    this -> setBrightness(brightness + i);
  }

  void darker (int_fast16_t i = 1) {
    this -> setBrightness(brightness - i);
  }

  int_fast16_t getBrightness() {
    return brightness;
  }

  void toggle() {
    on = !on;
  }

  bool isOn() {
    return on;
  }

  int_fast16_t get() {
    return on * brightness;
  }
};

class CwwStrip: public LedStrip {
protected:
  int_fast16_t warmLevel, coldLevel;

public:
  cwwStrip(int_fast16_t brightness = 255, bool on = false, int_fast16_t warmLevel = 1, int_fast16_t coldLevel = 1): ledStrip(brightness, on) {
    this -> warmLevel = warmLevel;
    this -> coldLevel = coldLevel;
  }

  void setTemperature(int_fast16_t warm = 100, int_fast16_t cold = 100) {
    int_fast16_t sum = warm + cold;
    float warmNormal, coldNormal;

    warmNormal = warm / sum;
    coldLevel = cold / sum;

    warmLevel = warmNormal * 255;
    coldLevel = coldNormal * 255;
  }

  void warmer(int_fast16_t i = 1) {
    warmLevel += i;
    if (warmLevel > 255) {
      warmLevel = 255;
    }

    coldLevel -= i;
    if (coldLevel < 0) {
      coldLevel = 0;
    }
  }

  void colder(int_fast16_t i = 1) {
    coldLevel += i;
    if (coldLevel > 255) {
      coldLevel = 255;
    }

    warmLevel -= i;
    if (warmLevel < 0) {
      warmLevel = 0;
    }
  }

  int_fast16_t getWarm() {
    return on * (warmLevel / 255) * brightness;
  }

  int_fast16_t getCold() {
    return on * (coldLevel / 255) * brightness;
  }
};

void timerIsr() {
  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    encoders[i] -> service();
  }
}

void setup() {
  Serial.begin(115200);

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

  strip1 = new ledStrip()
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
      last[i] = value[i];
      switch(i) {
        case 0: {

        } break;
        default: {
          Serial.print("Encoder ");
          Serial.print(i);
          Serial.print(" : ");
          Serial.println(value[i]);
        } break;
      }
    }

    // button handle
    button = encoders[i] -> getButton();
    if (button != ClickEncoder::Open) {
      switch(i) {
        default: {
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
        } break;
      }
    }
  }
}
