/*
 * Control Panel
 * LCD liquid crystal dan 4 tombol
 */
#include <LiquidCrystal.h>
/*
 * Assign pins
 */
#define bBack     33
#define bSelect   34
#define bCLK      35
#define bDT       32
LiquidCrystal lcd(25, 26, 27, 14, 13, 2); //rs, en, d4,d5,d6,d7

void panelInit() {
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  pinMode(bSelect, INPUT);
  pinMode(bBack, INPUT);
  pinMode(bCLK, INPUT);
  pinMode(bDT, INPUT);
}

int panelRead() {
  int button = 0; //1 select, 2 back, 3 up, 4 down
  static int lastStateCLK = 0;
  int currentStateCLK = digitalRead(bCLK);

  //return value according to button that is readed
  if (digitalRead(bSelect)) {button=1;}
  else if (digitalRead(bBack)) {button=2;}
  //Handling for the Rotary Encoder, reff:https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/#:~:text=SD%20Card%20Module-,How%20Rotary%20Encoder%20Works%20and%20Interface%20It%20with%20Arduino,the%20knob%20is%20being%20rotated.
  else if (currentStateCLK != lastStateCLK && currentStateCLK ==1){
    Serial.print("current: ");
    Serial.print(currentStateCLK);
    Serial.print(" last: ");
    Serial.print(lastStateCLK);
    if (digitalRead(bDT) != currentStateCLK) {
      button = 4;  
    } else {
      button = 3;
    }
    Serial.print(" button: ");
    Serial.println(button);
  }
  lastStateCLK = currentStateCLK;
//  if(button != buttonLast){
//    buttonLast = button;
//    return  button;
//  }
  return button;
}
