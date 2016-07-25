#include <madCopyright.h>

/********************************************************************************
* madDrvConfig.h
*
* DESCRIPTION:
*       Includes driver level configuration and initialization function.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __madDrvConfigh
#define __madDrvConfigh

#include <madApi.h>
#include <madDrvPhyRegs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* madDriverConfig
*
* DESCRIPTION:
*       This function initializes the driver level of the quarterDeck software.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success, or
*       MAD_OUT_OF_CPU_MEM   - if failed to allocate CPU memory,
*       MAD_FAIL             - otherwise.
*
* COMMENTS:
*       1.  This function should perform the following:
*           -   Initialize the global switch configuration structure.
*           -   Initialize Mii Interface
*           -   Set the CPU port into trailer mode (Ingress and Egress).
*
*******************************************************************************/
MAD_STATUS madDriverConfig(IN MAD_DEV *dev);


/*******************************************************************************
* madPagedAccessStart
*
* DESCRIPTION:
*       This function stores page register and Auto Reg Selection mode if needed.
*
* INPUTS:
*       hwPort   - port number where the Phy is connected
*       pageType - type of the page registers
*
* OUTPUTS:
*       autoOn  - MAD_TRUE if Auto Reg Selection enabled, MAD_FALSE otherwise.
*       pageReg - Page Register Data
*
* RETURNS:
*       MAD_OK   - if success
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPagedAccessStart
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_U8      pageType,
    OUT MAD_BOOL    *autoOn,
    OUT MAD_U16     *pageReg
);


/*******************************************************************************
* madPagedAccessStop
*
* DESCRIPTION:
*       This function restores page register and Auto Reg Selection mode if needed.
*
* INPUTS:
*       hwPort   - port number where the Phy is connected
*       pageType - type of the page registers
*       autoOn   - MAD_TRUE if Auto Reg Selection enabled, MAD_FALSE otherwise.
*       pageReg  - Page Register Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK 	- if success
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPagedAccessStop
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_U8      pageType,
    IN  MAD_BOOL    autoOn,
    IN  MAD_U16     pageReg
);


/*******************************************************************************
* madFindPhyInformation
*
* DESCRIPTION:
*       This function gets feature set of the given PHY, such as VCT capability
*       adn register type, DTE capability and register type, Packet Generator
*       capability and register type, etc.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       phyInfo - Phy Feature Set
*
* RETURNS:
*       MAD_OK   - if the given is the known Marvell Phy,
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madFindPhyInformation
(
    IN  MAD_DEV     *dev,
    OUT MAD_PHY_INFO    *phyInfo
);

#ifdef __cplusplus
}
#endif

#endif /* __madDrvConfigh */
