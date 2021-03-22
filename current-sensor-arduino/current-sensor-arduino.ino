const String githubRepo("https://github.com/chrisxkeith/current-sensor-arduino");
const String githubHash("to be filled in after 'git push'");

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1327_EA_W128128_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_inb63_mn);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void drawUTF8(String val) {
  u8g2.firstPage();
  do {
      u8g2_prepare();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawUTF8(0, 0, val.c_str());
  } while( u8g2.nextPage() );
}

void drawInt(int val) {
  u8g2.firstPage();
  do {
      u8g2_prepare();
      u8g2.drawUTF8(30, 10, String(val).c_str());
  } while( u8g2.nextPage() );
}

void setup_OLED() {
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(10, 0);
  digitalWrite(9, 0);
  u8g2.begin();
}

class Sensor {
  private:
    int     pin;
    String  name;
    int     nSamples;
    double  total;

  public:
    Sensor(int pin, String name) {
      this->pin = pin;
      this->name = name;
      clearData();
      pinMode(pin, INPUT);
    }
    
    void sample() {
      if (pin >= A0 && pin <= A5) {
          total += analogRead(pin);
      } else {
          total += digitalRead(pin);
      }
      nSamples++;
      delay(2);
    }
    
    void clearData() {
      nSamples = 0;
      total = 0.0;
    }

    int getValue() {
        return round(total / nSamples);
    }
};

Sensor sensor1(A0,  "Current sensor");

unsigned int lastDisplayInSeconds = 0;
const unsigned int displayIntervalInSeconds = 2;

void display_at_interval() {
  if ((lastDisplayInSeconds + displayIntervalInSeconds) <= (millis() / 1000)) {
    lastDisplayInSeconds = millis() / 1000;
    sensor1.clearData();
    for (int i = 0; i < 100; i++) {
      sensor1.sample();
    }
    int v = sensor1.getValue();
    Serial.println(String(v));
    drawInt(v);
  }
}

void setup(void) {
  Serial.begin(57600);
  Serial.println("Started setup...");
  setup_OLED();
  Serial.println(githubRepo);
  Serial.println(githubHash);
  drawUTF8(githubHash.substring(0,1));
  delay(3000);
  Serial.println("Finished setup...");
}

void loop() {
  display_at_interval();
}
