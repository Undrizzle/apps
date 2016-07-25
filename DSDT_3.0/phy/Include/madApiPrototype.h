#include <madCopyright.h>

/********************************************************************************
* madApiPrototype.h
*
* DESCRIPTION:
*       API Prototypes for Marvell Alaska Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __madApiPrototype_h
#define __madApiPrototype_h

#ifdef __cplusplus
extern "C" {
#endif

#define IN          

#define OUT        


/* madSysConfig.c */

/*******************************************************************************
* mdLoadDriver
*
* DESCRIPTION:
*       Marvell Alaska Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this Marvell Phy
*       device. This pointer (*dev) is then used for all the API functions. 
*       The following is the job performed by this routine:
*           1. store SMI read/write function into the given MAD_DEV structure
*           2. look for the Marvell Device
*           3. run Device specific initialization routine
*           4. disable Marvell Device interrupt
*           5. create semaphore if required
*
* INPUTS:
*       sysCfg      - Holds system configuration parameters.
*
* OUTPUTS:
*       dev         - Holds general system information.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL             - on error
*
* COMMENTS:
*     madUnloadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
MAD_STATUS mdLoadDriver
(
    IN  MAD_SYS_CONFIG   *sysCfg,
    OUT MAD_DEV    *dev
);


/*******************************************************************************
* mdUnloadDriver
*
* DESCRIPTION:
*       This function frees semaphore created by Marvell Alaska Driver,
*       disables Device interrupt, and clears MAD_DEV structure.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK           - on success
*       MAD_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           madLoadDriver().
*
*******************************************************************************/
MAD_STATUS mdUnloadDriver
(
    IN MAD_DEV* dev
);


/* madSysCtrl.c */

/*******************************************************************************
* mdGetLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status of the current active media.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       linkOn - MAD_TRUE if link is established, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Reg 17, Bit 10 : Link
*
*******************************************************************************/
MAD_STATUS mdGetLinkStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *linkOn
);


/*******************************************************************************
* mdGetSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current Speed  of the current active media.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current speed (MAD_SPEED_MODE).
*               MAD_SPEED_10M, for 10 Mbps
*               MAD_SPEED_100M, for 100 Mbps
*               MAD_SPEED_1000M, for 1000 Mbps
*               MAD_SPEED_UNKNOWN, when speed is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Reg 17, Bit 15:14,11 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdGetSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
);

/*******************************************************************************
* mdGetMacSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current SGMII Mac Speed.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current speed (MAD_SPEED_MODE).
*               MAD_SPEED_10M, for 10 Mbps
*               MAD_SPEED_100M, for 100 Mbps
*               MAD_SPEED_1000M, for 1000 Mbps
*               MAD_SPEED_UNKNOWN, when speed is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Reg 21 Page2, Bit 2,1,0 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdGetMacSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
);

/*******************************************************************************
* mdGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrieves the current Duplex of the current active media. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current duplex mode (MAD_DUPLEX_MODE).
*               MAD_HALF_DUPLEX, for half duplex mode
*               MAD_FULL_DUPLEX, for half duplex mode
*               MAD_DUPLEX_UNKNOWN, when duplex is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Reg 17, Bit 13,11 : Duplex, Duplex resolved
*
*******************************************************************************/
MAD_STATUS mdGetDuplexStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_DUPLEX_MODE *mode
);

/*******************************************************************************
* mdSysGetPhyReg
*
* DESCRIPTION:
*       This function reads a phy register of the current page.
*       This API is provided for debugging or general use.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    regAddr,
    OUT MAD_U32    *data
);


/*******************************************************************************
* mdSysSetPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register of the current page..
*       This API is provided for debugging or general use.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    regAddr,
    IN  MAD_U32    data
);


/*******************************************************************************
* mdSysGetPagedPhyReg
*
* DESCRIPTION:
*       This function reads a phy register in paged mode.
*       This API is provided for debugging or general use.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    pageNum,
    IN  MAD_U32    regAddr,
    OUT MAD_U32    *data
);


/*******************************************************************************
* mdSysSetPagedPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register in paged mode.
*       This API is provided for debugging or general use.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    pageNum,
    IN  MAD_U32    regAddr,
    IN  MAD_U32    data
);


/*******************************************************************************
* mdSysSetPhyEnable
*
* DESCRIPTION:
*       This function enables a phy by powering up.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en      - MAD_TRUE to enable or MAD_FALSE to disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetPhyEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdSysGetPhyEnable
*
* DESCRIPTION:
*       This function can be use to check if phy is powered up.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE if enabled, or MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPhyEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);

/*******************************************************************************
* mdSysSetQSGMIIPwDown
*
* DESCRIPTION:
*       This function set Global QSGMII power down.
*
* INPUTS:
*       port - Port number to be configured.
*       en      - MAD_TRUE to power down or MAD_FALSE to power up
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetQSGMIIPwDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdSysGetQSGMIIPwDown
*
* DESCRIPTION:
*       This function get Global QSGMII power down statue
*
* INPUTS:
*       port - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE to power down or MAD_FALSE to power up
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetQSGMIIPwDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);

/*******************************************************************************
* mdSysSetMacPowerDown
*
* DESCRIPTION:
*       This function enables Mac Interface of the phy by powering up.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en      - MAD_TRUE to enable or MAD_FALSE to disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetMacPowerDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdSysGetMacPowerDown
*
* DESCRIPTION:
*       This function can be use to check if Mac interface is powered up.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE if enabled, or MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetMacPowerDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);

/*******************************************************************************
* mdSysSGtDetectPowerDownModeSt
*
* DESCRIPTION:
*       This function get status of Energe Detect Power down Modes.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       status - 1 if active, 0 if sleep
*       change - 1 if enege detect state changed, 0 if no changed
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetDetectPowerDownModeSt
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16      *status,
    OUT MAD_U16      *change
);

/*******************************************************************************
* mdSysSetDetectPowerDownMode
*
* DESCRIPTION:
*       This function set modes of Energe Detect Power down.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mode - MAD_ENERGE_DETECT_MODE_NO if normal, 
*               For chip group 30xx
*              MAD_ENERGE_DETECT_MODE_YES if Setting mode, 
*               For chip group 1xxx.
*              MAD_ENERGE_DETECT_MODE_1 if mode1, 
*              MAD_ENERGE_DETECT_MODE_2 if mode0
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetDetectPowerDownMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN MAD_U16      mode
);

/*******************************************************************************
* mdSysGetDetectPowerDownMode
*
* DESCRIPTION:
*       This function get modes of Energe Detect Power down.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       mode - MAD_ENERGE_DETECT_MODE_NO if normal, 
*               For chip group 30xx
*              MAD_ENERGE_DETECT_MODE_YES if Setting mode, 
*               For chip group 1xxx.
*              MAD_ENERGE_DETECT_MODE_1 if mode1, 
*              MAD_ENERGE_DETECT_MODE_2 if mode0
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetDetectPowerDownMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *mode
);

/*******************************************************************************
* mdSysSetSnoopCtrl
*
* DESCRIPTION:
*       This function set modes of Snooping Control.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mode -    MAD_SNOOPING_OFF        
*                MAD_SNOOPING_FROM_NET
*                MAD_SNOOPING_FROM_MAC

*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetSnoopCtrl
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN    MAD_U16     mode
);

/*******************************************************************************
* mdSysGetSnoopCtrl
*
* DESCRIPTION:
*       This function get modes of Snooping Control.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       mode -    MAD_SNOOPING_OFF        
*                MAD_SNOOPING_FROM_NET
*                MAD_SNOOPING_FROM_MAC
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetSnoopCtrl
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *mode
);

/*******************************************************************************
* mdSysGetLinkDisconnectCounter
*
* DESCRIPTION:
*       This function get Link Disconnect counter.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       count -    
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLinkDisconnectCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *count
);
/*******************************************************************************
* mdSysGetLateCollision
*
* DESCRIPTION:
*       This function get Duplex Mismatch indicator: Late collision 97-128 bytes.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       count -    MAD_LATE_COLLISION_COUNTER:
*                count_65;
*                count_97;
*                count_129;
*                count_192;
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLateCollision
(
    IN  MAD_DEV                        *dev,
    IN  MAD_LPORT                    port,
    OUT MAD_LATE_COLLISION_COUNTER  *count
);

/*******************************************************************************
* mdSysSetLateCollision_winAdj
*
* DESCRIPTION:
*       This function set Late collision Window adjust.
*
* INPUTS:
*       portNum - Port number to be configured.
*       adj -    Number of bytes ro advance in late collision window.
*                0 start at 64 byte, 1 = start at 63rd byte... etc.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetLateCollision_winAdj
(
    IN  MAD_DEV        *dev,
    IN  MAD_LPORT    port,
    IN MAD_U8        adj
);

/*******************************************************************************
* mdSysGetLateCollision_winAdj
*
* DESCRIPTION:
*       This function get Late collision Window adjust.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       adj -    Number of bytes ro advance in late collision window.
*                0 start at 64 byte, 1 = start at 63rd byte... etc.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLateCollision_winAdj
(
    IN  MAD_DEV        *dev,
    IN  MAD_LPORT    port,
    OUT MAD_U8        *adj
);

/*******************************************************************************
* mdSysSetLEDCtrl
*
* DESCRIPTION:
*       This function set LED Control Data.
*
* INPUTS:
*       portNum - Port number to be configured.
*       ledData - ledCtrl[6];
*                  ledPolarity[6];
*                  ledMixPercen0;
*                  ledMixPercen1;
*                  ledForceInt;
*                  ledPulseDuration;
*                  ledBlinkRate;
*                  ledSppedOffPulse_perio;
*                  ledSppedOnPulse_perio;
*                  ledFuncMap3;
*                  ledFuncMap2;
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetLEDCtrl
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN    MAD_LED_CTRL_DATA   *ledData
);

/*******************************************************************************
* mdSysGetLEDCtrl
*
* DESCRIPTION:
*       This function Get LED Control Data.
*
* INPUTS:
*       hwPort - Port number to be configured.
*
* OUTPUTS:
*       ledData - ledCtrl[6];
*                  ledPolarity[6];
*                  ledMixPercen0;
*                  ledMixPercen1;
*                  ledForceInt;
*                  ledPulseDuration;
*                  ledBlinkRate;
*                  ledSppedOffPulse_perio;
*                  ledSppedOnPulse_perio;
*                  ledFuncMap3;
*                  ledFuncMap2;
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLEDCtrl
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN    MAD_LED_CTRL_DATA   *ledData
);

/*******************************************************************************
* mdSysSetUniDirTrans
*
* DESCRIPTION:
*       This function set 802.3ah unidirectioanal transmit.
*
* INPUTS:
*       port    - Port number to be configured.
*       en      - MAD_TRUE to be enabled or MAD_FALSE to be disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetUniDirTrans
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdSysGetUniDirTrans
*
* DESCRIPTION:
*       This function get 802.3ah unidirectioanal transmit.
*
* INPUTS:
*       port - Port number to be configured.
*
* OUTPUTS:
*       en   - MAD_TRUE to be enabled or MAD_FALSE to be disabled
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetUniDirTrans
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);

/*******************************************************************************
* mdSysSoftReset
*
* DESCRIPTION:
*       This function set soft reset.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSoftReset
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
);

/*******************************************************************************
* mdSysModSoftReset
*
* DESCRIPTION:
*       This function set different type soft reset.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mod - Reset mode to be configured.(MAD_RESET_MODE_PORT_COPPER: 0_0.15,
*                                     MAD_RESET_MODE_PORT_FIBER: 0_1.15,
*                                     MAD_RESET_MODE_PORT_QSGMII: 0_4.15,
*                                     MAD_RESET_MODE_ALL_QSGNII: 20_4.15,
*                                     MAD_RESET_MODE_PORT_COUNTER: 20_6.15,
*                                     MAD_RESET_MODE_ALL_CHIP: 27_4.15)
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysModSoftReset
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U32        mode
);


/* madCpCtrl.c */

/*******************************************************************************
* mdCopperSetAutoNeg
*
* DESCRIPTION:
*       This routine enables or disables Copper Auto-Neg with the specified mode.
*        When enables Auto-Neg, advertisement register value is updated with the  
*       combination of the followings:
*           Asym Pause, Pause, 100FDX, 100HDX, 10FDX, 10HDX,
*           Master/Slave Manual, Force Master, Port Type, 1000FDX, and 1000HDX
*       When disables Auto-Neg, only one of the following modes is supported:
*           100FDX, 100HDX, 10FDX, or 10HDX.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_AUTO_AD_ASYM_PAUSE,
*                   MAD_AUTO_AD_PAUSE,
*                   MAD_AUTO_AD_100FDX,
*                   MAD_AUTO_AD_100HDX,
*                   MAD_AUTO_AD_10FDX,
*                   MAD_AUTO_AD_10HDX,
*                   MAD_AUTO_AD_1000FDX,
*                   MAD_AUTO_AD_1000HDX,
*                   MAD_AUTO_AD_MANUAL_CONF_MS,
*                   MAD_AUTO_AD_FORCE_MASTER, and
*                   MAD_AUTO_AD_PREFER_MULTI_PORT
*              if en is MAD_FALSE,
*                   MAD_PHY_100FDX,
*                   MAD_PHY_100HDX,
*                   MAD_PHY_10FDX, or
*                   MAD_PHY_10HDX 
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15,13,12,8 : Soft reset,Speed,Auto-neg enable,Duplex
*       Page 0, Reg 4, Bit 11,10,8:5  : Advertisement Register
*       Page 0, Reg 9, Bit 12:8       : 1000 Base-T Control Register
*
*******************************************************************************/
MAD_STATUS mdCopperSetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en,
    IN  MAD_U32   mode
);


/*******************************************************************************
* mdCopperGetAutoNeg
*
* DESCRIPTION:
*       This routine retrieves Copper Auto-Neg mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_AUTO_AD_ASYM_PAUSE,
*                   MAD_AUTO_AD_PAUSE,
*                   MAD_AUTO_AD_100FDX,
*                   MAD_AUTO_AD_100HDX,
*                   MAD_AUTO_AD_10FDX,
*                   MAD_AUTO_AD_10HDX,
*                   MAD_AUTO_AD_1000FDX,
*                   MAD_AUTO_AD_1000HDX,
*                   MAD_AUTO_AD_MANUAL_CONF_MS,
*                   MAD_AUTO_AD_FORCE_MASTER, and
*                   MAD_AUTO_AD_PREFER_MULTI_PORT
*              if en is MAD_FALSE,
*                   MAD_PHY_100FDX,
*                   MAD_PHY_100HDX,
*                   MAD_PHY_10FDX, or
*                   MAD_PHY_10HDX 
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 13,12,8   : Speed, Auto-neg enable, and Duplex
*       Page 0, Reg 4, Bit 11,10,8:5 : Advertisement Register
*       Page 0, Reg 9, Bit 12:8      : 1000 Base-T Control Register
*
*******************************************************************************/
MAD_STATUS mdCopperGetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    OUT MAD_BOOL  *en,
    OUT MAD_U32   *mode
);

/*******************************************************************************
* mdCopperSetAutoRestart
*
* DESCRIPTION:
*       This routine restarts Copper Auto-Neg.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*       Page 0, Reg 0, Bit 9 : Copper Control 0 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperSetAutoRestart
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port
);

/*******************************************************************************
* mdGetAutoNegoMode
*
* DESCRIPTION:
*       This routine transltes auto nego status, speed, and dyplex mode into auto nego mode.
*
* INPUTS:
*        en            - Auto nego enble/disable.
*        speedMode    - MAD_SPEED_MODE.
*        duplexMode    - MAD_DUPLEX_MODE.
* OUTPUTS:
*       mode        - Auto nego mode.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdGetAutoNegoMode
(
    IN    MAD_BOOL en, 
    IN    MAD_SPEED_MODE speedMode, 
    IN    MAD_DUPLEX_MODE duplexMode, 
    OUT    MAD_U32 *autoMode
);

/*******************************************************************************
* mdCopperSetMDIMode
*
* DESCRIPTION:
*       This routine sets Copper MDI Crossover modes.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - MAD_MDI_MODE:
*                   MAD_AUTO_MDI_MDIX,
*                   MAD_FORCE_MDI, or
*                   MAD_FORCE_MDIX
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15   : Soft reset
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperSetMDIMode
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_MDI_MODE    mode
);


/*******************************************************************************
* mdCopperGetMDIMode
*
* DESCRIPTION:
*       This routine gets Copper MDI Crossover modes.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - MAD_MDI_MODE:
*                   MAD_AUTO_MDI_MDIX,
*                   MAD_FORCE_MDI, or
*                   MAD_FORCE_MDIX
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperGetMDIMode
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_MDI_MODE    *mode
);


/*******************************************************************************
* mdCopperSetDownshiftEnable
*
* DESCRIPTION:
*       This routine enables or disables Downshift.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*       count  - Number of Downshift counter
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15  : Soft reset
*       Page 0, Reg 16, Bit 11 : Copper Control 1 (Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperSetDownshiftEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en,
    IN  MAD_U16     count
);


/*******************************************************************************
* mdCopperGetDownshiftEnable
*
* DESCRIPTION:
*       This routine gets if Downshift is enabled.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       count  - Number of Downshift counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 11 : Copper Control 1 (Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDownshiftEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en,
    OUT MAD_U16     *count
);


/*******************************************************************************
* mdCopperSetDTEDetectEnable
*
* DESCRIPTION:
*       This routine enables or disables DTE (such as IP Phone) detect.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*       dropHys   - DTE detect status drop hysteresis
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15 : Soft reset
*       Page 0, Reg 26, Bit 8 : Copper Control 2 (DTE Detect)
*
*******************************************************************************/
MAD_STATUS mdCopperSetDTEDetectEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en,
    IN  MAD_U16        dropHys
);


/*******************************************************************************
* mdCopperGetDTEDetectEnable
*
* DESCRIPTION:
*       This routine gets if DTE Detect is enabled.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       dropHys   - DTE detect status drop hysteresis
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 26, Bit 8 : Copper Control 2 (DTE Detect)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDTEDetectEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en,
    OUT MAD_U16     *dropHys
);

/*******************************************************************************
* mdCopperSetLinkDownDelay
*
* DESCRIPTION:
*       This function set Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*       delay -    MAD_1G_LINKDOWN_DELAY_0  (0ms)
*                MAD_1G_LINKDOWN_DELAY_10 (10 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_20 (20 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_40 (40 +/- 2ms)
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperSetLinkDownDelay
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN    MAD_U16     delay
);

/*******************************************************************************
* mdCopperGetLinkDownDelay
*
* DESCRIPTION:
*       This function get Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       delay -    MAD_1G_LINKDOWN_DELAY_0  (0ms)
*                MAD_1G_LINKDOWN_DELAY_10 (10 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_20 (20 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_40 (40 +/- 2ms)
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkDownDelay
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *delay
);

/*******************************************************************************
* mdCopperSetLinkDownDelayEnable
*
* DESCRIPTION:
*       This function set enable of Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperSetLinkDownDelayEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdCopperGetLinkDownDelayEnable
*
* DESCRIPTION:
*       This function get Gigabit Link Down Delay enable status
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkDownDelayEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);

/*******************************************************************************
* mdCopperSetSpeedDuplex
*
* DESCRIPTION:
*       Set Phy speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - port number.
*       speed   -  speed 10, 100, 1000.  
*       duplex   - Duplex MAD_TRUE, Half MAD_FALSE.  
*   
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/

MAD_STATUS mdCopperSetSpeedDuplex 
(
    IN  MAD_DEV    *dev,
    IN  int         port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
);

/*******************************************************************************
* mdCopperSet1000TMasterMode
*
* DESCRIPTION:
*       This routine sets the ports 1000Base-T Master mode and restart the Auto
*        negotiation.
* INPUTS:
*       port - the logical port number.
*       mode - MAD_1000T_MASTER_SLAVE structure
*                autoConfig   - MAD_TRUE for auto, MAD_FALSE for manual setup.
*                masterPrefer - MAD_TRUE if Master configuration is preferred.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 9, Bit 10-12   : Master/Slave configuration 
*
*******************************************************************************/
MAD_STATUS mdCopperSet1000TMasterMode
(
    IN  MAD_DEV                   *dev,
    IN  MAD_LPORT                 port,
    IN  MAD_1000T_MASTER_SLAVE    *mode
);

/*******************************************************************************
* mdCopperGet1000TMasterMode
*
* DESCRIPTION:
*       This routine gets the ports 1000Base-T Master mode and restart the Auto
*        negotiation.
*
* INPUTS:
*       port - the logical port number.
*   
* OUTPUTS:
*       mode - MAD_1000T_MASTER_SLAVE structure
*                autoConfig   - MAD_TRUE for auto, MAD_FALSE for manual setup.
*                masterPrefer - MAD_TRUE if Master configuration is preferred.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperGet1000TMasterMode
(
    IN  MAD_DEV                   *dev,
    IN  MAD_LPORT                 port,
    OUT MAD_1000T_MASTER_SLAVE    *mode
);

/* madMACCtrl.c */

/*******************************************************************************
* mdSGMIISetAutoNeg
*
* DESCRIPTION:
*       This routine enables or disables SGMII Auto-Neg.
*       Supported modes are 1000M, 100M, or 10M.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_SPEED_MODE to setup default SGMII speed:
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15       : Soft reset
*       Page 2, Reg 21, Bit 13,12,6 : MAC If Control 1 (Speed,Auto-neg enable)
*
*******************************************************************************/
MAD_STATUS mdSGMIISetAutoNeg
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_BOOL         en,
    IN MAD_SPEED_MODE   mode
);


/*******************************************************************************
* mdSGMIIGetAutoNeg
*
* DESCRIPTION:
*       This routine retrieves SGMII Auto-Neg mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       mode - default SGMII speed (MAD_SPEED_MODE):
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 21, Bit 13,12,6 : MAC If Control 1 (Speed,Auto-neg enable)
*
*******************************************************************************/
MAD_STATUS mdSGMIIGetAutoNeg
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_BOOL        *en,
    OUT MAD_SPEED_MODE  *mode
);

/*******************************************************************************
* mdRGMIISetSpeed
*
* DESCRIPTION:
*       This routine sets RGMII speed.
*       Supported modes are 1000M, 100M, or 10M.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - MAD_SPEED_MODE to setup RGMII speed:
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15       : Soft reset
*       Page 2, Reg 21, Bit 13,6    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdRGMIISetSpeed
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_SPEED_MODE   mode
);

/*******************************************************************************
* mdRGMIIGetSpeed
*
* DESCRIPTION:
*       This routine retrieves RGMII speed.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - RGMII speed (MAD_SPEED_MODE):
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 21, Bit 13,6 : MAC If Control 1 (Speed)
*
*******************************************************************************/
MAD_STATUS mdRGMIIGetSpeed
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_SPEED_MODE  *mode
);

/*******************************************************************************
* mdMacSetFifoDepth
*
* DESCRIPTION:
*       This routine sets Mac transmit/Receive FIFO depth.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       transFifoDepth - MAD_SYNC_TRANS_FIFO_XX 
*       recvFifoDepth  - MAD_SYNC_RECV_FIFO_XX
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 12,13,14,15    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdMacSetFifoDepth
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_U8            transFifoDepth,
    IN MAD_U8            recvFifoDepth
);

/*******************************************************************************
* mdMacGetFifoDepth
*
* DESCRIPTION:
*       This routine sets Mac transmit/Receive FIFO depth.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       transFifoDepth - MAD_SYNC_TRANS_FIFO_XX 
*       recvFifoDepth  - MAD_SYNC_RECV_FIFO_XX
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 12,13,14,15    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdMacGetFifoDepth
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_U8            *transHifoDepth,
    OUT MAD_U8            *recvHifoDepth
);


/* madSyncClkCtrl.c  */

/*******************************************************************************
* mdSyncClkSetRClk
*
* DESCRIPTION:
*       This routine sets the RCLK1 and RCLK2 pins of the chip outputs 
*        either a 125MHz or 25 MHz clock
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       rclk - MAD_SYNC_CLK_RCLK1/MAD_SYNC_CLK_RCLK2.
*        frq     - MAD_SYNC_CLK_FREQ_25M/MAD_SYNC_CLK_FREQ_125M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 8, 9, 12    
*
*******************************************************************************/
MAD_STATUS mdSyncClkSetRClk
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            rclk,
    IN MAD_8            freq
);

/*******************************************************************************
* mdSyncClkGetRClk
*
* DESCRIPTION:
*       This routine gets the RCLK1 and RCLK2 pins of the chip outputs 
*        either a 125MHz or 25 MHz clock
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       rclk - MAD_SYNC_CLK_RCLK1/MAD_SYNC_CLK_RCLK2.
*        frq     - MAD_SYNC_CLK_FREQ_25M/MAD_SYNC_CLK_FREQ_125M
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 8, 9, 12    
*
*******************************************************************************/
MAD_STATUS mdSyncClkGetRClk
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    OUT MAD_8            *rclk,
    OUT MAD_8            *freq
);

/*******************************************************************************
* mdSyncClkSetClkSelect
*
* DESCRIPTION:
*       This routine sets to select between the local reference clock and a 
*        cleaned up recovered clock.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       interf - MAD_SYNC_CLK_MODE_PORT_COPPER
*                    MAD_SYNC_CLK_MODE_PORT_125SERDES
*                    MAD_SYNC_CLK_MODE_PORT_QSGMII
*        refClk     - MAD_SYNC_CLK_REF_CLK_XTAL/MAD_SYNC_CLK_REF_CLK_SCLK
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 7, 6    
*       Page 4, Reg 26, Bit 14   
*
*******************************************************************************/
MAD_STATUS mdSyncClkSetClkSelect
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            interf,
    IN MAD_8            refClk
);

/*******************************************************************************
* mdSyncClkGetClkSelect
*
* DESCRIPTION:
*       This routine gets to select between the local reference clock and a 
*        cleaned up recovered clock.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       interf - MAD_SYNC_CLK_MODE_PORT_COPPER
*                    MAD_SYNC_CLK_MODE_PORT_125SERDES
*                    MAD_SYNC_CLK_MODE_PORT_QSGMII
*   
* OUTPUTS:
*        refClk     - MAD_SYNC_CLK_REF_CLK_XTAL/MAD_SYNC_CLK_REF_CLK_SCLK
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 7, 6    
*       Page 4, Reg 26, Bit 14   
*
*******************************************************************************/
MAD_STATUS mdSyncClkGetClkSelect
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            interf,
    OUT MAD_8           *refClk
);

/* madCpStatus.c */

/*******************************************************************************
* mdCopperGetDownshiftStatus
*
* DESCRIPTION:
*       This routine retrieves the status of Downshift.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if downshifted, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 5 : Copper Status 1 (Downshift status)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDownshiftStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);


/*******************************************************************************
* mdCopperGetLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       linkOn - MAD_TRUE if link is established, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 10 : Copper Link
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *linkOn
);

/*******************************************************************************
* mdCopperGetSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current Speed.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current speed (MAD_SPEED_MODE).
*               MAD_SPEED_10M, for 10 Mbps
*               MAD_SPEED_100M, for 100 Mbps
*               MAD_SPEED_1000M, for 1000 Mbps
*               MAD_SPEED_UNKNOWN, when speed is not resolved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 15:14,11 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdCopperGetSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
);

/*******************************************************************************
* mdCopperGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrieves the current Duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current duplex mode (MAD_DUPLEX_MODE).
*               MAD_HALF_DUPLEX, for half duplex mode
*               MAD_FULL_DUPLEX, for half duplex mode
*               MAD_DUPLEX_UNKNOWN, when duplex is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 13,11 : Duplex, Duplex resolved
*
*******************************************************************************/
MAD_STATUS mdCopperGetDuplexStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_DUPLEX_MODE *mode
);


/*******************************************************************************
* mdCopperGetDTEDetectStatus
*
* DESCRIPTION:
*       This routine retrieves the status of DTE (such as IP Phone) Detect.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if DTE detected, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 2 : Copper Status (DTE Detect)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDTEDetectStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
);


/* madLoopback.c */

/*******************************************************************************
* mdDiagSetMACIfLoopback
*
* DESCRIPTION:
*       This routine enables or disables MAC Interface Loopback.
*        When enables loopback test, one of the following modes is required:
*           Loopback mode 0,
*           Loopback mode 1 (link should be established),
*           Loopback mode 2 (link should be established), and
*           Loopback mode 3 (link should be established)
*       If link is not established for the mode 1,2,and 3, it will return MAD_FAIL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_MAC_LOOPBACK_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported Loopback Modes are:
*                   MAD_MAC_LOOPBACK_MODE_0,
*                   MAD_MAC_LOOPBACK_MODE_1,
*                   MAD_MAC_LOOPBACK_MODE_2,
*                   MAD_MAC_LOOPBACK_MODE_3,
*                    MAD_MAC_LOOPBACK_MODE_SYS,
*                    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125,
*                    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_5
*       speed - MAD_SPEED_MODE.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       After MAC Interface Loopback, the followings need to be reconfigured:
*           MDI Crossover mode (mdCopperSetMDIMode API), and
*           SGMII Auto-Neg mode (mdSGMIISetAutoNeg API)
*
*       Accessed Registers:
*       
*******************************************************************************/
MAD_STATUS mdDiagSetMACIfLoopback
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed
);


/*******************************************************************************
* mdDiagSetLineLoopback
*
* DESCRIPTION:
*       This routine enables or disables Line Loopback.
*       Link should be established with full duplex mode before enabling Line 
*       loopback. Otherwise, this routine returns fail.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 10 : Copper Link
*       Page 2, Reg 21, Bit 14 : Line loopback enable
*
*******************************************************************************/
MAD_STATUS mdDiagSetLineLoopback
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en
);


/*******************************************************************************
* mdDiagSetExternalLoopback
*
* DESCRIPTION:
*       This routine forces speed to the specified mode and if 1000Base-T mode
*       is used, it enables stub test as well.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_SPEED_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported Speed modes are:
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       After External Loopback, the followings need to be reconfigured:
*           Copper Speed/Duplex and Auto-Nego mode (mdCopperSetAutoNeg)
*
*       Accessed Registers:
*       Page 0, Reg 0, Bit 15,13,12,8,6 : 
*                                   Soft reset,speed,Auto-Neg enable,duplex
*       Page 0, Reg 4, Bit 11,10,8:5  : Advertisement Register
*       Page 0, Reg 9, Bit 12:8       : 1000 Base-T Control Register
*       Page 6, Reg 16, Bit 5 : Enable stub test
*
*******************************************************************************/
MAD_STATUS mdDiagSetExternalLoopback
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_SPEED_MODE  mode
);


/* madPktGen.c */

/*******************************************************************************
* mdDiagSetPktGenEnable
*
* DESCRIPTION:
*       This routine enables or disables Packet Generator.
*       Link should be established first prior to enabling the packet generator,
*       and generator will generate packets at the speed of the established link.
*        When enables packet generator, the following information should be 
*       provided:
*           Payload Type:  either Random or 5AA55AA5
*           Packet Length: either 64 or 1514 bytes
*           Error Packet:  either Error packet or normal packet
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       en      - 1 to enable, 0 to disable
*       pktInfo - packet information(MAD_PG structure pointer), if en is MAD_TRUE
*                 ignored, if en is MAD_FALSE
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 10  : Copper Link
*       Page 6, Reg 16, Bit 3:0 : Packet Generation Register
*       Page 6, Reg 16, Bit 7:5 : Packet Generation Register for 88E1340
*
*******************************************************************************/
MAD_STATUS mdDiagSetPktGenEnable
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_U32   en,
    IN  MAD_PG    *pktInfo
);


/* madVod.c */

/*******************************************************************************
* mdDiagSetSGMIIVod
*
* DESCRIPTION:
*       This routine sets differential output voltage of SGMII.
*        The following VODs are supported:
*           MAD_SGMII_VOD_14mV,
*           MAD_SGMII_VOD_112mV,
*           MAD_SGMII_VOD_210mV,
*           MAD_SGMII_VOD_308mV, and
*           MAD_SGMII_VOD_406mV
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       vod  - MAD_SGMII_VOD value
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 26, Bit 2:0 : SGMII Output Amplitude
*
*******************************************************************************/
MAD_STATUS mdDiagSetSGMIIVod
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_SGMII_VOD   vod
);


/*******************************************************************************
* mdDiagSetCopperVod
*
* DESCRIPTION:
*       This routine sets differential output voltage of Copper.
*       VOD amplitude on the MDI pins can be adjusted for 10/100/1000Mbps speeds
*       of operation with this function.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - Speed mode (MAD_SPEED_MODE: 10M,100M,or 1000M)
*       vod  - Copper VOD value (MAD_COPPER_VOD)
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page ff, Reg 24, Bit 15:0        : VOD adjust for each MDIs
*       Page ff, Reg 23, Bit 15,13,5,1,0 : apply VOD adjustment
*
*******************************************************************************/
MAD_STATUS mdDiagSetCopperVod
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_SPEED_MODE  mode,
    IN  MAD_COPPER_VOD  vod
);


/* madIEEETest.c */

/*******************************************************************************
* mdDiagSetIEEETest
*
* DESCRIPTION:
*       This routine starts or stop IEEE test mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to start, MAD_FALSE to stop
*       mode - MAD_IEEE_TEST_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported test modes are:
*                   MAD_IEEE_TEST_WAVEFORM,
*                   MAD_IEEE_TEST_MASTER_JITTER,
*                   MAD_IEEE_TEST_SLAVE_JITTER, or
*                   MAD_IEEE_TEST_DISTORTION
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 4, Reg 30, Bit 4:0  : Enable TX_TCLK
*       Page 0, Reg 9, Bit 15:13 : Test modes
*
*******************************************************************************/
MAD_STATUS mdDiagSetIEEETest
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_IEEE_TEST_MODE  mode
);


/* madVct.c */

/*******************************************************************************
* mdDiagGetCableStatus
*
* DESCRIPTION:
*       This routine performs the virtual cable test for the requested port,
*       and returns the the status per MDI pair.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 5, Reg 21, Bit 6    : DSP Valid
*       Page 5, Reg 16, Bit 15:0 : MDI[0] VCT Register
*       Page 5, Reg 17, Bit 15:0 : MDI[1] VCT Register
*       Page 5, Reg 18, Bit 15:0 : MDI[2] VCT Register
*       Page 5, Reg 19, Bit 15:0 : MDI[3] VCT Register
*
*******************************************************************************/
MAD_STATUS mdDiagGetCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    OUT MAD_CABLE_STATUS *cableStatus
);


/*******************************************************************************
* mdDiagGet1000BTExtendedStatus
*
* DESCRIPTION:
*       This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset 
*       Page 5, Reg 20, Bit 15:0 : 1000B-T Pair Skew Register
*       Page 5, Reg 21, Bit 6:0  : 1000B-T Pair Swap and Polarity
*
*******************************************************************************/
MAD_STATUS mdDiagGet1000BTExtendedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
);


/*******************************************************************************
* mdDiagGetAdvCableStatus
*
* DESCRIPTION:
*       This routine performs the advanced VCT for the requested port,
*       and returns the status per MDI pair.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 5, Reg 21, Bit 6    : DSP Valid
*       Page 8, Reg 20, Bit 15:6 : Adv VCT Configuration Register
*       Page 8, Reg 16~19, Bit 15:0 : MDI[0] VCT Register
*       Page 8, Reg 24~27, Bit 15:0 : MDI[1] VCT Register
*       Page 9, Reg 16~19, Bit 15:0 : MDI[2] VCT Register
*       Page 9, Reg 24~27, Bit 15:0 : MDI[3] VCT Register
*
*******************************************************************************/
MAD_STATUS mdDiagGetAdvCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
);

/*******************************************************************************
* mdDiagGetAltCableStatus
*
* DESCRIPTION:
*       This routine performs the alternate VCT for the requested port,
*       and returns the status per MDI pairand cable length.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       diagType - advance VCT type:
*                    MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT |
*                    MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT  |
*                    MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:/
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 7, Reg 16-21, 
*
*******************************************************************************/
MAD_STATUS mdDiagGetAltCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_U32            diagType,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
);

/* madStats.c */

/*******************************************************************************
* mdStatsGetIdleErrorCounter
*
* DESCRIPTION:
*       This routine gets idle error counter. Idle error counter is cleared 
*       after this function call.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - Idle error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 10, Bit 7:0 : Idle Error Count
*
*******************************************************************************/
MAD_STATUS mdStatsGetIdleErrorCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
);


/*******************************************************************************
* mdStatsSetCRCCheckerEnable
*
* DESCRIPTION:
*       This routine enables or disables CRC Checker. When CRC Checker is 
*       enabled, both CRC Error Counter Frame Counter are enabled. The counters
*       do not clear on a read. To clear counters, disable and enable the CRC
*       Checker.
*       Both counters will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*        mode - Counter path (Copper or SGMII...)
*                For all exept for 1340
*                    0: Copper receive payh
*                    1: SGMII input path
*                For 1340
*                    MAD_CHECKER_CTRL_DISABLE    0
*                    MAD_CHECKER_CTRL_COPPER        2
*                    MAD_CHECKER_CTRL_SGMII        4
*                    MAD_CHECKER_CTRL_MACSEC_TR    5
*                    MAD_CHECKER_CTRL_QSGMII        6
*                    MAD_CHECKER_CTRL_MACSEC_REC    7
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 16, Bit 4 : CRC Counter Enable
*
*******************************************************************************/
MAD_STATUS mdStatsSetCRCCheckerEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8      mode,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdStatsGetCRCCheckerEnable
*
* DESCRIPTION:
*       This routine gets enable path qnd status of CRC Checker. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*        mode - Counter path (Copper or SGMII...)
*                For all exept for 1340
*                    0: Copper receive payh
*                    1: SGMII input path
*                For 1340
*                    MAD_CHECKER_CTRL_DISABLE    0
*                    MAD_CHECKER_CTRL_COPPER        2
*                    MAD_CHECKER_CTRL_SGMII        4
*                    MAD_CHECKER_CTRL_MACSEC_TR    5
*                    MAD_CHECKER_CTRL_QSGMII        6
*                    MAD_CHECKER_CTRL_MACSEC_REC    7
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 16, Bit 4 : CRC Counter Enable
*
*******************************************************************************/
MAD_STATUS mdStatsGetCRCCheckerEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT  MAD_U8     *mode,
    OUT  MAD_BOOL   *en
);

/*******************************************************************************
* mdStatsGetCRCErrorCounter
*
* DESCRIPTION:
*       This routine gets CRC error counter.
*       In order to activate CRC error counter, CRC Checker should be enabled
*       with mdStatsSetCRCCounterEnable function.
*       In order to clear the counter, CRC counter should be disabled and then
*       enabled again.
*       This counter will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - CRC error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 17, Bit 7:0 : CRC error counter
*
*******************************************************************************/
MAD_STATUS mdStatsGetCRCErrorCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
);


/*******************************************************************************
* mdStatsGetFrameCounter
*
* DESCRIPTION:
*       This routine gets Frame counter. 
*       In order to activate CRC error counter, CRC Checker should be enabled
*       with mdStatsSetCRCCounterEnable function.
*       In order to clear the counter, CRC counter should be disabled and then
*       enabled again.
*       This counter will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - CRC error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 17, Bit 15:8 : Frame counter
*
*******************************************************************************/
MAD_STATUS mdStatsGetFrameCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
);

/*******************************************************************************
* mdStatsGetRxErCapture
*
* DESCRIPTION:
*       This routine gets RX_ER Capture Valid bit. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       capture - MAD_TRUE: Capture data valid. MAD_FALSE: otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 15 : RX_ER Capture
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxErCapture
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL     *capture
);

/*******************************************************************************
* mdStatsGetRxErByteNun
*
* DESCRIPTION:
*       This routine gets RX_ER Byte number increments after every read.. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       byteNum - RX_ER Byte number increments
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 13:12 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxErByteNun
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL     *byteNum
);

/*******************************************************************************
* mdStatsGetRxEr
*
* DESCRIPTION:
*       This routine gets RX error.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxEr - RX_ER Error. Normaly is 0. 
*                            When it is long sequence of RX_EX, it ia 1
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 9 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxEr
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_U8     *rxEr
);

/*******************************************************************************
* mdStatsGetRxDv
*
* DESCRIPTION:
*       This routine gets RX data valid.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxDv - RX data valid. MAD_TRUE: RX data valid. MAD_FALSE: otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 8 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxDv
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL    *rxDv
);

/*******************************************************************************
* mdStatsGetRxData
*
* DESCRIPTION:
*       This routine gets RX_ER Data. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxData - RX_ER Data
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 7:0 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxData
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_U8     *rxData
);

/* madInt.c */

/*******************************************************************************
* mdIntSetEnable
*
* DESCRIPTION:
*       Enable/Disable PHY Interrupts.
*       This register determines whether the INT# pin is asserted when an 
*       interrupt event occurs. When an interrupt occurs, the corresponding bit 
*       is set and remains set until register 19 is read via the SMI. 
*       When interrupt enable bits are not set in register 18, interrupt status 
*       bits in register 19 are still set when the corresponding interrupt events
*       occur. However, the INT# is not asserted.
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt to enable/disable. 
*               Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                    MAD_COPPER_SPEED_CHANGED,
*                    MAD_COPPER_DUPLEX_CHANGED,
*                    MAD_COPPER_PAGE_RECEIVED,
*                    MAD_COPPER_AUTO_NEG_COMPLETED,
*                    MAD_COPPER_LINK_STATUS_CHANGED,
*                    MAD_COPPER_SYMBOL_ERROR,
*                    MAD_COPPER_FALSE_CARRIER,
*                    MAD_COPPER_CROSSOVER_CHANGED,
*                    MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                Group 0 High 16 bits:
*                    MAD_SGMII_TRANS_FIFO_FLOW.
*                    MAD_SGMII_RECV_FIFO_FLOW.
*                    MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                    MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                    MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                    MAD_SGMII_RECV_FIFO_IDLE_DELET
*
*               Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 18, Bit 15:0 : Copper Interrupt Enable Register
*       Page 1, Reg 18, Bit 15:0 : Fiber Interrupt Enable Register (88E114x)
*       Page 2, Reg 18, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
MAD_STATUS mdIntSetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_INT_TYPE    *intType
);

/*******************************************************************************
* mdIntGetEnable
*
* DESCRIPTION:
*       Get interrupt setting
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt which causes an interrupt.
*                  Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                   MAD_COPPER_SPEED_CHANGED,
*                   MAD_COPPER_DUPLEX_CHANGED,
*                   MAD_COPPER_PAGE_RECEIVED,
*                   MAD_COPPER_AUTO_NEG_COMPLETED,
*                   MAD_COPPER_LINK_STATUS_CHANGED,
*                   MAD_COPPER_SYMBOL_ERROR,
*                   MAD_COPPER_FALSE_CARRIER,
*                   MAD_COPPER_CROSSOVER_CHANGED,
*                   MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                  High 16 bits:
*                   MAD_SGMII_TRANS_FIFO_FLOW.
*                   MAD_SGMII_RECV_FIFO_FLOW.
*                   MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                   MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                   MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                   MAD_SGMII_RECV_FIFO_IDLE_DELET
*                  Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 18, Bit 15:0 : Copper Interrupt Status Register
*       Page 1, Reg 18, Bit 15:0 : Fiber Interrupt Status Register (88E114x)
*       Page 2, Reg 18, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
MAD_STATUS mdIntGetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
);

/*******************************************************************************
* mdIntGetStatus
*
* DESCRIPTION:
*       Check to see if a specific type of interrupt occured
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt which causes an interrupt.
*               Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                    MAD_COPPER_SPEED_CHANGED,
*                    MAD_COPPER_DUPLEX_CHANGED,
*                    MAD_COPPER_PAGE_RECEIVED,
*                    MAD_COPPER_AUTO_NEG_COMPLETED,
*                    MAD_COPPER_LINK_STATUS_CHANGED,
*                    MAD_COPPER_SYMBOL_ERROR,
*                    MAD_COPPER_FALSE_CARRIER,
*                    MAD_COPPER_CROSSOVER_CHANGED,
*                    MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                Group 0 High 16 bits:
*                    MAD_SGMII_TRANS_FIFO_FLOW.
*                    MAD_SGMII_RECV_FIFO_FLOW.
*                    MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                    MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                    MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                    MAD_SGMII_RECV_FIFO_IDLE_DELET
*
*               Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 19, Bit 15:0 : Copper Interrupt Status Register
*       Page 1, Reg 19, Bit 15:0 : Fiber Interrupt Status Register (88E114x)
*       Page 2, Reg 19, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
MAD_STATUS mdIntGetStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
);


/*******************************************************************************
* mdIntGetPortSummary
*
* DESCRIPTION:
*       Lists the ports that have active interrupts. It provides a quick way to 
*       isolate the interrupt so that the MAC or switch does not have to poll the
*       interrupt status register (19) for all ports. Reading this register does 
*       not de-assert the INT# pin
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*       intPortVec - port vector with the bits set for the corresponding 
*                   ports with active interrupt. E.g., the bit number 0 and 2 are
*                   set when port number 0 and 2 have active interrupt
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 18, Bit 7:0 : Copper Global Interrupt Status
*
*******************************************************************************/
MAD_STATUS mdIntGetPortSummary
(
    IN  MAD_DEV     *dev,
    OUT MAD_U32     *intPortVec
);
/*******************************************************************************
* mdTempIntSetEnable
*
* DESCRIPTION:
*       Enable/Disable Temperature sensor Interrupt.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
*        en        - MAD_TRUE: enable, MAD_FALSE: disable.
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 7    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntSetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdTempIntGetEnable
*
* DESCRIPTION:
*       Get Enable/Disable Temperature sensor Interrupt status.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
* OUTPUTS:
*        en        - MAD_TRUE: enable, MAD_FALSE: disable.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 7    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntGetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT  MAD_BOOL   *en
);

/*******************************************************************************
* mdTempIntGetStatus
*
* DESCRIPTION:
*       Get Temperature sensor Interrupt status.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
* OUTPUTS:
*        status    - MAD_TRUE: Temperature reached threshold, MAD_FALSE: not reched.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 6    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntGetStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT  MAD_BOOL   *status
);

/*******************************************************************************
* madPatCtrlSetPRBS
*
* DESCRIPTION:
*       This function select PRBS test -  PRBS 7, PRBS23, PRBS31.
*
* INPUTS:
*       port - The logical port number
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlSetPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN  MAD_PRBS_GEN_CONFIG *genCfg
);

/*******************************************************************************
* madPatCtrlGetPRBS
*
* DESCRIPTION:
*       This function get PRBS test setting -  PRBS 7, PRBS23, PRBS31.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlGetPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    OUT  MAD_PRBS_GEN_CONFIG *genCfg
);

/*******************************************************************************
* madResetCounters
*
* DESCRIPTION:
*       This function reset Counter. 
*
* INPUTS:
*        None
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madResetPRBSCounters
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port
);

/* 1.25G1.25G  PRBS */
/*******************************************************************************
* madPatCtrlSet125GPRBS
*
* DESCRIPTION:
*       This function select 1.25G PRBS test.
*
* INPUTS:
*       port - The logical port number
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlSet125GPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN  MAD_PRBS_GEN_CONFIG *genCfg
);

/*******************************************************************************
* madPatCtrlGet125GPRBS
*
* DESCRIPTION:
*       This function get 1.25G PRBS test setting
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlGet125GPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    OUT  MAD_PRBS_GEN_CONFIG *genCfg
);

/*******************************************************************************
* madReset125GPRBSCounters
*
* DESCRIPTION:
*       This function reset Counter. 
*
* INPUTS:
*        None
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madResetCounters
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port
);

#ifdef MAD_SM_VCT
/*******************************************************************************
* mdGetSMState
*
* DESCRIPTION:
*       This routine retrieves the state of VCT State Machine.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*        MAD_PENDDING - on state machine pending
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdGetSMState
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
);

/*******************************************************************************
* mdStopSM
*
* DESCRIPTION:
*       This routine stop to run VCT State Machine, and set SM free.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*        MAD_PENDDING - on state machine pending
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdStopSM
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
);
#endif

/*******************************************************************************
* mdPtpSetEnable
*
* DESCRIPTION:
*       This routine set PTP Enable/Disable.
*
* INPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdPtpGetEnable
*
* DESCRIPTION:
*       This routine set PTP Enable/Disable.
*
* INPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetEnable
(
    IN  MAD_DEV     *dev,
    OUT  MAD_BOOL    *en
);

/*******************************************************************************
* mdPtpSetIntEnable
*
* DESCRIPTION:
*       This routine set PTP interrupt Enable/Disable.
*
* INPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetIntEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_BOOL    en
);

/*******************************************************************************
* mdPtpGetIntEnable
*
* DESCRIPTION:
*       This routine get PTP Interrupt Enable/Disable.
*
* INPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetIntEnable
(
    IN  MAD_DEV     *dev,
    OUT  MAD_BOOL    *en
);

/*******************************************************************************
* mdPtpSetConfig
*
* DESCRIPTION:
*       This routine writes PTP configuration parameters.
*
* INPUTS:
*        ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_PTP_CONFIG    *ptpData
);;


/*******************************************************************************
* mdPtpGetConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetConfig
(
    IN  MAD_DEV     *dev,
    OUT MAD_PTP_CONFIG    *ptpData
);;

/*******************************************************************************
* mdPtpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes PTP global configuration parameters.
*
* INPUTS:
*        ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetGlobalConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_PTP_GLOBAL_CONFIG    *ptpData
);;


/*******************************************************************************
* mdPtpGlobalGetConfig
*
* DESCRIPTION:
*       This routine reads PTP global configuration parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetGlobalConfig
(
    IN  MAD_DEV     *dev,
    OUT MAD_PTP_GLOBAL_CONFIG    *ptpData
);;

/*******************************************************************************
* mdPtpSetPortConfig
*
* DESCRIPTION:
*       This routine writes PTP port configuration parameters.
*
* INPUTS:
*       port    - The logical port number
*        ptpData  - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortConfig
(
    IN  MAD_DEV     *dev,
    IN    MAD_LPORT    port,
    IN  MAD_PTP_PORT_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpGetPortConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters for a port.
*
* INPUTS:
*       port    - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP port configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortConfig
(
    IN  MAD_DEV     *dev,
    IN    MAD_LPORT    port,
    OUT MAD_PTP_PORT_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP.
*
* INPUTS:
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPTPEn
(
    IN  MAD_DEV     *dev,
    IN  MAD_BOOL        en
);


/*******************************************************************************
* mdPtpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPEn
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL        *en
);

/*******************************************************************************
* mdPtpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*       port    - The logical port number
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN    MAD_LPORT    port,
    IN  MAD_BOOL        en
);
/*******************************************************************************
* mdPtpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       port    - The logical port number
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN    MAD_LPORT    port,
    OUT MAD_BOOL        *en
);

/*******************************************************************************
* mdPtpGetPTPInt
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for each port.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.);
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPInt
(
    IN  MAD_DEV     *dev,
    OUT MAD_U32        *ptpInt
);

/*******************************************************************************
* mdPtpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*        running clock.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPGlobalTime
(
    IN  MAD_DEV     *dev,
    OUT MAD_U32        *ptpTime
);

/*******************************************************************************
* mdPtpGetTimeStamped
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value 
*        and sequce Id that are captured by PTP logic for a PTP frame that needs 
*        to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type);
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTimeStamped
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN    MAD_PTP_TIME    timeToRead,
    OUT MAD_PTP_TS_STATUS    *ptpStatus
);

/*******************************************************************************
* mdPtpGetTSValidSt
*
* DESCRIPTION:
*        This routine gets PTP Time valid bit.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*    isValid        - MAD_TRUE: valid TS, MAD_FALSE: invalid TS.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTSValidSt
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToReset,
    OUT MAD_BOOL    *isValid
);

/*******************************************************************************
* mdPtpResetTimeStamp
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type);
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpResetTimeStamp
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN    MAD_PTP_TIME    timeToReset
);

/*******************************************************************************
* mdPtpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    OUT MAD_U32        *data
);

/*******************************************************************************
* mdPtpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    IN  MAD_U32        data
);

/*******************************************************************************
* madTaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetEventConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_TAI_EVENT_CONFIG    *eventData
);


/*******************************************************************************
* madTaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets TAI Event Capture configuration parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventConfig
(
    IN  MAD_DEV     *dev,
    OUT MAD_TAI_EVENT_CONFIG    *eventData
);


/*******************************************************************************
* madTaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets TAI Event Capture status.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventStatus
(
    IN  MAD_DEV     *dev,
    OUT MAD_TAI_EVENT_STATUS    *status
);


/*******************************************************************************
* madTaiGetEventInt
*
* DESCRIPTION:
*       This routine gets TAI Event Capture Interrupt status.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventInt
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL        *intStatus
);

/*******************************************************************************
* madTaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets TAI Trigger Interrupt status.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigInt
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL        *intStatus
);

/*******************************************************************************
* madTaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE);.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetTrigConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_BOOL     trigEn,
    IN  MAD_TAI_TRIGGER_CONFIG    *trigData
);
/*******************************************************************************
* madTaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets TAI Trigger configuration parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE);.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigConfig
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL     *trigEn,
    OUT MAD_TAI_TRIGGER_CONFIG    *trigData
);


/*******************************************************************************
* madTaiGetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*         None
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiGetTSClkPer
(
    IN  MAD_DEV     *dev,
    OUT MAD_U32        *clk
);


/*******************************************************************************
* madTaiSetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*        clk        - time stamping clock period
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer
(
    IN  MAD_DEV     *dev,
    IN  MAD_U32        clk
);

/*******************************************************************************
* madTaiSetMultiPTPSync
*
* DESCRIPTION:
*         This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt);.
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO); and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE);
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync
(
    IN  MAD_DEV     *dev,
    IN  MAD_BOOL     multiEn,
    IN  MAD_32        syncTime
);
/*******************************************************************************
* madTaiGetMultiPTPSync
*
* DESCRIPTION:
*         This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt);.
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO); and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*       lport    - The logical port number
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE);
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL     *multiEn,
    OUT MAD_32        *syncTime
);

/*******************************************************************************
* madTaiGetTimeIncDec
*
* DESCRIPTION:
*         This routine retrieves Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*        expired    - MAD_TRUE if inc/dec occurred, MAD_FALSE otherwise
*        inc        - MAD_TRUE if increment, MAD_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         Time increment or decrement will be excuted once.
*
*******************************************************************************/
MAD_STATUS madTaiGetTimeIncDec
(
    IN  MAD_DEV     *dev,
    OUT MAD_BOOL        *expired,
    OUT MAD_BOOL        *inc,
    OUT MAD_U32        *amount
);

/*******************************************************************************
* madTaiSetTimeInc
*
* DESCRIPTION:
*         This routine enables time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        amount    - time increment amount (0 ~ 0x7FF);
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc
(
    IN  MAD_DEV     *dev,
    IN  MAD_U32        amount
);

/*******************************************************************************
* madTaiSetTimeDec
*
* DESCRIPTION:
*         This routine enables time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*        amount    - time decrement amount (0 ~ 0x7FF);
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec
(
    IN  MAD_DEV     *dev,
    IN  MAD_U32        amount
);
















/*******************************************************************************
* mdPtpSetEnable_port
*
* DESCRIPTION:
*       This routine set port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*       port - The logical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL    en
);
/*******************************************************************************
* mdPtpGetEnable_port
*
* DESCRIPTION:
*       This routine Get port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
);

/*******************************************************************************
* mdPtpSetIntEnable_port
*
* DESCRIPTION:
*       This routine set port PTP interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL    en
);

/*******************************************************************************
* mdPtpGetIntEnable_port
*
* DESCRIPTION:
*       This routine get port PTP Interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
);

/*******************************************************************************
* mdPtpSetConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_PTP_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpGetConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpSetGlobalConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_PTP_GLOBAL_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpGlobalGetConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_GLOBAL_CONFIG    *ptpData
);

/*******************************************************************************
* mdPtpSetPTPEn_port
*
* DESCRIPTION:
*       This routine enables or disables port PTP.
*
* INPUTS:
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL        en
);

/*******************************************************************************
* mdPtpGetPTPEn_port
*
* DESCRIPTION:
*       This routine checks if port PTP is enabled.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *en
);

/*******************************************************************************
* mdPtpGetPTPInt_port
*
* DESCRIPTION:
*       This routine gets port PTP interrupt status.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpInt
);

/*******************************************************************************
* mdPtpGetPTPGlobalTime_port
*
* DESCRIPTION:
*       This routine gets the port global timer value that is running off of the free
*        running clock.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPGlobalTime_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpTime
);

/*******************************************************************************
* madTaiSetEventConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_TAI_EVENT_CONFIG    *eventData
);

/*******************************************************************************
* madTaiGetEventConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_CONFIG    *eventData
);

/*******************************************************************************
* madTaiGetEventStatus_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventStatus_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_STATUS    *status
);

/*******************************************************************************
* madTaiGetEventInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *intStatus
);

/*******************************************************************************
* madTaiGetTrigInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *intStatus
);

/*******************************************************************************
* madTaiSetTrigConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     trigEn,
  IN  MAD_TAI_TRIGGER_CONFIG    *trigData
);

/*******************************************************************************
* madTaiGetTrigConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *trigEn,
  OUT MAD_TAI_TRIGGER_CONFIG    *trigData
);

/*******************************************************************************
* madTaiGetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the port PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiGetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *clk
);

/*******************************************************************************
* madTaiSetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the port PTP 
*        Global Time Counter.
*
* INPUTS:
*        clk        - time stamping clock period
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        clk
);

/*******************************************************************************
* madTaiSetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine sets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     multiEn,
  IN  MAD_32        syncTime
);


/*******************************************************************************
* madTaiGetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine gets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *multiEn,
  OUT MAD_32        *syncTime
);

/*******************************************************************************
* madTaiGetTimeIncDec_port
*
* DESCRIPTION:
*         This routine retrieves port Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        expired    - MAD_TRUE if inc/dec occurred, MAD_FALSE otherwise
*        inc        - MAD_TRUE if increment, MAD_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         Time increment or decrement will be excuted once.
*
*******************************************************************************/
MAD_STATUS madTaiGetTimeIncDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *expired,
  OUT MAD_BOOL        *inc,
  OUT MAD_U32        *amount
);

/*******************************************************************************
* madTaiSetTimeInc_port
*
* DESCRIPTION:
*         This routine enables port time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        amount    - time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
);

/*******************************************************************************
* madTaiSetTimeDec_port
*
* DESCRIPTION:
*         This routine enables port time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*       port - The logical port number
*        amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
);

/*******************************************************************************
* mdFiberSetAutoNeg
*
* DESCRIPTION:
*       This routine enables or disables Fiber Auto-Neg with the specified mode.
*       When enables Auto-Neg, advertisement register value is updated with the  
*       combination of the followings:
*           Asym Pause, Pause, 1000FDX, 1000HDX,
*           Master/Slave Manual, Force Master, Port Type, 1000FDX, and 1000HDX
*       When disables Auto-Neg, only one of the following modes is supported:
*           1000FDX, or 1000HDX.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_FIBER_AUTO_AD_ASYM_PAUSE,
*                   MAD_FIBER_AUTO_AD_PAUSE,
*                   MAD_FIBER_AUTO_AD_1000FDX,
*                   MAD_FIBER_AUTO_AD_1000HDX,
*              if en is MAD_FALSE,
*                   MAD_FIBER_AUTO_AD_ASYM_PAUSE,
*                   MAD_FIBER_AUTO_AD_PAUSE,
*                   MAD_PHY_1000FDX,
*                   MAD_PHY_1000HDX,
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 1, Reg 0, Bit 15,13,12,8 : Soft reset,Speed,Auto-neg enable,Duplex
*       Page 1, Reg 4, Bit 11,10,8:5  : Advertisement Register
*
*******************************************************************************/
MAD_STATUS mdFiberSetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en,
    IN  MAD_U32   mode
);

/*******************************************************************************
* mdFiberGetAutoNeg
*
* DESCRIPTION:
*       This routine get the specified mode of Fiber Auto-Neg.
*       When enables Auto-Neg, advertisement register value is updated with the  
*       combination of the followings:
*           Asym Pause, Pause, 1000FDX, 1000HDX,
*           Master/Slave Manual, Force Master, Port Type, 1000FDX, and 1000HDX
*       When disables Auto-Neg, only one of the following modes is supported:
*           1000FDX, or 1000HDX.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_FIBER_AUTO_AD_ASYM_PAUSE,
*                   MAD_FIBER_AUTO_AD_PAUSE,
*                   MAD_FIBER_AUTO_AD_1000FDX,
*                   MAD_FIBER_AUTO_AD_1000HDX,
*              if en is MAD_FALSE,
*                   MAD_FIBER_AUTO_AD_ASYM_PAUSE,
*                   MAD_FIBER_AUTO_AD_PAUSE,
*                   MAD_PHY_1000FDX,
*                   MAD_PHY_1000HDX,
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 1, Reg 0, Bit 13,12,8   : Speed, Auto-neg enable, and Duplex
*       Page 1, Reg 4, Bit 11,10,8:5 : Advertisement Register
*
*******************************************************************************/
MAD_STATUS mdFiberGetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    OUT MAD_BOOL  *en,
    OUT MAD_U32   *mode
);

#ifdef __cplusplus
}
#endif

#endif /* __msApi_h */
