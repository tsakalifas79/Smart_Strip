#include "main.h"


    float sample, value, envelope, beat, thresh;
    unsigned char i;
    int ret = 0;

    int sampling = 200;


// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 9.1f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample / 160.f;
    yv[0] = yv[1]; 
    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 7.015f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
}


int detect() {
    unsigned long time = micros(); // Used to track rate

    ret = 0;
    for(i = 0;i<sampling;++i){
        // Read ADC and center so +-512
        sample = pow((float)analogRead(A0),volumeOffset);//-503.f;

        if(sample > maxSample) maxSample = sample;
    
        // Filter only bass component
        value = bassFilter(sample);
    
        // Take signal amplitude and filter
        if(value < 0)value=-value;
        envelope = envelopeFilter(value);
        ret += envelope;
    
        // Every 200 samples (25hz) filter the envelope 
        if(i == sampling) {
                // Filter out repeating bass sounds 100 - 180bpm
                beat = beatFilter(envelope);
//                
//        
//                // Threshold it based on potentiometer on AN1
//                thresh = 0.02f * 100;//(float)analogRead(1);
//                
//                // If we are above threshold, light up LED
//                if(beat > thresh) ret = 1000;
//                else ret = 0;
//        
//                //Reset sample counter
//                i = 0;

//         if(autoGain) 
         volumeOffset = log(255)/log(maxSample);
        ret /= sampling;
//        rms = ret/pow(2,1/2);
//        ret = ret - rms;

//        ret -= beat;
        }
    
        // Consume excess clock cycles, to keep at 5000 hz
        for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
    }

    return ret;  
}
