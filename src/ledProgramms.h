#include "main.h"
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))  

typedef void (*SimplePatternList[6])();
// SimplePatternList gPatterns;//= { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
    static uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
    static uint8_t gHue = 0; // rotating "base color" used by many of the patterns

class ledProgramms
{
public:
    int numPatterns = 6;
    // List of patterns to cycle through.  Each is defined as a separate function below.
    const SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

    ledProgramms(){
        gHue = 0;
        gCurrentPatternNumber = 0;
        // gPatterns[0] = rainbow;
        // gPatterns[1] =  rainbowWithGlitter;
        // gPatterns[2] =  confetti;
        // gPatterns[3] =  sinelon;
        // gPatterns[4] =  juggle; 
        // gPatterns[5] =  bpm ;
    }
    virtual ~ledProgramms(){}
    
    
    static void rainbow() 
    {
        // FastLED's built-in rainbow generator
        fill_rainbow( leds, NUM_LEDS, gHue, 7);
    }

    static void rainbowWithGlitter() 
    {
        // built-in FastLED rainbow, plus some random sparkly glitter
        rainbow();
        addGlitter(80);
    }

    static void addGlitter( fract8 chanceOfGlitter) 
    {
        if( random8() < chanceOfGlitter) {
            leds[ random16(NUM_LEDS) ] += CRGB::White;
        }
    }

    static void confetti() 
    {
        // random colored speckles that blink in and fade smoothly
        fadeToBlackBy( leds, NUM_LEDS, 10);
        int pos = random16(NUM_LEDS);
        leds[pos] += CHSV( gHue + random8(64), 200, 255);
    }

    static void sinelon()
    {
        // a colored dot sweeping back and forth, with fading trails
        fadeToBlackBy( leds, NUM_LEDS, 20);
        int pos = beatsin16( 13, 0, NUM_LEDS-1 );
        leds[pos] += CHSV( gHue, 255, 192);
    }

    static void juggle() {
        // eight colored dots, weaving in and out of sync with each other
        fadeToBlackBy( leds, NUM_LEDS, 20);
        byte dothue = 0;
        for( int i = 0; i < 8; i++) {
            leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
    }

    static void nextPattern()
    {
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
    }

    static void bpm()
    {
        // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
        uint8_t BeatsPerMinute = fadeOffset;//62;
        CRGBPalette16 palette = PartyColors_p;
        uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
        for( int i = 0; i < NUM_LEDS; i++) { //9948
            leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
        }
    }

    
    
};