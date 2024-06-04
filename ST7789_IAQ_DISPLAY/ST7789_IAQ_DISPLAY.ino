
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


#define TFT_CS        14 // Don't care. not actually used.
#define TFT_RST       17
#define TFT_DC        15
#define TFT_MOSI      23
#define TFT_SCLK      18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define COLOR_EXCELLENT 0x6EE0
#define COLOR_GOOD 0x9E69
#define COLOR_LIGHELY_POLLUTED 0xFFE0
#define COLOR_MODERATELY_POLLUTED 0xEC21
#define COLOR_HEAVILY_POLLUTED 0xE1A2
#define COLOR_SEVERELY_POLLUTED 0x88E9
#define COLOR_EXTREMELY_POLLUTED 0x59A0

const int excellent_max = 50;
const int good_max = 100;
const int lightly_max = 150;
const int moderately_max = 200;
const int heavily_max = 250;
const int severely_max = 350;
const int extremely_max = 351;

#ifndef IAQ_STRUCT_H
#define IAQ_STRUCT_H
struct IaqStruct {
    uint8_t iaqAccuracy;
    float staticIaq;
    float temperature;
    float humidity;
    float pressure;
};
#endif // IAQ_STRUCT_H

IaqStruct* firstAccuracySample = new IaqStruct{1, 49.3, 20.3, 37, 1012.0};
IaqStruct* secondAccuracySample = new IaqStruct{2, 101.32, 23.3, 50, 900.0};
IaqStruct* activeAccuracySample = new IaqStruct{3, 160.5, 19.5, 10, 1200};
IaqStruct* severelyAccuracySample = new IaqStruct{3, 280, 19.5, 10, 1200};
IaqStruct* maxAccuracySample = new IaqStruct{3, 400, 19.5, 10, 1200};

void setup(void) {
  Serial.begin(9600);
  tft.init(240, 240, SPI_MODE3);    
  tft.setRotation(90);
}

void loop() {
  printIaqStatus(firstAccuracySample);
  delay(2000);
  printIaqStatus(secondAccuracySample);
  delay(2000);
  printIaqStatus(activeAccuracySample);
  delay(2000);
  printIaqStatus(severelyAccuracySample);
  delay(2000);
  printIaqStatus(maxAccuracySample);
  delay(2000);
}

void printIaqStatus(IaqStruct* iaq) {
  tft.setCursor(0,0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);

  if(iaq->iaqAccuracy>2) {
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.setTextColor(ST77XX_YELLOW);
  }

  tft.print("Accuracy ");
  tft.println(iaq->iaqAccuracy);

  tft.setTextSize(3);
  setIaqColor(iaq->staticIaq);
  tft.print("Iaq ");
  tft.println(iaq->staticIaq);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Temp ");
  tft.println(iaq->temperature);

  
  tft.print("Humid ");
  tft.println(iaq->humidity);

  tft.print("Pressure ");
  tft.println(iaq->pressure);

  tft.fillRect(0, 96, 240, 84, getIaqColor(iaq->staticIaq));
}

uint16_t getIaqColor(float staticIaq) {
  uint16_t targetColor;
  if(staticIaq>0 && staticIaq<excellent_max) {
    targetColor = COLOR_EXCELLENT;
  } else if(staticIaq>excellent_max && staticIaq<good_max) {
    targetColor = COLOR_GOOD;
  } else if(staticIaq>good_max && staticIaq<lightly_max) {
    targetColor = COLOR_LIGHELY_POLLUTED;
  } else if(staticIaq>lightly_max && staticIaq<moderately_max) {
    targetColor = COLOR_MODERATELY_POLLUTED;
  } else if(staticIaq>moderately_max && staticIaq<heavily_max) {
    targetColor = COLOR_HEAVILY_POLLUTED;
  } else if(staticIaq>heavily_max && staticIaq<severely_max) {
    targetColor = COLOR_SEVERELY_POLLUTED;
  } else {
    targetColor = COLOR_EXTREMELY_POLLUTED;
  }
  return targetColor;  
}

void setIaqColor(float staticIaq) {
  tft.setTextColor(getIaqColor(staticIaq));
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
}

