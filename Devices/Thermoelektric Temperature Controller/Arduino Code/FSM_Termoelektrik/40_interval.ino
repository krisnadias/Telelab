/*
 * Pustaka interval waktu 
 * Mekanisme membuat event waktu sederhana,
 */

// konstanta periode tak hingga
#define IP_INFINITE 0

// struktur data interval
struct interval {
  int periode;            // periode interval
  unsigned long t_last;   // waktu terakhir terpicu
  unsigned long t_start;  // waktu mulai
};

/*
 * Inisialisasi interval dengan lama periode tertentu dalam ms
 * Agar struct dapat dioleh dalam fungsi, 
 * maka parameter memakai tanda &, artinya reference ke variabel tersebut
 * perhatikan cara menulisnya: struct interval &parameter.
 * Selanjutnya field struct diakses pakai nama_variabel.nama_field
 */
void intervalInit(struct interval &itv, long ms) {
  itv.periode = ms;                     // simpan periode ke itv
  itv.t_start = itv.t_last = millis();  // inisialisasi waktu
}

/*
 * mengubah periode interval
 */
void intervalSetPeriode(struct interval &itv, long ms) {
  itv.periode = ms;
}

/*
 * mengambil selang waktu dari awal smapai terakhir terpicu
 */
unsigned long intervalGet(struct interval &itv) {
  return itv.t_last - itv.t_start;
}

/*
 * memeriksa apakah timeout sudah terjadi
 * jika ya, timeout time akan ditambah periode untuk interval berikutnya
 * lalu return true 
 */
boolean isTimeout(struct interval &itv) {
  // kalau periode belum di-set, langsung return false
  if (itv.periode == IP_INFINITE) return false;

  // kalau waktu picu belum sampai, return false juga
  if (millis() - itv.t_last < itv.periode) return false;
  
  // ok, sudah waktu picu  
  // siapkan dulu waktu picu berikutnya
  itv.t_last += itv.periode;

  // return true, karena event sudah terjadi
  return true;
}
