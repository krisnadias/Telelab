/* ========================================
 * Code for parsing strings from UART into commands
 * ========================================
*/

#include "serial.h"


/*Line for each command, needs to be in the same order as enum eCommand in serial.h!*/
const char* cmd_str []= {
  "", "S", "I", "CV","LSV","CA","CALI","RG",
};

int cmd=NONE;
int cmd_last=NONE;
int cmd_params[PARAM_MAX];
int cmd_nparam=0;
int cmd_error = 0;

const char cmd_end =  '\r';
const char cmd_separators[] = " ,;=";


/* ========================================
 * Operation: serial_parseLine
 * Description: Parse a string into the command and parameter variables
 * Input: char *s_cmd - char array of serial input like "CV 100 200"
 * Output: 
 *    return true/false - false if operation error
 *    int cmd - number that points what command is called according to cmd_str
 *    int cmd_params[] - array of value from the command 
 * ========================================
*/
uint8_t serial_parseLine(char *s_cmd) {
    memset(cmd_params, 0, PARAM_MAX);
    char* token;
    token = strtok(s_cmd,cmd_separators);
    
    if (token == NULL) {
        cmd_error = 1;
        return 0; //false
    }
    if (!serial_parseCommand(token)) {
        cmd_error = 2;
        return 0; //false    
    }
    for (int i=0; i<PARAM_MAX; i++) {
        token = strtok(NULL,cmd_separators);
        if (token == NULL) break;
        cmd_params[i] = atol(token); 
    }
    cmd_error = 0;
    return 1; //true
}
/* function to compare the input char* with the list of valid commands in cmd_str */
int serial_parseCommand(char *line) {
    for (int i=0; i<CMD_MAX; i++) {
        if (strcmp(line, cmd_str[i]) == 0) {
            cmd = i;
            return 1; //true
        }
    }
    return 0; //false  
}

/* ========================================
 * Operation: serial_checkInput
 * Description: reads UART input and holds it inside a buffer until a delimiter is readed
 * input: char* buffer - char array to store the serial input line
 * output:
 *      return true/false - return true after a input is complete (delimiter met)
 *      char* buffer - the array is filled with the input line 
 * ========================================
*/
uint8_t serial_checkInput(char *buffer) {
    uint8_t c;
    static uint8_t i=0;
    if(UART_GetRxBufferSize()){ //if UART input available
        c = UART_GetByte(); //Read 1 char first
        //UART_PutChar(c); //echo the char
        if(c=='\b'){//if backspace
            buffer[i] = ' ';
            i--;
            return 0;
        } else if(c=='\r' || c=='\n'){ //if found delimiter, input available
            i =0;
            UART_PutString("\r\n");
            return 1;
        } else if(i >= RX_BUFFER_SIZE){ //if maximum output, input available
            i = 0;
            return 1;
        } else{ //if no delimiter, hold input
            buffer[i] = c;
            i++;
            return 0;
        }
    }
    return 0;
}

/* [] END OF FILE */
