/* 
 * Finite State Machine Kontrol Termoelektrik
 */

//------------------------------------------------------
// 1. Definisi konstanta STATE dan EVENT
//State kontrol
enum st_kontrol {ST_STANDBY=1, ST_CO};
enum ev_kontrol {EV_STANDBY=1, EV_CO, EV_SENSOR_START, EV_CONTROL};

//------------------------------------------------------
// 2. Pesan variabel global

// definisikan dulu struct-nya
struct data_kontrol {
  unsigned prdControl;
  int targetTemp;   // hanya 1 ambang
  int currentTemp;  // variabel bacaan 
  int toleranceTemp;
  int toleranceTempL;
  boolean available;  // data siap dibaca
  boolean intervalOn;  // interval on/off
  DTYPE data[DATA_MAX];
};


// pesan data struct, dan langsung inisialisasi
struct data_kontrol data1 = {
  1000, // kontrol setiap 1 detik
  2700,   // targetTemp (x0.01 C)
  2700,   // currentTemp(x0.01 C)
  50,   //toleranceTemp (x0.01 C)
  20,   // toleranceTempL;
  false, 
  false,
  false    // data belum siap
};

struct interval itv_sensorStart;

//----------------------------------------------
// 3. Buat fungsi baca event, dengan bentuk
// int nama_fungsi(int id)
// return int : jenis event yang terjadi 
// (sesuai enum yang sudah didefinisikan)

/*
 * baca event dari interval dan keypad
 */
int evKontrol(int id) {
  if(DS18ConvertionAvailable()){
    return EV_CONTROL;
  }
  if (data1.intervalOn && isTimeout(itv_sensorStart)){
    return EV_SENSOR_START;
  }
  
  return EV_NONE;
}

//------------------------------------------------
// 4. Buat beberapa fungsi transisi, dengan bentuk
// boolean nama_fungsi(int id)
// selama transisi, algoritma umum adalah
// - matikan kondisi state sebelumnya
// - hidupkan kondisi di state baru
// - siapkan pemicu untuk event berikutnya
// return true jika transisi dijalankan

boolean tr_standby_co(int id) {
  //Serial.println("CONTROL_tr_standby_co");
  data1.intervalOn = true;
  fanWrite(RELAY_ON);
  intervalInit(itv_sensorStart, data1.prdControl);
  return true;
}

boolean tr_co_sensorStart(int id){
  //Serial.println("CONTROL_tr_co_sensorStart");
  DS18StartConvertion();
  data1.data[0] = intervalGet(itv_sensorStart);
}

boolean tr_co_control(int id) {
  // lakukan pengontrolan 
  //Serial.println("CONTROL_tr_co_control");
  
  int sensor = DS18Read(0);
  int output = outputRead();
  
  if (output == TEC_COOLING) {
    if (sensor < data1.targetTemp+data1.toleranceTempL) {
      outputWrite(TEC_OFF);    
    }
  }
  else if (output == TEC_HEATING) {
    if (sensor > data1.targetTemp-data1.toleranceTempL) {
      outputWrite(TEC_OFF);
      fanWrite(RELAY_ON);    
    }
  }
  else {
    if (sensor < data1.targetTemp-data1.toleranceTemp) { //jika panas melebihi toleransi, dinginkan
      if (data1.targetTemp<2000 && data1.targetTemp-sensor<500){
        outputWrite(TEC_OFF);
      }
      else{
        outputWrite(TEC_HEATING);
      } 
      fanWrite(RELAY_OFF);    
    }
    else if (sensor > data1.targetTemp+data1.toleranceTemp) {
      if (data1.targetTemp>3000 && sensor-data1.targetTemp<500){
        outputWrite(TEC_OFF);
        fanWrite(RELAY_ON); 
      }
      else{
        outputWrite(TEC_COOLING);
        fanWrite(RELAY_ON); 
      }    
    }    
  }
  
  // isi data
  // waktu ->diambil saat sensor mulai sampling pada 'tr_co_sensorStart'
  //data1.data[0] = intervalGet(itv_sensorStart); 
  // input temperatur
  data1.data[1]=sensor;
  // output relay
  data1.data[2] = outputRead();
  // setpoint temperatur
  data1.data[3] = data1.targetTemp;
  // input arus relay
  //data1.data[4]= ACS712Read();
  data1.available = true;
  return true;
}

boolean tr_co_standby(int id) {
  //Serial.println("CONTROL_tr_co_standby");
  // pastikan output mati
  outputWrite(TEC_OFF);
  fanWrite(RELAY_OFF);
  //matikan interval kontrol
  data1.intervalOn = false;
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

struct fsm_transition tr_kontrol[] = {
    {ST_STANDBY, EV_CO,           &tr_standby_co,     ST_CO},
    {ST_CO,      EV_SENSOR_START, &tr_co_sensorStart, ST_CO},
    {ST_CO,      EV_CONTROL,      &tr_co_control,     ST_CO},
    {ST_CO,      EV_STANDBY,      &tr_co_standby,     ST_STANDBY},
    {ST_FINAL,   0,               TR_NONE,            ST_FINAL}
};

//------------------------------------------------
// 6. Pesan variabel fsm_info 
// Inisialisasi dengan
// id : nomor unit, untuk di-pass ke fungsi getEvent & transisi
// state : state awal memakai enum state yang sudah didefinisikan
// transisi : transisi awal, selalu isi 0
// event : fungsi baca event yang sudah didefinisikan, perhatikan prefix &

struct fsm_info fsm_kontrol = {
  1,               // id FSM
  ST_STANDBY,      // state awal
  0,               // transisi awal
  &evKontrol       // fungsi baca event     
};

void controlInit() {
  // nothing
}

/* Mengirim EV_STANDBY atau EV_AUTO ke control
 * Dipanggil oleh menu
 */
void controlPushEvent(int ev) {
  fsmPushEvent(fsm_kontrol, ev);
}

boolean controlLoop() {
  int event = fsmRun(fsm_kontrol, tr_kontrol);
  return (event != EV_NONE);
}

void controlSetSP(int sp) {
  data1.targetTemp = sp;
}

boolean controlAvailable() {
  if (data1.available) {
    data1.available=false;
    return true;
  } 
  return false; 
}
