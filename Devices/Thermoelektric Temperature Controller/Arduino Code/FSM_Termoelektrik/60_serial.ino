/*
 * Baca serial sampai full line, taruh di buffer
 */

// agar cepat, naikkan baud
#define BAUD 115200

char sr_buffer[LINE_MAX];
int sr_length=0;
boolean sr_ready = false;

//====================================
// tampilan kontrol ke serial plotter

#define SEPARATOR ", "

void plotHead(char* h1, char* h2, char* h3) {
  Serial.print(h1);
  Serial.print(SEPARATOR);
  Serial.print(h2);
  Serial.print(SEPARATOR);
  Serial.println(h3);
}

void plotSignal(int s1, int s2, int s3) {
  Serial.print(s1);
  Serial.print(SEPARATOR);
  Serial.print(s2);
  Serial.print(SEPARATOR);
  Serial.println(s3);
}

// plot 4 variabel
void plotHead(char* h1, char* h2, char* h3, char*h4) {
  Serial.print(h1);
  Serial.print(SEPARATOR);
  plotHead(h2,h3,h4);
}

void plotSignal(int s1, int s2, int s3, int s4) {
  Serial.print(s1);
  Serial.print(SEPARATOR);
  plotSignal(s2,s3,s4);
}


void srInit() {
  Serial.begin(BAUD);
  sr_length=0;
  sr_ready = false;  
}

/* periksa buffer serial
 * return trus kalau terisi, dan harus segera diproses
 */
/*
boolean srAvailable() {
  if (sr_ready) {
    sr_ready = false;
    return true;
  }
  return false;
}
*/

/*
 * membaca data dari serial
 */
boolean srLoop() {
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      sr_buffer[sr_length] = '\0';
      parseLine(sr_buffer, sr_length);
      sr_length = 0;
      sr_ready = true;
    }
    else if (ch >= ' ') {
      sr_buffer[sr_length++] = ch;      
    }
    return true;
  }
  return false;
}
