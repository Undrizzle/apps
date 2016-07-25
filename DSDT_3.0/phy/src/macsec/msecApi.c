#include <madCopyright.h>

/********************************************************************************
* msecApi.c
* 
* DESCRIPTION:
*       APIs to control Marvell MacSec.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include "msecFunc.h"
#include "msecApi.h"

#undef DEBUG

#ifdef DEBUG
void msec_print_mac_ctrl (MAC_CTRL *mac_ctrl);
void msec_print_match_en (MATCH_EN *match_en);
void msec_print_match_fld (MATCH_FLD *match_fld);
void msec_print_act_fld (ACT_FLD *act_fld);
void msec_print_lkup (LKUP_T *lkup);
void msec_print_ect (CTX_T *ctx);
void msec_print_key (CTX_T *ctx);
void msec_print_hkey (CTX_T *ctx);
void msec_print_redir_hdr (REDIR_HDR_T *redir_hdr);
void msec_print_cmn_cfg (CMN_CFG_T *cmn_cfg);
void msec_print_egr_cfg (EGR_CFG_T *egr_cfg);
void msec_print_igr_def (IGR_DEF_T *igr_def);
void msec_print_redir (REDIR_T *redir);
void msec_print_sci_info (SCI_T *sci_info);
void msec_print_rpy_info (RPY_T *rpy_info);
void msec_print_igr_cfg (IGR_CFG_T *igr_cfg);
void msec_print_etm_cfg (ETM_CFG_T *etm_cfg);
void msec_print_fips_cfg (FIPS_CFG_T *fips_cfg);
#endif
void msec_print_macsec_stat (MACSEC_STAT_T *macsec_stat);
void msec_print_mib_stat (MIB_STAT_T *mib_stat);

int msec_bits_write (MAD_U32 *data, int in_data, int bit_to, int bit_from);
int msec_bits_get (int in_data, int bit_to, int bit_from);

/*******************************************************************************
* msecWritePortReg
*
* DESCRIPTION:
*       This routine write Port register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       addr - MacSec register addr.
*       data - data value to be written
*   
* OUTPUTS:
*		None
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
	IN	int		   port,
    IN  MAD_U16    addr,
    IN  MAD_U32    data
)
{
  MAD_STATUS   retVal;

  /* get base addr value */
  switch (port)
  {
      case 0 : 
          addr += VN_SLICE0(dev); 
          break;
      case 1 : 
          addr += VN_SLICE1(dev); 
          break;
      case 2 : 
          addr += VN_SLICE2(dev); 
          break;
      case 3 : 
          addr += VN_SLICE3(dev); 
          break;
	  default:
		MAD_DBG_ERROR("Do not supported port %d \n", port);
		return MAD_DRV_INVALID_PORT;
  }

#ifdef DEBUG_CPU
  MAD_CRITIC_INFO ("msecWriteReg                : Addr %04x Data %08x\n", 
	                     (unsigned short)addr, (unsigned int)data);
#endif

  retVal = msecWriteReg (dev, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to write register \n");
     return retVal|MAD_API_FAIL_WRITE_PAGED_REG;
  }

  return MAD_OK;
}

/*******************************************************************************
* msecReadPortReg
*
* DESCRIPTION:
*       This routine read Port register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
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
	IN	int		   port,
    IN  MAD_U16   addr,
	OUT MAD_U32   *data
)
{
  MAD_STATUS   retVal;

  /* get base addr value */
  switch (port)
  {
      case 0 :
          addr += VN_SLICE0(dev);
          break;
      case 1 :
          addr += VN_SLICE1(dev);
          break;
      case 2 :
          addr += VN_SLICE2(dev);
          break;
      case 3 :
          addr += VN_SLICE3(dev);
          break;
	  default:
		MAD_DBG_ERROR("Do not supported port %d \n", port);
		return MAD_DRV_INVALID_PORT;
  }
  retVal = msecReadReg (dev, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to read register \n");
     return retVal|MAD_API_FAIL_READ_PAGED_REG;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("msecReadReg                 : Addr %04x Data %08x\n", addr, (unsigned int)*data);
#endif

  return MAD_OK;

}
/* check test chip revision on deviceId MAD_88E1540 and MAD_88E1340 to differentiate MACSec changes */
MAD_MSEC_REV check_testchip_revision (MAD_DEV* dev)
{
  MAD_STATUS   retVal;
  unsigned short data;
  int phyaddr;
  phyaddr = dev->msec_ctrl.port_num + dev->msec_ctrl.dev_num * dev->numOfPorts;
  if (dev->revision == 1)
	  return MAD_MSEC_REV_A1A;
  else if (dev->revision == 2)
      return MAD_MSEC_REV_A2A;
  else if (dev->revision == 0 && dev->deviceId == MAD_88E1340) {
	  if((retVal = madHwReadPagedPhyReg(dev,phyaddr,63,30,&data))
		  != MAD_OK)
	  {
		 MAD_DBG_ERROR("Reading to paged phy page.reg (63,30) failed.\n");
		 return MAD_MSEC_REV_UNKNOWN;
	  }
	  if (data & 1)
	  {
		  return MAD_MSEC_REV_A0B;
	  } 
	  else
	  {
		if((retVal = madHwWritePagedPhyReg(dev,phyaddr,0xff,23,0x100f))
		   != MAD_OK)
		{
		  MAD_DBG_ERROR("Writing to paged phy page.reg (0xff,23) failed.\n");
		  return MAD_MSEC_REV_UNKNOWN;
		}
		if((retVal = madHwReadPagedPhyReg(dev,phyaddr,0xff,25,&data))
		   != MAD_OK)
		{
		   MAD_DBG_ERROR("Reading to paged phy page.reg (0xff,25) failed.\n");
		   return MAD_MSEC_REV_UNKNOWN;
		}
		if (data == 0xC000)
		{
		  MAD_CRITIC_INFO("Get MacSec revision Z0A\n");
		  return MAD_MSEC_REV_Z0A;
		}
		else if (data == 0xC800)
		{
		  MAD_CRITIC_INFO("Get MacSec revision Y0A\n");
		  return MAD_MSEC_REV_Y0A;
		} 
		else
		{
		  MAD_CRITIC_INFO("Get MacSec revision Unknown\n");
		  return MAD_MSEC_REV_UNKNOWN;
		}
	  }
  } else if (dev->revision == 0 && dev->deviceId == MAD_88E1540) {
	  /* 1540L reg 19_254.15:12, Z0 if 0x6, Y0 if 0xA */
		if((retVal = madHwReadPagedPhyReg(dev,phyaddr,0xfe,19,&data))
		   != MAD_OK)
		{
		   MAD_DBG_ERROR("Reading to paged phy page.reg (0xfe,19) failed.\n");
		   return MAD_MSEC_REV_UNKNOWN;
		} else if ((data & 0xF000) == 0x6000) {
			return MAD_MSEC_REV_Z0A;
		} else if ((data & 0xF000) == 0xA000) {
			return MAD_MSEC_REV_Y0A;
		} else {
		  return MAD_MSEC_REV_UNKNOWN;
		}
  }

  MAD_CRITIC_INFO("Get MacSec revision Unknown\n");
  return MAD_MSEC_REV_UNKNOWN;
}

/*******************************************************************************
* msec_maxwell_ae_start_init
*
* DESCRIPTION:
*		brief Initialize Maxwell AE core
*		Brings Maxwell core out of reset and into a basic bypass configuration with all
*		traffic passing through the core.  In this state the core is ready to begin
*		encrypt and decrypt operations.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       speed - Indcates port operation speed 
*				(2 -> 1000 Mbps, 1 -> 100 Mbps, 0 -> 10 Mbps)
* OUTPUTS:
*		None
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
    IN  MAD_DEV     *dev
)
{
  MAD_STATUS   retVal;
  int       port, orig_port;  
  CMN_CFG_T cmn_cfg;
  EGR_CFG_T egr_cfg;
  IGR_DEF_T igr_def; 
  
 
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_MACSEC_CAPABLE))
    return MAD_API_MACSEC_NOT_SUPPORT;

  orig_port = dev->msec_ctrl.port_num;

  MAD_CRITIC_INFO("!!!! msec_maxwell_ae_start_init \n");
/*#ifdef MX_REV_Z0_BUGFIXES */
  
  /* MacSec Configuration */
  for (port=0; port<4; port++)
  {
	MAD_CRITIC_INFO("!!!! MacSec Configuration  port %d \n", port);

	dev->msec_ctrl.port_num  = port;

	/* AE core init */
	retVal = msec_port_macsec_init (dev, port);
	if(retVal != MAD_OK)
	{
	   MAD_DBG_ERROR("Failed to call msec_port_macsec_init \n");
	   return retVal|MAD_MSEC_FAIL_INIT;
	}

	/* common configuration */
	cmn_cfg.trail_sz    = 0;   
	cmn_cfg.conf_offset = 0;
	cmn_cfg.def_vlan    = 0;
	cmn_cfg.mc_high     = 0LL;
	cmn_cfg.mc_low_msk  = 0;
	retVal = msec_port_cmn_cfg (dev, port,&cmn_cfg);
	if(retVal != MAD_OK)
	{
	   MAD_DBG_ERROR("Failed to call msec_port_cmn_cfg \n");
	   return retVal|MAD_MSEC_FAIL_CMN_CFG;
	}

	/* egress configuration */
	egr_cfg.def_drop   = 0;
	egr_cfg.parse_ae   = 0;
	egr_cfg.parse_vlan = 1; 
	retVal = msec_port_egr_cfg (dev, port, &egr_cfg);
	if(retVal != MAD_OK)
	{
	   MAD_DBG_ERROR("Failed to call msec_port_egr_cfg \n");
	   return retVal|MAD_MSEC_FAIL_EGR_CFG;
	}

	/* Ingress configuration  */
	/* not needed.. Default values are fine.*/
	/* Ingress configuration  */
	/* not needed.. Default values are fine except for badtag_chk*/
	igr_def.def_drop    = 0x0;
	igr_def.retain_hdr  = 0x0;
	igr_def.parse_vlan  = 0x1;
	igr_def.badtag_chk  = 0x1F;
	igr_def.def_redir   = 0x0;
	igr_def.drop_badtag = 0x0;
	retVal = msec_port_set_igr_def_act (dev, port, &igr_def);
	if(retVal != MAD_OK)
	{
	   MAD_DBG_ERROR("Failed to call msec_port_set_igr_def_act \n");
	   return retVal|MAD_MSEC_FAIL_IGR_DEF_ACT;   /* Xu define new error status */
	}


  }
 
  /*#endif */
  /* Clear MacSec and MIBs statistics.  */
  dev->msec_ctrl.port_num = 0;
  retVal = msec_stat_init (dev);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_stat_init \n");
     return retVal|MAD_MSEC_FAIL_STATUS_INIT;
  }
  dev->msec_ctrl.port_num = orig_port;

MAD_CRITIC_INFO("!!!! msec_maxwell_ae_start_init done\n");

  return MAD_OK;
}

MAD_STATUS msec_mac_cfg 
(
	IN  MAD_DEV	 *dev,
	IN  int		  speed
)
{
  int port;
  MAC_CTRL  mac_ctrl;
  MAD_STATUS   retVal=MAD_OK;  

  /* Mac configuration */
  for (port=0; port<4; port++)
  {
    dev->msec_ctrl.port_num    = port;
	MAD_CRITIC_INFO("!!!! Mac Configuration  port %d \n", port);

    /* now that configuration is complete, enable the MACs */

    /* SYS MAC control Configuration */
    mac_ctrl.mac_type     = 0;
    mac_ctrl.port_en      = 1;
    mac_ctrl.speed        = speed;
    mac_ctrl.hfd          = 1;
    mac_ctrl.mibcnt_en    = 0;
    mac_ctrl.lpbk_en      = 0;
    mac_ctrl.max_frame_sz = 0x5F2;
    retVal = msec_port_mac_init (dev, port, &mac_ctrl); /* enable sysmac first */
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msec_port_mac_init \n");
       return retVal|MAD_MSEC_FAIL_PORT_MAC_INIT;
	}

    /* WRE MAC control Configuration */
    mac_ctrl.mac_type     = 1;
    mac_ctrl.port_en      = 1;
    mac_ctrl.speed        = speed;
    mac_ctrl.hfd          = 1;
    mac_ctrl.mibcnt_en    = 1;
    mac_ctrl.lpbk_en      = 0;
    mac_ctrl.max_frame_sz = 0x5F2;
    retVal = msec_port_mac_init (dev, port, &mac_ctrl); /* then wire mac */
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msec_port_mac_init \n");
       return retVal|MAD_MSEC_FAIL_PORT_MAC_INIT;
	}
  }
  return retVal;
}
/*******************************************************************************
* msec_port_mac_init
*
* DESCRIPTION:
*		brief Mac Initialization for the given Port
*		Enable MIBS counter, Setup Loopback and MRU.
*		Put Mac out of reset.
*		Disable Mac. Set speed and duplex mode. Enable Mac
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctrl	-  MAC_CTRL  
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		   port,
    IN  MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  MAD_CRITIC_INFO ("Mac Initialization       : Port %0d\n", port);
  msec_print_mac_ctrl(mac_ctrl);
#endif

  /* Bring Mac link down */
  retVal = msec_port_set_mac_link (dev, port, mac_ctrl, 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_link \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_MAC_CTRL;
  }

  /* if Mac is not out of reset, bring Mac out of reset */
  retVal = msec_port_set_mac_rst (dev, port, mac_ctrl, 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_rst \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_MAC_CTRL;
  }

  /* set Mac enable, MIB counter en, loopback, MRU */
  retVal = msec_port_set_mac_ctrl (dev, port, mac_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_ctrl \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_MAC_CTRL;
  }

  /* if mac is not in reset, assert Mac in reset. If Mac is not disable, disable it. */
  /* set speed and duplex. Bring Mac out of reset and enable Mac */
  retVal = msec_port_set_mac_speed (dev, port, mac_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_speed \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_MAC_SPEED;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_mac_ctrl
*
* DESCRIPTION:
*			Per Port programming mac_control register
*			Port Enable/Disable.
*			Enable/Disable MIB counters update for this port
*			Set  Maximum Receive Packet Size - MRU
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctr   -  MAC_CTRL  
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
  MAD_U32 mac_ctrl0;
  MAD_U16 addr;

  mac_ctrl0 = 0x0;
  bit_write  (mac_ctrl0,  mac_ctrl->mibcnt_en,    16);
  msec_bits_write (&mac_ctrl0, mac_ctrl->max_frame_sz, 15, 2);
  bit_write  (mac_ctrl0,  mac_ctrl->lpbk_en,      1); 
  bit_write  (mac_ctrl0,  mac_ctrl->port_en,      0); 
  if (mac_ctrl->mac_type == 0)
      addr  = SYS_MAC_CONTROL0(dev);
  else
      addr  = WIR_MAC_CONTROL0(dev);
  retVal = msecWritePortReg (dev, port, addr, mac_ctrl0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_mac_ctrl
*
* DESCRIPTION:
*			Per Port Reading mac_control0 register
*			Read system or wire side Port mac_control0 register based on mac_trl.mac_type.
*			Returns port_en, mibcnt_en, lpbk_en and max_frame_sz value
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    OUT  MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;


  if (mac_ctrl->mac_type == 0)
      addr = SYS_MAC_CONTROL0(dev);
  else
      addr = WIR_MAC_CONTROL0(dev);
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

  mac_ctrl->mibcnt_en    = bit_get  (data, 16);
  mac_ctrl->max_frame_sz = msec_bits_get (data, 15, 2);
  mac_ctrl->lpbk_en      = bit_get  (data, 1);
  mac_ctrl->port_en      = bit_get  (data, 0);
  if (mac_ctrl->mac_type == 0)
      addr = SYS_MAC_CONTROL0(dev);
  else
      addr = WIR_MAC_CONTROL0(dev);
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  return retVal;
}

/*******************************************************************************
* msec_port_set_mac_link
*
* DESCRIPTION:
*		Per Port MAC Link Up/Down
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctr   - MAC_CTRL  
*       mac_link  - MAC Link. 1 -> Port MAC Link up, 0 -> Port MAC Link Down
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAC_CTRL	*mac_ctrl,
	IN  MAD_32		mac_link
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  if (mac_ctrl->mac_type == 0)
      addr = SYS_HIDDEN0(dev);
  else
      addr = WIR_HIDDEN0(dev);
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

  if (bit_get(data, 6) != mac_link)
  {
    bit_write (data, mac_link, 6);
    retVal = msecWritePortReg (dev, port, addr, data);  
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return MAD_OK;
}


/*******************************************************************************
* msec_port_set_mac_rst
*
* DESCRIPTION:
*		brief Per Port Assert/Deassert MAC from Reset
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctr   -  MAC_CTRL  
*		mac_rst   -  MAC Reset. 1 -> Port MAC is reset, 0 -> Port MAC is not reset
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
    IN  int			 port,
    IN  MAC_CTRL	*mac_ctrl,
    IN  int			 mac_rst
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
/*  MAD_U32 rst_val; */

  if (mac_ctrl->mac_type == 0)
      addr = SYS_MAC_CONTROL2(dev);
  else
      addr = WIR_MAC_CONTROL2(dev);
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  if (bit_get(data, 1) != mac_rst)
  {
    bit_write (data, mac_rst, 1);
    retVal = msecWritePortReg (dev, port, addr, data);  
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return MAD_OK;
}

/*******************************************************************************
* msec_port_mac_en_dis
*
* DESCRIPTION:
*			brief Per Port Enable/Disable Mac
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctr   -  MAC_CTRL  
*		mac_en    -  MAC Enable/Disable. 1 -> Port MAC is enable, 0 -> Port MAC is disable
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
    IN  int			 port,
    IN  MAC_CTRL	*mac_ctrl,
    IN  int			 mac_en
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  if (mac_ctrl->mac_type == 0)
      addr = SYS_MAC_CONTROL0(dev);
  else
      addr = WIR_MAC_CONTROL0(dev);
  retVal   = msecReadPortReg (dev, port, addr, &data);
  if (bit_get(data, 0) != mac_en)
  {
    bit_write (data, mac_en, 0);
    retVal = msecWritePortReg (dev, port, addr, data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_mac_speed
*
* DESCRIPTION:
*			Per Port Speed and Duplex setting for MAC
*			Disable Mac. Set speed and Duplex. Enable Mac.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       mac_ctr   -  MAC_CTRL  
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
  MAD_U32 speed_ctrl;
  MAD_U16 addr;

  /* Bring Mac link down */
  retVal = msec_port_set_mac_link (dev, port, mac_ctrl, 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_mac_en_dis \n");
     return retVal;
  }

  /* Program speed and duplex */
  speed_ctrl = 0x0;
  bit_write (speed_ctrl,  mac_ctrl->hfd,   4);
  msec_bits_write(&speed_ctrl, mac_ctrl->speed, 1, 0);
  if (mac_ctrl->mac_type == 0)
      addr  = SYS_AUTO_NEG_CONTROL(dev);
  else
      addr  = WIR_AUTO_NEG_CONTROL(dev);
  retVal = msecWritePortReg (dev, port, addr, speed_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }


  /* Bring Mac link up */
  retVal = msec_port_set_mac_link (dev, port, mac_ctrl, 0x1);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_link \n");
     return retVal;
  }


  return retVal;
}

MAD_STATUS msec_port_set_flow_ctrl
(
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAC_CTRL	*mac_ctrl
 )
{
  MAD_STATUS   retVal;
  MAD_U32 flow_ctrl;
  MAD_U16 addr;

  /* Program speed and duplex */
  flow_ctrl = 0x0;
  bit_write (flow_ctrl,  mac_ctrl->tx_fc_en,   3);
  bit_write (flow_ctrl,  mac_ctrl->rx_fc_en,   2);
  
  if (mac_ctrl->mac_type == 0)
      addr  = SYS_AUTO_NEG_CONTROL(dev);
  else
      addr  = WIR_AUTO_NEG_CONTROL(dev);
  retVal = msecWritePortReg (dev, port, addr, flow_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }


  /* Bring Mac link up */
  retVal = msec_port_set_mac_link (dev, port, mac_ctrl, 0x1);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_link \n");
     return retVal;
  }

  return retVal;
}
MAD_STATUS msec_port_set_autoneg
(
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAC_CTRL	*mac_ctrl
 )
{
  MAD_STATUS   retVal;
  MAD_U32 aneg_ctrl;
  MAD_U16 addr;

  /* Program speed and duplex */
  aneg_ctrl = 0x0;
  bit_write (aneg_ctrl,  mac_ctrl->aneg_fc_en,   6);
  bit_write (aneg_ctrl,  mac_ctrl->aneg_en,   5);
  
  if (mac_ctrl->mac_type == 0)
      addr  = SYS_AUTO_NEG_CONTROL(dev);
  else
      addr  = WIR_AUTO_NEG_CONTROL(dev);
  retVal = msecWritePortReg (dev, port, addr, aneg_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }


  /* Bring Mac link up */
  retVal = msec_port_set_mac_link (dev, port, mac_ctrl, 0x1);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_mac_link \n");
     return retVal;
  }

  return retVal;
}
/*******************************************************************************
* msec_port_get_mac_var
*
* DESCRIPTION:
*			brief Per Port Read MAC  Control Variables
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
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
MAD_STATUS msec_port_get_mac_var 
(
    IN  MAD_DEV		*dev,
	IN	int		    port,
    OUT MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
  retVal = msec_port_get_mac_ctrl   (dev, port, mac_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_mac_ctrl \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_MAC_CTRL;
  }
  retVal = msec_port_get_mac_speed  (dev, port, mac_ctrl);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_mac_speed \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_MAC_SPEED;
  }
#ifdef DEBUG
  MAD_CRITIC_INFO ("MAC_CTRL Structure       : Port %0d\n", port);
  msec_print_mac_ctrl(mac_ctrl);
#endif
 
  return   retVal;
}

/*******************************************************************************
* msec_port_get_mac_speed
*
* DESCRIPTION:
*			Per Port Reads Speed and Duplex setting for MAC
*			Reads system or wires side Port AUTO_NEG_CONTROL register based on mac_ctrl->mac_type.
*			Returns Speed and Duplex setting for MAC
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    OUT  MAC_CTRL	*mac_ctrl
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  if (mac_ctrl->mac_type == 0)
      addr = SYS_AUTO_NEG_CONTROL(dev);
  else
      addr = WIR_AUTO_NEG_CONTROL(dev);
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

  mac_ctrl->hfd   = bit_get  (data, 4);
  mac_ctrl->speed = msec_bits_get (data, 1, 0);

  return retVal;
}


/*******************************************************************************
* msec_port_macsec_init
*
* DESCRIPTION:
*		 MACSec Initialization for the given Port
*		 Bring MACSec core out of soft reset.
*		 Disable all the entries of ELU table.
*		 Disable all the entries of ILU table.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port
)
{
  MAD_STATUS   retVal;
  int ent_num;

  /* Soft Reset MacSec Block */
  retVal = msecWritePortReg(dev, port, RESET_CTL(dev), 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
  retVal = msecWriteReg(dev, IMB_CTL(dev), 0x8000);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  /* Disable ELU and ILU entries */
  for (ent_num = 0; ent_num < 8; ent_num++) 
  {
      msec_port_dis_elu_ent   (dev, port, ent_num);
      msec_port_dis_ilu_ent   (dev, port, ent_num);
  }

  return retVal;
}

/*******************************************************************************
* msec_port_dis_elu_ent
*
* DESCRIPTION:
*			Disable ELU entry of given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ELU(dev) + (ent_num * 8) + 4; 
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write(&data, 0x0, 31, 24);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_dis_ilu_ent
*
* DESCRIPTION:
*			Disable ILU entry of given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ILU(dev) + (ent_num * 8) + 7;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write(&data, 0x0, 16, 8);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_stat_init
*
* DESCRIPTION:
*		 MIBs and MacSEC Statistics Initialization
*		 Program MIBSs and MacSEC stats to clear on read.
*		 Enable TX and RX histograms
*		 Clears Macsec and MIB statistics
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  /* set TX/RX histogram, Program MIBs and MacSEC Stats clear on read */
  addr = STAT_GLOBAL(dev);
  retVal = msecReadReg (dev, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to read register \n");
     return 0;
  }
  msec_bits_write (&data, 0x3, 6, 4);
  bit_write  (data,  0x1, 0);
  retVal = msecWriteReg (dev, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to write register \n");
     return 0;
  }

  /* Clear Macsec Statistics */
  retVal = msec_clr_macsec_stat (dev);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_clr_macsec_stat \n");
     return retVal;
  }

  /* Clear Wire Mac MIB statistics */
  retVal = msec_clr_mib_stat (dev);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_clr_mib_stat \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_egr_entry
*
* DESCRIPTION:
*		 Program Egress Entry Security Association (SA) for a port
*		 Disable ELU entry.
*		 Program ECT and EKEY table entry.
*		 Calculate Hash key and program EHKEY table entry.
*		 Program ELU entry.s
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*		ent_num  - Entry number
*		egr_lkup - LKUP_T (Egress Lookup table fields)
*		egr_ctx  - CTX_T  (Egress Context, key, hkey table fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	int			ent_num, 
	IN	LKUP_T		*egr_lkup, 
	IN	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  LKUP_T    dbg_lkup; 
  ACT_FLD   dbg_act_fld;
  MATCH_EN  dbg_match_en;
  MATCH_FLD dbg_match_fld;
  CTX_T     dbg_ctx;

  MAD_CRITIC_INFO ("Programming EGR Entry    : Port %0d Entry %0d\n", port,  ent_num);
  egr_lkup->lk_match_en->en_scian = 0;
  egr_lkup->lk_match_fld->sci     = 0LL;
  egr_lkup->lk_match_fld->tci     = 0;
  egr_lkup->lk_match_fld->tci_msk = 0;
  msec_print_lkup (egr_lkup);
  msec_print_ect  (egr_ctx);
  msec_print_key  (egr_ctx);
  msec_print_hkey (egr_ctx);
#endif

  /* disable ELU entry */
  retVal = msec_port_dis_elu_ent (dev, port, ent_num);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_dis_elu_ent \n");
     return retVal|MAD_MSEC_FAIL_PORT_DIS_ELU_ENTRY;
  }

  /* program an entry for ECT table */
/*  retVal = msec_port_set_ect_entry (dev, port, egr_lkup->lk_act_fld->ctx_num, egr_ctx); */
  retVal = msec_port_set_ect_entry (dev, port, ent_num, egr_ctx); 
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ect_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_ECT_ENTRY;
  }

  /* program an entry for EKey table */
  retVal = msec_port_set_ekey_entry (dev, port, ent_num, egr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ekey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_EKEY_ENTRY;
  }

  /* calculate hkey */
  aes_hkey (egr_ctx);

  /* program an entry for EHkey table */
  retVal = msec_port_set_ehkey_entry (dev, port, ent_num, egr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ehkey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_EHKEY_ENTRY;
  }

  /* program an entry for ELU table */
  retVal = msec_port_set_elu_entry (dev, port, ent_num, egr_lkup);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_elu_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_ELU_ENTRY;
  }

#ifdef DEBUG
  /* Reading back just programmed value */
  dbg_lkup.lk_match_en  = &dbg_match_en;
  dbg_lkup.lk_match_fld = &dbg_match_fld;
  dbg_lkup.lk_act_fld   = &dbg_act_fld;
  retVal = msec_port_get_egr_entry (dev, port, ent_num, &dbg_lkup, &dbg_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_egr_entry \n");
     return retVal|MAD_MSEC_FAIL_GET_EG_ENTRY;
  }
#endif

  return MAD_OK;
}

/*******************************************************************************
* msec_port_set_elu_entry
*
* DESCRIPTION:
*			Program an ELU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		egr_lkup - LKUP_T (Egress Lookup table fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	LKUP_T		*egr_lkup
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ELU(dev) + (ent_num * 8);
  data = (MAD_U32) (egr_lkup->lk_match_fld->sa & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) (((egr_lkup->lk_match_fld->da & 0xFFFFLL) << 16) | ((egr_lkup->lk_match_fld->sa >> 32) & 0xFFFFLL));
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) (((egr_lkup->lk_match_fld->da >> 16) & 0xFFFFFFFFLL));
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  /* we must set data to 0 to avoid the hidden register being set by accident  */
  data  = 0x0;
#if 0
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
#endif
  if (dev->deviceId == MAD_88E1540)
	bit_write (data, egr_lkup->lk_act_fld->drop_maxpn, 30);
  bit_write  (data,  egr_lkup->lk_act_fld->redir,  28);
  msec_bits_write (&data, egr_lkup->lk_match_fld->vlan, 27, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  /* we must set data to 0 to avoid the hidden register being set by accident  */
  data  = 0x0;
#if 0
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
#endif
  bit_write  (data,  egr_lkup->lk_match_en->en_def,  30);
  bit_write  (data,  egr_lkup->lk_match_en->en_mc,   29);
  bit_write  (data,  egr_lkup->lk_match_en->en_da,   28);
  bit_write  (data,  egr_lkup->lk_match_en->en_sa,   27);
  bit_write  (data,  egr_lkup->lk_match_en->en_vlan, 26);
  bit_write  (data,  egr_lkup->lk_match_en->en_et,   24);
  if (dev->deviceId != MAD_88E1540)
	msec_bits_write (&data, ent_num,  22, 20);
  bit_write  (data,  egr_lkup->lk_act_fld->enc_en,   18);
  bit_write  (data,  egr_lkup->lk_act_fld->auth_en,  17);
  bit_write  (data,  egr_lkup->lk_act_fld->drop,     16);
  msec_bits_write (&data, egr_lkup->lk_match_fld->et,     15, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_elu_match_fld
*
* DESCRIPTION:
*			Program an ELU Table Match fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		match_fld  - MATCH_FLD (Egress Lookup table match fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	MATCH_FLD	*match_fld
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr  = VN_CFG_ELU(dev) + (ent_num * 8);
  data  = (MAD_U32) (match_fld->sa & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) (((match_fld->da & 0xFFFFLL) << 16) | ((match_fld->sa >> 32) & 0xFFFFLL));
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) (((match_fld->da >> 16) & 0xFFFFFFFFLL));
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
 
  msec_bits_write (&data, match_fld->vlan, 27, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write (&data, match_fld->et,     15, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
*  msec_port_set_elu_match_en
*
* DESCRIPTION:
*			Program an ELU Table Match Enable fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number 
*		match_en  - MATCH_EN (Egress Lookup table match enable fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	MATCH_EN	*match_en
)
{  
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ELU(dev) + (ent_num * 8) + 4;  
  data = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write (data,  match_en->en_def,  30);
  bit_write (data,  match_en->en_mc,   29);
  bit_write (data,  match_en->en_da,   28);
  bit_write (data,  match_en->en_sa,   27);
  bit_write (data,  match_en->en_vlan, 26);
  bit_write (data,  match_en->en_et,   24);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_elu_act_fld
*
* DESCRIPTION:
*			Program an ELU Table Action fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		act_fld  - ACT_FLD (Egress Lookup table Action fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	ACT_FLD		*act_fld
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ELU(dev) + (ent_num * 8) + 3;
  
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write (data, act_fld->drop_maxpn, 30);
  bit_write  (data,  0,  29);
  bit_write  (data,  act_fld->redir,  28);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }  
  bit_write  (data,  act_fld->enc_en,   18);
  bit_write  (data,  act_fld->auth_en,  17);
  bit_write  (data,  act_fld->drop,     16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ect_entry
*
* DESCRIPTION:
*			Program an ECT entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		egr_ctx  CTX_T (Egress Context table fields except key, hkey)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ELU_CTX(dev) + (ent_num * 4);
  data = (MAD_U32) (egr_ctx->sci & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) (((egr_ctx->sci >> 32) & 0xFFFFFFFFLL));
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  retVal = msecWritePortReg (dev, port, addr, egr_ctx->pn);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  if (dev->deviceId == MAD_88E1340)
	data = egr_ctx->tci;
  else if (dev->deviceId == MAD_88E1540)
	data = (MAD_U32)(((egr_ctx->offset_conf << 8) + egr_ctx->tci) & 0xFFFF);

  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ekey_entry
*
* DESCRIPTION:
*			Program an Encrypt Key Table (EKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		egr_ctx  CTX_T (Egress Context Key)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  int i, j;

  addr = VN_CFG_EGR_KEY(dev) + (ent_num * 4);
  for (i = 0; i<4; i++) 
  {
    data = 0;
    for (j=0; j<4; j++)
        data = data << 8 | egr_ctx->key[j+i*4];
    retVal = msecWritePortReg (dev, port, addr + 3 -i, data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ehkey_entry
*
* DESCRIPTION:
*			Program an Egress Hash Key Table (EHKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		egr_ctx  - CTX_T (Egress Context HKey)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  int i, j;

  addr = VN_CFG_EGR_HKEY(dev) + (ent_num * 4);
  for (i = 0; i<4; i++) 
  {
    data = 0;
    for (j=0; j<4; j++)
        data = data << 8 | egr_ctx->hkey[j+i*4];
    retVal = msecWritePortReg (dev, port, addr + 3 - i, data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_egr_entry
*
* DESCRIPTION:
*		 Read Egress Entry Security Association (SA) for a port
*		 Read an entry for ELU, ECT, EKEY and EHKEY tables
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*		ent_num  - Entry number
*
* OUTPUTS:
*		egr_lkup - LKUP_T (Egress Lookup table fields)
*		egr_ctx  - CTX_T  (Egress Context, key, hkey table fields)
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	int			ent_num, 
	OUT	LKUP_T		*egr_lkup, 
	OUT	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading EGR Entry        : Port %0d Entry %0d\n", port,  ent_num);
#endif

  /* get ELU table entry fields */
  retVal = msec_port_get_elu_entry (dev, port, ent_num, egr_lkup);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_elu_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_ELU_ENTRY;
  }

  /* get ECT table entry fields */
  retVal = msec_port_get_ect_entry (dev, port, ent_num, egr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_ect_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_ECT_ENTRY;
  }

  /* get EKey table entry fields */
  retVal = msec_port_get_ekey_entry (dev, port, ent_num, egr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_ekey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_EKEY_ENTRY;
  }

  /* get EHkey table entry fields */
  retVal = msec_port_get_ehkey_entry (dev, port, ent_num, egr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_ehkey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_EHKEY_ENTRY;
  }

  return MAD_OK;
}

/*******************************************************************************
* msec_port_get_elu_entry
*
* DESCRIPTION:
*			Read an ELU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		egr_lkup LKUP_T (Egress Lookup table fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	LKUP_T		*egr_lkup
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, tmp_data;

  addr                            = VN_CFG_ELU(dev) + (ent_num * 8);
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_lkup->lk_match_fld->sa      = ((MAD_U64) data) & 0xFFFFFFFF;

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  tmp_data                        = data & 0xFFFF;
  egr_lkup->lk_match_fld->sa     |= ((MAD_U64) tmp_data) << 32LL;
  egr_lkup->lk_match_fld->da      = (MAD_U64) ((data >> 16) & 0xFFFF);
  
  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_lkup->lk_match_fld->da     |= ((MAD_U64) data) << 16LL;

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_lkup->lk_act_fld->redir     = bit_get  (data, 28);
  egr_lkup->lk_match_fld->vlan    = (MAD_U16) msec_bits_get (data, 27, 16);

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_lkup->lk_match_en->en_def   = bit_get  (data, 30);
  egr_lkup->lk_match_en->en_mc    = bit_get  (data, 29);
  egr_lkup->lk_match_en->en_da    = bit_get  (data, 28);
  egr_lkup->lk_match_en->en_sa    = bit_get  (data, 27);
  egr_lkup->lk_match_en->en_vlan  = bit_get  (data, 26);
  egr_lkup->lk_match_en->en_et    = bit_get  (data, 24);
  egr_lkup->lk_act_fld->ctx_num   = msec_bits_get (data, 22, 20);
  egr_lkup->lk_act_fld->enc_en    = bit_get  (data, 18);
  egr_lkup->lk_act_fld->auth_en   = bit_get  (data, 17);
  egr_lkup->lk_act_fld->drop      = bit_get  (data, 16);
  egr_lkup->lk_match_fld->et      = (MAD_U16) msec_bits_get  (data, 15, 0);

  /* zero out not valid fields for egress path */
  egr_lkup->lk_match_en->en_scian = 0; 
  egr_lkup->lk_match_fld->sci     = 0LL;
  egr_lkup->lk_match_fld->tci     = 0;
  egr_lkup->lk_match_fld->tci_msk = 0;

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading ELU Entry        : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_lkup (egr_lkup);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_elu_match_fld
*
* DESCRIPTION:
*			Read an ELU Entry Match fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		elu_match  - MATCH_FLD (Egress Lookup table Match fields)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	MATCH_FLD	*elu_match
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, tmp_data;

  addr            = VN_CFG_ELU(dev) + (ent_num * 8);
  retVal            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_match->sa   = (MAD_U64) data; 

  addr           += 1;
  retVal            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  tmp_data        = data & 0xFFFF;
  elu_match->sa  |= (MAD_U64) tmp_data << 32;
  elu_match->da   = (MAD_U64) ((data >> 16) & 0xFFFF);

  addr           += 1;
  retVal            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_match->da  |= (MAD_U64) (MAD_U64) data << 16;

  addr           += 1;
  retVal            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_match->vlan = (MAD_U16) msec_bits_get (data, 27, 16);

  addr           += 1;
  retVal            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_match->et   = (MAD_U16) msec_bits_get  (data, 15, 0);

  return retVal;
}

/*******************************************************************************
* msec_port_get_elu_match_en
*
* DESCRIPTION:
*			Read an ELU Entry Match Enables for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		elu_match_en -  MATCH_EN (Egress Lookup table Match Enables)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	MATCH_EN	*elu_match_en
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                  = VN_CFG_ELU(dev) + (ent_num * 8) + 4;
  retVal                  = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_match_en->en_def  = bit_get  (data, 30);
  elu_match_en->en_mc   = bit_get  (data, 29);
  elu_match_en->en_da   = bit_get  (data, 28);
  elu_match_en->en_sa   = bit_get  (data, 27);
  elu_match_en->en_vlan = bit_get  (data, 26);
  elu_match_en->en_et   = bit_get  (data, 24);

  return retVal;
}

/*******************************************************************************
* msec_port_get_elu_act
*
* DESCRIPTION:
*			Read an ELU Entry Action Fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		elu_act  - ACT_FLD (Egress Lookup table Action fields)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	ACT_FLD		*elu_act
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr             = VN_CFG_ELU(dev) + (ent_num * 8) + 3;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_act->redir   = bit_get  (data, 28);

  addr            += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  elu_act->ctx_num = msec_bits_get (data, 22, 20);
  elu_act->enc_en  = bit_get  (data, 18);
  elu_act->auth_en = bit_get  (data, 17);
  elu_act->drop    = bit_get  (data, 16);

  return retVal;
}

/*******************************************************************************
* msec_port_get_ect_entry
*
* DESCRIPTION:
*			Read an ECT entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		egr_ctx  - CTX_T (Egress Context table fields except key, hkey)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr          = VN_CFG_ELU_CTX(dev) + (ent_num * 4);
  retVal          = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_ctx->sci  = (MAD_U64) data; 

  addr         += 1;
  retVal          = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_ctx->sci |= (MAD_U64) data << 32;

  addr         += 1;
  retVal          = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_ctx->pn   = (MAD_U32) data;

  addr         += 1;
  retVal          = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_ctx->tci  = (MAD_U8) (data & 0xFF);
  if (dev->deviceId == MAD_88E1540)
    egr_ctx->offset_conf = (MAD_U8) (data >> 8 & 0xFF);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading ECT Entry        : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_ect (egr_ctx);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_ekey_entry
*
* DESCRIPTION:
*			Read an Encrypt Key Table (EKEY) entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		egr_ctx  - CTX_T (Egress Context Key)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	CTX_T		*egr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, i;

/*  addr = VN_CFG_EGR_KEY(dev) + (ent_num * 8); */
  addr = VN_CFG_EGR_KEY(dev) + (ent_num * 4);  /* Xu */
  for (i = 4; i > 0; i--)
  {
    retVal                    = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
       return retVal;
	}
    egr_ctx->key[((i*4)-1)] = (MAD_U8) msec_bits_get (data,  7, 0);
    egr_ctx->key[((i*4)-2)] = (MAD_U8) msec_bits_get (data, 15, 8);
    egr_ctx->key[((i*4)-3)] = (MAD_U8) msec_bits_get (data, 23, 16);
    egr_ctx->key[((i*4)-4)] = (MAD_U8) msec_bits_get (data, 31, 24);
    addr++;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading EGR Key          : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_key (egr_ctx);
#endif

  return retVal;
}

MAD_STATUS msec_port_get_ehkey_entry (MAD_DEV *dev,  int port, int ent_num, CTX_T *egr_ctx)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, i;

/*  addr = VN_CFG_EGR_HKEY(dev) + (ent_num * 8); */
  addr = VN_CFG_EGR_HKEY(dev) + (ent_num * 4);
  for (i = 4; i > 0; i--)
  {
    retVal                     = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
       return retVal;
	}
    egr_ctx->hkey[((i*4)-1)] = (MAD_U8) msec_bits_get (data,  7, 0);
    egr_ctx->hkey[((i*4)-2)] = (MAD_U8) msec_bits_get (data, 15, 8);
    egr_ctx->hkey[((i*4)-3)] = (MAD_U8) msec_bits_get (data, 23, 16);
    egr_ctx->hkey[((i*4)-4)] = (MAD_U8) msec_bits_get (data, 31, 24);
    addr++;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading EGR HKey         : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_hkey (egr_ctx);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_set_igr_entry
*
* DESCRIPTION:
*		 Program Ingress Entry Security Association (SA) for a port
*		 Disable ILU entry.
*		 Program IKEY table entry.
*		 Calculate Hash key and program IHKEY table entry.
*		 Initialize replay table (NxtPn) entry to 1.
*		 Program ILU entry.
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*		ent_num  - Entry number
*		igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*		igr_ctx  - CTX_T  (Ingress Context(key, hkey) Table Fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	int			ent_num, 
	IN	LKUP_T		*igr_lkup, 
	IN	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  LKUP_T    dbg_lkup; 
  ACT_FLD   dbg_act_fld;
  MATCH_EN  dbg_match_en;
  MATCH_FLD dbg_match_fld;
  CTX_T     dbg_ctx;

  MAD_CRITIC_INFO ("Programming IGR Entry    : Port %0d Entry %0d\n", port,  ent_num);
  igr_lkup->lk_act_fld->enc_en = 0;
  msec_print_lkup (igr_lkup);
  msec_print_key  (igr_ctx);
  msec_print_hkey (igr_ctx);
#endif

  /* disable ILU entry*/
  retVal = msec_port_dis_ilu_ent (dev, port, ent_num);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_dis_ilu_ent \n");
     return retVal|MAD_MSEC_FAIL_PORT_DIS_ILU_ENTRY;
  }

  /* program an entry for IKEY table */
  retVal = msec_port_set_ikey_entry (dev, port, ent_num, igr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ikey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_IKEY_ENTRY;
  }

  /* calculate hash key */
  aes_hkey (igr_ctx);

  /* program an entry for IHKEY table*/
  retVal = msec_port_set_ihkey_entry (dev, port, ent_num, igr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ihkey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_IHKEY_ENTRY;
  }

  /* program NxtPn (replay table) entry to 1.*/ 
  /* retVal = msec_port_set_nxtpn_ent (dev, port, ent_num, 0x1); */
  retVal = msec_port_set_nxtpn_ent (dev, port, ent_num, igr_ctx->pn);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_nxtpn_ent \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_NEXPN_ENTRY;
  }

  /* program an entry for ILU table */
  retVal = msec_port_set_ilu_entry (dev, port, ent_num, igr_lkup);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_set_ilu_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_ILU_ENTRY;
  }

#ifdef DEBUG
  /* Reading back just programmed value*/
  dbg_lkup.lk_match_en  = &dbg_match_en;
  dbg_lkup.lk_match_fld = &dbg_match_fld;
  dbg_lkup.lk_act_fld   = &dbg_act_fld;
  retVal = msec_port_get_igr_entry (dev, port, ent_num, &dbg_lkup, &dbg_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_igr_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_IGR_ENTRY;
  }
#endif

  return MAD_OK;
}

/*******************************************************************************
* msec_port_set_ilu_entry
*
* DESCRIPTION:
*			Program an ILU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	LKUP_T		*igr_lkup
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ILU(dev) + (ent_num * 8);
  data = (MAD_U32) (igr_lkup->lk_match_fld->sci & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) ((igr_lkup->lk_match_fld->sci >> 32) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
#if 0
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
#endif
  msec_bits_write (&data, igr_lkup->lk_match_fld->vlan, 11, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
#if 0
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
#endif
  data  = (MAD_U32) (((igr_lkup->lk_match_fld->sa & 0xFFFFLL) << 16));
  msec_bits_write (&data, igr_lkup->lk_match_fld->et, 15, 0);
/*  retVal = msecWritePortReg (dev, port, addr, data);   Xu added
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
*/

  addr += 1;
  data  = (MAD_U32) ((igr_lkup->lk_match_fld->sa >> 16) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data = (MAD_U32) (igr_lkup->lk_match_fld->da & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  data  = (MAD_U32) ((igr_lkup->lk_match_fld->da >> 32) & 0xFFFFLL);
  msec_bits_write (&data, igr_lkup->lk_match_fld->tci_msk, 31, 24);
  bit_write  (data,  igr_lkup->lk_act_fld->redir,     23);
  if (dev->deviceId == MAD_88E1340)
	msec_bits_write (&data, ent_num,   22, 20);
  else if (dev->deviceId == MAD_88E1540)
	msec_bits_write (&data, igr_lkup->lk_act_fld->sec_level,   22, 20);
  bit_write  (data,  igr_lkup->lk_act_fld->auth_en,   17);
  bit_write  (data,  igr_lkup->lk_act_fld->drop,      16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  if (dev->deviceId == MAD_88E1540)
	msec_bits_write (&data, igr_lkup->lk_match_fld->offset_conf, 23, 16);
  bit_write  (data,  igr_lkup->lk_match_en->en_scian, 15);
  bit_write  (data,  igr_lkup->lk_match_en->en_def,   14);
  bit_write  (data,  igr_lkup->lk_match_en->en_mc,    13);
  bit_write  (data,  igr_lkup->lk_match_en->en_da,    12);
  bit_write  (data,  igr_lkup->lk_match_en->en_sa,    11);
  bit_write  (data,  igr_lkup->lk_match_en->en_vlan,  10);
  bit_write  (data,  igr_lkup->lk_match_en->en_et,    8);
  msec_bits_write (&data, igr_lkup->lk_match_fld->tci,     7, 0);
  
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return MAD_OK;
}

/*******************************************************************************
* msec_port_set_ilu_match_fld
*
* DESCRIPTION:
*			Program an ILU Table Match fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		match_fld - MATCH_FLD (Ingress Lookup Table Match Fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	MATCH_FLD	*match_fld
)
{  
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ILU(dev) + (ent_num * 8);
  data = (MAD_U32) (match_fld->sci & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = (MAD_U32) ((match_fld->sci >> 32) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  msec_bits_write (&data, match_fld->vlan, 11, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  data  = (MAD_U32) (((match_fld->sa & 0xFFFFLL) << 16));
  msec_bits_write (&data, match_fld->et, 15, 0);
/*
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
*/

  addr += 1;
  data  = (MAD_U32) ((match_fld->sa >> 16) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data = (MAD_U32) (match_fld->da & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  data  = 0x0;
  data  = (MAD_U32) ((match_fld->da >> 32) & 0xFFFFLL);
  msec_bits_write (&data, match_fld->tci_msk, 31, 24);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr += 1;
  retVal = msecReadPortReg(dev,port,addr,&data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write (&data, match_fld->tci,     7, 0);
  if (dev->deviceId == MAD_88E1540)
	msec_bits_write (&data, match_fld->offset_conf, 23, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ilu_act_fld
*
* DESCRIPTION:
*			Program an ILU Table Action fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		act_fld   - ACT_FLD (Ingress Lookup Table Action Fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	ACT_FLD		*act_fld
)
{  
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ILU(dev) + (ent_num * 8) + 6;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write  (data,  act_fld->redir,     23);
  if (dev->deviceId == MAD_88E1340)
	msec_bits_write (&data, ent_num,   22, 20);
  else if (dev->deviceId == MAD_88E1540)
	msec_bits_write (&data, act_fld->sec_level,   22, 20);
  bit_write  (data,  act_fld->auth_en,   17);
  bit_write  (data,  act_fld->drop,      16);
  retVal = msecWritePortReg (dev, port, addr, data);  
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ilu_match_en
*
* DESCRIPTION:
*			Program an ILU Table Match enable fields entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		match_en  - MATCH_EN  (Ingress Lookup Table Match enable Fields)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	MATCH_EN	*match_en
)
{  
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = VN_CFG_ILU(dev) + (ent_num * 8) + 7;
  data  = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write  (data,  match_en->en_scian, 15);
  bit_write  (data,  match_en->en_def,   14);
  bit_write  (data,  match_en->en_mc,    13);
  bit_write  (data,  match_en->en_da,    12);
  bit_write  (data,  match_en->en_sa,    11);
  bit_write  (data,  match_en->en_vlan,  10);
  bit_write  (data,  match_en->en_et,    8);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ikey_entry
*
* DESCRIPTION:
*			Program Ingress Decrypt key (IKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		igr_ctx  - CTX_T  (Ingress Context Key)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  int i, j;

  addr = VN_CFG_IGR_KEY(dev) + (ent_num * 4);
  for (i = 0; i < 4; i++) 
  {
    data = 0;
    for (j = 0; j < 4; j++)
        data = data << 8 | igr_ctx->key[j+i*4];
    retVal = msecWritePortReg (dev, port, addr + 3 - i, data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_ihkey_entry
*
* DESCRIPTION:
*			Program Ingress Hash key (IHKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*		igr_ctx  - CTX_T  (Ingress Context Hkey)
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	IN	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  int i, j;

  addr = VN_CFG_IGR_HKEY(dev) + (ent_num * 4);
  for (i = 0; i < 4; i++) 
  {
    data = 0;
    for (j = 0; j < 4; j++)
        data = data << 8 | igr_ctx->hkey[j+i*4];
    retVal = msecWritePortReg (dev, port, addr + 3 - i, data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
       return retVal;
	}
  }
  return retVal;
}


/*******************************************************************************
* msec_port_get_igr_entry
*
* DESCRIPTION:
*		 Read Ingress Entry Security Association (SA) for a port
*		 Read an entry for ILU, IKEY and IHKEY tables
*
* INPUTS:
*       dev      - pointer to MAD driver structure returned from mdLoadDriver
*		ent_num  - Entry number
*
* OUTPUTS:
*		igr_lkup - LKUP_T (Ingress Lookup Table Fields)
*		igr_ctx  - CTX_T  (Ingress Context(key, hkey) Table Fields)
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	int			ent_num, 
	OUT	LKUP_T		*igr_lkup, 
	OUT	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR Entry        : Port %0d Entry %0d\n", port,  ent_num);
#endif

  /* get ILU table entry fields */
  retVal = msec_port_get_ilu_entry (dev, port, ent_num, igr_lkup);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_ilu_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_ILU_ENTRY;
  }

  /* get IKey table entry fields */
  retVal = msec_port_get_ikey_entry (dev, port, ent_num, igr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec port_get_ikey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_IKEY_ENTRY;
  }

  /* get IHkey table entry fields */
  retVal = msec_port_get_ihkey_entry (dev, port, ent_num, igr_ctx);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_ihkey_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_GET_IHKEY_ENTRY;
  }

  /* get NxtPn (replay table) entry value */
  retVal = msec_port_get_nxtpn_entry (dev, port, ent_num, &igr_ctx->pn);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_nxtpn_entry \n");
     return retVal|MAD_MSEC_FAIL_PORT_SET_NEXPN_ENTRY;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_ilu_entry
*
* DESCRIPTION:
*			Read an ILU entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		igr_lkup - LKUP_T (Ingress Lookup Table Fields)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	LKUP_T		*igr_lkup
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                            = VN_CFG_ILU(dev) + (ent_num * 8);
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->sci     = (MAD_U64) data;

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->sci    |= (MAD_U64) data << 32;

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->vlan    = (MAD_U16) (data & 0xFFF);

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->et      = (MAD_U16) (data & 0xFFFF);
  igr_lkup->lk_match_fld->sa      = (MAD_U64) ((data >> 16) & 0xFFFF);

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->sa     |= (MAD_U64) data << 16;

  addr                           += 1;
  retVal                            = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->da      = (MAD_U64) data;

  addr                           += 1;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_fld->da     |= (MAD_U64) (data & 0xFFFF) << 32;
  igr_lkup->lk_match_fld->tci_msk = (MAD_U8) msec_bits_get (data, 31, 24);
  igr_lkup->lk_act_fld->redir     = bit_get  (data, 23);
  if (dev->deviceId == MAD_88E1340)
	igr_lkup->lk_act_fld->ctx_num   = msec_bits_get (data, 22, 20);
  else if (dev->deviceId == MAD_88E1540)
    igr_lkup->lk_act_fld->sec_level = msec_bits_get(data, 22, 20);
  igr_lkup->lk_act_fld->auth_en   = bit_get  (data, 17);
  igr_lkup->lk_act_fld->drop      = bit_get  (data, 16);
  igr_lkup->lk_act_fld->enc_en    = 0;

  addr                           += 1;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to msecReadPortReg port %x, addr %x \n", port, addr);
     return retVal;
  }
  igr_lkup->lk_match_en->en_scian = bit_get  (data, 15);
  igr_lkup->lk_match_en->en_def   = bit_get  (data, 14);
  igr_lkup->lk_match_en->en_mc    = bit_get  (data, 13);
  igr_lkup->lk_match_en->en_da    = bit_get  (data, 12);
  igr_lkup->lk_match_en->en_sa    = bit_get  (data, 11);
  igr_lkup->lk_match_en->en_vlan  = bit_get  (data, 10);
  igr_lkup->lk_match_en->en_et    = bit_get  (data, 8);
  igr_lkup->lk_match_fld->tci     = (MAD_U8) msec_bits_get (data, 7, 0);
  if (dev->deviceId == MAD_88E1540)
	  igr_lkup->lk_match_fld->offset_conf = (MAD_U8) msec_bits_get(data, 23, 16);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading ILU Entry        : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_lkup (igr_lkup);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_ilu_match_fld
*
* DESCRIPTION:
*			Read an ILU entry Match Fields for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		ilu_match - MATCH_FLD (Ingress Lookup Table Match Fields)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	MATCH_FLD	*ilu_match
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr               = VN_CFG_ILU(dev) + (ent_num * 8);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->sci     = (MAD_U64) data;

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->sci    |= (MAD_U64) data << 32;

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->vlan    = (MAD_U16) (data & 0xFFF);

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->et      = (MAD_U16) (data & 0xFFFF);
  ilu_match->sa      = (MAD_U64) ((data >> 16) & 0xFFFF);

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->sa     |= (MAD_U64) data << 16;

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->da      = (MAD_U64) data;

  addr              += 1;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->da     |= (MAD_U64) (data & 0xFFFF) << 32;
  ilu_match->tci_msk = (MAD_U8) msec_bits_get (data, 31, 24);

  addr              += 1;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match->tci     = (MAD_U8) msec_bits_get (data, 7, 0);
  if (dev->deviceId == MAD_88E1540)
	  ilu_match->offset_conf = (MAD_U8) msec_bits_get(data, 23, 16);

  return retVal;
}

/*******************************************************************************
* msec_port_get_ilu_match_en
*
* DESCRIPTION:
*			Read an ILU entry Match Enables for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		ilu_match_en - MATCH_EN (Ingress Lookup Table Match Enables)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	MATCH_EN	*ilu_match_en
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                   = VN_CFG_ILU(dev) + (ent_num * 8) + 7;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_match_en->en_scian = bit_get  (data, 15);
  ilu_match_en->en_def   = bit_get  (data, 14);
  ilu_match_en->en_mc    = bit_get  (data, 13);
  ilu_match_en->en_da    = bit_get  (data, 12);
  ilu_match_en->en_sa    = bit_get  (data, 11);
  ilu_match_en->en_vlan  = bit_get  (data, 10);
  ilu_match_en->en_et    = bit_get  (data, 8);

  return retVal;
}

/*******************************************************************************
* msec_port_get_ilu_act
*
* DESCRIPTION:
*			Read an ILU entry Actions for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		ilu_act  - ACT_FLD (Ingress Lookup Table Action Fields)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	ACT_FLD		*ilu_act
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr             = VN_CFG_ILU(dev) + (ent_num * 8) + 6;
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  ilu_act->redir   = bit_get  (data, 23);
  if (dev->deviceId == MAD_88E1340)
	ilu_act->ctx_num = msec_bits_get (data, 22, 20);
  else if (dev->deviceId == MAD_88E1540)
	ilu_act->sec_level = msec_bits_get (data, 22, 20);
  ilu_act->auth_en = bit_get  (data, 17);
  ilu_act->drop    = bit_get  (data, 16);

  return retVal;
}

/*******************************************************************************
* msec_port_get_ikey_entry
*
* DESCRIPTION:
*			Read Ingress Decrypt key (IKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		igr_ctx  - CTX_T (Ingress Context Key)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr = 0;
  MAD_U32 data, i;

  addr = VN_CFG_IGR_KEY(dev) + (ent_num * 4);  /* Xu */
  for (i = 4; i > 0; i--)
  {
/*    addr                   += (4 - i);  */
    retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
    igr_ctx->key[((i*4)-1)] = (MAD_U8) msec_bits_get (data,  7, 0);
    igr_ctx->key[((i*4)-2)] = (MAD_U8) msec_bits_get (data, 15, 8);
    igr_ctx->key[((i*4)-3)] = (MAD_U8) msec_bits_get (data, 23, 16);
    igr_ctx->key[((i*4)-4)] = (MAD_U8) msec_bits_get (data, 31, 24);
	addr++;
  }


#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR Key          : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_key (igr_ctx);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_ihkey_entry
*
* DESCRIPTION:
*			Read Ingress Hash key (IHKEY) table entry for a port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		igr_ctx  - CTX_T (Ingress Context HKey)
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT	CTX_T		*igr_ctx
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr = 0;
  MAD_U32 data, i;

/*  addr = VN_CFG_IGR_HKEY(dev) + (ent_num * 8); */
  addr = VN_CFG_IGR_HKEY(dev) + (ent_num * 4);
  for (i = 4; i > 0; i--)
  {
/*      addr                   += (4 - i); */
      retVal             = msecReadPortReg (dev, port, addr, &data);
      if(retVal != MAD_OK)
	  {
        MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
        return retVal;
	  }
      igr_ctx->hkey[((i*4)-1)] = (MAD_U8) msec_bits_get (data,  7, 0);
      igr_ctx->hkey[((i*4)-2)] = (MAD_U8) msec_bits_get (data, 15, 8);
      igr_ctx->hkey[((i*4)-3)] = (MAD_U8) msec_bits_get (data, 23, 16);
      igr_ctx->hkey[((i*4)-4)] = (MAD_U8) msec_bits_get (data, 31, 24);
	  addr++;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR HKey         : Port %0d Entry %0d\n", port,  ent_num);
  msec_print_key (igr_ctx);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_nxtpn_entry
*
* DESCRIPTION:
*			Read NxtPn Value to an entry or a context number in replay table for a given port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       ent_num  - Entry number to be disable  
*
* OUTPUTS:
*		data =  NxtPn value 
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  int			ent_num,
	OUT MAD_U32     *data
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  

  addr = VN_CFG_RPY(dev) + ent_num;
  retVal             = msecReadPortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading Replay Table     : Port %0d Entry %0d\n", port,  ent_num);
  MAD_CRITIC_INFO ("NxtPn Value              : %08x\n", *(unsigned int *)data);
#endif

  return retVal;
}

/*******************************************************************************
* msec_set_redir_hdr
*
* DESCRIPTION:
*		 Redirect header setting
*		 Set Redirect MAC DA, MAC SA and Ethertype
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		redir_fld - REDIR_HDR_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	REDIR_HDR_T *redir_fld
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  int      i;

#ifdef DEBUG
  MAD_CRITIC_INFO ("Programming Redir Hdr    : \n");
  msec_print_redir_hdr (redir_fld);
#endif

  addr = ENCAP_DA0(dev);
  data = (MAD_U32) (redir_fld->da & 0xFFFFFFFFLL);
  retVal = msecWriteReg (dev, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to write register \n");
     return 0;
  }

  addr = ENCAP_DA_ET(dev);
  data = (MAD_U32) ((redir_fld->da >> 32) & 0xFFFFLL);
  msec_bits_write (&data, redir_fld->et, 31, 16);
  retVal = msecWriteReg (dev, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to write register \n");
     return 0;
  }

  for (i = 0; i < 4; i++)
  {
      dev->msec_ctrl.port_num = i;
      retVal = msec_port_set_redir_sa (dev, i, redir_fld->sa[i]);
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_redir_sa
*
* DESCRIPTION:
*			edirect SA per port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		redir_sa - Redirect SA per port
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
    IN  MAD_U64	sa
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr = PORT_MAC_SA0(dev);
  data = (MAD_U32) (sa & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = PORT_MAC_SA1(dev);
  data = (MAD_U32) ((sa >> 32) & 0xFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_redir_sa
*
* DESCRIPTION:
*			Read Redirect SA per port
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		Redirect SA per port
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	OUT MAD_U64		*sa
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, tmp_data;
  

  addr     = PORT_MAC_SA0(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  *sa       = (MAD_U64) data;

  addr     = PORT_MAC_SA1(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  tmp_data = data & 0xFFFF;
  *sa      |= (MAD_U64) tmp_data << 32;

  return retVal;
}

/*******************************************************************************
* msec_get_redir_hdr
*
* DESCRIPTION:
*		 Read Redirect header setting
*		 Read Redirect MAC DA, MAC SA and Ethertype
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*		redir_fld - REDIR_HDR_T
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
    IN  MAD_DEV		*dev,
	OUT	REDIR_HDR_T *redir_fld
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, tmp_data, i;

  addr             = ENCAP_DA0(dev);
  retVal             = msecReadReg (dev, addr, &data);
  redir_fld->da    = (MAD_U64) data;

  addr             = ENCAP_DA_ET(dev);
  retVal             = msecReadReg (dev, addr, &data);
  tmp_data         = data & 0xFFFF;
  redir_fld->da   |= (MAD_U64) tmp_data << 32;
  redir_fld->et    = (MAD_U16) msec_bits_get (data, 31, 16);

  for (i = 0; i < 4; i++)
  {
      dev->msec_ctrl.port_num = i;
      retVal  = msec_port_get_redir_sa (dev, i, &redir_fld->sa[i]);
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading Redir Hdr        : \n");
  msec_print_redir_hdr (redir_fld);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_cmn_cfg
*
* DESCRIPTION:
*		 Per Port common MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*		cmn_cfg   - CMN_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	CMN_CFG_T	*cmn_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  
#ifdef DEBUG
  MAD_CRITIC_INFO ("Programming CMN_CFG_T    : Port %0d\n", port);
  msec_print_cmn_cfg (cmn_cfg);
#endif

  addr = CFG_SIZES(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write (&data, cmn_cfg->conf_offset, 31, 24);
  msec_bits_write (&data, cmn_cfg->trail_sz,    15, 8);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_DEFAULT_VLAN(dev);
  data = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  msec_bits_write (&data, cmn_cfg->def_vlan, 11, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_MC_HIGH0(dev);
  data = (MAD_U32) ((cmn_cfg->mc_high >> 4) & 0xFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_MC_HIGH1(dev);
  data = (MAD_U32) ((cmn_cfg->mc_high >> 32) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_MC_LOW_MSK(dev);
  data = (MAD_U32) (cmn_cfg->mc_low_msk  & 0xFFFF);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_cmn_cfg
*
* DESCRIPTION:
*		 Per Port common MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*
* OUTPUTS:
*		cmn_cfg   - CMN_CFG_T
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	OUT	CMN_CFG_T	*cmn_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                 = CFG_SIZES(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  cmn_cfg->conf_offset = (MAD_U8) ((data >> 24) & 0xFF);

  addr                 = CFG_DEFAULT_VLAN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  cmn_cfg->def_vlan    = (MAD_U16) (data & 0xFFF);

  addr                 = CFG_MC_HIGH0(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  cmn_cfg->mc_high     = (MAD_U64) ((data << 4) | 0x0);

  addr                 = CFG_MC_HIGH1(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  cmn_cfg->mc_high    |= (MAD_U64) data << 32;

  addr                 = CFG_MC_LOW_MSK(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  cmn_cfg->mc_low_msk  = (MAD_U16)(data & 0xFFFF);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading CMN_CFG_T        : Port %0d\n", port);
  msec_print_cmn_cfg (cmn_cfg);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_egr_cfg
*
* DESCRIPTION:
*		 Per Port Egress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*		egr_cfg   - EGR_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	EGR_CFG_T	*egr_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

#ifdef DEBUG
  MAD_CRITIC_INFO ("Programming EGR_CFG_T    : Port %0d\n", port);
  msec_print_egr_cfg (egr_cfg);
#endif

  addr = CFG_EGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write (data, egr_cfg->def_drop,   0);
  bit_write (data, egr_cfg->parse_vlan, 3);
  bit_write (data, egr_cfg->parse_ae,   16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_egr_cfg
*
* DESCRIPTION:
*		 Get Per Port Egress MACSec configuration value
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*
* OUTPUTS:
*		egr_cfg   - EGR_CFG_T
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	OUT	EGR_CFG_T	*egr_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  
  addr                = CFG_EGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  egr_cfg->def_drop   = (data & 0x1);
  egr_cfg->parse_vlan = ((data >>  3) & 0x1);
  egr_cfg->parse_ae   = ((data >> 16) & 0x1); 

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading EGR_CFG_T        : Port %0d\n", port);
  msec_print_egr_cfg (egr_cfg);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_igr_cfg
*
* DESCRIPTION:
*		 Per Port Ingress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*		igr_cfg   - IGR_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	IGR_CFG_T	*igr_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  
#ifdef DEBUG
  MAD_CRITIC_INFO ("Programming IGR_CFG_T    : Port %0d\n", port);
  msec_print_igr_cfg (igr_cfg);
#endif

  addr = CFG_IGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write (data, igr_cfg->def_act->def_drop,    0);
  bit_write (data, igr_cfg->def_act->retain_hdr,  1);
  bit_write (data, igr_cfg->def_act->parse_vlan,  18);
  bit_write (data, igr_cfg->def_act->drop_badtag, 29);
  bit_write (data, igr_cfg->def_act->def_redir,   28);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_ISC_GEN(dev);
  data = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write  (data,  igr_cfg->redir_vec->redir_untagged,  13);
  bit_write  (data,  igr_cfg->redir_vec->redir_badtag,    12);
  bit_write  (data,  igr_cfg->redir_vec->redir_unk_sci,   11);
  bit_write  (data,  igr_cfg->redir_vec->redir_unused_sa, 10);
  bit_write  (data,  igr_cfg->redir_vec->redir_unchecked, 9);
  bit_write  (data,  igr_cfg->redir_vec->redir_delayed,   8);
  bit_write  (data,  igr_cfg->redir_vec->redir_invalid,   7);
  bit_write  (data,  igr_cfg->rpy_var->replay_drop,       6);
  bit_write  (data,  igr_cfg->rpy_var->replay_protect,    2);
  msec_bits_write (&data, igr_cfg->rpy_var->validate_frames,   1, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }


  addr = CFG_REPLAY_WINDOW(dev);
  retVal = msecWritePortReg (dev, port, addr, igr_cfg->rpy_var->replay_window);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }


  addr = CFG_IMPLICIT_SCI0(dev);
  data = (MAD_U32) (igr_cfg->sci_info->implicit_sci & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_IMPLICIT_SCI1(dev);
  data = (MAD_U32) ((igr_cfg->sci_info->implicit_sci >> 32) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = PORT_NUMBERS(dev);
  msec_bits_write (&data, igr_cfg->sci_info->scb_port, 31, 16);
  msec_bits_write (&data, igr_cfg->sci_info->def_port, 15, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_igr_def_act
*
* DESCRIPTION:
*			Per Port Ingress MACSec default action configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		def_act - IGR_DEF_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	IN	IGR_DEF_T	*def_act
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  addr = CFG_IGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write (data, def_act->def_drop,    0);
  bit_write (data, def_act->retain_hdr,  1);
  bit_write (data, def_act->parse_vlan,  18);
  msec_bits_write (&data, def_act->badtag_chk,  27, 23);
  bit_write (data, def_act->def_redir,   28);
  bit_write (data, def_act->drop_badtag, 29);
  bit_write (data, def_act->def_redir,   28);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_igr_redir_vec
*
* DESCRIPTION:
*			Per Port Ingress MACSec Redirect action configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		redir_vec - REDIR_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	IN	REDIR_T		*redir_vec
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  addr = CFG_ISC_GEN(dev);
  data = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  bit_write  (data,  redir_vec->redir_untagged,  13);
  bit_write  (data,  redir_vec->redir_badtag,    12);
  bit_write  (data,  redir_vec->redir_unk_sci,   11);
  bit_write  (data,  redir_vec->redir_unused_sa, 10);
  bit_write  (data,  redir_vec->redir_unchecked, 9);
  bit_write  (data,  redir_vec->redir_delayed,   8);
  bit_write  (data,  redir_vec->redir_invalid,   7);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_igr_rpy_var
*
* DESCRIPTION:
*			Per Port Ingress MACSec Replay and Validate frame configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		rpy_var  - RPY_T 
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	IN	RPY_T		*rpy_var
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
 
  addr = CFG_ISC_GEN(dev);
  data = 0x0;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

  bit_write  (data,  rpy_var->replay_drop,       6);
  bit_write  (data,  rpy_var->replay_protect,    2);
  msec_bits_write (&data, rpy_var->validate_frames,   1, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_REPLAY_WINDOW(dev);
  retVal = msecWritePortReg (dev, port, addr, rpy_var->replay_window);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_set_igr_sci_info
*
* DESCRIPTION:
*			Per Port Ingress MACSec wdefault SCI related configuration
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		sci_info  - SCI_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	IN	SCI_T		*sci_info
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  addr = CFG_IMPLICIT_SCI0(dev);
  data = (MAD_U32) (sci_info->implicit_sci & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = CFG_IMPLICIT_SCI1(dev);
  data = (MAD_U32) ((sci_info->implicit_sci >> 32) & 0xFFFFFFFFLL);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = PORT_NUMBERS(dev);
  msec_bits_write (&data, sci_info->scb_port, 31, 16);
  msec_bits_write (&data, sci_info->def_port, 15, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_igr_cfg
*
* DESCRIPTION:
*		 Get Per Port Ingress MACSec configuration
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*
* OUTPUTS:
*		igr_cfg   - IGR_CFG_T
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	OUT	IGR_CFG_T	*igr_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                               = CFG_IGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  igr_cfg->def_act->def_drop          = bit_get (data, 0);
  igr_cfg->def_act->retain_hdr        = bit_get (data, 1);
  igr_cfg->def_act->drop_badtag       = bit_get (data, 29);
  igr_cfg->def_act->def_redir         = bit_get (data, 28);

  addr                               = CFG_ISC_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  igr_cfg->redir_vec->redir_untagged  = bit_get  (data, 13);
  igr_cfg->redir_vec->redir_badtag    = bit_get  (data, 12);
  igr_cfg->redir_vec->redir_unk_sci   = bit_get  (data, 11);
  igr_cfg->redir_vec->redir_unused_sa = bit_get  (data, 10);
  igr_cfg->redir_vec->redir_unchecked = bit_get  (data, 9);
  igr_cfg->redir_vec->redir_delayed   = bit_get  (data, 8);
  igr_cfg->redir_vec->redir_invalid   = bit_get  (data, 7);
  igr_cfg->rpy_var->replay_drop       = bit_get  (data, 6);
  igr_cfg->rpy_var->replay_protect    = bit_get  (data, 2);
  igr_cfg->rpy_var->validate_frames   = msec_bits_get (data, 1, 0);

  addr                               = CFG_REPLAY_WINDOW(dev);
  retVal     = msecReadPortReg (dev, port, addr, (MAD_U32*)&igr_cfg->rpy_var->replay_window);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

  addr                               = CFG_IMPLICIT_SCI0(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  igr_cfg->sci_info->implicit_sci     = (MAD_U64) data;

  addr                               = CFG_IMPLICIT_SCI1(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  igr_cfg->sci_info->implicit_sci    |= (MAD_U64) data << 32;

  addr                               = PORT_NUMBERS(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  igr_cfg->sci_info->scb_port         = msec_bits_get (data, 31, 16);
  igr_cfg->sci_info->def_port         = msec_bits_get (data, 15, 0);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR_CFG_T        : Port %0d\n", port);
  msec_print_igr_cfg (igr_cfg);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_igr_def_act
*
* DESCRIPTION:
*			Get Per Port Ingress MACSec Default Actions
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		def_act  - IGR_DEF_T
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	OUT	IGR_DEF_T	*def_act
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                 = CFG_IGR_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  def_act->def_drop    = bit_get (data, 0);
  def_act->retain_hdr  = bit_get (data, 1);
  def_act->parse_vlan  = bit_get (data, 18);
  def_act->badtag_chk  = msec_bits_get (data, 27, 23);
  def_act->def_redir   = bit_get (data, 28);
  def_act->drop_badtag = bit_get (data, 29);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR_DEF_T        : Port %0d\n", port);
  msec_print_igr_def (def_act);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_igr_redir_vec
*
* DESCRIPTION:
*			Get Per Port Ingress MACSec Redirect Vector
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		redir_vec - REDIR_T
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	OUT	REDIR_T		*redir_vec
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                       = CFG_ISC_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  redir_vec->redir_untagged  = bit_get  (data, 13);
  redir_vec->redir_badtag    = bit_get  (data, 12);
  redir_vec->redir_unk_sci   = bit_get  (data, 11);
  redir_vec->redir_unused_sa = bit_get  (data, 10);
  redir_vec->redir_unchecked = bit_get  (data, 9);
  redir_vec->redir_delayed   = bit_get  (data, 8);
  redir_vec->redir_invalid   = bit_get  (data, 7);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR REDIR_T      : Port %0d\n", port);
  msec_print_redir (redir_vec);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_igr_rpy_var
*
* DESCRIPTION:
*			Get Per Port Ingress MACSec Replay Vector values
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		rpy_var  - RPY_T  
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	OUT	RPY_T		*rpy_var
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr                     = CFG_ISC_GEN(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  rpy_var->replay_drop     = bit_get  (data, 6);
  rpy_var->replay_protect  = bit_get  (data, 2);
  rpy_var->validate_frames = msec_bits_get (data, 1, 0);

  addr                     = CFG_REPLAY_WINDOW(dev);
  retVal  = msecReadPortReg (dev, port, addr, (MAD_U32*)&rpy_var->replay_window);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR RPY_T        : Port %0d\n", port);
  msec_print_rpy_info (rpy_var);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_get_igr_sci_info
*
* DESCRIPTION:
*			Get Per Port Ingress MACSec SCI Information
*			Returns implicit_sci, default_sci abd scb_port values.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*
* OUTPUTS:
*		sci_info - SCI_T
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	OUT	SCI_T		*sci_info
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;
  addr                    = CFG_IMPLICIT_SCI0(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  sci_info->implicit_sci  = (MAD_U64) data;

  addr                    = CFG_IMPLICIT_SCI1(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  sci_info->implicit_sci |= (MAD_U64) data << 32;

  addr                    = PORT_NUMBERS(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  sci_info->scb_port      = msec_bits_get (data, 31, 16);
  sci_info->def_port      = msec_bits_get (data, 15, 0);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading IGR SCI_T        : Port %0d\n", port);
  msec_print_sci_info (sci_info);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_etm_cfg
*
* DESCRIPTION:
*		 Per Port ETM configuration
*		 Configures individual entries in the ethertype match table.  Values to match agains
*		 are specified by the etm[] entries, and the etm_eg_en and etm_ig_en enable matching
*		 on each of these on a per-bit basis.  For example, setting etm_eg_en to 0x13 would enable
*		 entries 0, 1, and 4.
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*		etm_cfg   - ETM_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	ETM_CFG_T	*etm_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

#ifdef DEBUG
  MAD_CRITIC_INFO ("Programming ETM_CFG_T    : Port %0d\n", port);
  msec_print_etm_cfg (etm_cfg);
#endif

  addr = ET_M0(dev);
  data = 0x0;
  msec_bits_write (&data, etm_cfg->etm[0], 15, 0);
  msec_bits_write (&data, etm_cfg->etm[1], 31, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = ET_M1(dev);
  data = 0x0;
  msec_bits_write (&data, etm_cfg->etm[2], 15, 0);
  msec_bits_write (&data, etm_cfg->etm[3], 31, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = ET_M2(dev);
  data = 0x0;
  msec_bits_write (&data, etm_cfg->etm[4], 15, 0);
  msec_bits_write (&data, etm_cfg->etm[5], 31, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = ET_M3(dev);
  data = 0x0;
  msec_bits_write (&data, etm_cfg->etm[6], 15, 0);
  msec_bits_write (&data, etm_cfg->etm[7], 31, 16);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  addr = ET_MATCH_CTL(dev);
  data = 0x0;
  if (etm_cfg->etm_ig_act == 2)
  {
      bit_set (data, 19);
  }
  if (etm_cfg->etm_ig_act == 1)
  {
      bit_set (data, 17);
  }
  if (etm_cfg->etm_eg_act == 1)
  {
      bit_set (data, 16);
  }
  msec_bits_write (&data, etm_cfg->etm_ig_en, 15, 8);
  msec_bits_write (&data, etm_cfg->etm_eg_en,  7, 0);
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_get_etm_cfg
*
* DESCRIPTION:
*		 Get Per Port ETM configuration.
*		 Get all the ETM ethertype and ETM control variables
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*
* OUTPUTS:
*		etm_cfg   - ETM_CFG_T
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	OUT	ETM_CFG_T	*etm_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data;

  addr            = ET_M0(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  etm_cfg->etm[0] = (MAD_U16) msec_bits_get (data, 15, 0);
  etm_cfg->etm[1] = (MAD_U16) msec_bits_get (data, 31, 16);

  addr            = ET_M1(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  etm_cfg->etm[2] = (MAD_U16) msec_bits_get (data, 15, 0);
  etm_cfg->etm[3] = (MAD_U16) msec_bits_get (data, 31, 16);

  addr            = ET_M2(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  etm_cfg->etm[4] = (MAD_U16) msec_bits_get (data, 15, 0);
  etm_cfg->etm[5] = (MAD_U16) msec_bits_get (data, 31, 16);

  addr            = ET_M3(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  etm_cfg->etm[6] = (MAD_U16) msec_bits_get (data, 15, 0);
  etm_cfg->etm[7] = (MAD_U16) msec_bits_get (data, 31, 16);

  addr            = ET_MATCH_CTL(dev);
  retVal             = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  if (bit_get (data, 19) == 0x1)
      etm_cfg->etm_ig_act = 2;
  else if (bit_get (data, 17) == 0x1)
      etm_cfg->etm_ig_act = 1;
  else
      etm_cfg->etm_ig_act = 0;
  if (bit_get (data, 16) == 0x1)
      etm_cfg->etm_eg_act = 1;
  else
      etm_cfg->etm_eg_act = 0;
  etm_cfg->etm_ig_en = (MAD_U8) msec_bits_get (data, 15, 8);
  etm_cfg->etm_eg_en = (MAD_U8) msec_bits_get (data,  7, 0);

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading ETM_CFG_T        : Port %0d\n", port);
  msec_print_etm_cfg (etm_cfg);
#endif

  return retVal;
}

/*******************************************************************************
* msec_port_fips_chk
*
* DESCRIPTION:
*		 Per Port FIPS Compliance Check 
*		 It programs 128 bit input vector (Plain Text).
*		 It	also programs key or hash key for egress or ingress path as defined by user.
*		 For AES operation, Key[0] is programmed.
*		 For GHASH operation, Hash Key[0] is programmed.
*		It triggers AES or GHASH operation and returns 128 bit output vector when operation is done.
*		After operation is done, It clears FIPS_CTL register.
*		It also toggles MACSEC core soft reset.
*
* INPUTS:
*       dev       - pointer to MAD driver structure returned from mdLoadDriver
*		port	  - port number.
*       fips_cfg  - FIPS_CFG_T
*
* OUTPUTS:
*		fips_out  - 128 bits AES or GHASH observation vector output 
*					(fips_out[3] -> [31:0], fips_out[0] -> [127:96])
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
    IN  MAD_DEV		*dev,
	IN	int			port,
	IN	FIPS_CFG_T	*fips_cfg,
	OUT MAD_U32    *fips_out
)
{
  MAD_STATUS   retVal;
#ifdef DEBUG
  int i;
  MAD_CRITIC_INFO ("Programming FIPS_CFG     : Port %0d\n", port);
  msec_print_fips_cfg (fips_cfg);
#endif

  /* set fips_vec */
  retVal = msec_set_fips_vec (dev, fips_cfg);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_set_fips_vec \n");
     return retVal;
  }

  /* per port FIPS Control setting */
  retVal = msec_port_fips_ctl (dev, port, fips_cfg);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_fips_ctl \n");
     return retVal;
  }

  /* FIPS Observation register */
  retVal = port_fips_obv (dev, port, fips_cfg, fips_out);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call port_fips_obv \n");
     return retVal;
  }

#ifdef Z0A_BUGFIXES
  /* Clear FIPS control */
  retVal = clear_fips_vec (dev, port);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call clear_fips_vec \n");
     return retVal;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("FIPS Observation VEC     : ");
  for (i = 0; i < 4; i++)
  {
    MAD_CRITIC_INFO ("%x", (unsigned int)fips_out[i]);
  }
#endif

  /* Toggle Macsec Soft Reset  */
  retVal = msecWritePortReg(dev, port, RESET_CTL, 0x7);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
  retVal = msecWritePortReg(dev, port, RESET_CTL, 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }
#endif
  return retVal;

}


/*******************************************************************************
* msec_set_fips_vec
*
* DESCRIPTION:
*			FIPS Vector setting
*			It programs 128 bit input vector (Plain Text).
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		fips_cfg FIPS_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	FIPS_CFG_T	*fips_cfg
)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  MAD_U32 data, i, j;

  addr = FIPS_VEC0(dev);
  data = 0x0;
  for (i = 4; i > 0; i--)
  {
      data  = 0x0;
      for (j = 0; j < 4; j++)
      {
          msec_bits_write (&data, fips_cfg->fips_vec[((i*4)-j-1)], ((j*8)+7), (j*8));
      }

      retVal = msecWriteReg (dev, addr, data);
	  if(retVal != MAD_OK)
	  {
        MAD_DBG_ERROR("Failed to write register \n");
        return retVal;
	  }
      addr++;
  }

  return retVal;
}

/*******************************************************************************
* msec_port_fips_ctl
*
* DESCRIPTION:
*			Per Port FIPS Control Setting
*			It programs key or hash key for egress or ingress path as defined by user.
*			For AES operation, Key[0] is programmed.
*			For GHASH operation, Hash Key[0] is programmed.
*			It triggers AES or GHASH operation.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*		fips_cfg FIPS_CFG_T
*
* OUTPUTS:
*		None
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
    IN  MAD_DEV		*dev,
	IN	int		    port,
	IN	FIPS_CFG_T	*fips_cfg
)
{
  MAD_STATUS   retVal;
  CTX_T    ctx;
  MAD_U16 addr;
  MAD_U32 data, i;

  addr = FIPS_CTL(dev); 
  data = 0x0;
  for (i = 0; i < 16; i++)    
  {
      ctx.key [i] = fips_cfg->key[i];
      ctx.hkey[i] = fips_cfg->key[i];
  }
  if (fips_cfg->egr_igr == 1)
  {
      if (fips_cfg->eng_type == 1)
      {
          bit_set (data, 1);
          retVal = msec_port_set_ekey_entry (dev, port, 0x0, &ctx);
          if(retVal != MAD_OK)
		  {
            MAD_DBG_ERROR("Failed to call msec_port_set_ekey_entry \n");
            return retVal|MAD_MSEC_FAIL_PORT_SET_EKEY_ENTRY;
		  }
      }
      else
      {
          bit_set (data, 0);
          retVal = msec_port_set_ehkey_entry (dev, port, 0x0, &ctx);
          if(retVal != MAD_OK)
		  {
            MAD_DBG_ERROR("Failed to call msec_port_set_ehkey_entry \n");
            return retVal|MAD_MSEC_FAIL_PORT_SET_EHKEY_ENTRY;
		  }
      }
  }
  else
  {
      if (fips_cfg->eng_type == 1)
      {
          bit_set (data, 3);
          retVal = msec_port_set_ikey_entry (dev, port, 0x0, &ctx);
          if(retVal != MAD_OK)
		  {
            MAD_DBG_ERROR("Failed to call msec_port_set_ikey_entry \n");
            return retVal|MAD_MSEC_FAIL_PORT_SET_IKEY_ENTRY;
		  }
      }
      else
      {
          bit_set (data, 2);
          retVal = msec_port_set_ihkey_entry (dev, port, 0x0, &ctx);
          if(retVal != MAD_OK)
		  {
            MAD_DBG_ERROR("Failed to call msec_port_set_ihkey_entry \n");
            return retVal|MAD_MSEC_FAIL_PORT_SET_IHKEY_ENTRY;
		  }
      }
  }
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

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
*		None
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
)
{
  MAD_STATUS   retVal;
  int port;
  MAD_U16 addr;
  MAD_U32 data;

  for (port =0; port<4; port++) {
#ifdef DEBUG
  MAD_CRITIC_INFO("before enable flow control: port %d\n", port);
    addr = 0x1d4;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
	{
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01d4 = %x\n", (unsigned int)data);
    addr = 0x1e1;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01e1 = %x\n", (unsigned int)data);
#endif  
    addr = 0x1e3;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01e3 = %x\n", (unsigned int)data);

    addr = 0x1e3;
    data = data | 0x8;
    retVal = msecWritePortReg (dev, port, addr, data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
    }

    addr = 0x1d4;
    data = 0x8;
    retVal = msecWritePortReg (dev, port, addr, data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
    }
    /* reduce sysmac IPG from default 12 to 6 */
    addr = 0x1e1;
    data = 0x20061;
    retVal = msecWritePortReg (dev, port, addr, data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
    }

#ifdef DEBUG
  MAD_CRITIC_INFO("after enable flow control: port %d\n", port);
    addr = 0x1d4;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
	{
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01d4 = %x\n", (unsigned int)data);
    addr = 0x1e1;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01e1 = %x\n", (unsigned int)data);
    addr = 0x1e3;
    retVal = msecReadPortReg (dev, port, addr, &data);
    if(retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed to read register \n");
      return retVal;
    }
  MAD_CRITIC_INFO("01e3 = %x\n", (unsigned int)data);
#endif

  }
  return MAD_OK;
}


/*******************************************************************************
* msec_set_phy_speed_duplex
*
* DESCRIPTION:
*       Set Phy speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       speed   -  speed 10, 100, 1000.  
*       duplex   - Duplex MAD_TRUE, Half MAD_FALSE.  
*   
* OUTPUTS:
*		None
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
	IN  int     	port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
)
{
  MAD_STATUS   retVal;
  MAD_U32      mode;

  switch (speed)
  {
	case 10:
		if (duplex==MAD_TRUE)
			mode = MAD_AUTO_AD_10FDX;
		else
			mode = MAD_AUTO_AD_10HDX;
		break;
	case 100:
		if (duplex==MAD_TRUE)
			mode = MAD_AUTO_AD_100FDX;
		else
			mode = MAD_AUTO_AD_100HDX;
		break;
	case 1000:
	default:
		if (duplex==MAD_TRUE)
			mode = MAD_AUTO_AD_1000FDX;
		else
			mode = MAD_AUTO_AD_1000HDX;
		break;
  }

  retVal = mdCopperSetAutoNeg(dev,port,MAD_TRUE, mode);
  if (retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Setting for (port %i, mode %i) returned ",(int)port,(int)mode);
    return retVal;             
  }
#ifdef DEBUG
  {
	  MAD_BOOL en;
      MAD_U32  chkMode;

    retVal = mdCopperGetAutoNeg(dev,port, &en, &chkMode);
    if (retVal != MAD_OK)
	{
      MAD_DBG_ERROR("Failed Setting for (port %i, mode %i) \n",(int)port,(int)chkMode);
      return retVal;             
	}
	MAD_CRITIC_INFO("Set AutoNeg enable %x get %x\n", en, MAD_TRUE);
	MAD_CRITIC_INFO("Set AutoNeg Mode %x get %x\n", (unsigned int)chkMode, (unsigned int)mode);
  }

#endif

  return MAD_OK;
}


/*******************************************************************************
* msec_set_macsec_speed_duplex
*
* DESCRIPTION:
*       Set MacSec speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		port - port number.
*       speed   -  speed 10, 100, 1000.  
*       duplex   - Duplex MAD_TRUE, Half MAD_FALSE.  
*   
* OUTPUTS:
*		None
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
	IN  int     	port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
)
{
  MAD_STATUS   retVal;
  MAD_U16      addr;
  MAD_U32      data;


  addr = 0x1c0;
  data = 0x117c8;
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Failed to write register \n");
    return retVal;
  }


  addr = 0x1C3;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Failed to read register \n");
    return retVal;
  }
  
  switch (speed)
  {
    case 10:
         data &= 0xFFFC;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
		break;
    case 100:
         data &= 0xFFFD;
		 data |= 0x1;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
		break;
    case 1000:
	default :
	  {
		 data |= 0x3;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
 		break;
      }
   }

   retVal = msecReadPortReg (dev, port, addr, &data);
   if(retVal != MAD_OK)
   {
     MAD_DBG_ERROR("Failed to read register \n");
     return retVal;
   }
  
   if (duplex == MAD_FALSE) {
     /* turn off bit4 full duplex */
	 data &= 0xFFEF;
     retVal = msecWritePortReg (dev, port, addr, data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to write register \n");
       return retVal;
	 }
   } else {
	 data |= 0x10;
     retVal = msecWritePortReg (dev, port, addr, data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to write register \n");
       return retVal;
	 }
   }

#ifdef DEBUG
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }
	MAD_CRITIC_INFO("msec_set_macsec_speed_duplex port %x wire speed %x \n duplex %x", 
		           port, (unsigned int)(data&0x3), (unsigned int)((data>>4)&0x1));
#endif

   addr = 0x1C2;
   data = 0;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
     MAD_DBG_ERROR("Failed to read register \n");
     return retVal;
   }

   addr = 0x1C0;
   data = 0x117c9;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
     MAD_DBG_ERROR("Failed to read register \n");
     return retVal;
   }


  /* set MACsec sysmac speed and duplex
     sysmac duplex mode shall always be full */

  addr = 0x1e0;
  data = 0x017c8;
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Failed to write register \n");
    return retVal;
  }


  addr = 0x1E3;
  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Failed to read register \n");
    return retVal;
  }
  
  switch (speed)
  {
    case 10:
         data &= 0xFFFC;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
		break;
    case 100:
         data &= 0xFFFD;
		 data |= 0x1;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
		break;
    case 1000:
	default :
	  {
		 data |= 0x3;
         retVal = msecWritePortReg (dev, port, addr, data);
         if(retVal != MAD_OK)
		 {
           MAD_DBG_ERROR("Failed to write register \n");
           return retVal;
		 }
 		break;
      }
   }

   retVal = msecReadPortReg (dev, port, addr, &data);
   if(retVal != MAD_OK)
   {
     MAD_DBG_ERROR("Failed to read register \n");
     return retVal;
   }
   
   /* Full duplex */
  
   data |= 0x10;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }

#ifdef DEBUG
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }
	MAD_CRITIC_INFO("msec_set_macsec_speed_duplex port %x system speed %x \n duplex %x", 
		           port, (unsigned int)(data&0x3), (unsigned int)((data>>4)&0x1));
#endif


   addr = 0x1E2;
   data = 0x0;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }

   addr = 0x1E0;
   data = 0x017c9;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }

   /* change trail_sz to 0 */
   addr = 0x1;
   data = 0x400030;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }

   /* clear tx and rx err cnt */
   addr =  0x11;
   data = 0;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }

   addr = 0x12;
   data = 0;
   retVal = msecWritePortReg (dev, port, addr, data);
   if(retVal != MAD_OK)
   {
      MAD_DBG_ERROR("Failed to write register \n");
      return retVal;
   }
  
  return MAD_OK;
}


/*******************************************************************************
* msec_set_mac_frame_size_limit
*
* DESCRIPTION:
*       Set Mac frame size limit
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*		size - frame size.
*   
* OUTPUTS:
*		None
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
)
{
  MAD_STATUS   retVal;
  int port;
  MAD_U16      addr;
  MAD_U32      data;

  for (port=0; port<dev->numOfPorts; port++)
  {
	 addr = 0x1c0;
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }

     data =(data & 0xF0003) | (size << 2 & 0xFFFC) ;
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_set_mac_frame_size_limit port %x %x before setting, data is %x \n", port, addr, (unsigned int)data);
#endif
     retVal = msecWritePortReg (dev, port, addr, data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to write register \n");
       return retVal;
	 }
#ifdef DEBUG
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }
	MAD_CRITIC_INFO("msec_set_mac_frame_size_limit port %x %x after setting, data is %x \n", port, addr, (unsigned int)data);
#endif

	 addr = 0x1e0;
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }

     data =(data & 0xF0003) | (size << 2 & 0xFFFC) ;
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_set_mac_frame_size_limit port %x %x before setting, data is %x \n", port, addr, (unsigned int)data);
#endif
     retVal = msecWritePortReg (dev, port, addr, data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to write register \n");
       return retVal;
	 }
#ifdef DEBUG
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }
	MAD_CRITIC_INFO("msec_set_mac_frame_size_limit port %x %x after setting, data is %x \n", port, addr, (unsigned int)data);
#endif

  }
  return MAD_OK;

}

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
*		None
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
)
{
  MAD_STATUS   retVal;
  int port;
  MAD_U16      addr;
  MAD_U32      data;

  for (port=0; port<dev->numOfPorts; port++)
  {
	 addr = 0x1d0;
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }

     /* set bit5,4,3,2 */
     data = data | 0x3c;
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_enable_non_disruptive_loopback port %x before setting, data is %x \n", port, (unsigned int)data);
#endif
     retVal = msecWritePortReg (dev, port, addr, data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to write register \n");
       return retVal;
	 }

#ifdef DEBUG
     retVal = msecReadPortReg (dev, port, addr, &data);
     if(retVal != MAD_OK)
	 {
       MAD_DBG_ERROR("Failed to read register \n");
       return retVal;
	 }
	MAD_CRITIC_INFO("msec_enable_non_disruptive_loopback port %x after setting, data is %x \n", port, (unsigned int)data);
#endif
  }
  return MAD_OK;
}


