#ifndef LedMachine_H
#define LedMachine_H

#include "main.h"
#include"audioProcessing.h"
#include "ledProgramms.h"

enum IntensityState {BRIGHTNESS, MUSIC, OFF};
enum ColorState {COLOR, PROGRAM};

class LedMachine
{
public: 
    LedMachine(){
        intensitySema = xSemaphoreCreateBinary();
        colorSema = xSemaphoreCreateBinary(); 
        digitalWrite(LED,HIGH);
        /////FILTER AUDIO NOISE  
        // wifi_set_sleep_type(NONE_SLEEP_T);
        ////////////
        // tell FastLED about the LED strip configuration
        FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
        FastLED.clear(); 
        FastLED.setBrightness(0);
        /////FILTER AUDIO NOISE   
        // wifi_set_sleep_type(NONE_SLEEP_T);
        ////////////
        digitalWrite(LED,LOW);
    }
    virtual ~LedMachine(){}

    //Getters / Setters
    void setIntensityState(IntensityState stateIntensityIn){ 
        if(stateIntensity != stateIntensityIn){
            if (xSemaphoreTake(intensitySema, portMAX_DELAY) == pdPASS)
                stateIntensity = stateIntensityIn; 
            xSemaphoreGive(intensitySema);
        }
    }
    void setColorState(ColorState stateColorIn){ 
        if (stateColor != stateColorIn){
            if (xSemaphoreTake(colorSema, portMAX_DELAY) == pdPASS)
                stateColor = stateColorIn; 
            xSemaphoreGive(colorSema);
        }
    }
    IntensityState getIntensityState(){
        return stateIntensity; 
    }
    ColorState getColorState(){ 
        return stateColor; 
    }

    String getIntensityStateString(){
        if(stateIntensity == MUSIC) return "MUSIC"; 
        else if(stateIntensity == BRIGHTNESS) return "BRIGHTNESS"; 
        else if(stateIntensity == OFF) return "OFF"; 
        
    }
    String getColorStateString(){ 
        if(stateColor == COLOR) return "COLOR"; 
        else if(stateColor == PROGRAM) return "PROGRAM"; 
    }

    virtual bool processIntensity(){
        FastLED.show(); 
        FastLED.delay(1000/FRAMES_PER_SECOND);//  Delay for framerate (ms)
        xSemaphoreTake(intensitySema,0);
        switch (getIntensityState())
        {
            case IntensityState::BRIGHTNESS:
                stateBRIGHTNESS();
                break;
            case IntensityState::MUSIC:
                stateMUSIC();
                break;
            case IntensityState::OFF:
                stateOFF();
                break;
            default:
                break;
        }
        xSemaphoreGive(intensitySema);
        FastLED.setBrightness(fade);
        return true;
    }

    virtual bool processColor(){
        FastLED.show(); 
        FastLED.delay(1000/FRAMES_PER_SECOND);//  Delay for framerate (ms)
        EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
        xSemaphoreTake(colorSema,0);
        switch (getColorState())
        {
            case ColorState::COLOR:
                stateCOLOR();
                break;
            case ColorState::PROGRAM:
                statePROGRAM();
                break;
            default:
                break;
        }
        xSemaphoreGive(colorSema);
        return true;
    }
private:
    IntensityState stateIntensity = IntensityState::OFF;
    ColorState stateColor = ColorState::COLOR;
    SemaphoreHandle_t  intensitySema; 
    SemaphoreHandle_t  colorSema; 
    uint8_t fade = 255;
    ledProgramms ledProgram;
    // audioProcessing audioProcessor;
    
    void stateBRIGHTNESS(){
        if (fade < BRIGHT){ EVERY_N_MILLISECONDS(10) {fade++;} }
        if (fade > BRIGHT){ EVERY_N_MILLISECONDS(10) {fade--;} }
    }

    void stateMUSIC(){
        led = 0;
        br = detect();
        if (br > 240 && fade < br){ fade+=50/fadeOffset; }
        else if (br > 160 && fade < br){  fade+=20/fadeOffset; }
        else if (br > 30){
            if (fade < br)  fade+=5/fadeOffset;
            if (fade > br && fade > 100 )fade-=20/fadeOffset;
            else if (fade > br) fade-=5/fadeOffset;
        }

        if (br <= 10)fade = 0;
        else if (br <= 30){
            if (fade < br)fade++;
            if (fade > br)fade--;
        }
        if (fade <=0) fade=0;
        fade = br;
    }

    //!TODO: Remove input parsers. Replace with state checks
    //       Remove FastLED.show(); 
    void stateCOLOR(){
        if (rr != r) rr = rr < r ? rr+1 : rr-1;
        if (gg != g) gg = gg < g ? gg+1 : gg-1;
        if (bb != b) bb = bb < b ? bb+1 : bb-1;

        if (prog == HIGH && pr && change == 0){
            change = 1;
            pr = false;
        }
        if (prog == HIGH && pr && change == 1){
            change = 0;
            pr = false;
        }
        if (prog == LOW && pr == false){
            pr = true;
        }
        if (change == 0){
            FastLED.clear(); 
            fill_solid(leds, NUM_LEDS, CRGB(rr, gg, bb));
        }
        if (change == 1){
            FastLED.clear(); 
            ledProgram.addGlitter(100);
            rr=0;gg=0;bb=0;
        }
        FastLED.show();  
    }

    //!TODO: Remove input parsers. Replace with state checks
    //       Remove FastLED.show(); 
    void statePROGRAM(){
        if (prog == HIGH && pr){
            ledProgram.nextPattern(); 
            pr = false;
        }
        if (prog == LOW && pr == false){
            pr = true;
        }
        FastLED.show();  
        // Call the current pattern function once, updating the 'leds' array
        ledProgram.gPatterns[gCurrentPatternNumber]();
    }

    void stateOFF(){
        // if (fade > 0) EVERY_N_MILLISECONDS(5){fade--;}
        if (fade == 0)led = 0;
        EVERY_N_MILLISECONDS(10) {
            if (led < (NUM_LEDS/2)+1){
                leds[led] = CRGB::Black; 
                leds[NUM_LEDS-led] = CRGB::Black; 
                FastLED.show();  
                led++; 
            }
        }
        FastLED.setBrightness(fade);
    }
};




// void renderColorTask( void * pvParameters ) {
//     for (;;) {
//         processColor();
//     }
//     vTaskDelete( NULL );
// }
// virtual bool processColor(){
//         FastLED.show(); 
//         // insert a delay to keep the framerate modest
//         FastLED.delay(1000/FRAMES_PER_SECOND); 
            
//         // do some periodic updates MAYBE...
//         EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
        
//         if( color == HIGH) stateCOLOR();
//         else statePROGRAM();

//         FastLED.setBrightness(fade);
//         return true;
//     }
#endif // LedMachine_H