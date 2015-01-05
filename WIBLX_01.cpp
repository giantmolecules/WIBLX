
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg
===============================================
*/

// This #include statement was automatically added by the Spark IDE.
#include "neopixel.h"
#include "application.h"

#define PIXEL_STRIP_PIN D7
#define PIXEL_SINGLE_PIN D6
#define PIXEL_STRIP_COUNT 16
#define PIXEL_SINGLE_COUNT 16
#define PIXEL_TYPE WS2812B
#define LED_PIN D4

#define DEBUG

#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_STRIP_COUNT, PIXEL_STRIP_PIN, PIXEL_TYPE);
Adafruit_NeoPixel single = Adafruit_NeoPixel(PIXEL_SINGLE_COUNT, PIXEL_SINGLE_PIN, PIXEL_TYPE);

int16_t ax, ay, az;
int16_t gx, gy, gz;
int x_dir, y_dir, z_dir = 0;

int loVal = 0;
int hiVal = 4095;
int minVal = 4095;
int maxVal = 0;
int rawIn = 0;
int analogIn=0;
int analogVal=0;

int mpu_interval = 1500;
long mpu_now = 0;
long mpu_then = 0;

int rf_interval = 1000;
long rf_then = 0;
long rf_now = 0;

int pub_interval = 1000;
long pub_then = 0;
long pub_now = 0;

char mpuXString[3];
char mpuYString[3];
char rfString[3];

bool blinkState = false;

void setup() {

    // START I2C

    Wire.begin();

    // START NEOPIXELS

    strip.begin();
    single.begin();
    strip.show(); // Initialize all pixels to 'off'
    single.show();

    // START SERIAL

    #ifdef DEBUG
    Serial.begin(38400);
    #endif

    // initialize device

    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection

    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);

    // SET TIMERS

    mpu_then = millis();
    rf_then = millis();
    pub_then = millis();
}


void loop() {


// READ MPU

    mpu_now=millis();

    if(mpu_now - mpu_then > mpu_interval){

      accelgyro.getAcceleration(&ax, &ay, &az);

        ax=constrain(ax, -16385, 16385);
        ax = map(ax, -16385, 16385, 0, 359);

        ay=constrain(ay, -16385, 16385);
        ay = map(ay, -16385, 16385, 0, 359);

        mpu_then=millis();

      }

    // READ RF

    rf_now = millis();

    if(rf_now - rf_then > rf_interval)
    {
        rf_then = rf_now;

        rawIn = analogRead(A0);

        if(rawIn > maxVal)
        {
            maxVal = rawIn;
        }

        if(rawIn < minVal)
        {
            minVal = rawIn;
        }

        loVal = constrain(minVal, 0, 4095);
        hiVal = constrain(maxVal, 0, 4095);
        analogIn = constrain(rawIn, loVal, hiVal);
        analogVal = map(analogIn, loVal, hiVal, 0, 255);
    }

    // COLOR PIXELS

    for(int p=0; p<PIXEL_STRIP_COUNT; p++)
    {
        strip.setPixelColor( p, analogVal, 0, 255-analogVal);
        //strip.setPixelColor( p, analogVal, analogVal, analogVal);
    }

    strip.show();

    for(int s=0; s<PIXEL_SINGLE_COUNT; s++)
    {
        single.setPixelColor( s, analogVal, 0, 255-analogVal);
        //single.setPixelColor( s, analogVal, analogVal, analogVal);
    }

    single.show();

    // PUBLISH TO SPARK CLOUD

    pub_now = millis();

    if(pub_now - pub_then > pub_interval){

    sprintf(rfString,"%u", analogVal);
    Spark.publish("analogVal",rfString);

    sprintf(mpuXString,"%u", ax);
    Spark.publish("ax",mpuXString);

    sprintf(mpuYString,"%u", ay);
    Spark.publish("ay",mpuYString);

    pub_then = millis();

  }

  // SERIAL DEBUGGING

  #ifdef DEBUG
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.println("");

  Serial.print("rawIn: ");
  Serial.print(rawIn);
  Serial.print(" loVal: ");
  Serial.print(loVal);
  Serial.print(" hiVal: ");
  Serial.print(hiVal);
  Serial.print(" analogVal: ");
  Serial.println(analogVal);
  #endif

}
