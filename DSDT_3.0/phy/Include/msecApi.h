#include <madCopyright.h>

/********************************************************************************
* msecApi.h
*
* DESCRIPTION:
*       Definitions for MacSec control API's
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __msecApi_h
#define __msecApi_h
#ifdef __cplusplus
extern "C" {
#endif

#include "msec_defines.h"
#include "aes.h"

#define IN          

#define OUT        


/*******************************************************************************
* msecWritePortReg
*
* DESCRIPTION:
*       This routine write Port register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       addr - MacSec register addr.
*       data - data value to be written
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
MAD_STATUS msecWritePortReg 
(
    IN  MAD_DEV    *dev,
    IN    int           port,
    IN  MAD_U16    addr,
    IN  MAD_U32    data
);


/*******************************************************************************
* msecReadPortReg
*
* DESCRIPTION:
*       This routine read Port register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       addr - MacSec register addr.
*   
* OUTPUTS:
*       data - read data value
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msecReadPortReg 
(
    IN  MAD_DEV    *dev,
    IN    int           port,
    IN  MAD_U16   addr,
    OUT MAD_U32   *data
);


/*******************************************************************************
* msec_maxwell_ae_start_init
*
* DESCRIPTION:
*        Initialize Maxwell AE core
*        Brings Maxwell core out of reset and into a basic bypass configuration with all
*        traffic passing through the core.  In this state the core is ready to begin
*        encrypt and decrypt operations.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       speed - Indcates port operation speed 
*                (2 -> 1000 Mbps, 1 -> 100 Mbps, 0 -> 10 Mbps)
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
MAD_STATUS msec_maxwell_ae_start_init 
(
    IN  MAD_DEV        *dev
);
MAD_STATUS msec_mac_cfg 
(
	IN  MAD_DEV	 *dev,
	IN  int		  speed
);
/*******************************************************************************
* msec_port_mac_init
*
* DESCRIPTION:
*        Mac Initialization for the given Port
*        Enable MIBS counter, Setup Loopback and MRU.
*        Put Mac out of reset.
*        Disable Mac. Set speed and duplex mode. Enable Mac
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctrl    -  MAC_CTRL  
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
MAD_STATUS msec_port_mac_init 
(
    IN  MAD_DEV        *dev,
    IN    int           port,
    IN  MAC_CTRL    *mac_ctrl
);

/*******************************************************************************
* msec_port_set_mac_ctrl
*
* DESCRIPTION:
*            Per Port programming mac_control register
*            Port Enable/Disable.
*            Enable/Disable MIB counters update for this port
*            Set  Maximum Receive Packet Size - MRU
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctr   -  MAC_CTRL  
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
MAD_STATUS msec_port_set_mac_ctrl 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  MAC_CTRL    *mac_ctrl
);

/*******************************************************************************
* msec_port_set_mac_link
*
* DESCRIPTION:
*        Per Port MAC Link Up/Down
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctr   - MAC_CTRL  
*       mac_link  - MAC Link. 1 -> Port MAC Link up, 0 -> Port MAC Link Down
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
MAD_STATUS msec_port_set_mac_link 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  MAC_CTRL    *mac_ctrl,
    IN  MAD_32        mac_link
);

/*******************************************************************************
* msec_port_set_mac_rst
*
* DESCRIPTION:
*        Per Port Assert/Deassert MAC from Reset
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctr   -  MAC_CTRL  
*        mac_rst   -  MAC Reset. 1 -> Port MAC is reset, 0 -> Port MAC is not reset
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
MAD_STATUS msec_port_set_mac_rst 
(
    IN  MAD_DEV        *dev,
    IN    int           port,
    IN  MAC_CTRL    *mac_ctrl,
    IN  int             mac_rst
);

/*******************************************************************************
* msec_port_mac_en_dis
*
* DESCRIPTION:
*            Per Port Enable/Disable Mac
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctr   -  MAC_CTRL  
*        mac_en    -  MAC Enable/Disable. 1 -> Port MAC is enable, 0 -> Port MAC is disable
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
MAD_STATUS msec_port_mac_en_dis 
(
    IN  MAD_DEV        *dev,
    IN    int           port,
    IN  MAC_CTRL    *mac_ctrl,
    IN  int             mac_en
);

/*******************************************************************************
* msec_port_set_mac_speed
*
* DESCRIPTION:
*            Per Port Speed and Duplex setting for MAC
*            Disable Mac. Set speed and Duplex. Enable Mac.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       mac_ctr   -  MAC_CTRL  
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
MAD_STATUS msec_port_set_mac_speed 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  MAC_CTRL    *mac_ctrl
);

/*******************************************************************************
* msec_port_get_mac_speed
*
* DESCRIPTION:
*            Per Port Reads Speed and Duplex setting for MAC
*            Reads system or wires side Port AUTO_NEG_CONTROL register based on mac_ctrl->mac_type.
*            Returns Speed and Duplex setting for MAC
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*       mac_ctr   -  MAC_CTRL  
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_mac_speed 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT  MAC_CTRL    *mac_ctrl
);

/*******************************************************************************
* msec_port_get_mac_ctrl
*
* DESCRIPTION:
*            Per Port Reading mac_control0 register
*            Read system or wire side Port mac_control0 register based on mac_trl.mac_type.
*            Returns port_en, mibcnt_en, lpbk_en and max_frame_sz value
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*       mac_ctr   -  MAC_CTRL  
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_mac_ctrl 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT  MAC_CTRL    *mac_ctrl
);


/*******************************************************************************
* msec_port_get_mac_var
*
* DESCRIPTION:
*            Per Port Read MAC  Control Variables
*            Reads system or wires side Port AUTO_NEG_CONTROL register based on 
*            mac_ctrl->mac_type.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*       mac_ctr   -  MAC_CTRL  
*                     Returns Speed and Duplex setting for MAC
*                     Returns port_en, mibcnt_en, lpbk_en and max_frame_sz value
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_mac_var 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT MAC_CTRL    *mac_ctrl
);


/*******************************************************************************
* msec_port_macsec_init
*
* DESCRIPTION:
*         MACSec Initialization for the given Port
*         Bring MACSec core out of soft reset.
*         Disable all the entries of ELU table.
*         Disable all the entries of ILU table.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
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
MAD_STATUS msec_port_macsec_init 
(
    IN  MAD_DEV        *dev,
    IN    int            port
);

/*******************************************************************************
* msec_port_dis_elu_ent
*
* DESCRIPTION:
*            Disable ELU entry of given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
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
MAD_STATUS msec_port_dis_elu_ent 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num
);

/*******************************************************************************
* msec_port_dis_ilu_ent
*
* DESCRIPTION:
*            Disable ILU entry of given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
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
MAD_STATUS msec_port_dis_ilu_ent 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num
);

/*******************************************************************************
* msec_port_set_elu_entry
*
* DESCRIPTION:
*            Program an ELU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        egr_lkup - LKUP_T (Egress Lookup table fields)
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
MAD_STATUS msec_port_set_elu_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    LKUP_T        *egr_lkup
);



/*******************************************************************************
* msec_port_set_elu_match_fld
*
* DESCRIPTION:
*            Program an ELU Table Match fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        match_fld  - MATCH_FLD (Egress Lookup table match fields)
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
MAD_STATUS msec_port_set_elu_match_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    MATCH_FLD    *match_fld
);

/*******************************************************************************
*  msec_port_set_elu_match_en
*
* DESCRIPTION:
*            Program an ELU Table Match Enable fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number 
*        match_en  - MATCH_EN (Egress Lookup table match enable fields)
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
MAD_STATUS msec_port_set_elu_match_en
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    MATCH_EN    *match_en
);

/*******************************************************************************
* msec_port_set_elu_act_fld
*
* DESCRIPTION:
*            Program an ELU Table Action fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        act_fld  - ACT_FLD (Egress Lookup table Action fields)
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
MAD_STATUS msec_port_set_elu_act_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    ACT_FLD        *act_fld
);

/*******************************************************************************
* msec_port_set_ect_entry
*
* DESCRIPTION:
*            Program an ECT entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        egr_ctx  CTX_T (Egress Context table fields except key, hkey)
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
MAD_STATUS msec_port_set_ect_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_set_ekey_entry
*
* DESCRIPTION:
*            Program an Encrypt Key Table (EKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        egr_ctx  CTX_T (Egress Context Key)
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
MAD_STATUS msec_port_set_ekey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_set_ehkey_entry
*
* DESCRIPTION:
*            Program an Egress Hash Key Table (EHKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        egr_ctx  - CTX_T (Egress Context HKey)
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
MAD_STATUS msec_port_set_ehkey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_get_elu_entry
*
* DESCRIPTION:
*            Read an ELU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        egr_lkup LKUP_T (Egress Lookup table fields)
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
MAD_STATUS msec_port_get_elu_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    LKUP_T        *egr_lkup
);

/*******************************************************************************
* msec_port_get_elu_match_fld
*
* DESCRIPTION:
*            Read an ELU Entry Match fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        elu_match  - MATCH_FLD (Egress Lookup table Match fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_elu_match_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    MATCH_FLD    *elu_match
);

/*******************************************************************************
* msec_port_get_elu_match_en
*
* DESCRIPTION:
*            Read an ELU Entry Match Enables for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        elu_match_en -  MATCH_EN (Egress Lookup table Match Enables)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_elu_match_en 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    MATCH_EN    *elu_match_en
);

/*******************************************************************************
* msec_port_get_elu_act
*
* DESCRIPTION:
*            Read an ELU Entry Action Fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        elu_act  - ACT_FLD (Egress Lookup table Action fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_elu_act 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    ACT_FLD        *elu_act
);


/*******************************************************************************
* msec_port_get_ect_entry
*
* DESCRIPTION:
*            Read an ECT entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        egr_ctx  - CTX_T (Egress Context table fields except key, hkey)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ect_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_get_ekey_entry
*
* DESCRIPTION:
*            Read an Encrypt Key Table (EKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        egr_ctx  - CTX_T (Egress Context Key)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ekey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_get_ehkey_entry
*
* DESCRIPTION:
*            Read an Egress Hash Key Table (EHKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        egr_ctx  - CTX_T (Egress Context HKey)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ehkey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    CTX_T        *egr_ctx
);


/*******************************************************************************
* msec_port_set_ilu_entry
*
* DESCRIPTION:
*            Program an ILU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        igr_lkup - LKUP_T (Ingress Lookup Table Fields)
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
MAD_STATUS msec_port_set_ilu_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    LKUP_T        *igr_lkup
);

/*******************************************************************************
* msec_port_set_ilu_match_fld
*
* DESCRIPTION:
*            Program an ILU Table Match fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        match_fld - MATCH_FLD (Ingress Lookup Table Match Fields)
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
MAD_STATUS msec_port_set_ilu_match_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    MATCH_FLD    *match_fld
);

/*******************************************************************************
* msec_port_set_ilu_match_en
*
* DESCRIPTION:
*            Program an ILU Table Match enable fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        match_en  - MATCH_EN  (Ingress Lookup Table Match enable Fields)
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
MAD_STATUS msec_port_set_ilu_match_en 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    MATCH_EN    *match_en
);

/*******************************************************************************
* msec_port_set_ilu_act_fld
*
* DESCRIPTION:
*            Program an ILU Table Action fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        act_fld   - ACT_FLD (Ingress Lookup Table Action Fields)
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
MAD_STATUS msec_port_set_ilu_act_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    ACT_FLD        *act_fld
);

/*******************************************************************************
* msec_port_set_ikey_entry
*
* DESCRIPTION:
*            Program Ingress Decrypt key (IKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        igr_ctx  - CTX_T  (Ingress Context Key)
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
MAD_STATUS msec_port_set_ikey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    CTX_T        *igr_ctx
);

/*******************************************************************************
* msec_port_set_ihkey_entry
*
* DESCRIPTION:
*            Program Ingress Hash key (IHKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        igr_ctx  - CTX_T  (Ingress Context Hkey)
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
MAD_STATUS msec_port_set_ihkey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    CTX_T        *igr_ctx
);


/*******************************************************************************
* msec_stat_init
*
* DESCRIPTION:
*         MIBs and MacSEC Statistics Initialization
*         Program MIBSs and MacSEC stats to clear on read.
*         Enable TX and RX histograms
*         Clears Macsec and MIB statistics
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
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
MAD_STATUS msec_stat_init 
(
    IN  MAD_DEV        *dev
);

/*******************************************************************************
* msec_port_set_egr_entry
*
* DESCRIPTION:
*         Program Egress Entry Security Association (SA) for a port
*         Disable ELU entry.
*         Program ECT and EKEY table entry.
*         Calculate Hash key and program EHKEY table entry.
*         Program ELU entry.s
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*        ent_num  - Entry number
*        egr_lkup - LKUP_T (Egress Lookup table fields)
*        egr_ctx  - CTX_T  (Egress Context, key, hkey table fields)
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
MAD_STATUS msec_port_set_egr_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    int            ent_num, 
    IN    LKUP_T        *egr_lkup, 
    IN    CTX_T        *egr_ctx
);


/*******************************************************************************
* msec_port_get_egr_entry
*
* DESCRIPTION:
*         Read Egress Entry Security Association (SA) for a port
*         Read an entry for ELU, ECT, EKEY and EHKEY tables
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*        ent_num  - Entry number
*
* OUTPUTS:
*        egr_lkup - LKUP_T (Egress Lookup table fields)
*        egr_ctx  - CTX_T  (Egress Context, key, hkey table fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_egr_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    int            ent_num, 
    OUT    LKUP_T        *egr_lkup, 
    OUT    CTX_T        *egr_ctx
);

/*******************************************************************************
* msec_port_set_igr_entry
*
* DESCRIPTION:
*         Program Ingress Entry Security Association (SA) for a port
*         Disable ILU entry.
*         Program IKEY table entry.
*         Calculate Hash key and program IHKEY table entry.
*         Initialize replay table (NxtPn) entry to 1.
*         Program ILU entry.
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*        ent_num  - Entry number
*        igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*        igr_ctx  - CTX_T  (Ingress Context(key, hkey) Table Fields)
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
MAD_STATUS msec_port_set_igr_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    int            ent_num, 
    IN    LKUP_T        *igr_lkup, 
    IN    CTX_T        *igr_ctx
);

/*******************************************************************************
* msec_port_get_igr_entry
*
* DESCRIPTION:
*         Read Ingress Entry Security Association (SA) for a port
*         Read an entry for ILU, IKEY and IHKEY tables
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*        ent_num  - Entry number
*
* OUTPUTS:
*        igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*        igr_ctx  - CTX_T  (Ingress Context(key, hkey) Table Fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    int            ent_num, 
    OUT    LKUP_T        *igr_lkup, 
    OUT    CTX_T        *igr_ctx
);

/*******************************************************************************
* msec_port_get_ilu_entry
*
* DESCRIPTION:
*            Read an ILU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ilu_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    LKUP_T        *igr_lkup
);

/*******************************************************************************
* msec_port_get_ilu_match_fld
*
* DESCRIPTION:
*            Read an ILU entry Match Fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        ilu_match - MATCH_FLD (Ingress Lookup Table Match Fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ilu_match_fld 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    MATCH_FLD    *ilu_match
);

/*******************************************************************************
* msec_port_get_ilu_match_en
*
* DESCRIPTION:
*            Read an ILU entry Match Enables for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        ilu_match_en - MATCH_EN (Ingress Lookup Table Match Enables)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ilu_match_en 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    MATCH_EN    *ilu_match_en
);

/*******************************************************************************
* msec_port_get_ilu_act
*
* DESCRIPTION:
*            Read an ILU entry Actions for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        ilu_act  - ACT_FLD (Ingress Lookup Table Action Fields)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ilu_act 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    ACT_FLD        *ilu_act
);

/*******************************************************************************
* msec_port_get_ikey_entry
*
* DESCRIPTION:
*            Read Ingress Decrypt key (IKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        igr_ctx  - CTX_T (Ingress Context Key)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ikey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    CTX_T        *igr_ctx
);

/*******************************************************************************
* msec_port_get_ihkey_entry
*
* DESCRIPTION:
*            Read Ingress Hash key (IHKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        igr_ctx  - CTX_T (Ingress Context HKey)
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_ihkey_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT    CTX_T        *igr_ctx
);

/*******************************************************************************
* msec_port_get_nxtpn_entry
*
* DESCRIPTION:
*            Read NxtPn Value to an entry or a context number in replay table for a given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*        data =  NxtPn value 
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_nxtpn_entry 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    OUT MAD_U32     *data
);

/*******************************************************************************
* msec_port_set_redir_sa
*
* DESCRIPTION:
*            edirect SA per port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        redir_sa - Redirect SA per port
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
MAD_STATUS msec_port_set_redir_sa 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  MAD_U64    sa
);

/*******************************************************************************
* msec_port_get_redir_sa
*
* DESCRIPTION:
*            Read Redirect SA per port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        Redirect SA per port
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_redir_sa 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT MAD_U64        *data
);

/*******************************************************************************
* msec_port_set_igr_def_act
*
* DESCRIPTION:
*            Per Port Ingress MACSec default action configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        def_act - IGR_DEF_T
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
MAD_STATUS msec_port_set_igr_def_act 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    IGR_DEF_T    *def_act
);

/*******************************************************************************
* msec_port_set_igr_redir_vec
*
* DESCRIPTION:
*            Per Port Ingress MACSec Redirect action configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        redir_vec - REDIR_T
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
MAD_STATUS msec_port_set_igr_redir_vec 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    REDIR_T        *redir_vec
);

/*******************************************************************************
* msec_port_set_igr_rpy_var
*
* DESCRIPTION:
*            Per Port Ingress MACSec Replay and Validate frame configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        rpy_var  - RPY_T 
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
MAD_STATUS msec_port_set_igr_rpy_var 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    RPY_T        *rpy_var
);

/*******************************************************************************
* msec_port_set_igr_sci_info
*
* DESCRIPTION:
*            Per Port Ingress MACSec wdefault SCI related configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        sci_info  - SCI_T
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
MAD_STATUS msec_port_set_igr_sci_info 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    SCI_T        *sci_info
);

/*******************************************************************************
* msec_port_get_igr_def_act
*
* DESCRIPTION:
*            Get Per Port Ingress MACSec Default Actions
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        def_act  - IGR_DEF_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_def_act 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    IGR_DEF_T    *def_act
);

/*******************************************************************************
* msec_port_get_igr_redir_vec
*
* DESCRIPTION:
*            Get Per Port Ingress MACSec Redirect Vector
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        redir_vec - REDIR_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_redir_vec 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    REDIR_T        *redir_vec
);

/*******************************************************************************
* msec_port_get_igr_rpy_var
*
* DESCRIPTION:
*            Get Per Port Ingress MACSec Replay Vector values
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        rpy_var  - RPY_T  
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_rpy_var 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    RPY_T        *rpy_var
);

/*******************************************************************************
* msec_port_get_igr_sci_info
*
* DESCRIPTION:
*            Get Per Port Ingress MACSec SCI Information
*            Returns implicit_sci, default_sci abd scb_port values.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        sci_info - SCI_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_sci_info 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    SCI_T        *sci_info
);

/*******************************************************************************
* msec_set_fips_vec
*
* DESCRIPTION:
*            FIPS Vector setting
*            It programs 128 bit input vector (Plain Text).
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        fips_cfg FIPS_CFG_T
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
int msec_set_fips_vec 
(
    IN  MAD_DEV        *dev,
    IN    FIPS_CFG_T    *fips_cfg
);


/*******************************************************************************
* msec_port_fips_ctl
*
* DESCRIPTION:
*            Per Port FIPS Control Setting
*            It programs key or hash key for egress or ingress path as defined by user.
*            For AES operation, Key[0] is programmed.
*            For GHASH operation, Hash Key[0] is programmed.
*            It triggers AES or GHASH operation.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        fips_cfg FIPS_CFG_T
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
MAD_STATUS msec_port_fips_ctl 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    FIPS_CFG_T    *fips_cfg
);

/*******************************************************************************
* msec_set_redir_hdr
*
* DESCRIPTION:
*         Redirect header setting
*         Set Redirect MAC DA, MAC SA and Ethertype
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        redir_fld - REDIR_HDR_T
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
MAD_STATUS msec_set_redir_hdr 
(
    IN  MAD_DEV        *dev,
    IN    REDIR_HDR_T *redir_fld
);

/*******************************************************************************
* msec_get_redir_hdr
*
* DESCRIPTION:
*         Read Redirect header setting
*         Read Redirect MAC DA, MAC SA and Ethertype
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*        redir_fld - REDIR_HDR_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_get_redir_hdr 
(
    IN  MAD_DEV        *dev,
    OUT    REDIR_HDR_T *redir_fld
);

/*******************************************************************************
* msec_port_cmn_cfg
*
* DESCRIPTION:
*         Per Port common MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*        cmn_cfg   - CMN_CFG_T
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
MAD_STATUS msec_port_cmn_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    CMN_CFG_T    *cmn_cfg
);

/*******************************************************************************
* msec_port_get_cmn_cfg
*
* DESCRIPTION:
*         Per Port common MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*
* OUTPUTS:
*        cmn_cfg   - CMN_CFG_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_cmn_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    CMN_CFG_T    *cmn_cfg
);

/*******************************************************************************
* msec_port_egr_cfg
*
* DESCRIPTION:
*         Per Port Egress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*        egr_cfg   - EGR_CFG_T
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
MAD_STATUS msec_port_egr_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    EGR_CFG_T    *egr_cfg
);

/*******************************************************************************
* msec_port_get_egr_cfg
*
* DESCRIPTION:
*         Get Per Port Egress MACSec configuration value
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*
* OUTPUTS:
*        egr_cfg   - EGR_CFG_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_egr_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    EGR_CFG_T    *egr_cfg
);

/*******************************************************************************
* msec_port_igr_cfg
*
* DESCRIPTION:
*         Per Port Ingress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*        igr_cfg   - IGR_CFG_T
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
MAD_STATUS msec_port_igr_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    IGR_CFG_T    *igr_cfg
);

/*******************************************************************************
* msec_port_get_igr_cfg
*
* DESCRIPTION:
*         Get Per Port Ingress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*
* OUTPUTS:
*        igr_cfg   - IGR_CFG_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_igr_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    IGR_CFG_T    *igr_cfg
);

/*******************************************************************************
* msec_port_etm_cfg
*
* DESCRIPTION:
*         Per Port ETM configuration
*         Configures individual entries in the ethertype match table.  Values to match agains
*         are specified by the etm[] entries, and the etm_eg_en and etm_ig_en enable matching
*         on each of these on a per-bit basis.  For example, setting etm_eg_en to 0x13 would enable
*         entries 0, 1, and 4.
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*        etm_cfg   - ETM_CFG_T
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
MAD_STATUS msec_port_etm_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    ETM_CFG_T    *etm_cfg
);

/*******************************************************************************
* msec_port_get_etm_cfg
*
* DESCRIPTION:
*         Get Per Port ETM configuration.
*         Get all the ETM ethertype and ETM control variables
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*
* OUTPUTS:
*        etm_cfg   - ETM_CFG_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_etm_cfg 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    ETM_CFG_T    *etm_cfg
);

/*******************************************************************************
* msec_port_fips_chk
*
* DESCRIPTION:
*         Per Port FIPS Compliance Check 
*         It programs 128 bit input vector (Plain Text).
*         It    also programs key or hash key for egress or ingress path as defined by user.
*         For AES operation, Key[0] is programmed.
*         For GHASH operation, Hash Key[0] is programmed.
*        It triggers AES or GHASH operation and returns 128 bit output vector when operation is done.
*        After operation is done, It clears FIPS_CTL register.
*        It also toggles MACSEC core soft reset.
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*        port      - port number.
*       fips_cfg  - FIPS_CFG_T
*
* OUTPUTS:
*        fips_out  - 128 bits AES or GHASH observation vector output 
*                    (fips_out[3] -> [31:0], fips_out[0] -> [127:96])
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_fips_chk 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    FIPS_CFG_T    *fips_cfg,
    OUT MAD_U32 *fips_out
);

/******************************************************************************
*
*   Added APIs
*
*******************************************************************************/
/*******************************************************************************
* msec_force_flow_ctrl_en
*
* DESCRIPTION:
*       Force to set flow control enable.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
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

MAD_STATUS msec_force_flow_ctrl_en 
(
    IN  MAD_DEV    *dev
);

/*******************************************************************************
* msec_set_phy_speed_duplex
*
* DESCRIPTION:
*       Set Phy speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
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

MAD_STATUS msec_set_phy_speed_duplex 
(
    IN  MAD_DEV    *dev,
    IN  int         port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
);

/*******************************************************************************
* msec_set_macsec_speed_duplex
*
* DESCRIPTION:
*       Set MacSec speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
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

MAD_STATUS msec_set_macsec_speed_duplex 
(
    IN  MAD_DEV    *dev,
    IN  int         port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
);

/*******************************************************************************
* msec_set_mac_frame_size_limit
*
* DESCRIPTION:
*       Set Mac frame size limit
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        size - frame size.
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

MAD_STATUS msec_set_mac_frame_size_limit 
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    size
);

/*******************************************************************************
* msec_enable_non_disruptive_loopback
*
* DESCRIPTION:
*       Set Non disruptive loopback enable
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
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

MAD_STATUS msec_enable_non_disruptive_loopback 
(
    IN  MAD_DEV    *dev
);

MAD_MSEC_REV check_testchip_revision (MAD_DEV* dev);
#ifdef __cplusplus
}
#endif

#endif /* __msecApi_h */
