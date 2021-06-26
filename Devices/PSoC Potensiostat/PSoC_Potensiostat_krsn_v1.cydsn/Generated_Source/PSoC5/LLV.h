/*******************************************************************************
* File Name: LLV.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LLV_H) /* Pins LLV_H */
#define CY_PINS_LLV_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LLV_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LLV__PORT == 15 && ((LLV__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LLV_Write(uint8 value);
void    LLV_SetDriveMode(uint8 mode);
uint8   LLV_ReadDataReg(void);
uint8   LLV_Read(void);
void    LLV_SetInterruptMode(uint16 position, uint16 mode);
uint8   LLV_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LLV_SetDriveMode() function.
     *  @{
     */
        #define LLV_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LLV_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LLV_DM_RES_UP          PIN_DM_RES_UP
        #define LLV_DM_RES_DWN         PIN_DM_RES_DWN
        #define LLV_DM_OD_LO           PIN_DM_OD_LO
        #define LLV_DM_OD_HI           PIN_DM_OD_HI
        #define LLV_DM_STRONG          PIN_DM_STRONG
        #define LLV_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LLV_MASK               LLV__MASK
#define LLV_SHIFT              LLV__SHIFT
#define LLV_WIDTH              1u

/* Interrupt constants */
#if defined(LLV__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LLV_SetInterruptMode() function.
     *  @{
     */
        #define LLV_INTR_NONE      (uint16)(0x0000u)
        #define LLV_INTR_RISING    (uint16)(0x0001u)
        #define LLV_INTR_FALLING   (uint16)(0x0002u)
        #define LLV_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LLV_INTR_MASK      (0x01u) 
#endif /* (LLV__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LLV_PS                     (* (reg8 *) LLV__PS)
/* Data Register */
#define LLV_DR                     (* (reg8 *) LLV__DR)
/* Port Number */
#define LLV_PRT_NUM                (* (reg8 *) LLV__PRT) 
/* Connect to Analog Globals */                                                  
#define LLV_AG                     (* (reg8 *) LLV__AG)                       
/* Analog MUX bux enable */
#define LLV_AMUX                   (* (reg8 *) LLV__AMUX) 
/* Bidirectional Enable */                                                        
#define LLV_BIE                    (* (reg8 *) LLV__BIE)
/* Bit-mask for Aliased Register Access */
#define LLV_BIT_MASK               (* (reg8 *) LLV__BIT_MASK)
/* Bypass Enable */
#define LLV_BYP                    (* (reg8 *) LLV__BYP)
/* Port wide control signals */                                                   
#define LLV_CTL                    (* (reg8 *) LLV__CTL)
/* Drive Modes */
#define LLV_DM0                    (* (reg8 *) LLV__DM0) 
#define LLV_DM1                    (* (reg8 *) LLV__DM1)
#define LLV_DM2                    (* (reg8 *) LLV__DM2) 
/* Input Buffer Disable Override */
#define LLV_INP_DIS                (* (reg8 *) LLV__INP_DIS)
/* LCD Common or Segment Drive */
#define LLV_LCD_COM_SEG            (* (reg8 *) LLV__LCD_COM_SEG)
/* Enable Segment LCD */
#define LLV_LCD_EN                 (* (reg8 *) LLV__LCD_EN)
/* Slew Rate Control */
#define LLV_SLW                    (* (reg8 *) LLV__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LLV_PRTDSI__CAPS_SEL       (* (reg8 *) LLV__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LLV_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LLV__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LLV_PRTDSI__OE_SEL0        (* (reg8 *) LLV__PRTDSI__OE_SEL0) 
#define LLV_PRTDSI__OE_SEL1        (* (reg8 *) LLV__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LLV_PRTDSI__OUT_SEL0       (* (reg8 *) LLV__PRTDSI__OUT_SEL0) 
#define LLV_PRTDSI__OUT_SEL1       (* (reg8 *) LLV__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LLV_PRTDSI__SYNC_OUT       (* (reg8 *) LLV__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LLV__SIO_CFG)
    #define LLV_SIO_HYST_EN        (* (reg8 *) LLV__SIO_HYST_EN)
    #define LLV_SIO_REG_HIFREQ     (* (reg8 *) LLV__SIO_REG_HIFREQ)
    #define LLV_SIO_CFG            (* (reg8 *) LLV__SIO_CFG)
    #define LLV_SIO_DIFF           (* (reg8 *) LLV__SIO_DIFF)
#endif /* (LLV__SIO_CFG) */

/* Interrupt Registers */
#if defined(LLV__INTSTAT)
    #define LLV_INTSTAT            (* (reg8 *) LLV__INTSTAT)
    #define LLV_SNAP               (* (reg8 *) LLV__SNAP)
    
	#define LLV_0_INTTYPE_REG 		(* (reg8 *) LLV__0__INTTYPE)
#endif /* (LLV__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LLV_H */


/* [] END OF FILE */
