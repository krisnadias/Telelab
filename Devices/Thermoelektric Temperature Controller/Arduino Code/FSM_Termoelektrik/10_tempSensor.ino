/*
 * Temperature Sensor Module using Dallas Temperature Sensor and OneWire Connection
 * Refference: https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/
 * not compatble if there are onewire sensors other than temperature sensors
 * 
 * Akurasi 0.5C
 */

/*
 * Assign pins and setup library for Dallas-onewire sensors
 */
#include <DallasTemperature.h>
#include <OneWire.h>
#include "nonBlockingDS18B20.h"

#define ONE_WIRE_BUS 23
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
nonBlockingDS18B20 tempSensors(&oneWire);
DeviceAddress tempDeviceAddress;  // We'll use this variable to store a found device address


int numDS18; // Number of oneWire (temperature) devices
const uint8_t RESOLUTION = 12;
const unsigned long maxWaitTime = 5000;
const unsigned long interMeasurementPeriod = 1000;

int DS18State = 0; //0 = nothing, 1 = converting, 2 = finisg converting

/*
 * Initiate onewire Sensors
 */
void DS18Init(bool header=LOW){
  DS18State = 0;
  
  // Locate and Enumerate the DS18xx-type Temperature Sensors
  numDS18 = tempSensors.begin(RESOLUTION);

  // Loop through each device, print out address as headers
  if(header == HIGH){
    for(int i=0;i<numDS18; i++){
      // Search the wire for address
      if(tempSensors.getAddress(tempDeviceAddress, i)){
        Serial.print("device");
        Serial.print(i, DEC);
        Serial.print(" with address: ");
        //printAddress(tempDeviceAddress);
        //Serial.println();
        Serial.print(" ");
      } else {
        Serial.print("Found ghost device at ");
        Serial.print(i, DEC);
        Serial.print(" but could not detect address. Check power and cabling");
      }
    }
    Serial.println();
  }   
}

/*
 * Baca sensor yang terpenting untuk kontrol
 */

boolean DS18StartConvertion() {
  if(numDS18 == 0){
    Serial.println(F("no sensors present"));
    numDS18 = tempSensors.begin(RESOLUTION);
    return false;
  }
  if (tempSensors.startConvertion()) {
    DS18State = 1;
    return true;
  }
  else {
    Serial.println(F("Failed to Start Conversion Cycle, Aborting"));
    return false;
  };
}

boolean DS18ConvertionAvailable(){
  if(tempSensors.isConversionDone()&& DS18State == 1){
    //Serial.println("done");
    DS18State = 2;
    return true;}
  else{return false;}
}
 
DTYPE DS18Read(int DeviceIndex) {
  if(DS18State == 2){
    DS18State = 0;
    float sensor = tempSensors.getLatestTempC(DeviceIndex)*100;
    return (DTYPE)sensor;
  }
  return 0;
}
