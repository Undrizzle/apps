/* ----------------------------------------------------------------------
//  Copyright (c) 2008-2010 Marvell Semiconductor
//  All Rights Reserved
// ----------------------------------------------------------------------
*/
/*/ \file macSecTest.cpp : Defines the entry point for the DLL application. */

#include "madApi.h"
#include "msecApi.h"
#include "msecFunc.h"
/* #include "msectest.h" */
#include "madSample.h"


MAD_STATUS msec_fixed (MAD_DEV *dev)
{
  int        port, cx;
  MAD_STATUS   retVal;
 
#ifdef MX_REV_Z0_BUGFIXES
  MAD_U32   my_data;
  MAD_U16   my_addr;

  /* Soft Error Fix */

  /* Enable NDL Buffer Bist Test */
  for (port=0; port<4; port++) 
  {
    dev->msec_ctrl.port_num = port;
    retVal = msecReadPortReg (dev, port, SLC_RAM_CTRL1, &my_data); 
    bit_set (my_data, 10);
    retVal = msecWritePortReg (dev, port, SLC_RAM_CTRL1, my_data);
  }

  /* Disable NDL Buffer Bist Test */
  for (port=0; port<4; port++)
  {
    dev->msec_ctrl.port_num = port;
    bit_clear (my_data, 10);
    retVal = msecWritePortReg (dev, port, SLC_RAM_CTRL1, my_data);
  }

  /* Force Macsec Reset */
  dev->msec_ctrl.port_num = 0x0;
  msec_force_macsec_rst (dev);

  /* Hard Error Fix */

  for (port=0; port<4; port++)
  {
    dev->msec_ctrl.port_num = port;

    /* Enable Egress Ghash FIPS CTL */
    retVal = msecWritePortReg (dev, port, FIPS_CTL, 0x1);

    /* Disable Egress Ghash FIPS CTL */
    retVal = msecWritePortReg (dev, port, FIPS_CTL, 0x0);

    /* Bring MACSEC Block out of soft reset */
    retVal = msecWritePortReg(dev, port, RESET_CTL, 0x0);

    /* Programing ECT Pn to 1 */
    for (cx = 0; cx < 8; cx++) 
    {
        my_addr = VN_CFG_ELU_CTX(dev) + (cx * 4) + 2;
        retVal = msecWritePortReg(dev, port, my_addr, 0x1);
    }
  }   
#endif

  return retVal;
}

MAD_STATUS msec_start (MAD_DEV *dev)
{
  MAD_STATUS   retVal;
 
  retVal = msec_fixed (dev);
  printf("!!!!!!!!!!!!!!!!!! start msec_start \n");


  /* initialze maxwell AE core */
  retVal = msec_maxwell_ae_start_init (dev);
  return retVal;
}

/*********************************************************************************************
*
*              Test Ingress Entry
*
********************************************************************************************/


MAD_U32 testInEntryAll(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;
    int         nCases;
	 int			ent_num;
  LKUP_T    test_lkup, check_lkup, save_lkup; 
  ACT_FLD   test_act_fld, check_act_fld, save_act_fld;
  MATCH_EN  test_match_en, check_match_en, save_match_en;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;
  CTX_T     test_ctx, save_ctx, check_ctx;
  MAD_U8    test_key[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

  retVal = msec_fixed (dev);

  save_lkup.lk_match_en  = &save_match_en;
  save_lkup.lk_match_fld = &save_match_fld;
  save_lkup.lk_act_fld   = &save_act_fld;
  test_lkup.lk_match_en  = &test_match_en;
  test_lkup.lk_match_fld = &test_match_fld;
  test_lkup.lk_act_fld   = &test_act_fld;
  check_lkup.lk_match_en  = &check_match_en;
  check_lkup.lk_match_fld = &check_match_fld;
  check_lkup.lk_act_fld   = &check_act_fld;

  test_ctx.sci = 0x555555555555555L;       /*/< SCI (Valid only for Egress path) */
  test_ctx.pn  = 0xaaaaaaaa;        /*/< Initial Packet Number (Valid only for Egress path) */
  test_ctx.tci = 0x5a;       /*/< TCI (Valid only for Egress path) */
  memcpy(test_ctx.key, test_key, 16);  /*/< 128 bit Encrytion/Decryption Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  memcpy(test_ctx.hkey, test_key, 16); /*/< 128 bit Encrytion/Decryption Hash Key (hkey[15] -> hkey[7:0], hkey[0] -> hkey[127:120]) */
#if 0
{
int i;
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
}
#endif

  test_match_en.en_def=0;   /*/< Enable default match (search will always hit this entry if no previous entry matched) */
  test_match_en.en_mc=1;    /*/< Enable Muticast match */
  test_match_en.en_da=0;    /*/< Enable MAC DA match */
  test_match_en.en_sa=1;    /*/< Enable MAC SA match */
  test_match_en.en_vlan=0;  /*/< Enable VLAN match */
  test_match_en.en_et=1;    /*/< Enable Ethertype match */
  test_match_en.en_scian=0; /*/< Enable SCI_AN match (Valid only for Ingress path) */

  test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  test_act_fld.drop=0x1;    /*/< Drop this Packet */
  test_act_fld.redir=0x1;   /*/< For Egress, redirect the packet to ingress path (NDL). For Ingress, redirect the packet to alternate destination. */
  test_act_fld.auth_en=0x1; /*/< Encapsulate and authenticate this packet. */
  test_act_fld.enc_en=0;  /*/< Encrypt this packet. auth_en must also be set when this bit is set. (Valid only for egress path). */


  MSG_PRINT("Testing set Ingress Entry ...\n");

  nCases = 3;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(ent_num=0; ent_num<nCases; ent_num++)
    { 
       test_act_fld.ctx_num=ent_num; /*/< Index to the context to use */
		/* save original ingress entry */
       retVal = msec_port_get_igr_entry (dev, port, ent_num, &save_lkup, &save_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ingress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* set test ingress entry */
       retVal = msec_port_set_igr_entry (dev, port, ent_num, &test_lkup, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting ingress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* get test ingress entry */
       retVal = msec_port_get_igr_entry (dev, port, ent_num, &check_lkup, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ingress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

		/* recover original ingress entry */
       retVal = msec_port_set_igr_entry (dev, port, ent_num, &save_lkup, &save_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting ingress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

#if 0
{
int i;
printf("!!! check ingress entry\n");
/*
printf("test_ctx.sci %llx check_ctx.sci %llx \n", test_ctx.sci, check_ctx.sci);
printf("test_ctx.pn %x check_ctx.pn %x \n", (unsigned int)test_ctx.pn, (unsigned int)check_ctx.pn);
printf("test_ctx.tci %x check_ctx.tci %x \n", test_ctx.tci, check_ctx.tci);
*/
printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
	   
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
printf("\n check_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.hkey[i]);
} 
#endif  
#if 1
{

printf("!!! check ingress entry\n");
printf("lkup->test_match_en  - lkup->check_match_enl:\n");
printf(" en_def %x - %x \n",  test_match_en.en_def, check_match_en.en_def);
printf(" en_mc %x - %x \n",  test_match_en.en_mc, check_match_en.en_mc);
printf(" en_da %x - %x \n",  test_match_en.en_da, check_match_en.en_da);
printf(" en_sa %x - %x \n",  test_match_en.en_sa, check_match_en.en_sa);
printf(" en_vlan %x - %x \n",  test_match_en.en_vlan, check_match_en.en_vlan);
printf(" en_et %x - %x \n",  test_match_en.en_et, check_match_en.en_et);
printf(" en_scian %x - %x \n", test_match_en.en_scian, check_match_en.en_scian);

printf("\n lkup->test_match_fld:\n");
printf("lkup->test_match_fld  - lkup->check_match_fld:\n");
printf(" da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf("  %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf("  %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

printf("\n lkup->test_act_fld:\n");
printf("lkup->test_act_fld  - lkup->check_act_fld:\n");
printf(" ctx_num %x - %x \n", test_act_fld.ctx_num, check_act_fld.ctx_num);
printf(" drop %x - %x \n", test_act_fld.drop, check_act_fld.drop);
printf(" redir %x - %x \n", test_act_fld.redir, check_act_fld.redir);
printf(" auth_en %x - %x \n", test_act_fld.auth_en, check_act_fld.auth_en);
printf(" enc_en %x - %x \n", test_act_fld.enc_en, check_act_fld.enc_en);

}
#endif
 		/* check test ingress entry */
/*	   if ((test_ctx.sci != check_ctx.sci) || 
           (test_ctx.pn  != check_ctx.pn) ||
           (test_ctx.tci != check_ctx.tci) ||
           (memcmp(test_ctx.key, check_ctx.key, 16)) ||
           (memcmp(test_ctx.hkey, check_ctx.hkey, 16)))
*/
	   if ((memcmp(test_ctx.key, check_ctx.key, 16)) ||
           (memcmp(test_ctx.hkey, check_ctx.hkey, 16)))
       {
          MSG_PRINT("Check ingress entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry ctx successful for (port %i, rntry number %i)\n",(int)port,ent_num);


	   if ((test_match_en.en_def!=check_match_en.en_def) ||
           (test_match_en.en_mc!=check_match_en.en_mc) ||
           (test_match_en.en_da!=check_match_en.en_da) ||
           (test_match_en.en_vlan!=check_match_en.en_vlan) ||
/*           (test_match_en.en_sa!=check_match_en.en_sa) ||  */
/*           (test_match_en.en_et!=check_match_en.en_et) || */
           (test_match_en.en_scian!=check_match_en.en_scian))
       {
          MSG_PRINT("Check ingress entry lk_match)en failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry lk_match)en successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_match_fld.da!=check_match_fld.da) || 
/*           (test_match_fld.sa!=check_match_fld.sa) ||  */
           (test_match_fld.vlan!=check_match_fld.vlan) ||
/*           (test_match_fld.et!=check_match_fld.et) ||  */
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
       {
          MSG_PRINT("Check ingress entry match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_act_fld.drop!=check_act_fld.drop) ||
           (test_act_fld.redir!=check_act_fld.redir) || 
           (test_act_fld.auth_en!=check_act_fld.auth_en) || 
           (test_act_fld.enc_en!=check_act_fld.enc_en))
       {
          MSG_PRINT("Check ingress entry act_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry act_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testInEntry(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;
	 int			ent_num;
	 MAD_U32 data;

  LKUP_T    test_lkup, check_lkup, save_lkup; 
  ACT_FLD   test_act_fld, check_act_fld, save_act_fld;
  MATCH_EN  test_match_en, check_match_en, save_match_en;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;
  CTX_T     test_ctx, check_ctx;
  MAD_U8    test_key[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

  retVal = msec_fixed (dev);

  save_lkup.lk_match_en  = &save_match_en;
  save_lkup.lk_match_fld = &save_match_fld;
  save_lkup.lk_act_fld   = &save_act_fld;
  test_lkup.lk_match_en  = &test_match_en;
  test_lkup.lk_match_fld = &test_match_fld;
  test_lkup.lk_act_fld   = &test_act_fld;
  check_lkup.lk_match_en  = &check_match_en;
  check_lkup.lk_match_fld = &check_match_fld;
  check_lkup.lk_act_fld   = &check_act_fld;

  test_ctx.sci = 0x555555555555555L;       /*/< SCI (Valid only for Egress path) */
  test_ctx.pn  = 0xaaaaaaaa;        /*/< Initial Packet Number (Valid only for Egress path) */
  test_ctx.tci = 0x5a;       /*/< TCI (Valid only for Egress path) */
  memcpy(test_ctx.key, test_key, 16);  /*/< 128 bit Encrytion/Decryption Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  memcpy(test_ctx.hkey, test_key, 16); /*/< 128 bit Encrytion/Decryption Hash Key (hkey[15] -> hkey[7:0], hkey[0] -> hkey[127:120]) */
#if 0
{
int i;
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
}
#endif

  test_match_en.en_def=0;   /*/< Enable default match (search will always hit this entry if no previous entry matched) */
  test_match_en.en_mc=1;    /*/< Enable Muticast match */
  test_match_en.en_da=0;    /*/< Enable MAC DA match */
  test_match_en.en_sa=1;    /*/< Enable MAC SA match */
  test_match_en.en_vlan=0;  /*/< Enable VLAN match */
  test_match_en.en_et=1;    /*/< Enable Ethertype match */
  test_match_en.en_scian=0; /*/< Enable SCI_AN match (Valid only for Ingress path) */

  test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  test_act_fld.drop=0x1;    /*/< Drop this Packet */
  test_act_fld.redir=0x1;   /*/< For Egress, redirect the packet to ingress path (NDL). For Ingress, redirect the packet to alternate destination. */
  test_act_fld.auth_en=0x1; /*/< Encapsulate and authenticate this packet. */
  test_act_fld.enc_en=0;  /*/< Encrypt this packet. auth_en must also be set when this bit is set. (Valid only for egress path). */

  MSG_PRINT("Testing set Ingress Entry ...\n");


  port=0;
  ent_num=0;
  test_act_fld.ctx_num=ent_num; /*/< Index to the context to use */

		/* set test ikey entry */
       retVal = msec_port_set_ikey_entry (dev, port, ent_num, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting iksy entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
    
	   /* get test ikey entry */
       retVal = msec_port_get_ikey_entry (dev, port, ent_num, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ikey entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
         /* check test ikey entry */
	     if (memcmp(test_ctx.key, check_ctx.key, 16)) 
		 {
            MSG_PRINT("Check ikey entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	     else
		 {
            MSG_PRINT("Check ikey entry ctx succesful for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	   }

#if 0
{
int i;
printf("!!! check ikey entry\n");
printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
}
#endif

		/* set test ihkey entry */
       retVal = msec_port_set_ihkey_entry (dev, port, ent_num, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting ihksy entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
    
	   /* get test ihkey entry */
       retVal = msec_port_get_ihkey_entry (dev, port, ent_num, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ihkey entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
 	     if (memcmp(test_ctx.hkey, check_ctx.hkey, 16))
		 {
            MSG_PRINT("Check ihkey entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	     else
		 {
            MSG_PRINT("Check ihkey entry ctx succesful for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	   }

#if 0
{
int i;
printf("!!! check ihkey entry\n");
printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
}
#endif
	   
        /* program NxtPn (replay table) entry to 1.*/ 
       retVal = msec_port_set_nxtpn_ent (dev, port, ent_num, 0x1);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Next pn failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
       retVal = msec_port_get_nxtpn_entry (dev, port, ent_num, &data);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Next pn failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
	     if (data==1)
		 {
		   MSG_PRINT("Test access nextpn (replay table entry) successful\n");
		 }
	     else
		 {
		   MSG_PRINT("Test access nextpn (replay table entry) failed\n");
		 }
	   }

       /* program an entry for ILU table */
       retVal = msec_port_set_ilu_entry (dev, port, ent_num, &test_lkup);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting entry for ILU table failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
       retVal = msec_port_get_ilu_entry (dev, port, ent_num, &check_lkup);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting entry for ILU table failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }

#if 0
{

printf("!!! check ingress entry\n");
printf("lkup->test_match_en  - lkup->check_match_enl:\n");
printf(" en_def %x - %x \n",  test_match_en.en_def, check_match_en.en_def);
printf(" en_mc %x - %x \n",  test_match_en.en_mc, check_match_en.en_mc);
printf(" en_da %x - %x \n",  test_match_en.en_da, check_match_en.en_da);
printf(" en_sa %x - %x \n",  test_match_en.en_sa, check_match_en.en_sa);
printf(" en_vlan %x - %x \n",  test_match_en.en_vlan, check_match_en.en_vlan);
printf(" en_et %x - %x \n",  test_match_en.en_et, check_match_en.en_et);
printf(" en_scian %x - %x \n", test_match_en.en_scian, check_match_en.en_scian);

printf("\n lkup->test_match_fld:\n");
printf("lkup->test_match_fld  - lkup->check_match_fld:\n");
printf(" da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf("  %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf("  %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

printf("\n lkup->test_act_fld:\n");
printf("lkup->test_act_fld  - lkup->check_act_fld:\n");
printf(" ctx_num %x - %x \n", test_act_fld.ctx_num, check_act_fld.ctx_num);
printf(" drop %x - %x \n", test_act_fld.drop, check_act_fld.drop);
printf(" redir %x - %x \n", test_act_fld.redir, check_act_fld.redir);
printf(" auth_en %x - %x \n", test_act_fld.auth_en, check_act_fld.auth_en);
printf(" enc_en %x - %x \n", test_act_fld.enc_en, check_act_fld.enc_en);

}
#endif

	   if ((test_match_en.en_def!=check_match_en.en_def) ||
           (test_match_en.en_mc!=check_match_en.en_mc) ||
           (test_match_en.en_da!=check_match_en.en_da) ||
           (test_match_en.en_sa!=check_match_en.en_sa) || 
           (test_match_en.en_vlan!=check_match_en.en_vlan) ||
           (test_match_en.en_et!=check_match_en.en_et) ||
           (test_match_en.en_scian!=check_match_en.en_scian))
       {
          MSG_PRINT("Check ingress entry lk_match)en failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry lk_match)en successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_match_fld.da!=check_match_fld.da) || 
/*           (test_match_fld.sa!=check_match_fld.sa) ||  */
           (test_match_fld.vlan!=check_match_fld.vlan) ||
/*           (test_match_fld.et!=check_match_fld.et) ||  */
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
       {
          MSG_PRINT("Check ingress entry match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_act_fld.drop!=check_act_fld.drop) ||
           (test_act_fld.redir!=check_act_fld.redir) || 
           (test_act_fld.auth_en!=check_act_fld.auth_en) || 
           (test_act_fld.enc_en!=check_act_fld.enc_en))
       {
          MSG_PRINT("Check ingress entry act_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check ingress entry act_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);


    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}


/*********************************************************************************************
*
*              Test Egress Entry
*
********************************************************************************************/
MAD_U32 testEgEntryAll(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;  
    int         nCases;
	 int			ent_num;
  LKUP_T    test_lkup, check_lkup, save_lkup; 
  ACT_FLD   test_act_fld, check_act_fld, save_act_fld;
  MATCH_EN  test_match_en, check_match_en, save_match_en;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;
  CTX_T     test_ctx, save_ctx, check_ctx;
  MAD_U8    test_key[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

  retVal = msec_fixed (dev);


  save_lkup.lk_match_en  = &save_match_en;
  save_lkup.lk_match_fld = &save_match_fld;
  save_lkup.lk_act_fld   = &save_act_fld;
  test_lkup.lk_match_en  = &test_match_en;
  test_lkup.lk_match_fld = &test_match_fld;
  test_lkup.lk_act_fld   = &test_act_fld;
  check_lkup.lk_match_en  = &check_match_en;
  check_lkup.lk_match_fld = &check_match_fld;
  check_lkup.lk_act_fld   = &check_act_fld;

  test_ctx.sci = 0x555555555555555L;       /*/< SCI (Valid only for Egress path) */
  test_ctx.pn  = 0xaaaaaaaa;        /*/< Initial Packet Number (Valid only for Egress path) */
  test_ctx.tci = 0x5a;       /*/< TCI (Valid only for Egress path) */
  memcpy(test_ctx.key, test_key, 16);  /*/< 128 bit Encrytion/Decryption Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  memcpy(test_ctx.hkey, test_key, 16); /*/< 128 bit Encrytion/Decryption Hash Key (hkey[15] -> hkey[7:0], hkey[0] -> hkey[127:120]) */
#if 0
{
int i;
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
}
#endif

  test_match_en.en_def=0;   /*/< Enable default match (search will always hit this entry if no previous entry matched) */
  test_match_en.en_mc=1;    /*/< Enable Muticast match */
  test_match_en.en_da=0;    /*/< Enable MAC DA match */
  test_match_en.en_sa=1;    /*/< Enable MAC SA match */
  test_match_en.en_vlan=0;  /*/< Enable VLAN match */
  test_match_en.en_et=1;    /*/< Enable Ethertype match */
  test_match_en.en_scian=0; /*/< Enable SCI_AN match (Valid only for Ingress path) */

  test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  test_act_fld.drop=0x1;    /*/< Drop this Packet */
  test_act_fld.redir=0x1;   /*/< For Egress, redirect the packet to ingress path (NDL). For Ingress, redirect the packet to alternate destination. */
  test_act_fld.auth_en=0x1; /*/< Encapsulate and authenticate this packet. */
  test_act_fld.enc_en=0;  /*/< Encrypt this packet. auth_en must also be set when this bit is set. (Valid only for egress path). */


  MSG_PRINT("Testing set Egress Entry ...\n");

  nCases = 3;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(ent_num=0; ent_num<nCases; ent_num++)
    { 
       test_act_fld.ctx_num=ent_num; /*/< Index to the context to use */
		/* save original ingress entry */
       retVal = msec_port_get_egr_entry (dev, port, ent_num, &save_lkup, &save_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting egress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* set test ingress entry */
       retVal = msec_port_set_egr_entry (dev, port, ent_num, &test_lkup, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting egress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* get test ingress entry */
       retVal = msec_port_get_egr_entry (dev, port, ent_num, &check_lkup, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting egress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

		/* recover original ingress entry */
       retVal = msec_port_set_egr_entry (dev, port, ent_num, &save_lkup, &save_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting egress entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

#if 0
{
int i;
printf("!!! check egress entry\n");
printf("test_ctx.sci %llx check_ctx.sci %llx \n", test_ctx.sci, check_ctx.sci);
printf("test_ctx.pn %x check_ctx.pn %x \n", (unsigned int)test_ctx.pn, (unsigned int)check_ctx.pn);
printf("test_ctx.tci %x check_ctx.tci %x \n", test_ctx.tci, check_ctx.tci);

printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
	   
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
printf("\n check_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.hkey[i]);
} 
printf("\n");
#endif  
 		/* check test egress entry */
	   if ((test_ctx.sci != check_ctx.sci) || 
           (test_ctx.pn  != check_ctx.pn) ||
           (test_ctx.tci != check_ctx.tci) ||
           (memcmp(test_ctx.key, check_ctx.key, 16)) ||
           (memcmp(test_ctx.hkey, check_ctx.hkey, 16)))
       {
          MSG_PRINT("Check egress entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check egress entry ctx successful for (port %i, rntry number %i)\n",(int)port,ent_num);

#if 0
{

printf("!!! check egress entry\n");
printf("lkup->test_match_en  - lkup->check_match_enl:\n");
printf(" en_def %x - %x \n",  test_match_en.en_def, check_match_en.en_def);
printf(" en_mc %x - %x \n",  test_match_en.en_mc, check_match_en.en_mc);
printf(" en_da %x - %x \n",  test_match_en.en_da, check_match_en.en_da);
printf(" en_sa %x - %x \n",  test_match_en.en_sa, check_match_en.en_sa);
printf(" en_vlan %x - %x \n",  test_match_en.en_vlan, check_match_en.en_vlan);
printf(" en_et %x - %x \n",  test_match_en.en_et, check_match_en.en_et);
printf(" en_scian %x - %x \n", test_match_en.en_scian, check_match_en.en_scian);

printf("\n lkup->test_match_fld:\n");
printf("lkup->test_match_fld  - lkup->check_match_fld:\n");
printf(" en_da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" en_sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf(" en_sa %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" en_vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" en_et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" en_sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf(" en_sci %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" en_tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" en_tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

printf("\n lkup->test_act_fld:\n");
printf("lkup->test_act_fld  - lkup->check_act_fld:\n");
printf(" ctx_num %x - %x \n", test_act_fld.ctx_num, check_act_fld.ctx_num);
printf(" drop %x - %x \n", test_act_fld.drop, check_act_fld.drop);
printf(" redir %x - %x \n", test_act_fld.redir, check_act_fld.redir);
printf(" auth_en %x - %x \n", test_act_fld.auth_en, check_act_fld.auth_en);
printf(" enc_en %x - %x \n", test_act_fld.enc_en, check_act_fld.enc_en);

}
#endif


	   if ((test_match_en.en_def!=check_match_en.en_def) ||
           (test_match_en.en_mc!=check_match_en.en_mc) ||
           (test_match_en.en_da!=check_match_en.en_da) ||
           (test_match_en.en_sa!=check_match_en.en_sa) || 
           (test_match_en.en_vlan!=check_match_en.en_vlan) ||
           (test_match_en.en_et!=check_match_en.en_et) ||
           (test_match_en.en_scian!=check_match_en.en_scian))
       {
          MSG_PRINT("Check egress entry lk_match_en failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check egress entry lk_match_en successful for (port %i, rntry number %i)\n",(int)port,ent_num);

      if ((test_match_fld.da!=check_match_fld.da) || 
           (test_match_fld.sa!=check_match_fld.sa) || 
           (test_match_fld.vlan!=check_match_fld.vlan) ||
           (test_match_fld.et!=check_match_fld.et))
/* For Ingress only 
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
*/
       {
          MSG_PRINT("Check egress entry match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check egress entry match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_act_fld.drop!=check_act_fld.drop) ||
           (test_act_fld.redir!=check_act_fld.redir) || 
           (test_act_fld.auth_en!=check_act_fld.auth_en) || 
           (test_act_fld.enc_en!=check_act_fld.enc_en))
       {
          MSG_PRINT("Check egress entry act_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check egress entry act_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testEgEntry(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;
	 int			ent_num;
	 MAD_U32 data;

  LKUP_T    test_lkup, check_lkup, save_lkup; 
  ACT_FLD   test_act_fld, check_act_fld, save_act_fld;
  MATCH_EN  test_match_en, check_match_en, save_match_en;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;
  CTX_T     test_ctx, check_ctx;
  MAD_U8    test_key[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

  retVal = msec_fixed (dev);

  save_lkup.lk_match_en  = &save_match_en;
  save_lkup.lk_match_fld = &save_match_fld;
  save_lkup.lk_act_fld   = &save_act_fld;
  test_lkup.lk_match_en  = &test_match_en;
  test_lkup.lk_match_fld = &test_match_fld;
  test_lkup.lk_act_fld   = &test_act_fld;
  check_lkup.lk_match_en  = &check_match_en;
  check_lkup.lk_match_fld = &check_match_fld;
  check_lkup.lk_act_fld   = &check_act_fld;

  test_ctx.sci = 0x555555555555555L;       /*/< SCI (Valid only for Egress path) */
  test_ctx.pn  = 0xaaaaaaaa;        /*/< Initial Packet Number (Valid only for Egress path) */
  test_ctx.tci = 0x5a;       /*/< TCI (Valid only for Egress path) */
  memcpy(test_ctx.key, test_key, 16);  /*/< 128 bit Encrytion/Decryption Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  memcpy(test_ctx.hkey, test_key, 16); /*/< 128 bit Encrytion/Decryption Hash Key (hkey[15] -> hkey[7:0], hkey[0] -> hkey[127:120]) */
#if 0
{
int i;
printf("\n test_ctx.hkey:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.hkey[i]);
}
#endif

  test_match_en.en_def=0;   /*/< Enable default match (search will always hit this entry if no previous entry matched) */
  test_match_en.en_mc=1;    /*/< Enable Muticast match */
  test_match_en.en_da=0;    /*/< Enable MAC DA match */
  test_match_en.en_sa=1;    /*/< Enable MAC SA match */
  test_match_en.en_vlan=0;  /*/< Enable VLAN match */
  test_match_en.en_et=1;    /*/< Enable Ethertype match */
  test_match_en.en_scian=0; /*/< Enable SCI_AN match (Valid only for Ingress path) */

  test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  test_act_fld.drop=0x1;    /*/< Drop this Packet */
  test_act_fld.redir=0x1;   /*/< For Egress, redirect the packet to ingress path (NDL). For Ingress, redirect the packet to alternate destination. */
  test_act_fld.auth_en=0x1; /*/< Encapsulate and authenticate this packet. */
  test_act_fld.enc_en=0;  /*/< Encrypt this packet. auth_en must also be set when this bit is set. (Valid only for egress path). */

  MSG_PRINT("Testing set Egress Entry ...\n");


  port=0;
  ent_num=0;
  test_act_fld.ctx_num=ent_num; /*/< Index to the context to use */

		/* set test ikey entry */
       retVal = msec_port_set_ekey_entry (dev, port, ent_num, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting ekey entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
    
	   /* get test ikey entry */
       retVal = msec_port_get_ekey_entry (dev, port, ent_num, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ekey entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
         /* check test ikey entry */
	     if (memcmp(test_ctx.key, check_ctx.key, 16)) 
		 {
            MSG_PRINT("Check ekey entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	     else
		 {
            MSG_PRINT("Check ekey entry ctx succesful for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	   }

#if 0
{
int i;
printf("!!! check ekey entry\n");
printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
}
#endif

		/* set test ehkey entry */
       retVal = msec_port_set_ehkey_entry (dev, port, ent_num, &test_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting ehksy entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
    
	   /* get test ihkey entry */
       retVal = msec_port_get_ehkey_entry (dev, port, ent_num, &check_ctx);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting ehkey entry failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
 	     if (memcmp(test_ctx.hkey, check_ctx.hkey, 16))
		 {
            MSG_PRINT("Check ehkey entry ctx failed for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	     else
		 {
            MSG_PRINT("Check ehkey entry ctx succesful for (port %i, rntry number %i)\n",(int)port,ent_num);
		 }
	   }

#if 0
{
int i;
printf("!!! check ehkey entry\n");
printf("\n test_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", test_ctx.key[i]);
printf("\n check_ctx.key:\n");
for (i=0; i<16; i++)
  printf(" %02x ", check_ctx.key[i]);
}
#endif
	   
        /* program NxtPn (replay table) entry to 1.*/ 
       retVal = msec_port_set_nxtpn_ent (dev, port, ent_num, 0x1);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Next pn failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
       retVal = msec_port_get_nxtpn_entry (dev, port, ent_num, &data);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Next pn failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
	   else
	   {
	     if (data==1)
		 {
		   MSG_PRINT("Test access nextpn (replay table entry) successful\n");
		 }
	     else
		 {
		   MSG_PRINT("Test access nextpn (replay table entry) failed\n");
		 }
	   }

       /* program an entry for ILU table */
       retVal = msec_port_set_elu_entry (dev, port, ent_num, &test_lkup);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting entry for ELU table failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }
       retVal = msec_port_get_elu_entry (dev, port, ent_num, &check_lkup);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting entry for ELU table failed for (port %i, rntry number %i)\n",(int)port,ent_num);
       }

#if 1
{

printf("!!! check egress entry\n");
printf("lkup->test_match_en  - lkup->check_match_enl:\n");
printf(" en_def %x - %x \n",  test_match_en.en_def, check_match_en.en_def);
printf(" en_mc %x - %x \n",  test_match_en.en_mc, check_match_en.en_mc);
printf(" en_da %x - %x \n",  test_match_en.en_da, check_match_en.en_da);
printf(" en_sa %x - %x \n",  test_match_en.en_sa, check_match_en.en_sa);
printf(" en_vlan %x - %x \n",  test_match_en.en_vlan, check_match_en.en_vlan);
printf(" en_et %x - %x \n",  test_match_en.en_et, check_match_en.en_et);
printf(" en_scian %x - %x \n", test_match_en.en_scian, check_match_en.en_scian);

printf("\n lkup->test_match_fld:\n");
printf("lkup->test_match_fld  - lkup->check_match_fld:\n");
printf(" en_da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" en_sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf(" en_sa %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" en_vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" en_et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" en_sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf(" en_sci %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" en_tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" en_tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

printf("\n lkup->test_act_fld:\n");
printf("lkup->test_act_fld  - lkup->check_act_fld:\n");
printf(" ctx_num %x - %x \n", test_act_fld.ctx_num, check_act_fld.ctx_num);
printf(" drop %x - %x \n", test_act_fld.drop, check_act_fld.drop);
printf(" redir %x - %x \n", test_act_fld.redir, check_act_fld.redir);
printf(" auth_en %x - %x \n", test_act_fld.auth_en, check_act_fld.auth_en);
printf(" enc_en %x - %x \n", test_act_fld.enc_en, check_act_fld.enc_en);

}
#endif

	   if ((test_match_en.en_def!=check_match_en.en_def) ||
           (test_match_en.en_mc!=check_match_en.en_mc) ||
           (test_match_en.en_da!=check_match_en.en_da) ||
           (test_match_en.en_sa!=check_match_en.en_sa) || 
           (test_match_en.en_vlan!=check_match_en.en_vlan) ||
           (test_match_en.en_et!=check_match_en.en_et) ||
           (test_match_en.en_scian!=check_match_en.en_scian))
       {
          MSG_PRINT("Check Egress entry lk_match_en failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Egress entry lk_match_en successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_match_fld.da!=check_match_fld.da) || 
           (test_match_fld.sa!=check_match_fld.sa) || 
           (test_match_fld.vlan!=check_match_fld.vlan) ||
           (test_match_fld.et!=check_match_fld.et))
/* For ingress only 
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
*/
       {
          MSG_PRINT("Check Egress entry match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Egress entry match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);

	   if ((test_act_fld.drop!=check_act_fld.drop) ||
           (test_act_fld.redir!=check_act_fld.redir) || 
           (test_act_fld.auth_en!=check_act_fld.auth_en) || 
           (test_act_fld.enc_en!=check_act_fld.enc_en))
       {
          MSG_PRINT("Check Egress entry act_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Egress entry act_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);


    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}





/*********************************************************************************************
*
*              Test MAC Control
*
********************************************************************************************/
MAD_U32 testMacCtrl(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;  
    MAC_CTRL	test_mac_ctrl, check_mac_ctrl, save_mac_ctrl;
	int macType;

#define TEST_MAC_TYPE 1 /*/< Indicates System or Wire side MAC (0 -> SYS, 1 -> WRE) */

  retVal = msec_fixed (dev);

  test_mac_ctrl.mac_type = 0;     /*/< Indicates System or Wire side MAC (0 -> SYS, 1 -> WRE) */
  test_mac_ctrl.port_en  = 1;      /*/< Enable Port (0 -> Packet reception is disabled, 1 -> port is enabled) */
  test_mac_ctrl.speed    = 2;        /*/< Indcates port operation speed (2 -> 1000 Mbps, 1 -> 100 Mbps, 0 -> 10 Mbps) */
  test_mac_ctrl.hfd      = 1;          /*/< Indcates Full-duplex enable ( 0 -> Half-Duplex, 1 -> Full-Duplex) */
  test_mac_ctrl.mibcnt_en= 1;    /*/< Enable MIB counters update for this port */
  test_mac_ctrl.lpbk_en  = 1;      /*/< Enable Port in loopback mode */
  test_mac_ctrl.max_frame_sz=0x777; /*/< Maximum Receive Packet Size - MRU (Default value is 0x5F2) */

  
  
  for(port=0; port<dev->numOfPorts; port++)
  {
	for (macType=0; macType<2; macType++)
    {
       test_mac_ctrl.mac_type = macType;     
       save_mac_ctrl.mac_type = macType;    
       check_mac_ctrl.mac_type = macType;    

		/* save original Mac Control */
       retVal = msec_port_get_mac_ctrl (dev, port, &save_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Mac control failed for (port %i)\n",(int)port);
          return retVal;             
       }
       retVal = msec_port_get_mac_speed (dev, port, &save_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Mac control speed failed for (port %i)\n",(int)port);
          return retVal;             
       }
    
		/* set test Mac Control */
       /* set Mac disable, MIB counter en, loopback, MRU */
       retVal = msec_port_set_mac_ctrl (dev, port, &test_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Mac control failed for (port %i)\n",(int)port);
          return retVal;             
       }
       /* if mac is not in reset, assert Mac in reset. If Mac is not disable, disable it. */
       /* set speed and duplex. Bring Mac out of reset and enable Mac */
       retVal = msec_port_set_mac_speed (dev, port, &test_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Mac control speed failed for (port %i)\n",(int)port);
          return retVal;             
       }
		/* get test Mac Control */
       retVal = msec_port_get_mac_ctrl (dev, port, &check_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Mac control failed for (port %i)\n",(int)port);
          return retVal;             
       }
       retVal = msec_port_get_mac_speed (dev, port, &check_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Mac control speed failed for (port %i)\n",(int)port);
          return retVal;             
       }

	   /* recover original Mac Control */
       retVal = msec_port_set_mac_ctrl (dev, port, &save_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Mac control failed for (port %i)\n",(int)port);
          return retVal;             
       }
       retVal = msec_port_set_mac_speed (dev, port, &save_mac_ctrl);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Mac control speed failed for (port %i)\n",(int)port);
          return retVal;             
       }
#if 1
{

printf("!!! check Mac Control\n");
printf("test_mac_ctrl  - check_mac_ctrl\n");
printf(" mac_type %x - %x \n",  test_mac_ctrl.mac_type, check_mac_ctrl.mac_type);
printf(" port_en %x - %x \n",  test_mac_ctrl.port_en, check_mac_ctrl.port_en);
printf(" speed %x - %x \n",  test_mac_ctrl.speed, check_mac_ctrl.speed);
printf(" hfd %x - %x \n",  test_mac_ctrl.hfd, check_mac_ctrl.hfd);
printf(" mibcnt_en %x - %x \n",  test_mac_ctrl.mibcnt_en, check_mac_ctrl.mibcnt_en);
printf(" lpbk_en %x - %x \n",  test_mac_ctrl.lpbk_en, check_mac_ctrl.lpbk_en);
printf(" max_frame_sz %x - %x \n",  test_mac_ctrl.max_frame_sz, check_mac_ctrl.max_frame_sz);

}
#endif

 		/* check test Mac Control */
	   if ((test_mac_ctrl.mac_type!=check_mac_ctrl.mac_type) ||
           (test_mac_ctrl.port_en!=check_mac_ctrl.port_en) ||
           (test_mac_ctrl.speed!=check_mac_ctrl.speed) ||
           (test_mac_ctrl.hfd!=check_mac_ctrl.hfd) || 
           (test_mac_ctrl.mibcnt_en!=check_mac_ctrl.mibcnt_en) ||
           (test_mac_ctrl.lpbk_en!=check_mac_ctrl.lpbk_en) ||
           (test_mac_ctrl.max_frame_sz!=check_mac_ctrl.max_frame_sz))
       {
          MSG_PRINT("Check Mac Control failed for (port %i)\n",(int)port);
          return 0;             
       }
       MSG_PRINT("Check Mac Control successful for (port %i type %x)\n",(int)port, macType);
	}
  }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}


/*********************************************************************************************
*
*              Test E/Ilu match field
*
********************************************************************************************/
MAD_U32 testEluMatchFld(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;  
    int         nCases;
	 int			ent_num;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;

  retVal = msec_fixed (dev);


 test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  MSG_PRINT("Testing set Egress Entry ...\n");

  nCases = 3;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(ent_num=0; ent_num<nCases; ent_num++)
    { 
		/* save original Elu match field */
       retVal = msec_port_get_elu_match_fld (dev, port, ent_num, &save_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Elu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* set test Elu match field */
       retVal = msec_port_set_elu_match_fld (dev, port, ent_num, &test_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Elu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* get test Elu match field */
       retVal = msec_port_get_elu_match_fld (dev, port, ent_num,  &check_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Elu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

		/* recover original Elu match field */
       retVal = msec_port_set_elu_match_fld (dev, port, ent_num, &save_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Elu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }


#if 0
{

printf("!!! check Elu match field\n");
printf("\n test_match_fld:\n");
printf("test_match_fld  - check_match_fld:\n");
printf(" en_da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" en_sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf(" en_sa %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" en_vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" en_et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" en_sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf(" en_sci %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" en_tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" en_tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

}
#endif


      if ((test_match_fld.da!=check_match_fld.da) || 
           (test_match_fld.sa!=check_match_fld.sa) || 
           (test_match_fld.vlan!=check_match_fld.vlan) ||
           (test_match_fld.et!=check_match_fld.et))
/* For Ingress only 
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
*/
       {
          MSG_PRINT("Check Elu match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Elu match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);
      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testIluMatchFld(MAD_DEV *dev)
{
    int  retVal;
    MAD_LPORT   port;  
    int         nCases;
	 int			ent_num;
  MATCH_FLD test_match_fld, check_match_fld, save_match_fld;

  retVal = msec_fixed (dev);


 test_match_fld.da=0x010203040506L;      /*/< MAC DA */
  test_match_fld.sa=0x3a3b3c3d3e3fL;      /*/< MAC SA */
  test_match_fld.vlan=0x0555;    /*/< VLAN */
  test_match_fld.et=0xaaaa;      /*/< Ethertype */
  test_match_fld.sci=0x2233445566778899L;     /*/< SCI (Valid only for Ingress path */
  test_match_fld.tci=0x55;     /*/< TCI (Valid only for Ingress path) */
  test_match_fld.tci_msk=0xaa; /*/< TCI Mask (Valid only for Ingress path) */

  MSG_PRINT("Testing set Egress Entry ...\n");

  nCases = 3;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(ent_num=0; ent_num<nCases; ent_num++)
    { 
		/* save original Ilu match field */
       retVal = msec_port_get_ilu_match_fld (dev, port, ent_num, &save_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Ilu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* set test Ilu match field */
       retVal = msec_port_set_ilu_match_fld (dev, port, ent_num, &test_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Ilu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* get test Ilu match field */
       retVal = msec_port_get_ilu_match_fld (dev, port, ent_num,  &check_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Ilu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

		/* recover original Ilu match field */
       retVal = msec_port_set_ilu_match_fld (dev, port, ent_num, &save_match_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Ilu match field failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }


#if 0
{

printf("!!! check Ilu match field\n");
printf("\n test_match_fld:\n");
printf("test_match_fld  - check_match_fld:\n");
printf(" en_da %08llx %08llx  - ", test_match_fld.da, test_match_fld.da>>32);
printf(" %08llx %08llx \n", check_match_fld.da, check_match_fld.da>>32);

printf(" en_sa %08llx - %08llx -", test_match_fld.sa, test_match_fld.sa>>32);
printf(" en_sa %08llx %08llx \n", check_match_fld.sa, check_match_fld.sa>>32);

printf(" en_vlan %x - %x \n", test_match_fld.vlan, check_match_fld.vlan);
printf(" en_et %x - %x \n", test_match_fld.et, check_match_fld.et);
printf(" en_sci %08llx - %08llx -", test_match_fld.sci, test_match_fld.sci>>32); 
printf(" en_sci %08llx %08llx \n", check_match_fld.sci, check_match_fld.sci>>32); 

printf(" en_tci %x - %x \n", test_match_fld.tci, check_match_fld.tci);
printf(" en_tci_msk %x - %x \n", test_match_fld.tci_msk, check_match_fld.tci_msk);

}
#endif


      if ((test_match_fld.da!=check_match_fld.da) || 
           (test_match_fld.sa!=check_match_fld.sa) || 
           (test_match_fld.vlan!=check_match_fld.vlan) ||
           (test_match_fld.et!=check_match_fld.et) ||
           (test_match_fld.sci!=check_match_fld.sci) || 
           (test_match_fld.tci!=check_match_fld.tci) ||
           (test_match_fld.tci_msk!=check_match_fld.tci_msk))
       {
          MSG_PRINT("Check Ilu match_fld failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Ilu match_fld successful for (port %i, rntry number %i)\n",(int)port,ent_num);
      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

#if 0
/*********************************************************************************************
*
*              Test Redirect Header
*
********************************************************************************************/

MAD_STATUS msec_set_redir_hdr 
(
    IN  MAD_DEV		*dev,
	IN	REDIR_HDR_T *redir_fld
)

MAD_U32 testReditHdr(MAD_DEV *dev)
{
    int  retVal;
	REDIR_HDR_T test_redir_fld, check_redir_fld, save_redir_fld;

  retVal = msec_fixed (dev);

  MAD_U64 da;     
  MAD_U16 et;     
  MAD_U64 sa [4]; 

  test_redir_fld.da=0x010203040506L;      /*/< Redirect DA */
  test_redir_fld.sa[0]=0x1a1b3c1d3e1fL;   /*/< Redirect SA for Port 0 */
  test_redir_fld.sa[1]=0x2a2b2c2d3e2fL;   /*/< Redirect SA for Port 1 */
  test_redir_fld.sa[2]=0x3a3b3c3d3e3fL;   /*/< Redirect SA for Port 2 */
  test_redir_fld.sa[3]=0x4a4b3c4d3e4fL;   /*/< Redirect SA for Port 3 */
  test_redir_fld.et=0x5555;      /*/< Redirect Ether Type */

  MSG_PRINT("Testing set Redirect Header ...\n");

		/* save original Redirect Header */
       retVal = msec_get_redir_hdr (dev, &save_redir_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Redirect Header failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* set test Redirect Header */
       retVal = msec_set_redir_hdr (dev,  &test_redir_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Redirect Header failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }
    
		/* get test Redirect Header */
       retVal = msec_get_redir_hdr (dev,  &check_redir_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Getting Redirect Header failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }

		/* recover original Redirect Header */
       retVal = msec_set_redir_hdr (dev, &save_redir_fld);
       if (retVal!=MAD_OK)
       {
          MSG_PRINT("Setting Redirect Header failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return retVal;             
       }


#if 0
{

printf("!!! check Redirect Header\n");
printf("\n test_redir_fld:\n");
printf("test_redir_fld  - check_redir_fld:\n");
printf(" da %08llx %08llx  - ", test_redir_fld.da, test_redir_fld.da>>32);
printf(" %08llx %08llx \n", check_redir_fld.da, check_redir_fld.da>>32);

printf(" sa %08llx - %08llx -", test_redir_fld.sa, test_redir_fld.sa>>32);
printf(" sa %08llx %08llx \n", check_redir_fld.sa, check_redir_fld.sa>>32);

printf(" et %x - %x \n", test_match_fld.et, check_match_fld.et);
}
#endif


      if ((test_redir_fld.da!=check_redir_fld.da) || 
           (test_redir_fld.sa!=check_redir_fld.sa) || 
           (test_redir_fld.et!=check_redir_fld.et))
       {
          MSG_PRINT("Check Redirect Header failed for (port %i, rntry number %i)\n",(int)port,ent_num);
          return 0;             
       }
       MSG_PRINT("Check Redirect Header successful for (port %i, rntry number %i)\n",(int)port,ent_num);

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

#endif
