#include <madCopyright.h>

/********************************************************************************
* msec_func.c
* 
* DESCRIPTION:
*       Functions to access platform layer by MacSec control program.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madSmiIf.h> 
#include <madHwCntl.h>
#include "msecFunc.h"
#include "msecApi.h"


#undef MSEC_USE_PLATFORM
int DEBUG_MSEC_WRITE = 0;
int DEBUG_MSEC_READ = 0;
int msec_bits_write (MAD_U32 *data, int in_data, int bit_to, int bit_from);
int msec_bits_get (int in_data, int bit_to, int bit_from);

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
*		None
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
)
{
  MAD_STATUS   retVal;
  int phyaddr;
  /* page number 11 and register 0~3 are defined for indirect access to MACsec address space:
     reg 0 - ReadAddr
     reg 1 - WriteAddr
     reg 2 - Data low 16 bits
     reg 3 - Data high 16 bits
    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);
  */
  phyaddr = dev->msec_ctrl.port_num;
#ifdef MSEC_USE_PLATFORM

  retVal = madSmiIfWriteRegister(dev, 0, 22, 16);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  /* do MDC/MDIO write to write addr */
  retVal = madSmiIfWriteRegister(dev, 0, 1, addr);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }

  /* do MDC/MDIO CPU write to DataL  */
  retVal = madSmiIfWriteRegister(dev, 0, 2, (MAD_U16)(data & 0xFFFF));
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  /* do MDC/MDIO CPU write to DataH */ 
  retVal = madSmiIfWriteRegister(dev, 0,3, data >> 16);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  /* Write page register 22 to 0 for current 1340L board*/
  retVal = madSmiIfWriteRegister(dev, 0, 22, 0);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
#else
        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,1,addr))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,1);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }



        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,2,(MAD_U16)(data & 0xFFFF)))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,2);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }



        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,3,(MAD_U16)(data >> 16)))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,3);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

#endif
if (DEBUG_MSEC_WRITE == 1)
		printf ("msecWriteReg: Addr %x Data %x\n", addr, (unsigned int)data);

  return MAD_OK;
}

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
)
{
  MAD_STATUS   retVal;
  MAD_U16 dataL, dataH;
  int phyaddr;
  /* do MDC/MDIO write to read addr */
  /* do MDC/MDIO CPU write to DataL (data[15:0]) */
  /* do MDC/MDIO CPU write to DataH (data[31:16]) */

  /* put read results in data */
/* page number 11 and register 0~3 are defined for indirect access to MACsec address space:
     reg 0 - ReadAddr
     reg 1 - WriteAddr
     reg 2 - Data low 16 bits
     reg 3 - Data high 16 bits
  */
 phyaddr = dev->msec_ctrl.port_num;
#ifdef MSEC_USE_PLATFORM
  retVal = madSmiIfWriteRegister(dev, 0, 22, 16);  
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  retVal = madSmiIfWriteRegister(dev,0, 0, addr);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  retVal = madSmiIfReadRegister(dev, 0, 2, &dataL);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to read register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  retVal = madSmiIfReadRegister(dev, 0, 3, &dataH);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to read register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
#else
        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,0,addr))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,0);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        if((retVal = madHwReadPagedPhyReg(dev,phyaddr,16,2,&dataL))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",phyaddr,2);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwReadPagedPhyReg(dev,phyaddr,16,3,&dataH))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",phyaddr,3);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

#endif

  *data = (dataH << 16) + (dataL & 0xFFFF);
if (DEBUG_MSEC_READ == 1)
		printf ("msecReadReg: Addr %x Data %x\n", addr, (unsigned int)*data);
  return MAD_OK;
  /*return dataL; */
}

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
*		None
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
)
{
  MAD_STATUS   retVal;
  int phyaddr;
  phyaddr = dev->msec_ctrl.port_num;
#ifdef MSEC_USE_PLATFORM
  retVal = madSmiIfWriteRegister (dev, 0, 22, 253);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  retVal = madSmiIfWriteRegister (dev, 0, 13, 0xc000);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
  retVal = madSmiIfWriteRegister (dev, 0, 13, 0x8000);
  if(retVal != MAD_OK)
  {
      MAD_DBG_ERROR("Failed to write register \n");
      return MAD_DRV_FAIL_WRITE_REG;
  }
#else
        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,253,13,0xc000))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,13);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,253,13,0x8000))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,13);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

#endif
  return MAD_OK;
}

/*#ifdef DEBUG */
/****************************************************************************************
*
*		Display MacSec information
*
*****************************************************************************************/
void msec_print_mac_ctrl (MAC_CTRL *mac_ctrl)
{
  if (mac_ctrl->mac_type == 1)
  MAD_CRITIC_INFO ("mac_type                 : WIRE MAC\n");
  else
  MAD_CRITIC_INFO ("mac_type                 : SYS MAC\n");
  MAD_CRITIC_INFO ("port_en                  : %0d\n", mac_ctrl->port_en);
  MAD_CRITIC_INFO ("speed                    : %0d\n", mac_ctrl->speed);
  MAD_CRITIC_INFO ("hfd                      : %0d\n", mac_ctrl->hfd);
  MAD_CRITIC_INFO ("mibcnt_en                : %0d\n", mac_ctrl->mibcnt_en);
  MAD_CRITIC_INFO ("lpbk_en                  : %0d\n", mac_ctrl->lpbk_en);
  MAD_CRITIC_INFO ("max_frame_sz             : %0d\n", mac_ctrl->max_frame_sz);

}

void msec_print_match_en (MATCH_EN *match_en)
{
  MAD_CRITIC_INFO ("en_def                   : %0d\n", match_en->en_def);
  MAD_CRITIC_INFO ("en_mc                    : %0d\n", match_en->en_mc);
  MAD_CRITIC_INFO ("en_da                    : %0d\n", match_en->en_da);
  MAD_CRITIC_INFO ("en_sa                    : %0d\n", match_en->en_sa);
  MAD_CRITIC_INFO ("en_vlan                  : %0d\n", match_en->en_vlan);
  MAD_CRITIC_INFO ("en_et                    : %0d\n", match_en->en_et);
  MAD_CRITIC_INFO ("en_scian                 : %0d  Valid only for Ingress path\n", match_en->en_scian);

}

void msec_print_match_fld (MATCH_FLD *match_fld)
{
  MAD_CRITIC_INFO ("da                       : %llx\n", match_fld->da);
  MAD_CRITIC_INFO ("sa                       : %llx\n", match_fld->sa);
  MAD_CRITIC_INFO ("vlan                     : %x  \n", match_fld->vlan);
  MAD_CRITIC_INFO ("et                       : %x  \n", match_fld->et);
  MAD_CRITIC_INFO ("sci                      : %llx Valid only for Ingress path\n", match_fld->sci);
  MAD_CRITIC_INFO ("tci                      : %x   Valid only for Ingress path\n", match_fld->tci);
  MAD_CRITIC_INFO ("tci_msk                  : %x   Valid only for Ingress path\n", match_fld->tci_msk);

}

void msec_print_act_fld (ACT_FLD *act_fld)
{
  MAD_CRITIC_INFO ("ctx_num                  : %0d\n", act_fld->ctx_num);
  MAD_CRITIC_INFO ("sec_level                  : %0d\n", act_fld->sec_level);
  MAD_CRITIC_INFO ("drop                     : %0d\n", act_fld->drop);
  MAD_CRITIC_INFO ("redir/ndl                : %0d\n", act_fld->redir);
  MAD_CRITIC_INFO ("auth_en                  : %0d\n", act_fld->auth_en);
  MAD_CRITIC_INFO ("enc_en                   : %0d  Valid only for Egress path\n", act_fld->enc_en);

}

void msec_print_lkup (LKUP_T *lkup)
{
  msec_print_match_en  (lkup->lk_match_en);  
  msec_print_match_fld (lkup->lk_match_fld);  
  msec_print_act_fld   (lkup->lk_act_fld);  

}

void msec_print_ect (CTX_T *ctx)
{
  MAD_CRITIC_INFO ("sci                      : %llx\n", ctx->sci);
  MAD_CRITIC_INFO ("pn                       : %x  \n", (unsigned int)ctx->pn);
  MAD_CRITIC_INFO ("tci                      : %x  \n", ctx->tci);

}

void msec_print_key (CTX_T *ctx)
{
  MAD_U32 i;
  MAD_CRITIC_INFO ("key                      : ");
  for (i = 0; i < 16; i++)
  {
    MAD_CRITIC_INFO ("%02x", ctx->key[i]);
  }
  MAD_CRITIC_INFO ("\n");

}

void msec_print_hkey (CTX_T *ctx)
{
  MAD_U32 i;
  MAD_CRITIC_INFO ("hkey                     : ");
  for (i = 0; i < 16; i++)
  {
    MAD_CRITIC_INFO ("%02x", ctx->hkey[i]);
  }
  MAD_CRITIC_INFO ("\n");

}

void msec_print_redir_hdr (REDIR_HDR_T *redir_hdr)
{
  MAD_CRITIC_INFO ("redir da                 : %llx\n", redir_hdr->da);
  MAD_CRITIC_INFO ("redir et                 : %llx\n", (unsigned long long)redir_hdr->et);
  MAD_CRITIC_INFO ("redir sa[0]              : %llx\n", redir_hdr->sa[0]);
  MAD_CRITIC_INFO ("redir sa[1]              : %llx\n", redir_hdr->sa[1]);
  MAD_CRITIC_INFO ("redir sa[2]              : %llx\n", redir_hdr->sa[2]);
  MAD_CRITIC_INFO ("redir sa[3]              : %llx\n", redir_hdr->sa[3]);

}

void msec_print_cmn_cfg (CMN_CFG_T *cmn_cfg)
{
  MAD_CRITIC_INFO ("trail_sz                 : %0d \n", cmn_cfg->trail_sz);
  MAD_CRITIC_INFO ("conf_offset              : %0d \n", cmn_cfg->conf_offset);
  MAD_CRITIC_INFO ("def_vlan                 : %x  \n", cmn_cfg->def_vlan);
  MAD_CRITIC_INFO ("mc_high                  : %llx\n", cmn_cfg->mc_high);
  MAD_CRITIC_INFO ("mc_low_msk               : %x  \n", cmn_cfg->mc_low_msk);
}

void msec_print_egr_cfg (EGR_CFG_T *egr_cfg)
{
  MAD_CRITIC_INFO ("def_drop                 : %0d\n", egr_cfg->def_drop);   
  MAD_CRITIC_INFO ("parse_ae                 : %0d\n", egr_cfg->parse_ae);   
  MAD_CRITIC_INFO ("parse_vlan               : %0d\n", egr_cfg->parse_vlan);

}

void msec_print_igr_def (IGR_DEF_T *igr_def)
{
  MAD_CRITIC_INFO ("def_drop                 : %0d\n", igr_def->def_drop);   
  MAD_CRITIC_INFO ("def_redir                : %0d\n", igr_def->def_redir);  
  MAD_CRITIC_INFO ("drop_badtag              : %0d\n", igr_def->drop_badtag);
  MAD_CRITIC_INFO ("retain_hdr               : %0d\n", igr_def->retain_hdr); 
  MAD_CRITIC_INFO ("parse_vlan               : %0d\n", igr_def->parse_vlan);  

}

void msec_print_redir (REDIR_T *redir)
{
  MAD_CRITIC_INFO ("redir_untagged           : %0d\n", redir->redir_untagged); 
  MAD_CRITIC_INFO ("redir_badtag             : %0d\n", redir->redir_badtag);   
  MAD_CRITIC_INFO ("redir_unk_sci            : %0d\n", redir->redir_unk_sci);  
  MAD_CRITIC_INFO ("redir_unused_sa          : %0d\n", redir->redir_unused_sa);
  MAD_CRITIC_INFO ("redir_unchecked          : %0d\n", redir->redir_unchecked);
  MAD_CRITIC_INFO ("redir_delayed            : %0d\n", redir->redir_delayed);  
  MAD_CRITIC_INFO ("redir_invalid            : %0d\n", redir->redir_invalid);  

}

void msec_print_sci_info (SCI_T *sci_info)
{
  MAD_CRITIC_INFO ("implicit_sci             : %llx\n", sci_info->implicit_sci);
  MAD_CRITIC_INFO ("scb_port                 : %x  \n", sci_info->scb_port);
  MAD_CRITIC_INFO ("def_port                 : %x  \n", sci_info->def_port);

}

void msec_print_rpy_info (RPY_T *rpy_info)
{
  MAD_CRITIC_INFO ("validate_frames          : %0d\n", rpy_info->validate_frames);
  MAD_CRITIC_INFO ("replay_protect           : %0d\n", rpy_info->replay_protect); 
  MAD_CRITIC_INFO ("replay_drop              : %0d\n", rpy_info->replay_drop);    
  MAD_CRITIC_INFO ("replay_window            : %0d\n", rpy_info->replay_window);  

}

void msec_print_igr_cfg (IGR_CFG_T *igr_cfg)
{
  msec_print_igr_def  (igr_cfg->def_act);
  msec_print_redir    (igr_cfg->redir_vec);
  msec_print_sci_info (igr_cfg->sci_info);
  msec_print_rpy_info (igr_cfg->rpy_var);

}

void msec_print_etm_cfg (ETM_CFG_T *etm_cfg)
{
  MAD_CRITIC_INFO ("etm [0]                  : %x\n", etm_cfg->etm[0]);
  MAD_CRITIC_INFO ("etm [1]                  : %x\n", etm_cfg->etm[1]);
  MAD_CRITIC_INFO ("etm [2]                  : %x\n", etm_cfg->etm[2]);
  MAD_CRITIC_INFO ("etm [3]                  : %x\n", etm_cfg->etm[3]);
  MAD_CRITIC_INFO ("etm [4]                  : %x\n", etm_cfg->etm[4]);
  MAD_CRITIC_INFO ("etm [5]                  : %x\n", etm_cfg->etm[5]);
  MAD_CRITIC_INFO ("etm [6]                  : %x\n", etm_cfg->etm[6]);
  MAD_CRITIC_INFO ("etm [7]                  : %x\n", etm_cfg->etm[7]);
  MAD_CRITIC_INFO ("etm_eg_en                : %x\n", etm_cfg->etm_eg_en);
  MAD_CRITIC_INFO ("etm_ig_en                : %x\n", etm_cfg->etm_ig_en);
  switch (etm_cfg->etm_eg_act)
  {
      case 0  : MAD_CRITIC_INFO ("etm_eg_act               : Bypass \n");break;
      case 1  : MAD_CRITIC_INFO ("etm_eg_act               : Drop   \n");break;
      default : MAD_CRITIC_INFO ("etm_eg_act               : Unknown\n");break;
  }
  switch (etm_cfg->etm_ig_act)
  {
      case 0  : MAD_CRITIC_INFO ("etm_ig_act               : Bypass  \n");break;
      case 1  : MAD_CRITIC_INFO ("etm_ig_act               : Drop    \n");break;
      case 2  : MAD_CRITIC_INFO ("etm_ig_act               : Redirect\n");break;
      default : MAD_CRITIC_INFO ("etm_ig_act               : Unknown \n");break;
  }
}

void msec_print_fips_cfg (FIPS_CFG_T *fips_cfg)
{
  MAD_U32 i;
  MAD_CRITIC_INFO ("fips_vec                 : ");
  for (i = 0; i < 16; i++)
  {
    MAD_CRITIC_INFO ("%02x", fips_cfg->fips_vec[i]);
  }
  MAD_CRITIC_INFO ("\n");
  MAD_CRITIC_INFO ("key                      : ");
  for (i = 0; i < 16; i++)
  {
    MAD_CRITIC_INFO ("%02x", fips_cfg->key[i]);
  }
  MAD_CRITIC_INFO ("\n");
  if (fips_cfg->egr_igr == 1)
      MAD_CRITIC_INFO ("egr_igr                  : Egress\n");
  else
      MAD_CRITIC_INFO ("egr_igr                  : Ingress\n");
  if (fips_cfg->eng_type == 1)
      MAD_CRITIC_INFO ("eng_type                 : AES\n");
  else
      MAD_CRITIC_INFO ("eng_type                 : GHASH\n");
 
}
/*#endif */

void msec_print_macsec_stat (MAD_DEV *dev, MACSEC_STAT_T *macsec_stat)
{
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_0         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+0     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_1         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+1     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_2         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+2     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_3         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+3     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_4         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+4     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_5         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+5     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_6         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+6     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_HIT_7         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_HIT(dev)+7     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_0          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+0      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_1          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+1      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_2          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+2      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_3          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+3      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_4          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+4      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_5          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+5      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_6          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+6      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OK_7          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OK(dev)+7      ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_0       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+0   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_1       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+1   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_2       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+2   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_3       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+3   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_4       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+4   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_5       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+5   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_6       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+6   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNCHK_7       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNCHK(dev)+7   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_0       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+0   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_1       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+1   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_2       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+2   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_3       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+3   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_4       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+4   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_5       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+5   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_6       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+6   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_DELAY_7       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_DELAY(dev)+7   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_0        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+0    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_1        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+1    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_2        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+2    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_3        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+3    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_4        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+4    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_5        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+5    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_6        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+6    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_LATE_7        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_LATE(dev)+7    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_0       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+0   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_1       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+1   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_2       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+2   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_3       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+3   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_4       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+4   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_5       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+5   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_6       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+6   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_INVLD_7       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_INVLD(dev)+7   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_0      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+0  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_1      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+1  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_2      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+2  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_3      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+3  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_4      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+4  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_5      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+5  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_6      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+6  ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTVLD_7      : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTVLD(dev)+7  ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_0    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+0]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_1    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+1]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_2    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+2]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_3    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+3]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_4    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+4]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_5    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+5]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_6    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+6]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_PROT_7    : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_PROT(dev)+7]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_0     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+0 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_1     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+1 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_2     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+2 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_3     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+3 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_4     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+4 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_5     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+5 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_6     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+6 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_PKT_ENC_7     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_PKT_ENC(dev)+7 ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_0         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+0     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_1         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+1     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_2         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+2     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_3         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+3     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_4         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+4     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_5         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+5     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_6         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+6     ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_HIT_7         : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_HIT(dev)+7     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OCT_VAL       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OCT_VAL(dev)   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OCT_DEC       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OCT_DEC(dev)   ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNTAG         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNTAG(dev)     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOTAG         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOTAG(dev)     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_BADTAG        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_BADTAG(dev)    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNKSCI        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNKSCI(dev)    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOSCI         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOSCI(dev)     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_UNUSSA        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_UNUSSA(dev)    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_NOUSSA        : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_NOUSSA(dev)    ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_OCT_TOT       : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_OCT_TOT(dev)   ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_OCT_PROT      : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_OCT_PROT(dev)  ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_OCT_ENC       : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_OCT_ENC(dev)   ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_OCT_TOTAL     : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_OCT_TOTAL(dev) ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_MISS          : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_MISS(dev)      ]);
  MAD_CRITIC_INFO ("MX_SEC_EGR_MISS          : %llx\n", macsec_stat->macsec_vec[MX_SEC_EGR_MISS (dev)     ]);
  MAD_CRITIC_INFO ("MX_SEC_IGR_REDIR         : %llx\n", macsec_stat->macsec_vec[MX_SEC_IGR_REDIR(dev)     ]);
 
}

void msec_print_mib_stat (MIB_STAT_T *mib_stat)
{
  MAD_CRITIC_INFO ("Good Octets Rcvd(LSB)    : %llx\n", mib_stat->mib_vec[0]);
  MAD_CRITIC_INFO ("Good Octets Rcvd(MSB)    : %llx\n", mib_stat->mib_vec[1]);
  MAD_CRITIC_INFO ("Bad  Octets Rcvd         : %llx\n", mib_stat->mib_vec[2]);
  MAD_CRITIC_INFO ("Tx CRC Err/Fifo Underrun : %llx\n", mib_stat->mib_vec[3]);
  MAD_CRITIC_INFO ("Good Unicast Frames Rcvd : %llx\n", mib_stat->mib_vec[4]);
  MAD_CRITIC_INFO ("Sent deferred            : %llx\n", mib_stat->mib_vec[5]);
  MAD_CRITIC_INFO ("Broadcast Frames Rcvd    : %llx\n", mib_stat->mib_vec[6]);
  MAD_CRITIC_INFO ("Multicast Frames Rcvd    : %llx\n", mib_stat->mib_vec[7]);
  MAD_CRITIC_INFO ("Frames 64 Octets         : %llx\n", mib_stat->mib_vec[8]);
  MAD_CRITIC_INFO ("Frames 65  - 127  Octets : %llx\n", mib_stat->mib_vec[9]);
  MAD_CRITIC_INFO ("Frames 128 - 255  Octets : %llx\n", mib_stat->mib_vec[10]);
  MAD_CRITIC_INFO ("Frames 256 - 511  Octets : %llx\n", mib_stat->mib_vec[11]);
  MAD_CRITIC_INFO ("Frames 512 - 1023 Octets : %llx\n", mib_stat->mib_vec[12]);
  MAD_CRITIC_INFO ("Frames 1024- Max  Octets : %llx\n", mib_stat->mib_vec[13]);
  MAD_CRITIC_INFO ("Good Octets Sent(LSB)    : %llx\n", mib_stat->mib_vec[14]);
  MAD_CRITIC_INFO ("Good Octets Sent(MSB)    : %llx\n", mib_stat->mib_vec[15]);
  MAD_CRITIC_INFO ("Unicast Frame Sent       : %llx\n", mib_stat->mib_vec[16]);
  MAD_CRITIC_INFO ("Excessive Collision      : %llx\n", mib_stat->mib_vec[17]);
  MAD_CRITIC_INFO ("Multicast Frames Sent    : %llx\n", mib_stat->mib_vec[18]);
  MAD_CRITIC_INFO ("Broadcast Frames Sent    : %llx\n", mib_stat->mib_vec[19]);
  MAD_CRITIC_INFO ("Sent Multiple            : %llx\n", mib_stat->mib_vec[20]);
  MAD_CRITIC_INFO ("FC Sent                  : %llx\n", mib_stat->mib_vec[21]);
  MAD_CRITIC_INFO ("FC Rcvd                  : %llx\n", mib_stat->mib_vec[22]);
  MAD_CRITIC_INFO ("Rcvd FIFO overrun        : %llx\n", mib_stat->mib_vec[23]);
  MAD_CRITIC_INFO ("Undersize (Rcvd)         : %llx\n", mib_stat->mib_vec[24]);
  MAD_CRITIC_INFO ("Fragments (Rcvd)         : %llx\n", mib_stat->mib_vec[25]);
  MAD_CRITIC_INFO ("Oversize  (Rcvd)         : %llx\n", mib_stat->mib_vec[26]);
  MAD_CRITIC_INFO ("Jabber    (Rcvd)         : %llx\n", mib_stat->mib_vec[27]);
  MAD_CRITIC_INFO ("Rx Err Frame Rcvd        : %llx\n", mib_stat->mib_vec[28]);
  MAD_CRITIC_INFO ("Bad CRC                  : %llx\n", mib_stat->mib_vec[29]);
  MAD_CRITIC_INFO ("Collisions               : %llx\n", mib_stat->mib_vec[30]);
  MAD_CRITIC_INFO ("Late Collision           : %llx\n", mib_stat->mib_vec[31]);

}

/*****************************************************************************************
*
*  Utility Functions
*
*****************************************************************************************/

int aes_hkey (CTX_T *ctx)
{
  int             i;
  MAD_U8         in [16];
  aes_encrypt_ctx acx[1];

  for (i = 0; i < 16; i++)    
     in[i] = 0;
  aes_encrypt_key (ctx->key, 16, acx);
  aes_encrypt     (in, ctx->hkey, acx);

  return 1;

}

int msec_bits_write (MAD_U32 *data, int in_data, int bit_to, int bit_from)
{
   int      i; 
   MAD_U32 orig;

   orig   = *data;
   for (i = bit_from; i <= bit_to; i++)
   {
     bit_write (*data, bit_get(in_data, i-bit_from), i);
   }

#ifdef DEBUG_BITS
   MAD_CRITIC_INFO ("msec_bits_write               : data %x [%d:%d] %x-> data %x\n", 
	   (unsigned int)orig, bit_to, bit_from, in_data, (unsigned int)*data);
#endif

   return 1;
}

int msec_bits_get (int in_data, int bit_to, int bit_from)
{
   int i, out, orig;

   orig   = in_data;
   out    = 0x0;
   for (i = bit_from; i <= bit_to; i++)
   {
     out |= (bit_get(in_data, i) << (i-bit_from));
   }

#ifdef DEBUG_BITS
   MAD_CRITIC_INFO ("msec_bits_get                 : data %x [%d:%d] %x-> data %x\n", orig, bit_to, bit_from, in_data, out);
#endif

   return out;
}

/****************************************************************************************
*
*   Internal Functions 
*
****************************************************************************************/

MAD_STATUS msec_port_set_nxtpn_ent (MAD_DEV *dev,  int port, int ent_num, MAD_U32 data)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;

  addr = VN_CFG_RPY(dev) + ent_num;
  retVal = msecWritePortReg (dev, port, addr, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}
MAD_STATUS port_fips_obv (MAD_DEV *dev,  int port, FIPS_CFG_T *fips_cfg, MAD_U32 *fips_out)
{
  MAD_STATUS   retVal;
  MAD_U16 addr;
  if (fips_cfg->egr_igr == 1)
  {
      if (fips_cfg->eng_type == 1)
      {
          addr = FIPS_EGR_AES3(dev);
      }
      else
      {
          addr = FIPS_EGR_GHASH3(dev);
      }
  }
  else
  {
      if (fips_cfg->eng_type == 1)
      {
          addr = FIPS_IGR_AES3(dev);
      }
      else
      {
          addr = FIPS_IGR_GHASH3(dev);
      }
  }
  retVal = msecReadPortReg (dev, port, addr, &fips_out[0]);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  addr--;
  retVal = msecReadPortReg (dev, port, addr, &fips_out[1]);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  addr--;
  retVal = msecReadPortReg (dev, port, addr, &fips_out[2]);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  addr--;
  retVal = msecReadPortReg (dev, port, addr, &fips_out[3]);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadPortReg \n");
     return retVal;
  }
  return retVal;
}

MAD_STATUS clear_fips_vec (MAD_DEV *dev,int port)
{
  MAD_STATUS   retVal;
  retVal = msecWritePortReg(dev, port, FIPS_CTL(dev), 0x0);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecWritePortReg \n");
     return retVal;
  }

  return retVal;
}

/**************************************************************************************
*
*	MacSec Functions of Stat
*
**************************************************************************************/

MAD_STATUS msec_port_get_macsec_stat_value (MAD_DEV *dev,  int port, int stat_name, MAD_U32 *data)
{
  MAD_STATUS   retVal;
  
  /* get base addr value */
  switch (port)
  {
    case 0 :
        stat_name += VN_SEC_STATS(dev);
        break;
    case 1 :
        stat_name += VN_SEC_STATS(dev) + NUM_OF_MACSEC_STAT(dev);
        break;
    case 2 :
        stat_name += VN_SEC_STATS(dev) + NUM_OF_MACSEC_STAT(dev) * 2;
        break;
    case 3 :
        stat_name += VN_SEC_STATS(dev) + NUM_OF_MACSEC_STAT(dev) * 3;
        break;
  }
  retVal = msecReadReg (dev, stat_name, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadReg \n");
     return retVal;
  }

  return retVal;
}

MAD_STATUS msec_port_get_macsec_stat (MAD_DEV *dev,  int port, MACSEC_STAT_T *macsec_stat)
{
  MAD_STATUS   retVal=MAD_OK;
  int i;
  MAD_U32 data;

  for (i = 0; i < NUM_OF_MACSEC_STAT(dev); i++)
  {
      retVal = msec_port_get_macsec_stat_value (dev, port, i, &data); 
      if(retVal != MAD_OK)
	  {
         MAD_DBG_ERROR("Failed to call msec_port_get_macsec_stat_value \n");
         return retVal;
	  }
      macsec_stat->macsec_vec[i] += data; 
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading MacSEC Stat      : Port %0d\n", port);
  msec_print_macsec_stat (macsec_stat);
#endif

  return retVal;
}

MAD_STATUS port_acc_macsec_stat (MAD_DEV *dev,  int port, MACSEC_STAT_T *macsec_stat)
{
  MAD_STATUS   retVal=MAD_OK;
  int i;
  MAD_U32 data;


  for (i = 0; i < NUM_OF_MACSEC_STAT(dev); i++)  
  {
      retVal = msec_port_get_macsec_stat_value (dev, port, i, &data); 
      if(retVal != MAD_OK)
	  {
         MAD_DBG_ERROR("Failed to call msec_port_get_macsec_stat_value \n");
         return retVal;
	  }
      macsec_stat->macsec_vec[i] += data; 
  }
  
#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading MacSEC Stat      : Port %0d\n", port);
  msec_print_macsec_stat (macsec_stat);
#endif

  return retVal;
}

MAD_STATUS port_rd_n_dump_macsec_stat (MAD_DEV *dev, int port)
{
  MAD_STATUS   retVal;
  MACSEC_STAT_T macsec_stat;

  retVal = msec_port_get_macsec_stat (dev, port, &macsec_stat);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_macsec_stat \n");
     return retVal;
  }
  msec_print_macsec_stat    (dev, &macsec_stat);

  return retVal;
}

int rd_n_dump_macsec_stat (MAD_DEV *dev)
{
  MAD_STATUS   retVal;
  int orig_portnum, i;

  orig_portnum = dev->msec_ctrl.port_num;
  for (i = 0; i < 4; i++)
  {
    retVal = port_rd_n_dump_macsec_stat (dev, i);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call port_rd_n_dump_macsec_stat \n");
       return retVal;
	}
  }
  dev->msec_ctrl.port_num = orig_portnum;

  return retVal;
}

MAD_STATUS msec_clr_macsec_stat (MAD_DEV *dev)
{
  MAD_STATUS   retVal;
  MACSEC_STAT_T  macsec_stat;
  int            orig_portnum, i;

  orig_portnum = dev->msec_ctrl.port_num;
  for (i = 0; i < 4; i++)
  {
    retVal = msec_port_get_macsec_stat (dev, i, &macsec_stat);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msec_port_get_macsec_stat \n");
       return retVal;
	}
  }
  dev->msec_ctrl.port_num = orig_portnum;

  return retVal;
}

MAD_STATUS port_get_mib_stat_value (MAD_DEV *dev,  int port, int stat_name, MAD_U32 *data)
{
  MAD_STATUS   retVal;
  
  /* get base addr value */
  switch (port)
  {
    case 0 :
        stat_name += VN_MAC_STATS(dev);
        break;
    case 1 :
        stat_name += VN_MAC_STATS(dev) + 0x20;
        break;
    case 2 :
        stat_name += VN_MAC_STATS(dev) + 0x20 + 0x20;
        break;
    case 3 :
        stat_name += VN_MAC_STATS(dev) + 0x20 + 0x20 + 0x20;
        break;
  }
  retVal = msecReadReg (dev, stat_name, data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msecReadReg \n");
     return retVal;
  }

  return retVal;
}

MAD_STATUS msec_port_get_mib_stat (MAD_DEV *dev,  int port, MIB_STAT_T *mib_stat)
{
  MAD_STATUS   retVal;
  int i;
  MAD_U32 data;

  for (i = 0; i < 32; i++)
  {
     retVal = port_get_mib_stat_value (dev, port, i, &data);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call port_get_mib_stat_value \n");
       return retVal;
	}
     mib_stat->mib_vec[i]  = data;
  }

#ifdef DEBUG
  MAD_CRITIC_INFO ("Reading WMAC MIB Stat    : Port %0d\n", port);
  msec_print_mib_stat (mib_stat);
#endif

  return retVal;
}

int rd_n_dump_mib_stat (MAD_DEV *dev)
{
  MAD_STATUS   retVal;
  int orig_portnum, i;

  orig_portnum = dev->msec_ctrl.port_num;
  for (i = 0; i < 4; i++)
  {
    retVal = port_rd_n_dump_mib_stat (dev, i);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call port_rd_n_dump_mib_stat \n");
       return retVal;
	}
  }
  dev->msec_ctrl.port_num = orig_portnum;

  return retVal;
}

MAD_STATUS port_rd_n_dump_mib_stat (MAD_DEV *dev, int port)
{
  MAD_STATUS   retVal;
  MIB_STAT_T  mib_stat;

  retVal = msec_port_get_mib_stat (dev, port, &mib_stat);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR("Failed to call msec_port_get_mib_stat \n");
     return retVal;
  }
  msec_print_mib_stat    (&mib_stat);

  return retVal;
}

MAD_STATUS msec_clr_mib_stat (MAD_DEV *dev)
{
  MAD_STATUS   retVal;
  MIB_STAT_T  mib_stat;
  int         orig_portnum, i;

  orig_portnum = dev->msec_ctrl.port_num;
  for (i = 0; i < 4; i++)
  {
    retVal = msec_port_get_mib_stat (dev, i, &mib_stat);
    if(retVal != MAD_OK)
	{
       MAD_DBG_ERROR("Failed to call msec_port_get_mib_stat \n");
       return retVal;
	}
  }
  dev->msec_ctrl.port_num = orig_portnum;

  return retVal;
}

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
*		None
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
)
{
  MAD_STATUS   retVal;
  MAD_U16 data;
  if((retVal = madHwReadPagedPhyReg(dev,0,253,13,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_dis_macsec_clk before setting, data is %x\n", data);
#endif

  if((retVal = madHwWritePagedPhyReg(dev,0,253,13,0x800))!= MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,2);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

  if((retVal = madHwReadPagedPhyReg(dev,0,253,13,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

#ifdef DEBUG
	MAD_CRITIC_INFO("msec_dis_macsec_clk before setting, data is %x, check bit 11 is 1\n", data);
#endif
  return MAD_OK;

}

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
*		None
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
)
{
  MAD_STATUS   retVal;
  MAD_U16 data;
  if((retVal = madHwReadPagedPhyReg(dev,0,253,13,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_en_macsec_clk before setting, data is %x\n", data);
#endif

  if((retVal = madHwWritePagedPhyReg(dev,0,253,13,0x0))!= MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,2);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

  if((retVal = madHwReadPagedPhyReg(dev,0,253,13,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
#ifdef DEBUG
	MAD_CRITIC_INFO("msec_en_macsec_clk after setting, data is %x check bit 11 is 0 \n", data);
#endif

  return MAD_OK;

}


/*******************************************************************************
* msec_en_PTP
*
* DESCRIPTION:
*       Set PTP function enable through bypass macsec.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        en    -  1: enale PTP, 0: disable PTP
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

MAD_STATUS msec_en_PTP 
(
    IN  MAD_DEV    *dev,
    IN  int            en
)
{
  MAD_STATUS   retVal;
  MAD_U32 data;
  
  if (en==1)
  {
    if((retVal = msecReadPortReg (dev, 0, SLC_RAM_CTRL1(dev), &data))!= MAD_OK)
    {
      MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",0,1);
      return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    bit_clear (data, 13);
    if((retVal = msecWritePortReg (dev, 0, SLC_RAM_CTRL1(dev), data))!= MAD_OK)
    {
      MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
      return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
  }
  else
  {
    if((retVal = msecReadPortReg (dev, 0, SLC_RAM_CTRL1(dev), &data))!= MAD_OK)
    {
      MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",0,1);
      return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    bit_set (data, 13);
    if((retVal = msecWritePortReg (dev, 0, SLC_RAM_CTRL1(dev), data))!= MAD_OK)
    {
      MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,1);
      return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
  }
  return MAD_OK;

}

