#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"

/* Variables for Serial Communication */
#define PARAM_MAX 5
#define RX_BUFFER_SIZE  32
#define TX_BUFFER_SIZE  32

enum eCommand {
  NONE=0, STOP, IDENTIFY, CV, LSV, CA, CALIBRATE, RANGE,
  CMD_MAX
};


extern int cmd;
extern int cmd_last;
extern int cmd_params[PARAM_MAX];
extern int cmd_nparam;
extern int cmd_error;

uint8_t serial_checkInput(char *buffer);
void serial_exportData(char array[]);
uint8_t serial_parseLine(char *s_cmd);
int serial_parseCommand(char *line);


/* [] END OF FILE */
