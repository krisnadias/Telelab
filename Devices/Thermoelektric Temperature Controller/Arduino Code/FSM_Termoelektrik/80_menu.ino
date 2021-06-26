/* 
 * Finite State Machine Menu Kontrol Panel
 */

#define MENU_ROWMAX 2

//------------------------------------------------------
// 1. Definisi konstanta STATE dan EVENT
//State Menu
enum st_menu {STM_MAIN=1, STM_ONLINE, STM_OFFLINE, STM_CO};
enum ev_menu {EVM_COMMAND=1, EVM_CONTROL, EVM_UP, EVM_DOWN, EVM_BACK, EVM_SELECT0, EVM_SELECT1, EVM_SELECT2};

//------------------------------------------------------
// 2. Pesan variabel global
// definisikan dulu struct-nya
struct data_menu {
  int submenu;
  int row;
  int control; 
  char* menuText[4][MENU_ROWMAX+1];
};

// pesan data struct, dan langsung inisialisasi
data_menu dataMenu = {
  0,
  0,
  0,
  {{"online","offline","-----"},        //main
  {"Online:", "SP:     T1:"}, //main>online
  {"mode CO","-----","-----"},          //main>offline
  {"[CO] Temp: ", "Target:"}            //main>offline>mode CO
  }
};
#define MENU_MAIN 0
#define MENU_ONLINE 1
#define MENU_OFFLINE 2
#define MENU_CO 3

interval buttonSampling;
interval displaySampling;

//----------------------------------------------
// 3. Buat fungsi baca event, dengan bentuk
// int nama_fungsi(int id)
// return int : jenis event yang terjadi 
// (sesuai enum yang sudah didefinisikan)

int evMenu(int id) {

  if(isTimeout(buttonSampling)){
    int a = panelRead();
    
    // baca button
    switch(a){
      case 1 : //select
        switch(dataMenu.row){ 
          case 0: return EVM_SELECT0; 
          case 1: return EVM_SELECT1; 
          case 2: return EVM_SELECT2;
        }
      case 2 : return EVM_BACK; //back
      case 3 : return EVM_UP; //up
      case 4 : return EVM_DOWN; //down
    }
  }
  
   
  if (cmdAvailable()) {
    return EVM_COMMAND;    
  }
  if (controlAvailable()) {
    return EVM_CONTROL;
  }
}

//------------------------------------------------
// 4. Buat beberapa fungsi transisi, dengan bentuk
// boolean nama_fungsi(int id)
// selama transisi, algoritma umum adalah
// - matikan kondisi state sebelumnya
// - hidupkan kondisi di state baru
// - siapkan pemicu untuk event berikutnya
// return true jika transisi dijalankan


//-----------------4.0 fungsi menu umum

void menuUpdate1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print('>');
  lcd.setCursor(1, 0);
  lcd.print(dataMenu.menuText[dataMenu.submenu][dataMenu.row]);
  lcd.setCursor(1, 1);
  lcd.print(dataMenu.menuText[dataMenu.submenu][dataMenu.row+1]);

  Serial.println("-----------");
  Serial.print('>');
  Serial.println(dataMenu.menuText[dataMenu.submenu][dataMenu.row]);
  Serial.print(' ');
  Serial.println(dataMenu.menuText[dataMenu.submenu][dataMenu.row+1]);
  Serial.println("-----------");
}
void menuUpdate2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dataMenu.menuText[dataMenu.submenu][dataMenu.row]);
  lcd.setCursor(0, 1);
  lcd.print(dataMenu.menuText[dataMenu.submenu][dataMenu.row+1]);
  
  Serial.println("-----------");
  Serial.println(dataMenu.menuText[dataMenu.submenu][dataMenu.row]);
  Serial.println(dataMenu.menuText[dataMenu.submenu][dataMenu.row+1]);
  Serial.println("-----------");
}
void changeSubMenu(int subMenu) {
  dataMenu.submenu=subMenu;
  dataMenu.row=0;
}



//-----------------4.1 transisisi main

boolean trm_row_up(int id) {
  if (dataMenu.row>0){
    dataMenu.row--;
    menuUpdate1();
    return true;
  }
  return false;
}

boolean trm_row_down(int id) {
  if (dataMenu.row<MENU_ROWMAX-1){
    dataMenu.row++;
    menuUpdate1();
    return true;
  }
  return false;
}

boolean trm_main_online(int id) {
  changeSubMenu(MENU_ONLINE);
  menuUpdate2();
  wifiInit();
  return true;
}

boolean trm_main_offline(int id) {
  changeSubMenu(MENU_OFFLINE);
  menuUpdate1();
  return true;
}



//-----------------4.2 transisisi online
  //LCD DISPLAY ROW
  // 012345678912345
  //"Online: Connect" 
  //"SP:2000 T1:2000"
  
boolean trm_online_command(int id) {
  int cmd = cmdGet();
  switch(cmd) {
    case OFF :
      controlPushEvent(EV_STANDBY);
      Serial.println("off");
      lcd.setCursor(3, 1);
      lcd.print("off ");
      return true;
    case CO :
      // jalankan perintah CO
      controlSetSP(cmd_params[0]);
      controlPushEvent(EV_CO);
      dataMenu.control = 0;
      return true;
  }  
  return false;
}

boolean trm_online_control(int id) {
  //kirim data ke MQTT dan Serial Monitor
  if (dataMenu.control == 0) {
    mqttPrintHead(cmd_str[cmd_last], DATA_MAX, data1.data);        
    dataMenu.control = 1;
  }
  else {
    mqttPrintData(DATA_MAX, data1.data);
    dataMenu.control++;
  }  
  //Update tampilan di layar
  lcd.setCursor(3, 1);
  lcd.print(data1.targetTemp);
  lcd.setCursor(12, 1);
  lcd.print(data1.data[1]);
  
  return true;
}

boolean trm_online_main(int id) {
  controlPushEvent(EV_STANDBY);
  changeSubMenu(MENU_MAIN);
  menuUpdate1();
  WiFi.disconnect();
  return true;
}

//-----------------4.3 transisisi offline
boolean trm_offline_co(int id) {
  dataMenu.control = 0;
  controlPushEvent(EV_CO);
  changeSubMenu(MENU_CO);
  menuUpdate2();
  Serial.println("Time, SensorTemp, TargetTemp, Heating/Cooling");
  return true;

}

boolean trm_offline_main(int id) {
  changeSubMenu(MENU_MAIN);
  menuUpdate1();
  return true;
}

//-----------------4.3 transisisi CO
  //LCD DISPLAY ROW
  // 012345678912345
  //"[CO] Temp:00.00" 
  //"Target:00.00  H"
  
boolean trm_co_control(int id) {
  lcd.setCursor(11, 0);
  lcd.print(data1.data[1]/100);
  lcd.print(".");
  lcd.print(data1.data[1]%100);
  lcd.setCursor(7, 1);
  lcd.print(data1.targetTemp/100);
  lcd.print(".");
  lcd.print(data1.targetTemp%100);
  lcd.print(" ");
  lcd.setCursor(15, 1);
  switch(outputRead()){
    case TEC_HEATING: lcd.print("+"); break;
    case TEC_COOLING: lcd.print("-"); break;
    case TEC_OFF    : lcd.print("="); break;
  }

  if(data1.data[0] % 30000==0){
  Serial.print(data1.data[0]);
  Serial.print(", ");
  Serial.print(data1.data[1]);
  Serial.print(", ");
  Serial.print(data1.targetTemp);
  Serial.print(", ");
  Serial.println(data1.data[2]);
  //Serial.print(", ");
  //Serial.println(data1.data[4]);
  }
  
  return true;
}

boolean trm_co_up(int id) {
   if(data1.targetTemp<8000){
   controlSetSP(data1.targetTemp+50);}
   return true;
}

boolean trm_co_down(int id) {
   if(data1.targetTemp>0){
   controlSetSP(data1.targetTemp-50);}
   return true;
}

boolean trm_co_offline(int id) {
   controlPushEvent(EV_STANDBY);
   changeSubMenu(MENU_OFFLINE);
   menuUpdate1();
   return true;
}

//------------------------------------------------
// 5. Pesan variabel fsm_row (berupa struct)
// langsung inisialisasi dengan 
// tabel transisi : array 2D 4 kolom, baris bisa banyak. Kolom:
//    state : state sekarang
//    event : event pemicu
//    transisi: fungsi_transisi yang sudah didefinisikan, pakai prefix & 
//    next_state : state selanjutnya
// baris terakhir HARUS diisi dengan state STATE_FINAL

struct fsm_transition trMenu[] = {
    {STM_MAIN,    EVM_UP,      &trm_row_up,               STM_MAIN},
    {STM_MAIN,    EVM_DOWN,    &trm_row_down,             STM_MAIN},
    {STM_MAIN,    EVM_SELECT0, &trm_main_online,          STM_ONLINE},
    {STM_MAIN,    EVM_SELECT1, &trm_main_offline,         STM_OFFLINE},
    
    {STM_ONLINE,  EVM_COMMAND, &trm_online_command,       STM_ONLINE},
    {STM_ONLINE,  EVM_CONTROL, &trm_online_control,       STM_ONLINE},
    {STM_ONLINE,  EVM_BACK,    &trm_online_main,          STM_MAIN},

    {STM_OFFLINE, EVM_UP,      &trm_row_up,               STM_OFFLINE},
    {STM_OFFLINE, EVM_DOWN,    &trm_row_down,             STM_OFFLINE},
    {STM_OFFLINE, EVM_SELECT0, &trm_offline_co,           STM_CO},   
    {STM_OFFLINE, EVM_BACK,    &trm_offline_main,         STM_MAIN},


    {STM_CO,      EVM_CONTROL, &trm_co_control,      STM_CO},
    {STM_CO,      EVM_UP,      &trm_co_up,           STM_CO},
    {STM_CO,      EVM_DOWN,    &trm_co_down,         STM_CO},
    {STM_CO,      EVM_BACK,    &trm_co_offline,      STM_OFFLINE},
    
    {ST_FINAL,    0,           TR_NONE,             ST_FINAL}
};

fsm_info fsmMenu = {
  9,                // id FSM
  STM_MAIN,         // state awal
  0,                // transisi awal
  &evMenu           // fungsi baca event     
};

void menuInit() {
  //Serial.println("==menu init==");
  changeSubMenu(MENU_MAIN);
  menuUpdate1();
  intervalInit(buttonSampling, 1);
}

void menuLoop() {
  //Serial.println("==menu loop==");
  fsmRun(fsmMenu, trMenu);
  
  if (fsmMenu.state == STM_ONLINE){
    srLoop();
    wifiLoop();
  }
}
