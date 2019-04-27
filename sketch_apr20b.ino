#include <ClickEncoder.h>
#include <TimerOne.h>

#define ENCODER1_PINA     7
#define ENCODER1_PINB     8
#define ENCODER1_BTN      2

#define ENCODER2_PINA     4
#define ENCODER2_PINB     12
#define ENCODER2_BTN      A1

#define ENCODER3_PINA     4
#define ENCODER3_PINB     12
#define ENCODER3_BTN      A1

#define ENCODER4_PINA     4
#define ENCODER4_PINB     12
#define ENCODER4_BTN      A1


#define ENCODER_STEPS_PER_NOTCH    1   // Change this depending on which encoder is used

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

ClickEncoder encoder4 = ClickEncoder(
  ENCODER4_PINA,
  ENCODER4_PINB,
  ENCODER4_BTN,
  ENCODER_STEPS_PER_NOTCH
);


class ledStrip {
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

class cwwStrip: public ledStrip {
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



void timerIsr() {      //Service methods from both instances must be included
  encoder1.service();
  encoder2.service();
  encoder3.service();
  encoder4.service();
}

void setup() {
  Serial.begin(115200);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  encoder1.setButtonHeldEnabled(false);
  encoder1.setDoubleClickEnabled(true);

  encoder2.setButtonHeldEnabled(false);
  encoder2.setDoubleClickEnabled(true);

  encoder3.setButtonHeldEnabled(false);
  encoder3.setDoubleClickEnabled(true);

  encoder4.setButtonHeldEnabled(false);
  encoder4.setDoubleClickEnabled(true);

  // Enable the button to be on pin 0.  Normally pin 0 is not recognized as a valid pin for a button,
  // this is to maintain backward compatibility with an old version of the library
  // This version can have the button on pin zero, and this call enables the feature.
  // in this version best to use pin -1 instead of 0 to disable button functions
  encoder1.setButtonOnPinZeroEnabled(false);
  encoder2.setButtonOnPinZeroEnabled(false);
  encoder3.setButtonOnPinZeroEnabled(false);
  encoder4.setButtonOnPinZeroEnabled(false);

}

void loop() {
  static int_fast16_t value1, value2, value3, value4;
  static int_fast16_t last1, last2, last3, last4;

  value1 += encoder1.getValue();
  value2 += encoder2.getValue();
  value3 += encoder3.getValue();
  value4 += encoder4.getValue();

  if (value1 != last1) {
    last1 = value1;
    analogWrite(3, value1);
    Serial.print("Encoder1 Value: ");
    Serial.println(value1);
  }

  if (value2 != last2) {
    last1 = value2;
    analogWrite(3, value2);
    Serial.print("Encoder2 Value: ");
    Serial.println(value2);
  }

  if (value3 != last3) {
    last1 = value3;
    analogWrite(3, value3);
    Serial.print("Encoder3 Value: ");
    Serial.println(value3);
  }

  if (value4 != last4) {
    last1 = value4;
    analogWrite(3, value4);
    Serial.print("Encoder4 Value: ");
    Serial.println(value4);
  }

  ClickEncoder::Button b1 = encoder1.getButton();
  if (b1 != ClickEncoder::Open) {
    Serial.print("Button1: ");
    #define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b1) {
      VERBOSECASE(ClickEncoder::Pressed);
      VERBOSECASE(ClickEncoder::Held)
      VERBOSECASE(ClickEncoder::Released)
      VERBOSECASE(ClickEncoder::Clicked)
      VERBOSECASE(ClickEncoder::DoubleClicked)
    }
  }

  ClickEncoder::Button b2 = encoder2.getButton();
  if (b2 != ClickEncoder::Open) {
    Serial.print("Button2: ");
    #define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b2) {
      VERBOSECASE(ClickEncoder::Pressed);
      VERBOSECASE(ClickEncoder::Held)
      VERBOSECASE(ClickEncoder::Released)
      VERBOSECASE(ClickEncoder::Clicked)
      VERBOSECASE(ClickEncoder::DoubleClicked)
    }
   }
}
