/*
 * Menangani komunikasi ke MQTT
 */
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient WiFiclient;
PubSubClient client(WiFiclient);


//===========================================================
//conection
//Insert wifi name/SSID and passwordfor the device to connect
const char* ssid = "username"; 
const char* password =  "password";

//Insert mqtt server and port
const char* mqttServer = ""; //Insert MQTT Server IP 
const int mqttPort = 1883;

#define MQTT_TOPIC "ITB/TF/labTF3/" KIT_ID

const char* clientID = MQTT_TOPIC;

enum eChannel {I_NONE, I_CMD, I_AJAX};

const char* chSubscribe = MQTT_TOPIC "/input/#";
const char* chSubCmd = MQTT_TOPIC "/input/cmd";
const char* chSubAjax = MQTT_TOPIC "/input/ajax";

const char* chPublish = MQTT_TOPIC "/output";
const char* chPubHead = MQTT_TOPIC "/output/head";
const char* chPubData = MQTT_TOPIC "/output/data";
const char* chPubAjax = MQTT_TOPIC "/output/ajax";

#define MQTT_STR_MAX  200

long lastReconnectAttempt = 0;
char mqtt_buffer[MQTT_STR_MAX];
int mqtt_length=0;
int mqtt_topic=0;

#define JSON_MASK 0x0FFF
#define JSON_HEAD 0x1FFF
#define JSON_DATA 0x0FFF

unsigned json_serial=0;

char *json_id = "{\
  \"program\": \"" PROGRAM_NAME "\",\
  \"version\": \"" PROGRAM_VERSION "\"}";

char *json_units1="\"%\",\"C\",\"V\",\"C\",\"mA\"";
char *json_headers1="\"t\",\"T1\",\"V\",\"TSP\",\"Ir\"";

char *json_units2="\"ms\",\"C\",\"V\",\"C\",\"mA\"";
char *json_headers2="\"t\",\"T1\",\"V\",\"TSP\",\"Ir\"";

// Header ini harus disesuaikan dengan data
char *json_head = "{\
\"no\":\"%u\",\
\"command\":\"%s\",\
\"title\":\"Pengukuran Temperatur\",\
\"units\":[%s],\
\"headers\":[%s],\
\"data\":[%s]}";

char *json_data = "{\
  \"no\":\"%u\",\
  \"data\":[%s]}";

char *json_respond = "{\
  \"command\":\"%s\",\
  \"respond\":\"%s\"}";

char *json_error = "{\
  \"command\":\"%s\",\
  \"error\":\"%s\"}";

char* jsonStr(char* sv, const char* str) {
  *sv++ = '"';
  while (*str) *sv++=*str++;
  *sv++ = '"';
  *sv = '\0';
  return sv;  
}

char* jsonInt(char* sv, DTYPE v) {
  char si[16];
  ltoa(v, si, 10);
  for (int i=0; si[i]; i++) {
    *sv++=si[i];
  }
  *sv = '\0';
  return sv;
}

char* jsonArray(char* sv, int nvalues, DTYPE values[]) {
  *sv++ = '[';
  sv = jsonInt(sv, values[0]);  
  for (int i=1; i<nvalues; i++) {
    *sv++ = ',';
    sv = jsonInt(sv, values[i]);
  }
  *sv++ = ']';
  *sv = '\0';
  return sv;
}

char* jsonObject(char* sv, int nvalues, const char* fields[], int values[]) {
  char si[10];
  *sv++ = '{';
  for (int i=0; ; ) {
    sv = jsonStr(sv, fields[i]);
    *sv++ = ':';
    sv = jsonInt(sv, values[i]);
    i++;
    if (i>=nvalues) break;    
    *sv++ = ',';
  }
  *sv++ = '}';
  *sv = '\0';
  return sv;  
}

// kirim header saja
void mqttPrintHead(const char* cmd, char* u, char* h, int nvalues, DTYPE values[]) { 
  char sPayload[MQTT_STR_MAX];
  char sValues[(DATA_MAX)*12];
  json_serial++;
  jsonArray(sValues, nvalues, values);
  sprintf(sPayload, json_head, json_serial, cmd, u, h, sValues);
  client.publish(chPubHead,sPayload);
  Serial.println(sPayload);
}

void mqttPrintHead(const char* cmd, int nvalues, DTYPE values[]) { 
  mqttPrintHead(cmd, json_units1, json_headers1, nvalues, values);
}

void mqttPrintHead2(const char* cmd, int nvalues, DTYPE values[]) { 
  mqttPrintHead(cmd, json_units2, json_headers2, nvalues, values);
}

// kirim data berikutnya
void mqttPrintData(int nvalues, DTYPE values[]) {
  char sPayload[MQTT_STR_MAX];
  char sValues[(DATA_MAX)*8];
  json_serial++;
  jsonArray(sValues, nvalues, values);
  sprintf(sPayload, json_data, json_serial, sValues);
  client.publish(chPubData, sPayload);
  Serial.println(sPayload);
}

void mqttPrintError(const char *cmd, const char *msg) {
  char sPayload[80]; 
  sprintf(sPayload, json_error, cmd, msg);
  client.publish(chPubAjax, sPayload);  
  Serial.println(sPayload);
}

void mqttPrintRespond(const char *cmd, const char *msg) {
  char sPayload[80]; 
  sprintf(sPayload, json_respond, cmd, msg);
  client.publish(chPubAjax, sPayload);
  Serial.println(sPayload);
}

void mqttPrintSettings(int ns, const char* str[], int vals[]) {
  char sPayload[MQTT_STR_MAX];
  jsonObject(sPayload, ns, str, vals);
  client.publish(chPubAjax, sPayload);  
  Serial.println(sPayload);
}

int mqttAvailable() {
  if (mqtt_topic) {
    int t = mqtt_topic;
    mqtt_topic = I_NONE;
    return t;
  }
  return I_NONE;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    int i;
    if (strcmp(topic, chSubCmd)==0) {
      mqtt_topic = I_CMD;
      parseLine((char*)payload, length);
    }
    else if (strcmp(topic, chSubAjax)==0) {
      mqtt_topic = I_AJAX;
    }
    else {
      mqtt_topic = I_NONE;
    }    
}


boolean mqttReconnect() {
  Serial.print("MQTT connecting to ");      
  Serial.print(mqttServer);
  Serial.print(":");
  Serial.print(mqttPort);        
  Serial.print(". . .");
  if (client.connect(clientID)) {
    client.subscribe(chSubscribe);
    Serial.println();
    Serial.print("Subscribe to ");
    Serial.println(chSubscribe);
    client.publish(chPublish,json_id);
    return true;
  }
  Serial.println("failed");
  return false;
}

void wifiInit() {
  Serial.print("Wifi connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Connect to WiFi.
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP:");
  Serial.println(WiFi.localIP());  

  client.setBufferSize(512);
  client.setServer(mqttServer, mqttPort); // Connect to MQTT
  client.setCallback(mqttCallback);
  
  lastReconnectAttempt = 0;
}

void wifiLoop() {
  if (client.connected()) {
    client.loop();    
  }
  else { //reconnect mqtt
    //Serial.println("MQTT disconnected");
    long now = millis();
    if (now - lastReconnectAttempt > 10000) { // Try to reconnect.
      lastReconnectAttempt = now;
      if (mqttReconnect()) { // Attempt to reconnect.
        lastReconnectAttempt = now;
      }
    }
  }
}
