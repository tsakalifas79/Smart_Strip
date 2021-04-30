#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "wifiKeys.h"
#include "ota.h"

#include <FastLED.h>
#include <Wire.h>


// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200


int prog = LOW;
int ONOFF = LOW;
int inMUSIC = LOW;
bool pr = false;
int color = LOW;
int r,g,b,rr,bb,gg;
int BRIGHT;
int br;
int temp = 0;
int change = 0;
double volumeOffset = 0;
int autoGain = 1;
float maxSample;

int oldDetect; 
int newDetect;
float rms;

double fadeOffset = 2;

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    120
CRGB leds[NUM_LEDS];
int led = 0;

#define FRAMES_PER_SECOND  240

#define LED 2

#endif // MAIN_H