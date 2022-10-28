/*
    Description: Control RGB LED to run rainbow light show
    Please install library before compiling:
    FastLED: https://github.com/FastLED/FastLED
*/
#include <M5Stack.h>

#include "FastLED.h"

#define Neopixel_PIN 3
#define NUM_LEDS     72

CRGB leds[NUM_LEDS];
uint8_t gHue                              = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle        = 0;

void setup() {
    M5.begin();
    M5.Power.begin();

    // Neopixel initialization
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(5);
    xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2,
                            NULL, 1);
}

void loop() {
}

void FastLEDshowTask(void *pvParameters) {
    for (;;) {
        fill_rainbow(leds, NUM_LEDS, gHue, 7);
        //fill_solid(leds, NUM_LEDS, CRGB(200,0,0));  // rainbow effect
        FastLED.show();  // must be executed for neopixel becoming effective
        EVERY_N_MILLISECONDS(20) {
            gHue++;
        }
    }
}