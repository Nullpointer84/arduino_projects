
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "bsec.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <pthread.h>

#define TFT_CS        14 // Don't care. not actually used.
#define TFT_RST       16 
#define TFT_DC        15 
#define TFT_MOSI      23 
#define TFT_SCLK      18 
#define COLOR_EXCELLENT 0x6EE0
#define COLOR_GOOD 0x9E69
#define COLOR_LIGHELY_POLLUTED 0xFFE0
#define COLOR_MODERATELY_POLLUTED 0xEC21
#define COLOR_HEAVILY_POLLUTED 0xE1A2
#define COLOR_SEVERELY_POLLUTED 0x88E9
#define COLOR_EXTREMELY_POLLUTED 0x59A0

Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const int excellent_max = 50;
const int good_max = 100;
const int lightly_max = 150;
const int moderately_max = 200;
const int heavily_max = 250;
const int severely_max = 350;
const int extremely_max = 351;

const int16_t rectX = 0;
const int16_t rectY = 84;
const int16_t rectWidth = 240;
const int16_t rectHeight = 64;

void checkIaqSensorStatus(void);
struct IaqStruct;

// Create an object of the class Bsec
Bsec iaqSensor;
String iaqSensorCallPath = "";
String output;
unsigned long lastUpload = 0;
const u_int16_t uploadStep = 60000;

String serverName_indoor = "http://192.168.1.11/tempsecret/tempindoor.php";
String serverName_iaq = "http://192.168.1.11/tempsecret/tempiaq.php";

const char* ssid     = "";
const char* password = "";

const uint8_t bsec_config_iaq_bme680_iaq_33v_3s_28d[1974] = 
     {0,0,4,2,189,1,0,0,0,0,0,0,158,7,0,0,176,0,1,0,0,168,19,73,64,49,119,76,0,192,40,72,0,192,40,72,137,65,0,191,205,204,204,190,0,0,64,191,225,122,148,190,10,0,3,0,0,0,96,64,23,183,209,56,0,0,0,0,0,0,0,0,0,0,0,0,205,204,204,189,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,82,73,157,188,95,41,203,61,118,224,108,63,155,230,125,63,191,14,124,63,0,0,160,65,0,0,32,66,0,0,160,65,0,0,32,66,0,0,32,66,0,0,160,65,0,0,32,66,0,0,160,65,8,0,2,0,236,81,133,66,16,0,3,0,10,215,163,60,10,215,35,59,10,215,35,59,13,0,5,0,0,0,0,0,100,254,131,137,87,88,0,9,0,229,208,34,62,0,0,0,0,0,0,0,0,218,27,156,62,225,11,67,64,0,0,160,64,0,0,0,0,0,0,0,0,94,75,72,189,93,254,159,64,66,62,160,191,0,0,0,0,0,0,0,0,33,31,180,190,138,176,97,64,65,241,99,190,0,0,0,0,0,0,0,0,167,121,71,61,165,189,41,192,184,30,189,64,12,0,10,0,0,0,0,0,0,0,0,0,13,5,11,0,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,10,10,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,0,88,1,254,0,2,1,5,48,117,100,0,44,1,112,23,151,7,132,3,197,0,92,4,144,1,64,1,64,1,144,1,48,117,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,48,117,48,117,8,7,8,7,8,7,8,7,8,7,100,0,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,100,0,100,0,255,255,255,255,255,255,255,255,255,255,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,255,255,255,255,255,255,255,255,255,255,112,23,112,23,112,23,112,23,8,7,8,7,8,7,8,7,112,23,112,23,112,23,112,23,112,23,112,23,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,112,23,112,23,112,23,112,23,255,255,255,255,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,5,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,44,1,0,5,10,5,0,2,0,10,0,30,0,5,0,5,0,5,0,5,0,5,0,5,0,64,1,100,0,100,0,100,0,200,0,200,0,200,0,64,1,64,1,64,1,10,0,0,0,0,0,213,52,0,0};

const uint8_t bsec_config_iaq_bme680_iaq_33v_3s_4d[1974] = 
     {0,0,4,2,189,1,0,0,0,0,0,0,158,7,0,0,176,0,1,0,0,192,168,71,64,49,119,76,0,0,97,69,0,0,97,69,137,65,0,191,205,204,204,190,0,0,64,191,225,122,148,190,10,0,3,0,0,0,96,64,23,183,209,56,0,0,0,0,0,0,0,0,0,0,0,0,205,204,204,189,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,128,63,82,73,157,188,95,41,203,61,118,224,108,63,155,230,125,63,191,14,124,63,0,0,160,65,0,0,32,66,0,0,160,65,0,0,32,66,0,0,32,66,0,0,160,65,0,0,32,66,0,0,160,65,8,0,2,0,236,81,133,66,16,0,3,0,10,215,163,60,10,215,35,59,10,215,35,59,13,0,5,0,0,0,0,0,100,35,41,29,86,88,0,9,0,229,208,34,62,0,0,0,0,0,0,0,0,218,27,156,62,225,11,67,64,0,0,160,64,0,0,0,0,0,0,0,0,94,75,72,189,93,254,159,64,66,62,160,191,0,0,0,0,0,0,0,0,33,31,180,190,138,176,97,64,65,241,99,190,0,0,0,0,0,0,0,0,167,121,71,61,165,189,41,192,184,30,189,64,12,0,10,0,0,0,0,0,0,0,0,0,13,5,11,0,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,10,10,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,128,63,0,0,0,88,1,254,0,2,1,5,48,117,100,0,44,1,112,23,151,7,132,3,197,0,92,4,144,1,64,1,64,1,144,1,48,117,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,48,117,48,117,8,7,8,7,8,7,8,7,8,7,100,0,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,100,0,100,0,255,255,255,255,255,255,255,255,255,255,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,255,255,255,255,255,255,255,255,255,255,112,23,112,23,112,23,112,23,8,7,8,7,8,7,8,7,112,23,112,23,112,23,112,23,112,23,112,23,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,112,23,112,23,112,23,112,23,255,255,255,255,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,5,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,44,1,0,5,10,5,0,2,0,10,0,30,0,5,0,5,0,5,0,5,0,5,0,5,0,64,1,100,0,100,0,100,0,200,0,200,0,200,0,64,1,64,1,64,1,10,0,0,0,0,0,21,122,0,0};

uint8_t maxAccuracy = 0;
float avgStaticIaq = 0.0f;
float avgCo2Equivalent = 0.0f;
float avgBreathVocEquivalent = 0.0f;
float avgGasPercentage = 0.0f;

#ifndef IAQ_STRUCT_H
#define IAQ_STRUCT_H
struct IaqStruct {
    uint8_t iaqAccuracy;
    float staticIaq;
    float co2Equivalent;
    float breathVocEquivalent;
    float gasPercentage;
    IaqStruct* next;
    float temperature;
    float humidity;
    float pressure;
};
#endif // IAQ_STRUCT_H

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

IaqStruct* reading = nullptr;
IaqStruct* parsing = nullptr;

void setup(void) {
  Serial.begin(115200);
  Serial.println(millis());
  display.init(240, 240, SPI_MODE3);    
  display.setRotation(90);

  Wire.begin(32, 33);
  iaqSensor.begin(BME68X_I2C_ADDR_LOW, Wire);  
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  iaqSensor.setConfig(bsec_config_iaq_bme680_iaq_33v_3s_4d);
  bsec_virtual_sensor_t sensorList[13] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_STABILIZATION_STATUS,
    BSEC_OUTPUT_RUN_IN_STATUS,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_GAS_PERCENTAGE
  };

  iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
    
  xTaskCreatePinnedToCore(
    coreBME680,      // Function to run on the second core
    "coreBME680",    // Name of the task
    10000,          // Stack size (adjust as needed)
    NULL,           // Parameter to pass to the task
    1,              // Priority of the task
    NULL,           // Task handle
    1               // Core to run the task on (0 or 1)
  );
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    startWifi();
  } 

  IaqStruct* displayResult;
  if(reading != nullptr) {
    pthread_mutex_lock(&myMutex);
    parsing = reading;
    reading = nullptr;
    pthread_mutex_unlock(&myMutex);

    if(parsing != nullptr) {
      String cleanHWId = WiFi.macAddress();
      cleanHWId.replace(":", "");

      String avg = calculateAverage(parsing);
      String callPath =   
          "?hwid=" + cleanHWId + avg;
      HTTPClient http;
      callPath.replace(".", "V");
      String serverPath = serverName_iaq + callPath;
      Serial.println(serverPath);
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();
      http.end();

      displayResult = new IaqStruct {maxAccuracy, avgStaticIaq, avgCo2Equivalent, avgBreathVocEquivalent, avgGasPercentage, nullptr, iaqSensor.temperature, iaqSensor.humidity, iaqSensor.pressure / 100.0F};
    }
  } else {
    Serial.print("reading* is empty at ");
    Serial.println(millis());
    displayResult = new IaqStruct{0, 0, 0, 0, 0, nullptr, iaqSensor.temperature, iaqSensor.humidity, iaqSensor.pressure / 100.0F};  
  }
  
  printIaqStatus(displayResult);

  float temperature = iaqSensor.temperature;
  float humidity = iaqSensor.humidity;
  float pressure = iaqSensor.pressure / 100.0F;
  HTTPClient http;
  String cleanHWId = WiFi.macAddress();
  cleanHWId.replace(":", "");
  String callPath = 
    "?hwid=" + cleanHWId + 
    "&temp=" + temperature + 
    "&humid=" + humidity + 
    "&pressure=" + pressure;
  callPath.replace(".", "V");

  String serverPath = serverName_indoor + callPath;
  http.begin(serverPath.c_str());

  int httpResponseCode = http.GET();
  http.end();
  delay(60000);
}



void deleteList(IaqStruct* head) {
    while (head != nullptr) {
        IaqStruct* temp = head;
        head = head->next;
        delete temp;
    }
}

String calculateAverage(IaqStruct* head) {
    // Initialize accumulators
    maxAccuracy = 0;
    avgStaticIaq = 0.0f;
    avgCo2Equivalent = 0.0f;
    avgBreathVocEquivalent = 0.0f;
    avgGasPercentage = 0.0f;

    int count = 0;

        // Find the maximum iaqAccuracy
    IaqStruct* currentMax = head;
    while (currentMax != nullptr) {
        maxAccuracy = std::max(maxAccuracy, currentMax->iaqAccuracy);
        currentMax = currentMax->next;
    }

    // Iterate through the list
    IaqStruct* current = head;
    Serial.println("----");
    while (current != nullptr) {
      // Use only entries with the maximum iaqAccuracy
      if (current->iaqAccuracy == maxAccuracy) {
        // Accumulate values
        Serial.print("iaqAccuracy ");
        Serial.print(current->iaqAccuracy);
        Serial.print(" ");
        Serial.print("staticIaq ");
        Serial.println(current->staticIaq);

        avgStaticIaq += current->staticIaq;
        avgCo2Equivalent += current->co2Equivalent;
        avgBreathVocEquivalent += current->breathVocEquivalent;
        avgGasPercentage += current->gasPercentage;

        count++;
      }
      // Move to the next node
      current = current->next;
    }
    Serial.println("----");

    // Calculate averages
    avgStaticIaq /= count;
    avgCo2Equivalent /= count;
    avgBreathVocEquivalent /= count;
    avgGasPercentage /= count;

    deleteList(head);

    String result;
    result += "&accuracy=" + String(static_cast<int>(maxAccuracy)) +
              "&iaq_static=" + String(avgStaticIaq) +
              "&co2=" + String(avgCo2Equivalent) +
              "&breath_voc=" + String(avgBreathVocEquivalent) +
              "&gas_percentage=" + String(avgGasPercentage);

    return result;
}

void printIaqStatus(IaqStruct* iaq) {
  display.setCursor(0,0);
  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(2);

  if(iaq->iaqAccuracy==0) {
    display.setTextColor(ST77XX_RED);
  } else if(iaq->iaqAccuracy<2) {
    display.setTextColor(ST77XX_YELLOW);
  } else {
    display.setTextColor(ST77XX_WHITE);
  }

  display.print("Accuracy ");
  display.println(iaq->iaqAccuracy);

  display.setTextSize(2);
  display.setTextColor(ST77XX_WHITE);
  display.print("Temp ");
  display.println(iaq->temperature);

  
  display.print("Humid ");
  display.println(iaq->humidity);

  display.print("Pressure ");
  display.println(iaq->pressure);

  display.println("");
  display.println("");

  display.fillRect(rectX, rectY, rectWidth, rectHeight, getIaqBackGroundColor(iaq->staticIaq));
  display.drawRect(rectX, rectY, rectWidth, rectHeight, getIaqForeGroundColor(iaq->staticIaq));
  display.setTextSize(3);
  display.setTextColor(getIaqForeGroundColor(iaq->staticIaq));
  display.print(" Iaq ");
  display.println(iaq->staticIaq);
}

uint16_t getIaqBackGroundColor(float staticIaq) {
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

uint16_t getIaqForeGroundColor(float staticIaq) {
  uint16_t targetColor;
  if(staticIaq>0 && staticIaq<excellent_max) {
    targetColor = ST77XX_BLACK;
  } else if(staticIaq>excellent_max && staticIaq<good_max) {
    targetColor = ST77XX_BLACK;
  } else if(staticIaq>good_max && staticIaq<lightly_max) {
    targetColor = ST77XX_BLACK;
  } else if(staticIaq>lightly_max && staticIaq<moderately_max) {
    targetColor = ST77XX_BLACK;
  } else if(staticIaq>moderately_max && staticIaq<heavily_max) {
    targetColor = ST77XX_BLACK;
  } else if(staticIaq>heavily_max && staticIaq<severely_max) {
    targetColor = ST77XX_WHITE;
  } else {
    targetColor = ST77XX_WHITE;
  }
  return targetColor;  
}
// Helper function definitions
void checkIaqSensorStatus(void) {
  if (iaqSensor.bsecStatus != BSEC_OK) {
    if (iaqSensor.bsecStatus < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
    } else {
      output = "BSEC warning code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme68xStatus != BME68X_OK) {
    if (iaqSensor.bme68xStatus < BME68X_OK) {
      output = "BME68X error code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
    } else {
      output = "BME68X warning code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
    }
  }
}

void coreBME680(void *parameter) {
  Serial.println("Starting coreBME680 Thread");
  while (1) {
    if(iaqSensor.run()) {
      if(iaqSensor.iaqAccuracy>0) {
        pthread_mutex_lock(&myMutex);

        IaqStruct* newNode = new IaqStruct{iaqSensor.iaqAccuracy, iaqSensor.staticIaq, iaqSensor.co2Equivalent, iaqSensor.breathVocEquivalent, 
                                  iaqSensor.gasPercentage, nullptr, iaqSensor.temperature, iaqSensor.humidity, iaqSensor.pressure};
        // Insert at the beginning of the list
        newNode->next = reading;
        reading = newNode;

        pthread_mutex_unlock(&myMutex);
        Serial.print("iaqSensor.iaqAccuracy ");
        Serial.println(iaqSensor.iaqAccuracy);
      } else {
        Serial.print("iaqSensor.iaqAccuracy too low at ");
        Serial.println(millis());
      }
    } else {
      checkIaqSensorStatus();
    }
    delay(3000);
  }
}

void startWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(100);
  // Will try for about 10 seconds (20x 500ms)
    int tryDelay = 500;
    int numberOfTries = 20;


    // Wait for the WiFi event
    while (true) {
        switch(WiFi.status()) {
          case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not found");
            break;
          case WL_CONNECT_FAILED:
            Serial.println("[WiFi] Failed - WiFi not connected!");
            return;
            break;
          case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection was lost");
            break;
          case WL_SCAN_COMPLETED:
            Serial.println("[WiFi] Scan is completed");
            break;
          case WL_DISCONNECTED:
            Serial.print("[WiFi] WiFi is disconnected ");
            Serial.println(numberOfTries);
            break;
          case WL_CONNECTED:
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
            return;
            break;
          default:
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
            break;
        }
        delay(tryDelay);
        
        if(numberOfTries <= 0){
          WiFi.disconnect();
          return;
        } else {
          numberOfTries--;
        }
    }
}