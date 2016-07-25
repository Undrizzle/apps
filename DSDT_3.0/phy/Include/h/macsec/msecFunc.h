#include <madCopyright.h>

/********************************************************************************
* msecFunc.h
*
* DESCRIPTION:
*       Definitions for MacSec control API's
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __msecFunc_h
#define __msecFunc_h
#ifdef __cplusplus
extern "C" {
#endif

#include "msec_defines.h"
#include "aes.h"

/*#ifdef DEBUG_ALL */
#if 0
  #define DEBUG      1
  #define DEBUG_CPU  1
  #define DEBUG_BITS 1
#endif
extern int DEBUG_MSEC_WRITE;
extern int DEBUG_MSEC_READ;
/*******************************************************************************
* msecWriteReg
*
* DESCRIPTION:
*       This routine write data Value to MAD MacSec register address.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       addr - MAD MacSec register addr.
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
MAD_STATUS msecWriteReg 
(
    IN  MAD_DEV    *dev,
    IN  MAD_U16    addr,
    IN  MAD_U32    data
);


/*******************************************************************************
* msecReadReg
*
* DESCRIPTION:
*       This routine read data Value to MAD MacSec register address.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       addr - MAD MacSec register addr.
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
MAD_STATUS msecReadReg 
(
    IN  MAD_DEV    *dev,
    IN  MAD_U16    addr,
    OUT  MAD_U32    *data
);


/*******************************************************************************
* msec_force_macsec_rst
*
* DESCRIPTION:
*       Task to toggle hard reset for olny MacSec Block.
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
MAD_STATUS msec_force_macsec_rst 
(
    IN  MAD_DEV    *dev
);


/*******************************************************************************
* msec_port_set_nxtpn_ent
*
* DESCRIPTION:
*            Program NxtPn Value to an entry or a context number in replay table for a given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*       ent_num  - Entry number to be disable  
*        nxt_pn   - NxtPn value to be programmed
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
MAD_STATUS msec_port_set_nxtpn_ent 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            ent_num,
    IN    MAD_U32        nxt_pn
);


/*******************************************************************************
* port_fips_obv
*
* DESCRIPTION:
*            Per Port FIPS Obeservation 128 bit vector after AES or GHASH operation is triggered.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        fips_cfg - FIPS_CFG_T
*
* OUTPUTS:
*        fips_out - 128 bits AES or GHASH observation vector output (fips_out[3] -> [31:0], fips_out[0] -> [127:96])
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS port_fips_obv 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  FIPS_CFG_T    *fips_cfg,
    OUT    MAD_U32        *fips_out
);

/*******************************************************************************
* clear_fips_vec
*
* DESCRIPTION:
*            Clears Per Port FIPS Control Register
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
MAD_STATUS clear_fips_vec 
(
    IN  MAD_DEV        *dev,
    IN    int            port
);

/*******************************************************************************
* msec_port_get_macsec_stat_value
*
* DESCRIPTION:
*            Read MACSec Statistics Counter
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        stat_name - Which Counter need to read
*
* OUTPUTS:
*        data -  Statistics Counter Value
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_macsec_stat_value 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            stat_name,
    OUT MAD_U32 *data
);

/*******************************************************************************
* msec_port_get_macsec_stat
*
* DESCRIPTION:
*            Read all MACSec Statistics Counters and store in MACSEC_STAT_T structure
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        macsec_stat MACSEC_STAT_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_macsec_stat 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    OUT    MACSEC_STAT_T    *macsec_stat
);

/*******************************************************************************
* port_acc_macsec_stat
*
* DESCRIPTION:
*            Accumualte all MACSec Statistics Counters in MACSEC_STAT_T structure
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        macsec_stat - MACSEC_STAT_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS port_acc_macsec_stat 
(
    IN  MAD_DEV            *dev,
    IN    int                port,
    IN    MACSEC_STAT_T    *macsec_stat
);

/*******************************************************************************
* port_rd_n_dump_macsec_stat
*
* DESCRIPTION:
*            Reads and prints all the Macsec Statistics per port
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
MAD_STATUS port_rd_n_dump_macsec_stat 
(
    IN  MAD_DEV        *dev,
    IN    int            port
);


/*******************************************************************************
* rd_n_dump_macsec_stat
*
* DESCRIPTION:
*            Program an ELU Table Match fields entry for a port
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
int rd_n_dump_macsec_stat 
(
    IN  MAD_DEV        *dev
);

/*******************************************************************************
* msec_clr_macsec_stat
*
* DESCRIPTION:
*            clears all the Macsec Statistics for all ports by reading them out
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
MAD_STATUS msec_clr_macsec_stat 
(
    IN  MAD_DEV        *dev
);

/*******************************************************************************
* port_get_mib_stat_value
*
* DESCRIPTION:
*            Read WireMAC MIB Statistics Counter
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*        stat_name - Which Counter need to read
*
* OUTPUTS:
*        data -  Statistics Counter Value
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS port_get_mib_stat_value 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN  int            stat_name,
    OUT MAD_U32        *data
);

/*******************************************************************************
* msec_port_get_mib_stat
*
* DESCRIPTION:
*            Read all WireMac MIB Statistics Counters and store in MIB_STAT_T structure
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        port - port number.
*
* OUTPUTS:
*        mib_stat - MIB_STAT_T
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_port_get_mib_stat 
(
    IN  MAD_DEV        *dev,
    IN    int            port,
    IN    MIB_STAT_T    *mib_stat
);

/*******************************************************************************
* port_rd_n_dump_mib_stat
*
* DESCRIPTION:
*            Reads and prints all the WireMac MIB Statistics per port
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
MAD_STATUS port_rd_n_dump_mib_stat 
(
    IN  MAD_DEV        *dev,
    IN    int            port
);

/*******************************************************************************
* rd_n_dump_mib_stat
*
* DESCRIPTION:
*            Reads and prints all the WireMac MIB Statistics for all ports
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
int rd_n_dump_mib_stat 
(
    IN  MAD_DEV        *dev
);

/*******************************************************************************
* msec_clr_mib_stat
*
* DESCRIPTION:
*            clears all the  WireMac MIB Statistics for all ports by reading them out
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*        None
*
* RETURNS:
*       1   - on success
*       0   - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS msec_clr_mib_stat 
(
    IN  MAD_DEV        *dev
);

/*******************************************************************************
* aes_hkey
*
* DESCRIPTION:
*            Calcluate AES hash
*
* INPUTS:
*        ctx  - CTX_T  (Context(key, hkey) Table Fields)
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
int aes_hkey 
(
    IN    CTX_T    *ctx
);

/*******************************************************************************
* msec_dis_macsec_clk
*
* DESCRIPTION:
*       Set macsec clock disable
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

MAD_STATUS msec_dis_macsec_clk 
(
    IN  MAD_DEV    *dev
);

/*******************************************************************************
* msec_en_macsec_clk
*
* DESCRIPTION:
*       Set macsec clock enable
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

MAD_STATUS msec_en_macsec_clk 
(
    IN  MAD_DEV    *dev
);

#ifdef __cplusplus
}
#endif

#endif /* __msecFunc_h */
