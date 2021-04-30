#include "main.h"
#include"ledMachine.h"

// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V10);

#define renderCore 0
static TaskHandle_t renderIntensityThread;
static TaskHandle_t renderColorThread;


LedMachine *LavidaLeda=nullptr;

void renderIntensityTask( void * pvParameters ) {
    for (;;) { 
        LavidaLeda->processIntensity();
    }
    vTaskDelete( NULL );
}

void renderColorTask( void * pvParameters ) {
    for (;;) { 
        LavidaLeda->processColor();
    }
    vTaskDelete( NULL );
}

void init_LedMachine(){
    xTaskCreatePinnedToCore(
    renderIntensityTask,   /* Task function. */
    "renderIntensityTask",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */ // MAX 12 (highest)
    &renderIntensityThread,      /* Task handle to keep track of created task */
    renderCore);          /* pin task to core 0 */
    
    xTaskCreatePinnedToCore(
    renderColorTask,   /* Task function. */
    "renderColorTask",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */ // MAX 12 (highest)
    &renderColorThread,      /* Task handle to keep track of created task */
    renderCore);          /* pin task to core 0 */
}

void setup() {
  Serial.begin(115200);
//   delay(3000); // 3 second delay for recovery
  pinMode(prog,INPUT);
  pinMode(LED,OUTPUT);

  LavidaLeda = new LedMachine;
  init_LedMachine(); //INITIALLIZE LED (2)THREADS COLOR / INTENSITY

  MultyWiFiBlynkBegin(); // INITIALIZE Internet Connection, OTA and UI (1)Thread
  setupOTA();
  terminal.clear();
	terminal.print("\nWiFi connected with IP ");
	terminal.println(ip);
}

void parseInput()
{
  long startTime = millis();

  String outMsg = String("UI states Pre: "+LavidaLeda->getIntensityStateString()+" "+LavidaLeda->getColorStateString());
  // Serial.println(outMsg);
  // terminal.println(outMsg);

  //Intensity State
  if (ONOFF == HIGH)
    LavidaLeda->setIntensityState(MUSIC == HIGH ? IntensityState::MUSIC : IntensityState::BRIGHTNESS);
  else  LavidaLeda->setIntensityState(IntensityState::OFF);
  //Color State
  LavidaLeda->setColorState(color == HIGH ? ColorState::COLOR : ColorState::PROGRAM);

  long duration = millis() - startTime;

  outMsg = String("UI states Post:"+LavidaLeda->getIntensityStateString()+" "+LavidaLeda->getColorStateString()+" "+String(duration));
  Serial.println(outMsg);
  terminal.println(outMsg);
}

void loop()
{
  // TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  // TIMERG0.wdt_feed = 1;
  // TIMERG0.wdt_wprotect = 0;
  Blynk.run();
  // parseInput();
}

//===========BLYNK FUNCTIONS ==========
// This function will run every time Blynk connection is established
// and sync all variables to server
BLYNK_CONNECTED() {
  Blynk.syncAll();
}
BLYNK_WRITE(V0){
    prog = param.asInt();
      parseInput();

}
BLYNK_WRITE(V1){
    ONOFF = param.asInt();
    parseInput();
}
///////////===OTA===////////////////////////
BLYNK_WRITE(V2)
{
	if(param.asInt() == 1){
    terminal.println("OTA enabled");
    // if(renderIntensityThread!=NULL)vTaskDelete( renderIntensityThread );
    // if(renderColorThread!=NULL)vTaskDelete( renderColorThread );
    enableOTA();
    terminal.println("OTA enabled");

  }
}
BLYNK_WRITE(V3)
{
	if(param.asInt() == 1){
    terminal.println("Reset Device");
    resetDevice();
  }
}
////////////////////////////////////////////
BLYNK_WRITE(V4){
    color = param.asInt();
      parseInput();
}
BLYNK_WRITE(V5){
    BRIGHT = param.asInt();
    BRIGHT = map(BRIGHT, 0, 1023, 0, 255);
      parseInput();
}
BLYNK_WRITE(V6){
    inMUSIC = param.asInt();
    parseInput();
}
BLYNK_WRITE(V7)
{
    volumeOffset = param.asDouble();
}
BLYNK_WRITE(V8)
{
    fadeOffset = param.asDouble();
}
BLYNK_WRITE(V9){
    r = param[0].asInt();
    g = param[1].asInt();
    b = param[2].asInt();
}







