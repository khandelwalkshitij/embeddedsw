/******************************************************************************
*
* Copyright (C) 2016 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file xilskey_eps_zynqmp_puf.h
* Contains the function prototypes, defines and macros for ZynqMP efusePs puf
* functionality.
*
* @note	None.
*
* </pre>
* MODIFICATION HISTORY:
*
* Ver   Who   Date     Changes
* ----- ---- -------- --------------------------------------------------------
* 6.1   vns  17/10/16 First release.
* </pre>
*
*****************************************************************************/
#ifndef __XSK_EPS_ZYNQMP_PUF_H__
#define __XSK_EPS_ZYNQMP_PUF_H__


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xilskey_eps_zynqmp.h"
#include "xilskey_utils.h"

/************************** Constant Definitions *****************************/
#if defined XSK_PUF_DEBUG
#define XSK_PUF_DEBUG_GENERAL (1U)
#else
#define XSK_PUF_DEBUG_GENERAL (0U)
#endif

#define		xPuf_printf(type,...) if ((type) == (1U)) \
				{xil_printf (__VA_ARGS__);}

#define		XSK_ZYNQMP_PUF_MODE4K			(0U)
#define		XSK_ZYNQMP_PUF_SYN_LEN			(386)

#define		XSK_ZYNQMP_PUF_REGISTRATION		(1U)

#define 	XSK_ZYNQMP_PUF_CFG0_INIT_VAL		(2U)
#define 	XSK_ZYNQMP_PUF_CFG1_INIT_VAL_4K		(0x0c230090U)

#define		XSK_ZYNQMP_EFUSEPS_PUF_TOTAL_ROWS	(128)
#define		XSK_ZYNQMP_PUF_SYN_DATA_LEN_IN_BYTES	(386)
#define		XSK_ZYNQMP_PUF_FORMATTED_SYN_DATA_LEN_IN_BYTES	(140)
#define		XSK_ZYNQMP_PUF_DBG2_DATA_LEN_IN_BYTES	(36)
#define		XSK_ZYNQMP_PUF_KEY_IV_LEN_IN_BYTES	(12)
#define		XSK_ZYNQMP_PUF_AUX_LEN_IN_BITS		(24)
#define		XSK_ZYNQMP_PUF_SHUTTER_VALUE		(0x0100005e)

/************************** Type Definitions ********************************/


/** @name xilinx eFUSE PUF secure bits
 * @{
 */
typedef enum {
	XSK_ZYNQMP_EFUSEPS_PUF_SYN_INVALID = 29,
	XSK_ZYNQMP_EFUSEPS_PUF_SYN_LOCK,
	XSK_ZYNQMP_EFUSEPS_PUF_REG_DIS
}XskEfusePS_Puf_SecureBits;
/*@}*/

/** @name contains secure bits of efuse PUF
 * @{
 */
typedef struct {
	u8 SynInvalid;
	u8 SynWrLk;
	u8 RegisterDis;
}XilSKey_Puf_Secure;
/*@}*/

/** @name PUF instance
 * @{
 */
typedef struct {
	u8 RegistrationMode;	/**< PUF Registration Mode: Always 4K Mode */
	u32 ShutterValue;	/**< PUF Shutter value */
	u32 SyndromeData[XSK_ZYNQMP_PUF_SYN_DATA_LEN_IN_BYTES];
				/**< Helper data */
	u32 EfuseSynData[XSK_ZYNQMP_PUF_FORMATTED_SYN_DATA_LEN_IN_BYTES];
				/**< Formatted Syndrome data */
	u32 Debug2Data[XSK_ZYNQMP_PUF_DBG2_DATA_LEN_IN_BYTES];
				/**< Debug 2 Data */
	u32 Chash;		/**< CHASH Value */
	u32 Aux;		/**< AUX Value */
	u8 RedKey[XSK_ZYNQMP_EFUSEPS_AES_KEY_LEN_IN_BYTES];
				/**< Red Key */
	u8 BlackKeyIV[XSK_ZYNQMP_PUF_KEY_IV_LEN_IN_BYTES];
				/**< Black key IV (IV used to encrypt the
				  *  red key using PUF key) */
	u8 BlackKey[XSK_ZYNQMP_EFUSEPS_AES_KEY_LEN_IN_BYTES];
				/**< Black Key generated */
} XilSKey_Puf;
/*@}*/

/****************************Prototypes***************************************/
u32 XilSKey_ZynqMp_EfusePs_WritePufHelprData(XilSKey_Puf *InstancePtr);
u32 XilSKey_ZynqMp_EfusePs_ReadPufHelprData(u32 *Address);

u32 XilSKey_ZynqMp_EfusePs_WritePufChash(XilSKey_Puf *InstancePtr);
u32 XilSKey_ZynqMp_EfusePs_ReadPufChash(u32 *Address, u8 ReadOption);

u32 XilSKey_ZynqMp_EfusePs_WritePufAux(XilSKey_Puf *InstancePtr);
u32 XilSKey_ZynqMp_EfusePs_ReadPufAux(u32 *Address, u8 ReadOption);

u32 XilSKey_Write_Puf_EfusePs_SecureBits(XilSKey_Puf_Secure *WriteSecureBits);
u32 XilSKey_Read_Puf_EfusePs_SecureBits(
		XilSKey_Puf_Secure *SecureBitsRead, u8 ReadOption);

u32 XilSKey_Puf_Debug2(XilSKey_Puf *InstancePtr);
u32 XilSKey_Puf_Registration(XilSKey_Puf *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif /* __XSK_EPS_ZYNQMP_HW_H__ */
