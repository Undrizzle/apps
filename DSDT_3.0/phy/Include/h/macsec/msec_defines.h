#include <madCopyright.h>

/********************************************************************************
* msec_defines.h
*
* DESCRIPTION:
*       Definitions for MacSec control functions's
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#ifndef __msec_defines_h
#define __msec_defines_h
#ifdef __cplusplus
extern "C" {
#endif

/*#define REV_A0_UP 1 */
/*#define PROD_1540L 1 */
/*! \brief Per Port MAC Control Variables
 */
typedef struct {
  int mac_type;     /*/< Indicates System or Wire side MAC (0 -> SYS, 1 -> WRE) */
  int port_en;      /*/< Enable Port (0 -> Packet reception is disabled, 1 -> port is enabled) */
  int speed;        /*/< Indcates port operation speed (2 -> 1000 Mbps, 1 -> 100 Mbps, 0 -> 10 Mbps) */
  int hfd;          /*/< Indcates Full-duplex enable ( 0 -> Half-Duplex, 1 -> Full-Duplex) */
  int mibcnt_en;    /*/< Enable MIB counters update for this port */
  int lpbk_en;      /*/< Enable Port in loopback mode */
  int max_frame_sz; /*/< Maximum Receive Packet Size - MRU (Default value is 0x5F2) */
  int tx_fc_en;
  int rx_fc_en;
  int aneg_fc_en;
  int aneg_en;
} MAC_CTRL;

/*! \brief Per Port Match Enable Variables
 */
typedef struct {
  int en_def;   /*/< Enable default match (search will always hit this entry if no previous entry matched) */
  int en_mc;    /*/< Enable Muticast match */
  int en_da;    /*/< Enable MAC DA match */
  int en_sa;    /*/< Enable MAC SA match */
  int en_vlan;  /*/< Enable VLAN match */
  int en_et;    /*/< Enable Ethertype match */
  int en_scian; /*/< Enable SCI_AN match (Valid only for Ingress path) */
} MATCH_EN;

/*! \brief Per Port Match Fields
 */
typedef struct {
  MAD_U64 da;      /*/< MAC DA */
  MAD_U64 sa;      /*/< MAC SA */
  MAD_U16 vlan;    /*/< VLAN */
  MAD_U16 et;      /*/< Ethertype */
  MAD_U64 sci;     /*/< SCI (Valid only for Ingress path */
  MAD_U8  tci;     /*/< TCI (Valid only for Ingress path) */
  MAD_U8  tci_msk; /*/< TCI Mask (Valid only for Ingress path) */
  MAD_U8  offset_conf; /*/< Confidential offset (Valid only for Ingress path) */
} MATCH_FLD;

/*! \brief Variables to set action for an entry within a port
 */
typedef struct {
  int ctx_num; /*/< Index to the context to use */
  int sec_level; /* < Security Level to validate frames per context based */
  int drop_maxpn; /* < Enable packet drop when max pn is reached for this context>*/
  int drop;    /*/< Drop this Packet */
  int redir;   /*/< For Egress, redirect the packet to ingress path (NDL). For Ingress, redirect the packet to alternate destination. */
  int auth_en; /*/< Encapsulate and authenticate this packet. */
  int enc_en;  /*/< Encrypt this packet. auth_en must also be set when this bit is set. (Valid only for egress path). */
} ACT_FLD;

/*! \brief Structure to set lookup entry table within a port
 */
typedef struct {
  MATCH_EN  *lk_match_en;  /*/< Per Port Match Enables */
  MATCH_FLD *lk_match_fld; /*/< Per Port Match Fields */
  ACT_FLD   *lk_act_fld;   /*/< Action to take for an entry within a port */
} LKUP_T;

/*! \brief Structure to set Context entry table within a port
 *
 * Sets the variables required to program Egress Context table, Egress/Ingress Key and Hash key tables
 *
 */
typedef struct {
  MAD_U64 sci;       /*/< SCI (Valid only for Egress path) */
  MAD_U32 pn;        /*/< Initial Packet Number (Valid only for Egress path) */
  MAD_U8  tci;       /*/< TCI (Valid only for Egress path) */
  MAD_U8  offset_conf; /*/<Offset confidentiality on egress path, per 1540*/
  MAD_U8  key  [16]; /*/< 128 bit Encrytion/Decryption Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  MAD_U8  hkey [16]; /*/< 128 bit Encrytion/Decryption Hash Key (hkey[15] -> hkey[7:0], hkey[0] -> hkey[127:120]) */
} CTX_T;

/*! \brief Structure to set Redirect Header
 */
typedef struct {
  MAD_U64 da;     /*/< Redirect DA */
  MAD_U16 et;     /*/< Redirect Ether Type */
  MAD_U64 sa [4]; /*/< Redirect SA for Port 0,1,2 and 3 */
} REDIR_HDR_T;

/*! \brief Structure to set all the MACSec Common Configuration variables
 */
typedef struct {
  MAD_U8  trail_sz;    /*/< Trail Size must be program to 0 	*/
  MAD_U8  conf_offset; /*/< Amount of additional data to send auth-only to the receiver a.k.a Auth_adjust */
  MAD_U16 def_vlan;   /*/< Default VLAN used for lookups on packets which do not arrive with an 802.1Q tag */
  MAD_U64 mc_high;    /*/< Multicast match register [47:0], Lower 4 bits should be ignored */
  MAD_U16 mc_low_msk; /*/< Muticast match mask register */
} CMN_CFG_T;

/*! \brief Structure to set per port Egress Configuration
 */
typedef struct {
  int def_drop;    /*/< If set, default action for packets missing ELU table is drop. */
  int parse_ae;    /*/< If set, parser will look for packets with sectag and replace it with sectag chosen */
  int parse_vlan;  /*/< If set, enable ingress vlan parsing */
} EGR_CFG_T;

/*! \brief Structure to set per port Ingress default actions
 */
typedef struct {
  int def_drop;    /*/< If set, default action for packets missing ILU table is drop. */
  int def_redir;   /*/< If set, default action for packets missing ILU table is redirect. */
  int drop_badtag; /*/< If set, packets whioch are classified as bad_tag are dropped. */
  int retain_hdr;  /*/< If set, sectag is not removed from the packet after decryption. */
  int parse_vlan;  /*/< If set, enable ingress vlan parsing */
  int badtag_chk;  /* < 5 bit value. All bits 1, bad_tag check enable. */
} IGR_DEF_T;

/*! \brief Structure to set per port Redirect vector
 */
typedef struct {
  int redir_untagged;  /*/< If set, redirect packets due to untagged or no_tag classification. */
  int redir_badtag;    /*/< If set, redirect packets due to bad_tag classification. */
  int redir_unk_sci;   /*/< If set, redirect packets due to unknown_sci or no_sci classification. */
  int redir_unused_sa; /*/< If set, redirect packets due to unused_sa or not_using_sa classification. */
  int redir_unchecked; /*/< If set, redirect packets due to unchecked classification. */
  int redir_delayed;   /*/< If set, redirect packets due to delayed or late classification. */
  int redir_invalid;   /*/< If set, redirect packets due to invalid or not_valid classification. */
} REDIR_T;

/*! \brief Structure to set per port SCI parameters for Final SCI calculation on Ingress path
 */
typedef struct {
  MAD_U64 implicit_sci; /*/< Implicit SCI */
  MAD_U16 scb_port;     /*/< SCB Port */
  MAD_U16 def_port;     /*/< Default Port */
} SCI_T;

/*! \brief Structure to set per port Replay parameters on Ingress path
 */
typedef struct {
  int  validate_frames; /*/< Values for validate_frames are disabled=1, check=2, strict=3. */
  int  replay_protect;  /*/< Enables replay checking */
  int  replay_drop;     /*/< If set, enables dropping of packets which fail replay protection.*/
  int  replay_window;   /*/< Replay window register. */
} RPY_T;

/*! \brief Structure to set per port all Ingress Configuration
 */
typedef struct {
  IGR_DEF_T *def_act;   /*/< Ingress Default Action */
  REDIR_T   *redir_vec; /*/< Redirect vector */
  SCI_T     *sci_info;  /*/< SCI parameters for Final SCI calculation on Ingress path */
  RPY_T     *rpy_var;   /*/< Replay parameters on Ingress path */
} IGR_CFG_T;

/*! \brief Structure to set per port ETM Configuration
 */
typedef struct {
  MAD_U16 etm [8];    /*/< 0-7 Ethertype Match register */
  MAD_U8  etm_eg_en;  /*/< Ethertype Match Egress enable bit field */
  MAD_U8  etm_ig_en;  /*/< Ethertype Match Ingress enable bit field */
  MAD_U8  etm_eg_act; /*/< Ethertype Match Egress Action  (0 = Bypass, 1 = Drop) */
  MAD_U8  etm_ig_act; /*/< Ethertype Match Ingress Action (0 = Bypass, 1 = Drop, 2 = Redirect) */
} ETM_CFG_T;

/*! \brief Structure to set per port FIPS Configuration
 */
typedef struct {
  MAD_U8 fips_vec [16]; /*/< 128 bit FIPS vector Injection (vec[15] -> vec[7:0], vec[0] -> vec[127:120]) */
  MAD_U8 key      [16]; /*/< 128 bit Encrytion/Decryption Key or Hash Key (key[15] -> key[7:0], key[0] -> key[127:120]) */
  MAD_U8 egr_igr;       /*/< If 1, Egress port, else ingress */
  MAD_U8 eng_type;      /*/< If 1, AES engine, else GHASH engine */
} FIPS_CFG_T;

/*! \brief Structure to accumulate all the MACSec Statistics in 64bit vector
 */
typedef struct {
  MAD_U64 macsec_vec [100]; /*/< 64 bit Vector array to store all the MACSec Statistics. */
} MACSEC_STAT_T;

/*! \brief Structure to accumulate all the MIBs Statistics in 64bit vector
 */
typedef struct {
  MAD_U64 mib_vec [32]; /*/< 64 bit Vector array to store all the MIBs Statistics. */
} MIB_STAT_T; 

/*! \defgroup RegDefines Maxwell Page 11 (LinkCrypt) Top Level memory map
 *@{
 */  
#define GET_REV_DATA(_dev, a, b, c)    \
        (((_dev)->deviceId==MAD_88E1540)?a:    \
         ((_dev)->msec_ctrl.msec_rev==MAD_MSEC_REV_A0B)?b:c)  /* */  


/* Top Level Memory Map */
/* #if defined (REV_A0_UP) || defined(PROD_1540L) */
  #define VN_SLICE0(_dev)    GET_REV_DATA(_dev, 0, 0, 0) /*/< Port 0 registers/memory */
  #define VN_SLICE1(_dev)    GET_REV_DATA(_dev, 0x800, 0x800, 0x400) /*/< Port 1 registers/memory */
  #define VN_SLICE2(_dev)    GET_REV_DATA(_dev, 0x1000, 0x1000, 0x800)  /*/< Port 2 registers/memory */
  #define VN_SLICE3(_dev)    GET_REV_DATA(_dev, 0x1800, 0x1800, 0xc00) /*/< Port 3 registers/memory */
  #define VN_MAC_STATS(_dev)    GET_REV_DATA(_dev, 0x2000, 0x2000, 0x1000) /*/< MAC Statistics */
  #define VN_GLOBAL(_dev)    GET_REV_DATA(_dev, 0x2100, 0x2100, 0x1100) /*/< Global registers */
  #define VN_PET_STATS(_dev) (((_dev)->deviceId==MAD_88E1540)?0x2200:0)  /*/<Per EtherType statistics */
  #define VN_SEC_STATS(_dev)    GET_REV_DATA(_dev, 0x2800, 0x2400, 0x1200) /*/< MACsec Statistics  */
/*@}*/

/*! \defgroup RegDefines1 Per-Port Memory Map
 *@{
 */
/* Per-Port Memory Map */
  #define VN_CFG_RUMI_REGS(_dev)       GET_REV_DATA(_dev, 0, 0, 0) /*/< LinkCrypt General Registers */
  #define VN_CFG_EGR_KEY(_dev)      GET_REV_DATA(_dev, 0x400, 0x400, 0x040) /*/< Encrypt Keys Table */
  #define VN_CFG_EGR_HKEY(_dev)      GET_REV_DATA(_dev, 0x480, 0x480, 0x060) /*/< Egress Hash Keys Table */
  #define VN_CFG_ELU(_dev)      GET_REV_DATA(_dev, 0x100, 0x100, 0x080) /*/< Egress Look up (ELU) Table */
  #define VN_CFG_ILU(_dev)      GET_REV_DATA(_dev, 0x200, 0x200, 0x100) /*/< Ingress Look up (ILU) Table */
  #define VN_CFG_IGR_KEY(_dev)      GET_REV_DATA(_dev, 0x500, 0x500, 0x140)  /*/< Decrypt Keys Table */
  #define VN_CFG_IGR_HKEY(_dev)      GET_REV_DATA(_dev, 0x580, 0x580, 0x160)  /*/< Ingress Hash Keys Table */
  #define VN_CFG_RPY(_dev)      GET_REV_DATA(_dev, 0x80, 0x80, 0x180)  /*/< Ingress Replay Value Tables */
  #define VN_CFG_WIR_MAC(_dev)      GET_REV_DATA(_dev, 0x40, 0x40, 0x1c0)  /*/< Wire Side Mac Configuration */
  #define VN_CFG_SYS_MAC(_dev)      GET_REV_DATA(_dev, 0x50, 0x50, 0x1e0) /*/< System Side Mac Configuration */
  #define VN_CFG_SLC(_dev)      GET_REV_DATA(_dev, 0x70, 0x70, 0x1d0)  /*/< Slice Control Registers */
  #define VN_CFG_ELU_CTX(_dev)      GET_REV_DATA(_dev, 0x300, 0x300, 0x200)  /*/< Egress Context (ECT) Table */
/*@}*/

/*! \defgroup RegDefines2 LinkCrypt General Register Defines
 *@{
 */
/* LinkCrypt General Register Defines */
#define ETHERTYPES(_dev)             VN_CFG_RUMI_REGS(_dev)+0x0
#define CFG_SIZES(_dev)              VN_CFG_RUMI_REGS(_dev)+0x1
#define CFG_DEFAULT_VLAN(_dev)       VN_CFG_RUMI_REGS(_dev)+0x2
#define RESET_CTL(_dev)              VN_CFG_RUMI_REGS(_dev)+0x3
#define CFG_MC_HIGH0(_dev)           VN_CFG_RUMI_REGS(_dev)+0x4
#define CFG_MC_HIGH1(_dev)           VN_CFG_RUMI_REGS(_dev)+0x5
#define CFG_MC_LOW_MSK(_dev)         VN_CFG_RUMI_REGS(_dev)+0x6
#define PORT_NUMBERS(_dev)           VN_CFG_RUMI_REGS(_dev)+0x7
#define RM_RESERVED6(_dev)          VN_CFG_RUMI_REGS(_dev)+0x8
#define CFG_EGR_GEN(_dev)          VN_CFG_RUMI_REGS(_dev)+0x9
#define RM_RESERVED0(_dev)          VN_CFG_RUMI_REGS(_dev)+0xa
#define CFG_IGR_GEN(_dev)          VN_CFG_RUMI_REGS(_dev)+0xb
#define CFG_IMPLICIT_SCI0(_dev)         VN_CFG_RUMI_REGS(_dev)+0xc
#define CFG_IMPLICIT_SCI1(_dev)         VN_CFG_RUMI_REGS(_dev)+0xd
#define CFG_REPLAY_WINDOW(_dev)         VN_CFG_RUMI_REGS(_dev)+0xe
#define RM_RESERVED3(_dev)          VN_CFG_RUMI_REGS(_dev)+0xf
#define CFG_ISC_GEN(_dev)          VN_CFG_RUMI_REGS(_dev)+0x10
#define CNT_TX_ERR(_dev)          VN_CFG_RUMI_REGS(_dev)+0x11
#define CNT_RX_ERR(_dev)          VN_CFG_RUMI_REGS(_dev)+0x12
#define RM_RESERVED1(_dev)          VN_CFG_RUMI_REGS(_dev)+0x13
#define RM_RESERVED2(_dev)          VN_CFG_RUMI_REGS(_dev)+0x14
#define CFG_HDR_DATA0(_dev)         VN_CFG_RUMI_REGS(_dev)+0x15
#define CFG_HDR_DATA1(_dev)         VN_CFG_RUMI_REGS(_dev)+0x16
#define CFG_HDR_DATA2(_dev)         VN_CFG_RUMI_REGS(_dev)+0x17
#define CFG_HDR_DATA3(_dev)         VN_CFG_RUMI_REGS(_dev)+0x18
#define RM_RESERVED7(_dev)          VN_CFG_RUMI_REGS(_dev)+0x19
#define PROP_HDR_CTL(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1a
#define RM_RESERVED5(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1b
#define LC_INTR(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1c
#define LC_INTR_MSK(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1d
#define FIPS_CTL(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1e
#define ET_MATCH_CTL(_dev)          VN_CFG_RUMI_REGS(_dev)+0x1f
#define FIPS_EGR_AES0(_dev)         VN_CFG_RUMI_REGS(_dev)+0x20
#define FIPS_EGR_AES1(_dev)         VN_CFG_RUMI_REGS(_dev)+0x21
#define FIPS_EGR_AES2(_dev)         VN_CFG_RUMI_REGS(_dev)+0x22
#define FIPS_EGR_AES3(_dev)         VN_CFG_RUMI_REGS(_dev)+0x23
#define FIPS_EGR_GHASH0(_dev)         VN_CFG_RUMI_REGS(_dev)+0x24
#define FIPS_EGR_GHASH1(_dev)         VN_CFG_RUMI_REGS(_dev)+0x25
#define FIPS_EGR_GHASH2(_dev)         VN_CFG_RUMI_REGS(_dev)+0x26
#define FIPS_EGR_GHASH3(_dev)         VN_CFG_RUMI_REGS(_dev)+0x27
#define FIPS_IGR_AES0(_dev)         VN_CFG_RUMI_REGS(_dev)+0x28
#define FIPS_IGR_AES1(_dev)         VN_CFG_RUMI_REGS(_dev)+0x29
#define FIPS_IGR_AES2(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2a
#define FIPS_IGR_AES3(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2b
#define FIPS_IGR_GHASH0(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2c
#define FIPS_IGR_GHASH1(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2d
#define FIPS_IGR_GHASH2(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2e
#define FIPS_IGR_GHASH3(_dev)         VN_CFG_RUMI_REGS(_dev)+0x2f
#define ET_M0(_dev)          VN_CFG_RUMI_REGS(_dev)+0x30
#define ET_M1(_dev)          VN_CFG_RUMI_REGS(_dev)+0x31
#define ET_M2(_dev)          VN_CFG_RUMI_REGS(_dev)+0x32
#define ET_M3(_dev)          VN_CFG_RUMI_REGS(_dev)+0x33
/*@}*/

/*! \defgroup RegDefines3 System MAC Configuration Defines
 *@{
 */
/*  System MAC Configuration Defines */
#define SYS_MAC_CONTROL0(_dev)         VN_CFG_SYS_MAC(_dev)+0x0
#define SYS_MAC_CONTROL1(_dev)         VN_CFG_SYS_MAC(_dev)+0x1
#define SYS_MAC_CONTROL2(_dev)         VN_CFG_SYS_MAC(_dev)+0x2
#define SYS_AUTO_NEG_CONTROL(_dev)         VN_CFG_SYS_MAC(_dev)+0x3
#define SYS_MAC_STATUS0(_dev)         VN_CFG_SYS_MAC(_dev)+0x4
#define SYS_MAC_INTR(_dev)          VN_CFG_SYS_MAC(_dev)+0x5
#define SYS_MAC_INTR_MSK(_dev)         VN_CFG_SYS_MAC(_dev)+0x6
#define SYS_HIDDEN0(_dev)          VN_CFG_SYS_MAC(_dev)+0x7
#define SYS_HIDDEN1(_dev)          VN_CFG_SYS_MAC(_dev)+0x8
/*@}*/

/*! \defgroup RegDefines4 Wire MAC Configuration Defines
 *@{
 */
/*  Wire MAC Configuration Defines */
#define WIR_MAC_CONTROL0(_dev)         VN_CFG_WIR_MAC(_dev)+0x0
#define WIR_MAC_CONTROL1(_dev)         VN_CFG_WIR_MAC(_dev)+0x1
#define WIR_MAC_CONTROL2(_dev)         VN_CFG_WIR_MAC(_dev)+0x2
#define WIR_AUTO_NEG_CONTROL(_dev)   VN_CFG_WIR_MAC(_dev)+0x3
#define WIR_MAC_STATUS0(_dev)         VN_CFG_WIR_MAC(_dev)+0x4
#define WIR_MAC_INTR(_dev)          VN_CFG_WIR_MAC(_dev)+0x5
#define WIR_MAC_INTR_MSK(_dev)         VN_CFG_WIR_MAC(_dev)+0x6
#define WIR_HIDDEN0(_dev)          VN_CFG_WIR_MAC(_dev)+0x7
#define WIR_HIDDEN1(_dev)          VN_CFG_WIR_MAC(_dev)+0x8
/*@}*/

/*! \defgroup RegDefines5 Slice Control register Defines
 *@{
 */
/* Slice Control register Defines */
#define SLC_CFG_GEN (_dev)          VN_CFG_SLC(_dev)+0x0
#define PORT_MAC_SA0(_dev)          VN_CFG_SLC(_dev)+0x1
#define PORT_MAC_SA1(_dev)          VN_CFG_SLC(_dev)+0x2
#define RSVD1(_dev)          VN_CFG_SLC(_dev)+0x3
#define PAUSE_CTL(_dev)          VN_CFG_SLC(_dev)+0x4
#define SLC_RAM_CTRL0(_dev)         VN_CFG_SLC(_dev)+0x5
#define SLC_RAM_CTRL1(_dev)         VN_CFG_SLC(_dev)+0x6
#define WMAC_MUX_DROP_CNT(_dev)      VN_CFG_SLC(_dev)+0x7
#define SMAC_MUX_DROP_CNT(_dev)      VN_CFG_SLC(_dev)+0x8
#define D_BUF_THRESH(_dev)          VN_CFG_SLC(_dev)+0x9
#define SLC_BIST_RESULT(_dev)         VN_CFG_SLC(_dev)+0xa
#define FIFO_BACKDOOR_CFG(_dev)      VN_CFG_SLC(_dev)+0xb
#define EGR_BD_DATA(_dev)          VN_CFG_SLC(_dev)+0xc
#define IGR_BD_DATA(_dev)          VN_CFG_SLC(_dev)+0xd
#define MIN_PAD_LEN(_dev)          VN_CFG_SLC(_dev)+0xe
#define OTHER_BUF_USAGE(_dev)         VN_CFG_SLC(_dev)+0xf
/*@}*/

/*! \defgroup RegDefines6 Global register Defines
 *@{
 */
/* Global register Defines */
#define RESERVED0(_dev)              VN_GLOBAL(_dev)+0x00
#define RESERVED1(_dev)              VN_GLOBAL(_dev)+0x01
#define ENCAP_DA0(_dev)              VN_GLOBAL(_dev)+0x02
#define ENCAP_DA_ET(_dev)              VN_GLOBAL(_dev)+0x03
#define IMB_CTL(_dev)              VN_GLOBAL(_dev)+0x04
#define IMB_DATA(_dev)              VN_GLOBAL(_dev)+0x05
#define FIPS_VEC0(_dev)              VN_GLOBAL(_dev)+0x06
#define FIPS_VEC1(_dev)              VN_GLOBAL(_dev)+0x07
#define FIPS_VEC2(_dev)              VN_GLOBAL(_dev)+0x08
#define FIPS_VEC3(_dev)              VN_GLOBAL(_dev)+0x09
#define STAT_GLOBAL(_dev)              VN_GLOBAL(_dev)+0x0a
#define INTR_SOURCE(_dev)              VN_GLOBAL(_dev)+0x0b
#define GLOBAL_INTR(_dev)              VN_GLOBAL(_dev)+0x0c
#define GLOBAL_INTR_MSK(_dev)              VN_GLOBAL(_dev)+0x0d
#define SVEC_CFG_GLOBAL(_dev)              VN_GLOBAL(_dev)+0x0e
#define SVEC_CFG_ADD(_dev)              VN_GLOBAL(_dev)+0x0f
#define MIB_PORT_TO_COPY(_dev)              VN_GLOBAL(_dev)+0x10
#define POWER_CTL(_dev)              VN_GLOBAL(_dev)+0x11
#define MIB_STATUS(_dev)              VN_GLOBAL(_dev)+0x12
/*@}*/

/*! \defgroup RegDefines8 MACSec Statistics defines per port
 *@{
 */
/* defined MACSec Statistics */
  #define MX_SEC_IGR_HIT(_dev)    GET_REV_DATA(_dev, 0, 0, 0) /*/< Ingress Packets which hit this entry (Per-CTX) */
  #define MX_SEC_IGR_OK(_dev)    GET_REV_DATA(_dev, 0x20, 0x10, 0x08) /*/< Packets decrypted/authenticated ok (Per-CTX) */
  #define MX_SEC_IGR_UNCHK(_dev)    GET_REV_DATA(_dev, 0x40, 0x20, 0x10) /*/< ICV not checked, but passed replay or replay was disable (Per-CTX) */
  #define MX_SEC_IGR_DELAY(_dev)    GET_REV_DATA(_dev, 0x60, 0x30, 0x18) /*/< Failed replay check (Per-CTX) */
  #define MX_SEC_IGR_LATE(_dev)    GET_REV_DATA(_dev, 0x80, 0x40, 0x20) /*/< Failed replay check, packet dropped (Per-CTX) */
  #define MX_SEC_IGR_INVLD(_dev)    GET_REV_DATA(_dev, 0xa0, 0x50, 0x28) /*/< Packet failed ICV check and dropped (Per-CTX) */
  #define MX_SEC_IGR_NOTVLD(_dev)    GET_REV_DATA(_dev, 0xc0, 0x60, 0x30) /*/< Packet failed ICV check (Per-CTX) */
  #define MX_SEC_EGR_PKT_PROT(_dev)    GET_REV_DATA(_dev, 0xe0, 0x70, 0x38) /*/< Egress packets sent auth-only (Per-CTX) */
  #define MX_SEC_EGR_PKT_ENC(_dev)    GET_REV_DATA(_dev, 0x100, 0x80, 0x40) /*/< Egress packets sent encrypted (Per-CTX) */
  #define MX_SEC_EGR_HIT(_dev)    GET_REV_DATA(_dev, 0x120, 0x90, 0x48) /*/< Egress Packets which hit this entry (Per-CTX) */
  #define MX_SEC_IGR_OCT_VAL(_dev)    GET_REV_DATA(_dev, 0x140, 0xa0, 0x50) /*/< Octet count of aunthenticated Ingress packets */
  #define MX_SEC_IGR_OCT_DEC(_dev)    GET_REV_DATA(_dev, 0x141, 0xa1, 0x51) /*/< Octet count of decrypted Ingress packets */
  #define MX_SEC_IGR_UNTAG(_dev)    GET_REV_DATA(_dev, 0x142, 0xa2, 0x52) /*/< Packets received without MACSec tag */
  #define MX_SEC_IGR_NOTAG(_dev)    GET_REV_DATA(_dev, 0x143, 0xa3, 0x53) /*/< Packets received without MACSec tag during STRICT mode */
  #define MX_SEC_IGR_BADTAG(_dev)    GET_REV_DATA(_dev, 0x144, 0xa4, 0x54) /*/< Packets received with incorrect MACSec Tag */
  #define MX_SEC_IGR_UNKSCI(_dev)    GET_REV_DATA(_dev, 0x145, 0xa5, 0x55) /*/< SCI not found in lookup table */
  #define MX_SEC_IGR_NOSCI(_dev)    GET_REV_DATA(_dev, 0x146, 0xa6, 0x56) /*/< SCI not found in lookup table */
  #define MX_SEC_IGR_UNUSSA(_dev)    GET_REV_DATA(_dev, 0x147, 0xa7, 0x57) /*/< SCI not found in lookup table */
  #define MX_SEC_IGR_NOUSSA(_dev)    GET_REV_DATA(_dev, 0x148, 0xa8, 0x58) /*/< SCI not found in lookup table */
  #define MX_SEC_IGR_OCT_TOT(_dev)    GET_REV_DATA(_dev, 0x149, 0xa9, 0x59) /*/< Octet count of all received Ingress packets */
  #define MX_SEC_EGR_OCT_PROT(_dev)    GET_REV_DATA(_dev, 0x14a, 0xaa, 0x5a) /*/< Octet count of all Egress Packets sent auth-only */
  #define MX_SEC_EGR_OCT_ENC(_dev)    GET_REV_DATA(_dev, 0x14b, 0xab, 0x5b) /*/< Octet count of all Egress Packets sent encrypted */
  #define MX_SEC_EGR_OCT_TOTAL(_dev)    GET_REV_DATA(_dev, 0x14c, 0xac, 0x5c) /*/< Octet count of all Egress Packets sent */
  #define MX_SEC_IGR_MISS(_dev)    GET_REV_DATA(_dev, 0x14d, 0xad, 0x5d) /*/< Number of packets which did not hit any ingress entry */
  #define MX_SEC_EGR_MISS(_dev)    GET_REV_DATA(_dev, 0x14e, 0xae, 0x5e) /*/< Number of packets which did not hit any ingress entry */
  #define MX_SEC_IGR_REDIR(_dev)    GET_REV_DATA(_dev, 0x14f, 0xaf, 0x5f) /*/< Number of packets which were redirected */
  #define NUM_OF_MACSEC_STAT(_dev)    GET_REV_DATA(_dev, 0x150, 0xb0, 0x60) /*/< Total Number of MACSec Statistics */
/*@}*/

/*! \defgroup UtilityDefines Macros to do bit manuplation
 *@{
 */
/* Bit Manuplation releated Macros */
#define BIT(x)           ((MAD_U32)0x00000001 << (x))
#define bit_get(p,x)     (((p) &  (BIT(x))) >> (x))
#define bit_set(p,x)     ((p) |=  (BIT(x)))
#define bit_clear(p,x)   ((p) &= ~(BIT(x)))
#define bit_flip(p,x)    ((p) ^=  (BIT(x)))
#define bit_write(p,c,x) (c ? bit_set(p,x) : bit_clear(p,x))
/*@}*/
#ifdef __cplusplus
}
#endif

#endif /* __msec_defines_h */
