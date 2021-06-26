/*
 * Termoelectric Cooler (TEC) & Heating  
 * Relay H-Bridge Controlled
 * Relay Active LOW
 */

/*
 * Assign pins
 */
#define PIN_RELAY_TEC1 16   //termoelectric hot direction
#define PIN_RELAY_TEC2 17  //termoelectric cool direction
#define PIN_RELAY_FAN1 18  //Fan
#define PIN_RELAY_4    19  //Fan


#define TEC_HEATING 1
#define TEC_COOLING -1
#define TEC_OFF     0
#define RELAY_OFF   HIGH
#define RELAY_ON    LOW

// nilai output TEC
int tec_out;

/*
 * Pin Initialization
 */
void outputInit() {
  pinMode(PIN_RELAY_TEC1, OUTPUT);
  pinMode(PIN_RELAY_TEC2, OUTPUT);
  pinMode(PIN_RELAY_FAN1, OUTPUT);  
  pinMode(PIN_RELAY_4, OUTPUT); 
  
  digitalWrite(PIN_RELAY_TEC1,HIGH);
  digitalWrite(PIN_RELAY_TEC2,HIGH);
  digitalWrite(PIN_RELAY_FAN1,HIGH); 
  digitalWrite(PIN_RELAY_4,HIGH); 
}

/*
 * TEC heating/cooling control function
 * tecDirect: TEC_HEATING, TEC_COOLING, other=off 
 * Relay Active LOW
 */
void outputWrite(int tecDirect){
  tec_out = tecDirect;
  switch(tecDirect){
    case TEC_HEATING: 
      digitalWrite(PIN_RELAY_TEC1,HIGH);
      digitalWrite(PIN_RELAY_TEC2,LOW);
      //digitalWrite(PIN_RELAY_FAN1,LOW);
      break;
    case TEC_COOLING:
      digitalWrite(PIN_RELAY_TEC1,LOW);
      digitalWrite(PIN_RELAY_TEC2,HIGH);    
      //digitalWrite(PIN_RELAY_FAN1,LOW);
      break;
    default:
      digitalWrite(PIN_RELAY_TEC1,HIGH);
      digitalWrite(PIN_RELAY_TEC2,HIGH);
      //digitalWrite(PIN_RELAY_FAN1,HIGH);  
  }
}
void fanWrite(bool i){
  digitalWrite(PIN_RELAY_FAN1,i);
}

/*
 * Baca kembali nilai TEC terakhir
 */
DTYPE outputRead() {
  return tec_out;
}
  
