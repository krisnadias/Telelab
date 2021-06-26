/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"

/* Operation States */
int state = 0;
enum eState { 
    ST_NONE=0, ST_CV=1, ST_CA, ST_CALI
};

/* [] END OF FILE */
