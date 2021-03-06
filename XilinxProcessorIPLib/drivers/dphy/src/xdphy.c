/******************************************************************************
*
* Copyright (C) 2015 - 2016 Xilinx, Inc. All rights reserved.
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
*
* @file xdphy.c
* @addtogroup dphy_v1_0
* @{
*
* This file implements the functions to control and get info from the DPHY.
*
* <pre>
* MODIFICATION HISTORY:
* Ver Who Date     Changes
* --- --- -------- ------------------------------------------------------------
* 1.0 vsa 07/08/15 Initial release
* 1.1 sss 08/17/16 Added 64 bit support
* </pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xstatus.h"
#include "xdphy.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/************************** Macros Definitions *****************************/
#define XDPHY_SOFTRESET_TIMEOUT 	5000UL

/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/****************************************************************************/
/**
* Initialize the XDphy instance provided by the caller based on the
* given Config structure.
*
* @param 	InstancePtr is the XDphy instance to operate on.
* @param 	CfgPtr is the device configuration structure containing
*  		information about a specific DPhy instance.
* @param	EffectiveAddr is the base address of the device. If address
*		translation is being used, then this parameter must reflect the
*		virtual base address. Otherwise, the physical address should be
*		used.
* @return
* 		- XST_SUCCESS Initialization was successful.
*
* @note		None.
*****************************************************************************/
u32 XDphy_CfgInitialize(XDphy *InstancePtr, XDphy_Config *CfgPtr,
						UINTPTR EffectiveAddr)
{
	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(CfgPtr != NULL);
	Xil_AssertNonvoid(EffectiveAddr != 0);

	/* Setup the instance */
	InstancePtr->Config = *CfgPtr;

	InstancePtr->Config.BaseAddr = EffectiveAddr;

	InstancePtr->IsReady = (XIL_COMPONENT_IS_READY);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
* Configure the registers of the Dphy instance
*
* @param 	InstancePtr is the XDphy instance to operate on.
* @param 	Handle to one of the registers to be configured
* @param	Value to be set for the particular Handle of the DPHY instance
*
* @return
* 		- XST_SUCCESS on successful register update.
* 		- XST_FAILURE If incorrect handle was passed
*
* @note		There is a limit on the minimum and maximum values of
*		the HS Timeout register.
*
*****************************************************************************/
u32 XDphy_Configure(XDphy *InstancePtr, u8 Handle, u32 Value)
{
	u32 Status = XST_SUCCESS;

	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Handle <= XDPHY_HANDLE_MAX);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);

	/* Based on the Handle, write Value to the specific register */
	switch (Handle) {
		case XDPHY_HANDLE_IDELAY:
			Value &= (XDPHY_HSEXIT_IDELAY_REG_READY_MASK |
					XDPHY_HSEXIT_IDELAY_REG_TAP_MASK);
			XDphy_WriteReg((InstancePtr)->Config.BaseAddr,
					XDPHY_HSEXIT_IDELAY_REG_OFFSET, Value);
			break;

		case XDPHY_HANDLE_INIT_TIMER:
			XDphy_WriteReg((InstancePtr)->Config.BaseAddr,
					XDPHY_INIT_REG_OFFSET, Value);
			break;
		case XDPHY_HANDLE_WAKEUP:
			XDphy_WriteReg((InstancePtr)->Config.BaseAddr,
					XDPHY_WAKEUP_REG_OFFSET, Value);
			break;

		case XDPHY_HANDLE_HSTIMEOUT:
			if (InstancePtr->Config.EnableTimeOutRegs != 0) {
				XDphy_WriteReg((InstancePtr)->Config.BaseAddr,
						XDPHY_HSTIMEOUT_REG_OFFSET,
						Value);
			}
			else {
				Status = XST_FAILURE;
			}
			break;

		case XDPHY_HANDLE_ESCTIMEOUT:
			Xil_AssertNonvoid(Value >= XDPHY_HS_TIMEOUT_MIN_VALUE);
			Xil_AssertNonvoid(Value <= XDPHY_HS_TIMEOUT_MAX_VALUE);

			if (InstancePtr->Config.EnableTimeOutRegs != 0) {
				XDphy_WriteReg((InstancePtr)->Config.BaseAddr,
						XDPHY_ESCTIMEOUT_REG_OFFSET,
						Value);
			}
			else {
				Status = XST_FAILURE;
			}
			break;

		case XDPHY_HANDLE_CLKLANE:
		case XDPHY_HANDLE_DLANE0:
		case XDPHY_HANDLE_DLANE1:
		case XDPHY_HANDLE_DLANE2:
		case XDPHY_HANDLE_DLANE3:
			Status = XST_FAILURE;
			break;

		default:
			break;
	}

	return Status;
}

/****************************************************************************/
/**
* Get if register interface is present from the config structure for specified
* DPHY instance.
*
* @param 	InstancePtr is the XDphy instance to operate on.
*
* @return
* 		- 1 if register interface is present
* 		- 0 if register interface is absent
*
* @note		None.
*****************************************************************************/
u8 XDphy_GetRegIntfcPresent(XDphy *InstancePtr)
{
	/* Verify argument */
	Xil_AssertNonvoid(InstancePtr != NULL);

	return (InstancePtr->Config.IsRegisterPresent);
}

/****************************************************************************/
/**
* Get information stored in the DPhy instance based on the handle passed
*
* @param 	InstancePtr is the XDphy instance to operate on.
* @param 	Handle to one of the registers to be configured
*
* @return 	The value stored in the corresponding register
*
* @note		None.
*****************************************************************************/
u32 XDphy_GetInfo(XDphy *InstancePtr, u8 Handle)
{
	u32 RegVal = 0;

	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Handle <= XDPHY_HANDLE_MAX)
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);

	/* Based on Handle, return value from the corresponding registers */
	switch (Handle) {
		case XDPHY_HANDLE_IDELAY:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_HSEXIT_IDELAY_REG_OFFSET);
			break;
		case XDPHY_HANDLE_INIT_TIMER:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_INIT_REG_OFFSET);
			break;
		case XDPHY_HANDLE_WAKEUP:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_WAKEUP_REG_OFFSET);
			break;
		case XDPHY_HANDLE_HSTIMEOUT:
			/* If the Timeout Registers are disable */
			if (InstancePtr->Config.EnableTimeOutRegs == 0) {
				RegVal = InstancePtr->Config.HSTimeOut;
			}
			else {
				RegVal = XDphy_ReadReg((InstancePtr)->Config.\
						BaseAddr,
						XDPHY_HSTIMEOUT_REG_OFFSET);
			}
			break;
		case XDPHY_HANDLE_ESCTIMEOUT:
			/* If the Timeout Registers are disable */
			if (InstancePtr->Config.EnableTimeOutRegs == 0) {
				RegVal = InstancePtr->Config.EscTimeout;
			}
			else {
				RegVal = XDphy_ReadReg((InstancePtr)->Config.\
						BaseAddr,
						XDPHY_ESCTIMEOUT_REG_OFFSET);
			}
			break;
		case XDPHY_HANDLE_CLKLANE:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_CLSTATUS_REG_OFFSET);
			break;
		case XDPHY_HANDLE_DLANE0:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_DL0STATUS_REG_OFFSET);
			break;
		case XDPHY_HANDLE_DLANE1:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_DL1STATUS_REG_OFFSET);
			break;
		case XDPHY_HANDLE_DLANE2:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_DL2STATUS_REG_OFFSET);
			break;
		case XDPHY_HANDLE_DLANE3:
			RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
						 XDPHY_DL3STATUS_REG_OFFSET);
			break;
		default:
			break;
	}

	return RegVal;
}

/****************************************************************************/
/**
* This is used to do a soft reset of the DPhy IP instance.
* The reset takes approx 20 core clock cycles to become effective.
*
* @param 	InstancePtr is the XDphy instance to operate on.
*
* @return 	None
*
* @note		None.
*****************************************************************************/
void XDphy_Reset(XDphy *InstancePtr)
{
	u32 Value = XDPHY_SOFTRESET_TIMEOUT;
	u32 RegVal;

	/* Verify arguments */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->Config.IsRegisterPresent != 0);

	/* Set the Reset bit in Control Register */
	RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
			XDPHY_CTRL_REG_OFFSET);

	RegVal |= XDPHY_CTRL_REG_SOFTRESET_MASK;

	XDphy_WriteReg((InstancePtr)->Config.BaseAddr, XDPHY_CTRL_REG_OFFSET, RegVal);

	InstancePtr->IsReady = 0;

	/* Wait for at least 20 core clock cycles for reset to occur */
	while (Value--) {
	};

	/* Clear the reset bit */
	RegVal = XDphy_ReadReg((InstancePtr)->Config.BaseAddr,
			XDPHY_CTRL_REG_OFFSET);

	RegVal &= ~XDPHY_CTRL_REG_SOFTRESET_MASK;

	XDphy_WriteReg((InstancePtr)->Config.BaseAddr, XDPHY_CTRL_REG_OFFSET, RegVal);

	/* Mark instance to be ready to be used */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;
}

/****************************************************************************/
/**
* This is used to clear the Abort Error (Escape or High Speed) bits
* in the Data Lane 0 through 3
*
* @param 	InstancePtr is the XDphy instance to operate on.
* @param 	DataLane represents which Data Lane to act upon
* @param 	Mask contains information about which bits to reset
*
* @return 	None
*
* @note     	None.
*****************************************************************************/
void XDphy_ClearDataLane(XDphy *InstancePtr, u8 DataLane, u32 Mask)
{
	/* Verify arguments */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->Config.IsRegisterPresent != 0);
	Xil_AssertVoid(DataLane < InstancePtr->Config.MaxLanesPresent);
	Xil_AssertVoid((Mask & (XDPHY_DLXSTATUS_REG_ESCABRT_MASK |
			XDPHY_DLXSTATUS_REG_HSABRT_MASK)) != 0);

	/* Ensure only Escape Abort or HS Abort are set */
	Mask = Mask & (XDPHY_DLXSTATUS_REG_ESCABRT_MASK |
			XDPHY_DLXSTATUS_REG_HSABRT_MASK);

	XDphy_WriteReg(InstancePtr->Config.BaseAddr,
			XDPHY_DL0STATUS_REG_OFFSET + (DataLane * 4), Mask);
}

/****************************************************************************/
/**
* This is used to get information about Clock Lane status
*
* @param 	InstancePtr is the XDphy instance to operate on.
*
* @return 	Bitmask containing which of the events have occured along with
* 		the mode of the Clock Lane in DPhy
*
* @note 	None.
*****************************************************************************/
u32 XDphy_GetClkLaneStatus(XDphy *InstancePtr)
{
	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);

	return (XDphy_ReadReg(InstancePtr->Config.BaseAddr,
				XDPHY_CLSTATUS_REG_OFFSET));

}

/****************************************************************************/
/**
* This is used to get specific Lane mode information about Clock Lane.
*
* @param 	InstancePtr is the XDphy instance to operate on.
*
* @return 	Bitmask containing mode in which the Clock Lane in DPhy is in.
*
* @note		None.
*****************************************************************************/
u32 XDphy_GetClkLaneMode(XDphy *InstancePtr)
{
	u32 Value;

	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);

	Value = XDphy_GetClkLaneStatus(InstancePtr);

	return (Value & XDPHY_CLSTATUS_REG_MODE_MASK);
}

/****************************************************************************/
/**
* This is used to get information about a Data Lane status
*
* @param	InstancePtr is the XDphy instance to operate on.
* @param	DataLane for which the status is sought for.
*
* @return	Bitmask containing which of the events have occured along with
* 		the mode of the Data Lane in DPhy
*
* @note		None.
*****************************************************************************/
u32 XDphy_GetDataLaneStatus(XDphy *InstancePtr, u8 DataLane)
{
	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);
	Xil_AssertNonvoid(DataLane < InstancePtr->Config.MaxLanesPresent);

	return (XDphy_ReadReg(InstancePtr->Config.BaseAddr,
				(XDPHY_DL0STATUS_REG_OFFSET + (4 * DataLane))));
}

/****************************************************************************/
/**
* This is used to get specfic Lane mode information about a Data Lane.
*
* @param	InstancePtr is the XDphy instance to operate on.
* @param	DataLane for which the mode info is requested.
*
* @return	Bitmask containing mode in which the Data Lane in DPhy is in.
*
* @note		None.
*****************************************************************************/
u32 XDphy_GetDataLaneMode(XDphy *InstancePtr, u8 DataLane)
{
	u32 Value;

	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);
	Xil_AssertNonvoid(DataLane < InstancePtr->Config.MaxLanesPresent);

	Value = XDphy_GetDataLaneStatus(InstancePtr, DataLane);

	return (Value & XDPHY_DLXSTATUS_REG_MODE_MASK);
}

/****************************************************************************/
/**
* This is used to get count of packets received on each lane
*
* @param	InstancePtr is the XDphy instance to operate on.
* @param	DataLane for which the mode info is requested.
*
* @return	Bitmask containing mode in which the Data Lane in DPhy is in.
*
* @note		None.
*
*****************************************************************************/
u16 XDphy_GetPacketCount(XDphy *InstancePtr, u8 DataLane)
{
	u32 Value;

	/* Verify arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.IsRegisterPresent != 0);
	Xil_AssertNonvoid(DataLane < InstancePtr->Config.MaxLanesPresent);

	Value = (XDphy_ReadReg(InstancePtr->Config.BaseAddr,
				(XDPHY_DL0STATUS_REG_OFFSET + (4 * DataLane))));
	Value = Value & XDPHY_DLXSTATUS_REG_PACKETCOUNT_MASK;
	Value >>= XDPHY_DLXSTATUS_REG_PACKCOUNT_OFFSET;

	return (u16)Value;
}

/****************************************************************************/
/**
* This function is used to enable or disable the DPhy core.
*
* @param	InstancePtr is the XDphy instance to operate on.
* @param	Flag denoting whether to enable or disable the DPhy core
*
* @return	None.
*
* @note 	None.
*****************************************************************************/
void XDphy_Activate(XDphy *InstancePtr, u8 Flag)
{
	u32 Value;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->Config.IsRegisterPresent != 0);

	Value = XDphy_ReadReg(InstancePtr->Config.BaseAddr,
				XDPHY_CTRL_REG_OFFSET);

	if (Flag == XDPHY_ENABLE_FLAG) {
		Value |= XDPHY_CTRL_REG_DPHYEN_MASK;
	}
	else if (Flag == XDPHY_DISABLE_FLAG) {
		Value &= ~(XDPHY_CTRL_REG_DPHYEN_MASK);
	}

	/* Set or reset the DPhy Enable bit in Control Register based
	 * on the flag
	 */
	XDphy_WriteReg(InstancePtr->Config.BaseAddr,
			XDPHY_CTRL_REG_OFFSET, Value);
}
/** @} */
