/* ========================================
 * Potensiostat
 * By Krisna Diastama
 * ========================================
*/

#include <stdio.h>
#include "project.h"
#include "potensiostat.h"
#include "serial.h"

/* Interrupt function for the timer, counts milliseconds */
uint32 millis = 0;
uint32 millisPrevious=0;
uint32 millisPrevious2=0;
CY_ISR(timerInterrupt){
    Timer_1_STATUS;
    millis++;
}

#define DACmVperBit 1
#define DACReffmVperBit 16
#define IDACuAperBit 1

int16 TIARes=20; //kohm
int16 TIACalMulti=1;
int16 TIACalDivide=1;
int16 TIACal=0; //nA
uint16 samplingPeriod=200;
int16 mVReff= 2040; 
//int16 mVReffCalibrate= -200; //mV 
int16 mVStart=0;
int16 mVEnd=0;
int16 mVStep=1;
uint16 repeat=0;
uint16 sweepSpeed=0;
uint32 msEnd=0;
uint16 halfWavePeriod=0;



int16 mVOut =0;
uint16 repeatDone=0;

int16 lutArray[9000];
int16 lutLength = 0;
void CV_config();
void LSV_config();
void CA_config();
void CALI_config();
void RANGE_config();

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* initialize interrupts*/
    isr_timer_StartEx(timerInterrupt);
    
    /* initialize components*/
    UART_Start();
    DVDAC_1_Start();
    Opamp_1_Start();
    TIA_1_Start();
    VDAC8_1_Start();
    ADC_DelSig_1_Start();
    Timer_1_Start();

    
    UART_PutString("publish pot/head potensiostat ready \r\n");
    UART_PutString("subscribe pot/cmd \r\n");
    
    
    /* initialize variables*/
    int32 result =0;
    char inputbuffer[RX_BUFFER_SIZE];
    char buffer[32]; //for sprintf purposes
    state = ST_NONE;
    cmd = NONE;
    
    
    /* Main Program loop */
    for(;;)
    {
        /* Read the UART, see serial.c for the code */
        if(serial_checkInput(inputbuffer)){
            /*  
            * ==========================================================================
            * if a line is completed, parse the line and get the command.
            * ==========================================================================
            * a input is parsed as "[cmd] cmd_params[0] cmd_params[1] ... cmd_params[5]"
            * 
            * for example: "CV -1000 1000" would set 
            * cmd = "CV", cmd_params[0] = -1000, and cmd_params[1] = 1000
            * 
            * note:
            * -List of available cmd is listed in cmd_str[] in serial.c and is called using
            * their enum listed in enum eCommand serial.h.
            * -All cmd_params are converted into integers.
            * ===========================================================================
            */
            serial_parseLine(inputbuffer);
            memset(inputbuffer,0,RX_BUFFER_SIZE);
        }
        
        /*
        ====================================================================================
        * Event Handler: respond once to commands (cmd) from serial input then reset cmd.  
        *                End every event reading with cmd = NONE or else it would activate 
        *                multiple times.                                                    
        ====================================================================================
        */
        switch(cmd){
        case IDENTIFY: // I
            UART_PutString("publish pot/head PSoC-Potensiostat, range: +-1V, +-200uA \r\n");
            UART_PutString("publish pot/head Methods: CV, LSV, CA, I, S, CALI, RG \r\n");
            cmd = NONE;
            break;
        case CV: //CV []
            UART_PutString("publish pot/head Cyclic Voltametry,Cycle,Vout(mV),Iin(nA) \r\n");
            CV_config();
            state = ST_CV;
            cmd = NONE;
            break;    
        case LSV: //LSV
            UART_PutString("publish pot/head Linier Sweep Voltametry,Cycle,Vout(mV),Iin(nA) \r\n");
            LSV_config();
            state = ST_CV;
            cmd = NONE;
            break;
        case CA: //CA
            UART_PutString("publish pot/head Chronoamperometry,Time(ms),Vout(mV),Iin(nA \r\n");
            CA_config();
            state = ST_CA;
            cmd = NONE;
            break;    
        case STOP: //S
            UART_PutString("publish pot/head STOP \r\n");
            state = ST_NONE;
            cmd = NONE;
            break;
        case CALIBRATE: //CALI
            UART_PutString("publish pot/head Calibrate,Multiplier,Divide,Offset(nA) \r\n");
            sprintf(buffer,"publish pot/head initial %i,%i,%i\r\n", TIACalMulti,TIACalDivide, TIACal);
            UART_PutString(buffer);
            CALI_config();
            sprintf(buffer,"publish pot/head changed to: %i,%i,%i\r\n", TIACalMulti,TIACalDivide, TIACal);
            UART_PutString(buffer);
            //state = ST_CALI;
            cmd = NONE;
            break;
        case RANGE:
            UART_PutString("publish pot/head Choose Current Range \r\n");
            RANGE_config();
            sprintf(buffer,"publish pot/head Range set to : %i (TIA Resistor = %ikohm)\r\n",cmd_params[0], TIARes);
            UART_PutString(buffer);
            cmd = NONE;
            break;
        }
        
        /*
        *====================================================================================
        * State Handler: runs continiously until finished, activated by setting state=ST_... 
        *                To end the state, just use state=0                                  
        *====================================================================================
        */
        switch(state){
        case ST_CV: 
            if(millis-millisPrevious > sweepSpeed){
                millisPrevious += sweepSpeed;
                static int i = 0;
                i++;
                if(i>=lutLength){
                    i = 0;
                    repeatDone+=1;
                }
                
                if(repeatDone>=repeat){
                    state =0;
                }
                mVOut = lutArray[i];
                DVDAC_1_SetValue(mVOut);
                
            }
            if(millis-millisPrevious2 > samplingPeriod){
                millisPrevious2 += samplingPeriod;
                result = ADC_DelSig_1_CountsTo_uVolts(ADC_DelSig_1_GetResult16());
                result = (-result/TIARes)*TIACalMulti/TIACalDivide+TIACal;
                sprintf(buffer,"publish pot/data %i,%i,%li\r\n", repeatDone, ((mVOut*DACmVperBit)-mVReff), result);
                UART_PutString(buffer);
            }
            break; 
        case ST_CA:
            if(millis-millisPrevious > halfWavePeriod){
                millisPrevious+=halfWavePeriod;
                static int i = 0;
                if(i == 0){
                    mVOut= mVEnd;
                    i = 1;
                }
                else{
                    mVOut= mVStart;
                    i = 0;
                }
                DVDAC_1_SetValue(mVOut);
                
                //Stop when end time reached
                if(millis>msEnd){
                    DVDAC_1_SetValue(mVReff/DACmVperBit);
                    state =0;
                }
            }
            if(millis-millisPrevious2 > samplingPeriod){
                millisPrevious2+=samplingPeriod;
                result = ADC_DelSig_1_CountsTo_uVolts(ADC_DelSig_1_GetResult16());
                result = (-result/TIARes)*TIACalMulti/TIACalDivide+TIACal;
                sprintf(buffer,"publish pot/data %lu,%i,%li\r\n", millis,((mVOut*DACmVperBit)-mVReff), result);
                UART_PutString(buffer);
            }  
            break;
        case ST_CALI:
            break;
        }
    }
}

/*====================================================================================*/
/* Event Handling Functions: Things to do when a event is called                      */
/*====================================================================================*/

void CV_config(){
    mVStart = (cmd_params[0]+mVReff)/DACmVperBit;
    mVEnd = (cmd_params[1]+mVReff)/DACmVperBit;
    sweepSpeed = 1000/(cmd_params[2]);
    samplingPeriod = 50;
    repeat = cmd_params[3];
    mVStep = 1/DACmVperBit;
    if(mVStart>4000){mVStart=4000;}
    if(mVStart<0){mVStart=0;}
    if(mVEnd>4000){mVEnd=4000;}
    if(mVEnd<0){mVEnd=0;}
    if(samplingPeriod <1){samplingPeriod=1;}
    if(sweepSpeed <1){sweepSpeed=1;}
    if(repeat <1){repeat=1;}
    if(mVStep <1){mVStep=1;}
    mVOut = mVStart;
    repeatDone = 0;
    
    /*create lookup table for output*/
    /*cyclic voltametry output is a triangular wave*/
    int i = mVStart;
    int j =0;
    if(mVStart<=mVEnd){
        while(i<=mVEnd){
           lutArray[j]=i;
           i+=mVStep;
           j++;
        }
        while(i>=mVStart){
           lutArray[j]=i;
           i-=mVStep;
           j++;
        }
        lutLength =j;
    } else{
        while(i>=mVEnd){
           lutArray[j]=i;
           i-=mVStep;
           j++;
        }
        while(i<=mVStart){
           lutArray[j]=i;
           i+=mVStep;
           j++;
        }
        lutLength =j;
    }
    VDAC8_1_SetValue(mVReff/DACReffmVperBit);
    DVDAC_1_SetValue(mVOut);
    ADC_DelSig_1_StartConvert();
    millis = 0;
    millisPrevious = 0;
    millisPrevious2 = 0;
}

void LSV_config(){
    mVStart = (cmd_params[0]+mVReff)/DACmVperBit;
    mVEnd = (cmd_params[1]+mVReff)/DACmVperBit;
    sweepSpeed = 1000/cmd_params[2];
    samplingPeriod = 50;
    repeat = cmd_params[3];
    mVStep = 1/DACmVperBit;
    if(mVStart>4000){mVStart=4000;}
    if(mVStart<0){mVStart=0;}
    if(mVEnd>4000){mVEnd=4000;}
    if(mVEnd<0){mVEnd=0;}
    if(samplingPeriod <1){samplingPeriod=1;}
    if(sweepSpeed <1){sweepSpeed=1;}
    if(repeat <1){repeat=1;}
    if(mVStep <1){mVStep=1;}
    mVOut = mVStart;
    repeatDone = 0;
    
    /*create lookup table for output*/
    /*cyclic voltametry output is a triangular wave*/
    int i = mVStart;
    int j =0;
    if(mVStart<=mVEnd){
        while(i<=mVEnd){
           lutArray[j]=i;
           i+=mVStep;
           j++;
        }
        lutLength =j;
    } else{
        while(i>=mVEnd){
           lutArray[j]=i;
           i-=mVStep;
           j++;
        }
        lutLength =j;
    }
    VDAC8_1_SetValue(mVReff/DACReffmVperBit);
    DVDAC_1_SetValue(mVOut);
    ADC_DelSig_1_StartConvert();
    millis = 0;
    millisPrevious = 0;
    millisPrevious2 = 0;
}

void CA_config(){
    mVStart = (cmd_params[0]+mVReff)/DACmVperBit;
    mVEnd = (cmd_params[1]+mVReff)/DACmVperBit;
    samplingPeriod = 10;
    msEnd = cmd_params[2];
    halfWavePeriod = 500/cmd_params[3];
    if(mVStart>4000){mVStart=4000;}
    if(mVStart<0){mVStart=0;}
    if(mVEnd>4000){mVEnd=4000;}
    if(mVEnd<0){mVEnd=0;}
    if(samplingPeriod <1){samplingPeriod=1;}
    if(halfWavePeriod <1){halfWavePeriod=1;}
    
    mVOut = mVStart;
    VDAC8_1_SetValue(mVReff/DACReffmVperBit);
    DVDAC_1_SetValue(mVOut);
    ADC_DelSig_1_StartConvert();
    millis = 0;
    millisPrevious = 0;
    millisPrevious2 = 0;
}

void CALI_config(){
    TIACalMulti=cmd_params[0];
    TIACalDivide=cmd_params[1];
    TIACal=cmd_params[2];
    
    VDAC8_1_SetValue(mVReff/DACReffmVperBit);
    
}

void RANGE_config(){
    switch(cmd_params[0]){
        case 100:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_20K);
            TIARes = 20;
            break;
        case 50:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_40K);
            TIARes = 40;
            break;
        case 25:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_80K);
            TIARes = 80;
            break;
        case 16:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_120K);
            TIARes = 120;
            break;
        case 8:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_250K);
            TIARes = 250;
            break;
        case 4:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_500K);
            TIARes = 500;
            break;
        case 2:
            TIA_1_SetResFB(TIA_1_RES_FEEDBACK_1000K);
            TIARes = 1000;
            break;
        default:
            UART_PutString("publish pot/head Range not available! try 100,50,25,16,8,4,or 2!");
            return;
            break;
    }
}
/* [] END OF FILE */
