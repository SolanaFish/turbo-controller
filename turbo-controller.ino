#include <ClickEncoder.h>
#include <TimerOne.h>

#define ENCODERS    4

#define ENCODER0_PINA     2
#define ENCODER0_PINB     4
#define ENCODER0_BTN      7

#define ENCODER1_PINA     8
#define ENCODER1_PINB     12
#define ENCODER1_BTN      13

#define ENCODER2_PINA     A1
#define ENCODER2_PINB     A0
#define ENCODER2_BTN      A2

#define ENCODER3_PINA     A3
#define ENCODER3_PINB     A4
#define ENCODER3_BTN      A6

#define STRIP1A           3
#define STRIP1B           6

#define STRIP2A           6
#define STRIP2B           9

#define STRIP3A           5

#define PSU_ON            A5

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

class LedStrip {
  protected:
  int_fast16_t brightness;
  bool on;

  public:
  LedStrip(int_fast16_t brightness = 255, bool on = false) {
    this -> brightness = brightness;
    this -> on = on;
  }

  virtual void update() = 0;

  void setBrightness(int_fast16_t i) {
    if (i > 255) {
      i = 255;
    }
    if (i < 0) {
      i = 0;
    }

    brightness = i;
    this -> update();
  }

  void brighter (int_fast16_t i = 5) {
    this -> setBrightness(brightness + i);
    on = true;
    this -> update();
  }

  void darker (int_fast16_t i = 5) {
    this -> setBrightness(brightness - i);
    on = true;
    this -> update();
  }

  void toggle() {
    on = !on;
    this -> update();
  }

  int_fast16_t getBrightness() {
    return brightness;
  }

  bool isOn() {
    return on;
  }
};

class SingleStrip: public LedStrip {
  int_fast16_t pin;

public:
  SingleStrip(int_fast16_t pin, int_fast16_t brightness = 255, bool on = false): LedStrip(brightness, on) {
    this -> pin = pin;
    update();
  }

  void update() {
    {
      Serial.print((int)on);
      Serial.print(" ");
      Serial.print(brightness);
      Serial.print(" ");
      Serial.print(pin);
      Serial.print(" ");
      Serial.println(on * brightness);
    }
    analogWrite(pin, on * brightness);
  }
};

class CwwStrip: public LedStrip {
protected:
  int_fast16_t warmLevel, coldLevel;
  int_fast16_t pinA, pinB;

public:
  CwwStrip(int_fast16_t pinA, int_fast16_t pinB, int_fast16_t brightness = 255, bool on = false, int_fast16_t warmLevel = 255, int_fast16_t coldLevel = 255): LedStrip(brightness, on) {
    this -> pinA = pinA;
    this -> pinB = pinB;

    this -> warmLevel = warmLevel;
    this -> coldLevel = coldLevel;
    update();
  }

  void update() {
    {
      Serial.print("On: ");
      Serial.print((int)on);
      Serial.print(" B: ");
      Serial.print(brightness);
      Serial.print(" A: ");
      Serial.print(pinA);
      Serial.print(" B: ");
      Serial.print(pinB);
      Serial.print(" W: ");
      Serial.print(warmLevel);
      Serial.print(" C: ");
      Serial.print(coldLevel);
      Serial.print(" : ");
      Serial.print((int)(on * ((float)warmLevel / 255.0) * brightness));
      Serial.print(" : ");
      Serial.println((int)(on * ((float)coldLevel / 255.0) * brightness));
    }
    analogWrite(pinA, (int)(on * ((float)warmLevel / 255.0) * brightness));
    analogWrite(pinB, (int)(on * ((float)coldLevel / 255.0) * brightness));
  }

  void setTemperature(int_fast16_t warm = 100, int_fast16_t cold = 100) {
    int_fast16_t bigger = warm > cold ? warm : cold;
    float warmNormal, coldNormal;

    warmNormal = (float)warm / (float)bigger;
    coldNormal = (float)cold / (float)bigger;

    warmLevel = (int)(warmNormal * 255.0);
    coldLevel = (int)(coldNormal * 255.0);

    on = true;
    update();
  }

  void warmer(int_fast16_t i = 5) {
    warmLevel += i;
    if (warmLevel >= 255) {
      warmLevel = 255;

      coldLevel -= i;
      if (coldLevel < 0) {
        coldLevel = 0;
      }
    }

    on = true;
    update();
  }

  void colder(int_fast16_t i = 5) {
    coldLevel += i;
    if (coldLevel > 255) {
      coldLevel = 255;

      warmLevel -= i;
      if (warmLevel < 0) {
        warmLevel = 0;
      }
    }

    on = true;
    update();
  }

  int_fast16_t getWarm() {
    return on * (warmLevel / 255) * brightness;
  }

  int_fast16_t getCold() {
    return on * (coldLevel / 255) * brightness;
  }
};

CwwStrip strip1 = CwwStrip(STRIP1A, STRIP1B);

SingleStrip strip3 = SingleStrip(STRIP3A);

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
  pinMode(PSU_ON, OUTPUT);
}

void loop() {
  static int_fast16_t value[ENCODERS];
  static int_fast16_t last[ENCODERS];
  ClickEncoder::Button button;

  if(strip1.isOn() || strip3.isOn()) {
    digitalWrite(PSU_ON, HIGH);
  } else {
    digitalWrite(PSU_ON, LOW);
  }

  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    value[i] += encoders[i] -> getValue();
  }

  for (int_fast16_t i = 0; i < ENCODERS; i += 1) {
    // turn handle
    if (value[i] != last[i]) {
      switch(i) {
        case 0: {
          if (value[i] < last[i]) {
            strip1.darker();
          }

          if (value[i] > last[i]) {
            strip1.brighter();
          }
        } break;
        case 2: {
          if (value[i] < last[i]) {
            strip3.darker();
          }

          if (value[i] > last[i]) {
            strip3.brighter();
          }
        } break;
        case 1: {
          if (value[i] < last[i]) {
            strip1.colder();
          }

          if (value[i] > last[i]) {
            strip1.warmer();
          }
        } break;
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
        case 0: {
          switch(button) {
            case ClickEncoder::Clicked: {
              strip1.toggle();
            } break;
          }
        } break;
        case 2: {
          switch(button) {
            case ClickEncoder::Clicked: {
              strip3.toggle();
            } break;
          } break;
        } break;
        case 1: {
          switch(button) {
            case ClickEncoder::Clicked: {
              strip1.setTemperature();
            } break;
          }
        } break;

        default: {

        } break;
      }
    }
  }
}
