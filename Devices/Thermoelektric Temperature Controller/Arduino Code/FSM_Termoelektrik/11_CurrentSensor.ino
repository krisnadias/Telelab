/*
 * ACS712 Current Sensor 30A
 * output error 1.5%
 */
 

/*
 * Assign pins
 */
#define PIN_ACS712 4

//Baca Arus dari ACS712 
int ACS712Read(){
  //Konversi adalah (analogRead*5/4095) untuk jadi tegangan
  //kemudian -2.5V agar dapat nilai tengah (arus bisa positif maupun negatif)
  //kemudian dibagi 0.001V/mV* 66mV/A yg merupakan konstanta konversi untuk ACS712 30A
  //kemudian dikali 1000mA/A agar dikirim sebagai integer 
  //untuk meminimalkan proses, semua itu disatukan jadi:
  int Current = ((analogRead(PIN_ACS712)-2047)*37/20);
  return Current;
}
