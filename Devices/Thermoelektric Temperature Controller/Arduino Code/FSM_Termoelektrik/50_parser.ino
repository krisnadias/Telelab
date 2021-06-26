/* Parse perintah, dengan format
 * CMD [param1 [param2 [param3 [param4]]]] 
 */

#define PARAM_MAX 4

enum eCommand {
  NONE, RESET, DOT,
  SNA, SND, SNS, STA, STD, STS, 
  CLR, OFF, OP, DC, AC, 
  TP, TR, TS, TF, TM, 
  CO, CP, CPI, CPID,
  CMD_MAX
};
  
const char* cmd_str[CMD_MAX] = {
  "", "..", ".",   
  ".NA", ".ND", ".NS", ".TA", ".TD", ".TS",
  "CLR", "OFF", "OP", "DC", "AC", 
  "TP", "TR", "TS", "TF", "TM",
  "CO", "CP", "CPI", "CPID"
};

int cmd=NONE;
int cmd_last=NONE;
DTYPE cmd_params[PARAM_MAX];
int cmd_nparam=0;
int cmd_error;

//=======================================================
// Parser

#define LINE_MAX 64
const char cmd_end =  '\r';
const char cmd_separators[] = " ,;=";

char cmd_buffer[LINE_MAX];

boolean parseCommand(char *line) {
  for (int i=0; i<CMD_MAX; i++) {
    if (strcmp(line, cmd_str[i]) == 0) {
      cmd = i;
      return true;
    }
  }
  return false;  
}

boolean parseLine(const char *s_cmd, int l_cmd) {
  char* token;
  int i;
  for (i=0; (i<l_cmd) && (i<LINE_MAX-1); i++) {
    cmd_buffer[i] = toupper(s_cmd[i]); //touper mengconvert semua lowercase into uppercase
  }
  cmd_buffer[i] = '\0';
  token = strtok(cmd_buffer,cmd_separators); //strtok 
  if (token == NULL) {
    cmd_error = 1;
    return false;
  }
//  Serial.print("Parse CMD ");
//  Serial.println(token);  
  if (!parseCommand(token)) {
    cmd_error = 2;
    return false;    
  }
  for (int i=0; i<PARAM_MAX; i++) {
    token = strtok(NULL,cmd_separators);
    if (token == NULL) break;
    cmd_params[i] = (DTYPE) atol(token); // ubah sesuai DTYPE
  }
  cmd_nparam=i;
  cmd_error = 0;
  return true;
}

boolean cmdAvailable() {
  return cmd != NONE;
}

int cmdGet() {
  if (cmd != NONE) {
    cmd_last = cmd;
    cmd = NONE;
    return cmd_last;    
  }
  return NONE;
}

int cmdError() {
  return cmd_error;  
}
