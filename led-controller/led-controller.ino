#include <SoftwareSerial.h>

#define ENCODERS          4

#define STRIP1A           3

#define STRIP2A           6
#define STRIP2B           9

#define STRIP3A           5

#define PSU_ON            A3

#define BUTTON_CLICKED    1

SoftwareSerial inputSerial(A5, A4);

class LedStrip {
  protected:
  int_fast16_t brightness;
  bool on;
  float animationDelta;
  float animationMultiplayer = 1;

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

  void brighter (int_fast16_t i = 20) {
    this -> setBrightness(brightness + i);
    on = true;
    this -> update();
  }

  void darker (int_fast16_t i = 20) {
    this -> setBrightness(brightness - i);
    on = true;
    this -> update();
  }

  void toggle() {
    this -> transition(!on);
    if (!on) {
      on = true;
    }
  }

  int_fast16_t getBrightness() {
    return brightness;
  }

  bool isOn() {
    return on;
  }

  void animationService () {
    if (animationDelta == 0) return;
    if (animationMultiplayer >= 1 || animationMultiplayer <= 0) {
      if (animationDelta < 0) {
        on = !on;
      }
      animationDelta = 0;
      animationMultiplayer = 1;
      return;
    }
    animationMultiplayer += animationDelta;
    // Serial.print(animationMultiplayer);
    // Serial.print(" ");
    // Serial.println(animationDelta);
    this -> update();
  }

  void transition (bool direction, float delta = 0.02) {
    animationDelta = (direction ? 1.0 : (-1.0)) * delta;
    animationMultiplayer = direction ? 0.00001 : 0.99999;
  }
};

class SingleStrip: public LedStrip {
  int_fast16_t pin;

public:
  SingleStrip(int_fast16_t pin, int_fast16_t brightness = 255, bool on = false): LedStrip(brightness, on) {
    this -> pin = pin;
    // Serial.println("b");
    // Serial.println("b");
    update();
  }

  void update() {
    {
      // Serial.print((int)on);
      // Serial.print(" ");
      // Serial.print(brightness);
      // Serial.print(" ");
      // Serial.print(pin);
      // Serial.print(" ");
      // Serial.print(animationMultiplayer);
      // Serial.print(" ");
      // Serial.println(on * brightness * animationMultiplayer);
    }
    analogWrite(pin, on * brightness * animationMultiplayer);
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
      // Serial.print("On: ");
      // Serial.print((int)on);
      // Serial.print(" B: ");
      // Serial.print(brightness);
      // Serial.print(" A: ");
      // Serial.print(pinA);
      // Serial.print(" B: ");
      // Serial.print(pinB);
      // Serial.print(" W: ");
      // Serial.print(warmLevel);
      // Serial.print(" C: ");
      // Serial.print(coldLevel);
      // Serial.print(" : ");
      // Serial.print(animationMultiplayer);
      // Serial.print(" : ");
      // Serial.print((int)(on * ((float)warmLevel / 255.0) * brightness));
      // Serial.print(" : ");
      // Serial.println((int)(on * ((float)coldLevel / 255.0) * brightness));
    }
    analogWrite(pinA, (int)(on * ((float)warmLevel / 255.0) * brightness * animationMultiplayer));
    analogWrite(pinB, (int)(on * ((float)coldLevel / 255.0) * brightness * animationMultiplayer));
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

SingleStrip strip1 = SingleStrip(STRIP1A);

SingleStrip strip3 = SingleStrip(STRIP3A);

void setup() {
  Serial.begin(9600);
  inputSerial.begin(9600);

  pinMode(PSU_ON, OUTPUT);
}

void requestEvent(int howMany) {
    if (howMany >= 4) {
        byte encoder = inputSerial.read();
        if (encoder == 0xff) return;
        byte action = inputSerial.read();
        if (action == 0xff) return;
        byte attr = inputSerial.read();
        if (attr == 0xff) return;
        byte end = inputSerial.read();

        switch(action) {
            case 1: { // encoder
                switch(encoder) {
                    case 0: {
                        if (attr == 0) {
                          strip1.brighter();
                        }

                        if (attr == 1) {
                          strip1.darker();
                        }
                    } break;

                    case 1: {

                    } break;

                    case 2: {
                    } break;

                    case 3: {
                        if (attr == 0) {
                          strip3.brighter();
                        }

                        if (attr == 1) {
                          strip3.darker();
                        }
                    } break;
                }
            } break;
            case 2: { // button
                switch(encoder) {
                    case 0: {
                        switch(attr) {
                            case BUTTON_CLICKED: {
                                strip1.toggle();
                            } break;
                        }
                    } break;

                    case 1: {
                        switch(attr) {
                            case BUTTON_CLICKED: {

                            } break;
                        }
                    } break;

                    case 2: {
                        switch(attr) {
                            case BUTTON_CLICKED: {

                            } break;
                        } break;
                    } break;

                    case 3: {
                        switch(attr) {
                            case BUTTON_CLICKED: {
                                strip3.toggle();
                            } break;
                        } break;
                    } break;
                }
            }
        }
    }
}

void loop() {
    if (strip1.isOn() || strip3.isOn()) {
        digitalWrite(PSU_ON, HIGH);
    } else {
        digitalWrite(PSU_ON, LOW);
    }
    strip1.animationService();
    strip3.animationService();
    if (inputSerial.available()) {
      requestEvent(inputSerial.available());
    }
    delay(10);
}
