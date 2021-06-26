//struct interval itv_test;


void setup() {
  srInit();
  Serial.println(TITLE);
  
  DS18Init();
  outputInit();
  panelInit();
  //wifiInit();
  controlInit();
  menuInit();
  
  //intervalInit(itv_test, 500);
  
}

void loop(){
  menuLoop();
  controlLoop();
}
