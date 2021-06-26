/*
 * Pustaka Simple Finite State Machine 
 * Ini berbeda dengan praktikum sebelumnya
 * Karena dibatasi syntax C versi lama yg bisa jalan di tinkerCAD
 */

// konstata state standar
#define ST_START 0
#define ST_FINAL -1

// konstata event standar
#define EV_NONE  0

// konstata transisi standar
#define TR_NONE  NULL

// struct untuk menyimpan 1 baris state transition
struct fsm_transition {
  int state;        // state saat ini
  int event;        // event pemicu
  boolean (*transition)(int);  // transisi yang akan jalan
  int next;         // state berikutnya setelah transisi
};

// struct untuk menyimpan info FSM lengkap
// termasuk tabel state transition
struct fsm_info {
  int id;               // id unik
  int state;            // state awal, akan berubah
  int transition;       // transisi awal, akan berubah
  int (*getEvent)(int id);  // fungsi baca event
  int event;            // event yang di-push
};

/* 
 *  melakukan transisi berdasar state dan event
 *  return event yg diterima
 */
int fsmRun(struct fsm_info &fsm, struct fsm_transition table[]) {
  // kalau sudah final, abort
  if (fsm.state == ST_FINAL) return EV_NONE;

  // cari posisi awal state di tabel
  int i=0;
  while(table[i].state != fsm.state) {
    // kalau sampai akhir tak ada
    // state invalid, abort
    if (fsm.state == ST_FINAL) {
      fsm.state = ST_FINAL;
      return EV_NONE;
    }
    i++;
  }


  // panggil fungsi getEvent
  int event = fsm.event;
  if (event == EV_NONE) {
    event = fsm.getEvent(fsm.id);
  }
  else{fsm.event = EV_NONE;}


  // cari posisi event di tabel
  // untuk state saat ini saja
  while(table[i].event != event) {
    i++;
    // kalau tak ketemu, return EV_NONE
    if (table[i].state != fsm.state) return EV_NONE;
  }

  // ketemu baris state transisition, catat
  fsm.transition = i;
  
  // jika tak ada transisinya ...
  if (table[i].transition == TR_NONE) {
    // ganti state baru saja
    fsm.state = table[i].next;    
  }
  else {
    // jalankan transisi, jika transition return true
    if (table[i].transition(fsm.id)) {
      // ganti state baru
      fsm.state = table[i].next;
    }
  }
  return event;
}

void fsmPushEvent(struct fsm_info &fsm, int event) {
  fsm.event = event;
}
