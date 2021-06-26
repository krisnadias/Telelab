 /* KONTROL LDR LED
 * Menyalakan LED jika LDR > ambang 
 * - mengamati cara kerja kontrol ON-OFF dengan 1 ambang
 */

#define TITLE "Kontrol Termoelektrik"
#define PROGRAM_NAME  "TF TeleLab - " TITLE
#define PROGRAM_VERSION  "1.0"

#define KIT_NAME "Termoelektrik1"
#define KIT_NUMBER "01"
#define KIT_ID KIT_NAME "/" KIT_NUMBER

// Tipe data pengukuran, sebaiknya tetap int
// kalau terpaksa boleh float, dengan konsekuensi
//  - ubah cara baca data
//  - ubah format pengiriman mqtt

#define DTYPE int


// banyaknya data
#define INPUT_MAX 2 //jumlah sensor: 1 Temperatur, 1 Arus
#define OUTPUT_MAX 1 //jumlah aktuator: Relay modul
// total waktu, input, output, +SetPoint
#define DATA_MAX (1+OUTPUT_MAX+INPUT_MAX+1)
