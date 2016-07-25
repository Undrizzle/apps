#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <netinet/in.h>
#include "tm_interface.h"
#include "tm_reg.h"
#include "template.h"
#include <dbsapi.h>
#include <boardapi.h>

#define MAX_SMI_REG_SETTINGS_NUMS	256

extern T_DBS_DEV_INFO *dbsdev;

#ifndef __BYTE_ORDER
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define intohl(x)	(x)
# define intohs(x)	(x)
# define ihtonl(x)	(x)
# define ihtons(x)	(x)
#else
# if __BYTE_ORDER == __BIG_ENDIAN
#  define intohl(x)	__bswap_32 (x)
#  define intohs(x)	__bswap_16 (x)
#  define ihtonl(x)	__bswap_32 (x)
#  define ihtons(x)	__bswap_16 (x)
# endif
#endif

typedef struct
{
	uint16_t reg;
	uint32_t value;
}var_register;

var_register ar8236_default_settings[MAX_SMI_REG_SETTINGS_NUMS] = 
{
	{0x2c,	0x3E3E007E},
	{0x04,	0x0},
	{0x100,	0x1080 },
	{0x30,	0x19f005ee},
	{0x3c,	0xCF00004E},
	{0x104,	0x4004},
	{0x204,	0x4004},
	{0x304,	0x4004},
	{0x404,	0x4004},
	{0x504,	0x4004},
	{0x118,	0x0},
	{0x114,	0x8001B},
	{0x214,	0x8001B},
	{0x314,	0x8001B},
	{0x414,	0x8001B},
	{0x514,	0x8001B},
	{0x108,	0x10000},
	{0x208,	0x10000},
	{0x308,	0x10000},
	{0x408,	0x10000},
	{0x508,	0x10000},
	{0x110,	0x18001FFF},
	{0x210,	0x18001FFF},
	{0x310,	0x18001FFF},
	{0x410,	0x18001FFF},
	{0x510,	0x18001FFF},
	{0x120,	0x1FFF},
	{0x220,	0x1FFF},
	{0x320,	0x1FFF},
	{0x420,	0x1FFF},
	{0x520,	0x1FFF},
	{0x10c,	0x3E0000},
	{0x20c,	0x3D0000},
	{0x30c,	0x3B0000},
	{0x40c,	0x370000},
	{0x50c,	0x2F0000},	
};

uint32_t __tm_checksum_32_pib (register const void * memory, register size_t extent, register uint32_t checksum) 
{

#ifdef __GNUC__

	while (extent >= sizeof (checksum)) 
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}

#else

	uint32_t * offset = (uint32_t *)(memory);
	while (extent >= sizeof (uint32_t)) 
	{
		extent -= sizeof (uint32_t);
		checksum ^= *offset++;
	}

#endif

	return (~checksum);
}

BOOLEAN __is_cnu_profile_valid(uint16_t clt_index, uint16_t cnu_index)
{
	DB_INTEGER_V iValue;
	
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		return BOOL_FALSE;
	}
	else
	{
		if( 0 == iValue.len )
		{
			return BOOL_FALSE;
		}
		else if( 1 == iValue.integer )
		{
			return BOOL_TRUE;
		}
		else
		{
			return BOOL_FALSE;
		}
	}
}

BOOLEAN __is_cnu_valid(uint16_t clt_index, uint16_t cnu_index)
{
 	uint8_t INT_MUL_ADDR[6] = {0x00, 0xB0, 0x52,0x00, 0x00, 0x01 };
	uint8_t NULL_ADDR[6] = {0x00, 0x00, 0x00,0x00, 0x00, 0x00 };
	uint8_t macaddr[6] = {0};
	st_dbsCnu cnu;

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index, &cnu))
	{
		return BOOL_FALSE;
	}

	if( boardapi_macs2b(cnu.col_mac, macaddr) != 0 )
	{
		return BOOL_FALSE;
	}
	
	if( cnu.col_row_sts == 0 )
	{
		return BOOL_FALSE;
	}
	else if( memcmp(INT_MUL_ADDR, macaddr, 6) == 0 )
	{
		return BOOL_FALSE;
	}
	else if( memcmp(NULL_ADDR, macaddr, 6) == 0 )
	{
		return BOOL_FALSE;
	}
	else if( boardapi_isCnuTrusted(cnu.col_model) == BOOL_FALSE )
	{
		return BOOL_FALSE;
	}
	else
	{
		return __is_cnu_profile_valid(clt_index, cnu_index);
	}
}

uint32_t __dbsGetProfileSts(int tid, uint32_t *sts)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	v.ci.row = tid;
	v.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS;
	v.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS == dbsGetInteger(dbsdev, &v) )
	{
		if( v.ci.colType == DBS_INTEGER)
		{
			*sts = v.integer;
		}
		else
		{
			*sts = 0;
		}
		return CMM_SUCCESS;
	}
	else
	{
		*sts = 0;
		return CMM_FAILED;
	}
}

int __tmDbsGetCnu(uint16_t cltid, uint16_t cnuid, st_dbsCnu*cnu)
{
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	return dbsGetCnu(dbsdev, id, cnu);
}

int __tmDbsGetProfile(uint16_t cltid, uint16_t cnuid, st_dbsProfile*profile)
{
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	return dbsGetProfile(dbsdev, id, profile);
}

int __tmDbsGetCnuOnlineSts(uint16_t cltid, uint16_t cnuid)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_STS;
	v.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS == dbsGetInteger(dbsdev, &v) )
	{
		if( v.ci.colType == DBS_INTEGER)
		{
			return v.integer;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}


int __tmDbsGetWlsts(uint32_t *status)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	v.ci.row = 1;
	v.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL;
	v.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS == dbsGetInteger(dbsdev, &v) )
	{
		if( v.ci.colType == DBS_INTEGER)
		{
			*status = v.integer;
		}
		else
		{
			*status = 0;
		}
	}
	else
	{
		return CMM_FAILED;
	}
	return CMM_SUCCESS;
}

int __tmDbsWlistEnable(void)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	v.ci.row = 1;
	v.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 1;
	
	return dbsUpdateInteger(dbsdev, &v);
}

int __tmDbsWlistDisable(void)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	v.ci.row = 1;
	v.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 0;
	
	return dbsUpdateInteger(dbsdev, &v);
}

int __tmDbsAddCnuToWlist(uint16_t cltid, uint16_t cnuid)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 1;

	return dbsUpdateInteger(dbsdev, &v);
}

int __tmDbsSetCnuAuthEnable(uint16_t cltid, uint16_t cnuid)
{
	DB_INTEGER_V v;
	
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 1;

	return dbsUpdateInteger(dbsdev, &v);
}

int __tmDbsSetCnuAuthDisable(uint16_t cltid, uint16_t cnuid)
{
	DB_INTEGER_V v;
	
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 0;

	return dbsUpdateInteger(dbsdev, &v);
}

int __dbsCnuEnable(uint16_t cltid, uint16_t cnuid)
{
	uint32_t sts = 0;
	DB_INTEGER_V v;
	
	if( CMM_SUCCESS == __dbsGetProfileSts((cltid-1)*MAX_CNUS_PER_CLT+cnuid, &sts) )
	{
		if(sts)
		{
			/* col_psctlSts = 1; col_cpuPortSts = 1 */
			v.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
			v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
			v.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_PSCTL;
			v.ci.colType = DBS_INTEGER;
			v.len = sizeof(uint32_t);
			v.integer = 1;
			if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &v) )
			{
				return CMM_FAILED;
			}
			/* col_psctlSts = 1; col_cpuPortSts = 1 */
			v.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
			v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
			v.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS;
			v.ci.colType = DBS_INTEGER;
			v.len = sizeof(uint32_t);
			v.integer = 1;
			if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &v) )
			{
				return CMM_FAILED;
			}
			return CMM_SUCCESS;
		}
		else
		{
			return CMM_FAILED;
		}
	}
	else
	{
		return CMM_FAILED;
	}
}

int __dbsCnuDisable(uint16_t cltid, uint16_t cnuid)
{
	uint32_t sts = 0;
	DB_INTEGER_V v;
	
	if( CMM_SUCCESS == __dbsGetProfileSts((cltid-1)*MAX_CNUS_PER_CLT+cnuid, &sts) )
	{
		if(sts)
		{
			/* col_psctlSts = 1; col_cpuPortSts = 1 */
			v.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
			v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
			v.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_PSCTL;
			v.ci.colType = DBS_INTEGER;
			v.len = sizeof(uint32_t);
			v.integer = 1;
			if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &v) )
			{
				return CMM_FAILED;
			}
			/* col_psctlSts = 1; col_cpuPortSts = 1 */
			v.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
			v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
			v.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS;
			v.ci.colType = DBS_INTEGER;
			v.len = sizeof(uint32_t);
			v.integer = 0;
			if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &v) )
			{
				return CMM_FAILED;
			}
			return CMM_SUCCESS;
		}
		else
		{
			return CMM_FAILED;
		}
	}
	else
	{
		return CMM_FAILED;
	}
}

int __tmDbsDelCnuFromWlist(uint16_t cltid, uint16_t cnuid)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	v.ci.colType = DBS_INTEGER;
	v.len = sizeof(uint32_t);
	v.integer = 0;

	return dbsUpdateInteger(dbsdev, &v);
}

void __dump_reg_settings(var_register *smi, uint32_t len)
{
	uint32_t i = 0;
	assert( NULL != smi );
	printf("\r\n\r\n  ==================================================\n");
	for( i=0; i<len; i++ )
	{
		if( 0 != (smi+i)->reg )
		{
			printf("  [ 0x%04X,  0x%08X ]", (smi+i)->reg, (smi+i)->value);
			/* 每2 组换行输出*/
			if( 0 == (i+1)%2 )
			{
				printf("\r\n");
			}
		}
	}
	printf("\n  ==================================================\n");
}

void __dump_atheros_mod(const uint8_t*b, uint32_t len)
{
	int i = 0;
	uint8_t temp = 0;
	
	assert( NULL != b );
	
	/* 打印参数块*/
	printf("\r\n\r\n  ==================================================\n");
	printf("  __dump_atheros_mod len = %d", len);
	printf("\n  ==================================================");
	printf("\r\n  ");
	for( i=0; i<len; i++ )
	{		
		temp = *(b+i);
		printf("%02X ", temp);
		if( (i+1)%16 == 0 )
		{
			printf("\r\n  ");
		}
	}
	printf("\n  ==================================================\n");
	/* 打印参数块*/
}

void __dump_atheros_pib(const uint8_t*b, uint32_t len)
{
	int i = 0;
	uint8_t temp = 0;
	
	assert( NULL != b );
	
	/* 打印参数块*/
	printf("\r\n\r\n  ========================================================\n");
	printf("  __dump_atheros_pib len = %d", len);
	printf("\n  ========================================================");
	printf("\r\n  ");
	for( i=0; i<len; i++ )
	{	
		if( i%16 == 0 )
		{
			printf("%04X:  ", i);
		}
		temp = *(b+i);
		printf("%02X ", temp);
		if( (i+1)%16 == 0 )
		{
			printf("\r\n  ");
		}
	}
	printf("\n  ========================================================\n");
	/* 打印参数块*/
}

uint32_t __uint32t_set_bit(const uint32_t src, uint32_t bit, uint32_t bitValue)
{
	uint32_t ret = src;
	
	if( bit > 32 )
	{
		return ret;
	}
	
	if( 0 == bitValue )
	{
		ret &= (~(1<<bit));
		return ret;
	}
	else if( 1 == bitValue )
	{
		ret |= (1<<bit);
		return ret;
	}
	else
	{
		return ret;
	}
}

int __tmSmiWrite(uint8_t phy, uint8_t reg, uint16_t value, uint16_t mask, uint8_t *buf)
{
	uint16_t command[3] = {0x0000, 0x0000, 0x0000};
	command[0] = ihtons(((0x8005 | (phy<<4)) | (reg<<9)));	
	command[1] = ihtons(value);
	command[2] = ihtons(mask);	
	memcpy(buf, command, 6);
	return 6;
}

size_t __smiWrite_block(uint32_t reg, uint32_t value, size_t offset, uint8_t *buf)
{
	uint32_t reg_word_addr;	
	uint32_t last_cmd_num = 0;
	uint32_t new_cmd_num = 0;
	uint16_t svalue = 0;
	size_t len = 0;

	if( 0 == offset )
	{
		new_cmd_num = 3;
		svalue = ihtons((uint16_t)(1|(new_cmd_num<<6)));
		memcpy((buf+offset), &svalue, sizeof(uint16_t));
		offset += sizeof(uint16_t);
		len += sizeof(uint16_t);
	}
	else
	{
		last_cmd_num = ihtons(*(uint16_t *)buf)>>6;
		new_cmd_num = 3;
		svalue = ihtons((uint16_t)(1|((last_cmd_num+new_cmd_num)<<6)));
		memcpy(buf, &svalue, sizeof(uint16_t));
	}

	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = (reg & 0xfffffffc) >> 1;

	/* configure register high address */
	offset += __tmSmiWrite(0x18, 0x0, ((reg_word_addr>>8)&0x3ff), 0xffff, (buf+offset));
	len += 3*sizeof(uint16_t);

	/* For some registers such as ARL and VLAN, since they include BUSY bit */
	/* in lower address, we should write the higher 16-bit register then the */
	/* lower one */
	
	/* write register in higher address */
	reg_word_addr++;
	offset += __tmSmiWrite((0x10|((reg_word_addr>>5)&0x7)), (reg_word_addr&0x1f), ((value>>16)&0xffff), 0xffff, (buf+offset));
	len += 3*sizeof(uint16_t);

	/* write register in lower address *//* bit7-5 of reg address *//* bit4-0 of reg address */
	reg_word_addr--;
	offset += __tmSmiWrite((0x10|((reg_word_addr>>5)&0x7)), (reg_word_addr&0x1f), (value&0xffff), 0xffff, (buf+offset));
	len += 3*sizeof(uint16_t);
	
	return len;
}

uint32_t __tm_checksum_32_mod (const uint8_t * ptr, register size_t len) 

{
	uint32_t crc32_tab[256]=
	{
		0x0,0x4c11db7,0x9823b6e,0xd4326d9,   
		0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,   
		0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,   
		0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,   
		0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,   
		0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,   
		0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,   
		0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,   
		0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,   
		0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,   
		0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,   
		0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,   
		0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,   
		0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,   
		0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,   
		0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,   
		0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,   
		0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,   
		0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,   
		0x18aeb13,0x54bf6a4,0x808d07d,0xcc9cdca,   
		0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,   
		0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,   
		0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,   
		0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,   
		0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,   
		0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,   
		0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,   
		0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,   
		0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,   
		0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,   
		0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,   
		0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,   
		0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,   
		0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,   
		0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,   
		0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,   
		0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,   
		0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,   
		0x315d626,0x7d4cb91,0xa97ed48,0xe56f0ff,   
		0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,   
		0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,   
		0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,   
		0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,   
		0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,   
		0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,   
		0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,   
		0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,   
		0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,   
		0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,   
		0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,   
		0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,   
		0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,   
		0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,   
		0x29f3d35,0x65e2082,0xb1d065b,0xfdc1bec,   
		0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
		0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
		0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
		0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
		0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
		0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
		0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
		0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c, 
		0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,   
		0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
	};
	//int i = 0;
	uint32_t crc = 0;
	uint32_t oldcrc32;   
	uint8_t  temp;
	size_t length = len;

	/* 打印参数块*/
	//__dump_atheros_mod(ptr, length);
	
	while ( (length--) != 0 )   
	{
		temp = (crc>>24)&0xff;   
		oldcrc32 = crc32_tab[*ptr^temp];   
		crc = (crc<<8)^oldcrc32;   
		ptr++;   
	}   
	return(crc); 
}

int __tm_gen_pib_file_path(uint8_t *path, const uint8_t ODA[])
{
	//uint8_t path_mac[32] = {0};
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.PIB", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	__tm_debug("gen_pib_file_path = [%s]\n", path);
	return 0;
}

int __tm_gen_mod_file_path(uint8_t *path, const uint8_t ODA[])
{
	//uint8_t path_mac[32] = {0};
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.BIN", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	__tm_debug("gen_mod_file_path = [%s]\n", path);
	return 0;
}

size_t __gen_atheros_mdio_module(var_register *smi, size_t len, uint8_t **mod)
{
	uint32_t i = 0;
	size_t offset = 0;	
	static uint8_t block[1280] = {0};

	*mod = block;
	bzero(block, 1280);

	for( i=0; i<len; i++ )
	{
		if( (smi+i)->reg != 0 )
		{
			offset += __smiWrite_block((smi+i)->reg, (smi+i)->value, offset, block);
			if( offset > 1262 )
			{
				
				fprintf(stderr, "ERROR: __gen_atheros_mdio_module offset !");
				dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__gen_mod_crc offset error");
				return -1;
			}
		}
	}
	if( 0 != ( offset % 4 ) )
	{
		offset += 2;
	}
	return offset;	
}

void __init_ar8236_mii_phy_mode(var_register *smi, uint32_t len)
{
	int i = 0;
	
	for( i=0;i<len;i++)
	{
		if( (smi+i)->reg == 0x2c )
		{
			/* 在这里，这个寄存器应该不用修改的*/
			(smi+i)->value = 0x7e3f003f;
		}
		else if( (smi+i)->reg == 0x04 )
		{
			/* reg 0x04 bit[8] <MAC0_PHY_MII_RXCLK_SEL> set to <1'b1> */			
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
			/* reg 0x04 bit[10] <MAC0_PHY_MII_EN> set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
		}
		else if( (smi+i)->reg == 0x100 )
		{
			/* reg 0x100 bit[1:0] <Speed mode> set to <2'b01> 100 Mbps */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
			/* reg 0x100 bit[2] <TXMAC_EN> set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			/* reg 0x100 bit[3] <RXMAC_EN> set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 3, 1);
			/* reg 0x100 bit[4] <TX_FLOW_EN> set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 4, 1);
			/* reg 0x100 bit[5] <RX_FLOW_EN> set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			/* reg 0x100 bit[6] <Duplex mode> set to <1'b1> Full-duplex mode */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 6, 1);
			/* reg 0x100 bit[7] <TX_HALF_FLOW_EN> set to <1'b0> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 7, 0);
			/* reg 0x100 bit[12] <FLOW_LINK_EN> set to <1'b0> mac can be config by software */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
		}
		else if( (smi+i)->reg == 0x30 )
		{
			/* reg 0x100 bit[13:0] <MAX_FRAME_SIZE> set from 1518<5ee> to 1522<5f2> */
			(smi+i)->value = 0x19f005f2;
		}
	}
}

void __init_ar8236_port_loop(var_register *smi, uint32_t len)
{
	int i = 0;
	
	for( i=0;i<len;i++)
	{
		if( (smi+i)->reg == 0x204 )
		{
			/* reg 0x204 bit[6:5] set to <2'b11> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 6, 1);
		}
		else if( (smi+i)->reg == 0x304 )
		{
			/* reg 0x304 bit[6:5] set to <2'b11> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 6, 1);
		}
		else if( (smi+i)->reg == 0x404 )
		{
			/* reg 0x404 bit[6:5] set to <2'b11> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 6, 1);
		}
		else if( (smi+i)->reg == 0x504 )
		{
			/* reg 0x504 bit[6:5] set to <2'b11> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 6, 1);
		}
	}
}

void __init_ar8236_hw_igmp(var_register *smi, uint32_t len)
{
	int i = 0;
	
	for( i=0;i<len;i++)
	{
		if( (smi+i)->reg == 0x2c )
		{
			/* reg 0x2c bit[5:0] set to <6'b111111> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 3, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 4, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 5, 1);
			/* reg 0x2c bit[13:8] set to <6'b000001> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 11, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 13, 0);
			/* reg 0x2c bit[21:16] set to <6'b000000> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 0);
		}
		else if( (smi+i)->reg == 0x3c )
		{
			/* reg 0x3c bit[19:16] set to <4'b1111> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
			/* reg 0x3c bit[22] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 22, 1);
		}
		else if( (smi+i)->reg == 0x104 )
		{
			/* reg 0x104 bit[10] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			/* reg 0x104 bit[20] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
			/* reg 0x104 bit[21] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
		}
		else if( (smi+i)->reg == 0x204 )
		{
			/* reg 0x204 bit[10] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			/* reg 0x204 bit[20] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
			/* reg 0x204 bit[21] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
		}
		else if( (smi+i)->reg == 0x304 )
		{
			/* reg 0x304 bit[10] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			/* reg 0x304 bit[20] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
			/* reg 0x304 bit[21] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
		}
		else if( (smi+i)->reg == 0x404 )
		{
			/* reg 0x404 bit[10] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			/* reg 0x404 bit[20] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
			/* reg 0x404 bit[21] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
		}
		else if( (smi+i)->reg == 0x504 )
		{
			/* reg 0x504 bit[10] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			/* reg 0x504 bit[20] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
			/* reg 0x504 bit[21] set to <1'b1> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
		}
	}
}

void __init_ar8236_strom_filter(var_register *smi, uint32_t len, st_stormFilter *sf)
{
	int i = 0;

	for( i=0;i<len;i++)
	{
		if( (smi+i)->reg == 0x118 )
		{
			if( sf->BroadStormEn )
			{
				/* reg 0x118 bit[8] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
			}
			else
			{
				/* reg 0x118 bit[8] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
			}
			
			if( sf->UniStormEn )
			{
				/* reg 0x118 bit[9] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 1);
			}
			else
			{
				/* reg 0x118 bit[9] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			
			if( sf->MultiStormEn )
			{
				/* reg 0x118 bit[10] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 1);
			}
			else
			{
				/* reg 0x118 bit[10] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 10, 0);
			}
			
			/* reg 0x118 bit[3:0] set to <StormRateLevel> */
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, (((sf->StormRateLevel)>>0)&0x1)?1:0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, (((sf->StormRateLevel)>>1)&0x1)?1:0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, (((sf->StormRateLevel)>>2)&0x1)?1:0);
			(smi+i)->value = __uint32t_set_bit((smi+i)->value, 3, (((sf->StormRateLevel)>>3)&0x1)?1:0);
		}
	}	
}

void __init_ar8236_port_pri(var_register *smi, uint32_t len, st_portPri *config)
{
	int i = 0;

	if(config->portPriEn)
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x214 )
			{
				/* reg 0x214 bit[19] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				/* reg 0x214 bit[1:0] set to <pri0~3> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, (((config->pri[0])>>0)&0x1)?1:0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, (((config->pri[0])>>1)&0x1)?1:0);
				/* reg 0x208 bit[31:29] set to <pri> ?? */
			}
			else if( (smi+i)->reg == 0x314 )
			{
				/* reg 0x314 bit[19] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				/* reg 0x314 bit[1:0] set to <pri0~3> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, (((config->pri[0])>>0)&0x1)?1:0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, (((config->pri[0])>>1)&0x1)?1:0);
				/* reg 0x208 bit[31:29] set to <pri> ?? */
			}
			else if( (smi+i)->reg == 0x414 )
			{
				/* reg 0x414 bit[19] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				/* reg 0x414 bit[1:0] set to <pri0~3> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, (((config->pri[0])>>0)&0x1)?1:0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, (((config->pri[0])>>1)&0x1)?1:0);
				/* reg 0x208 bit[31:29] set to <pri> ?? */
			}
			else if( (smi+i)->reg == 0x514 )
			{
				/* reg 0x514 bit[19] set to <1'b1> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				/* reg 0x514 bit[1:0] set to <pri0~3> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, (((config->pri[0])>>0)&0x1)?1:0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, (((config->pri[0])>>1)&0x1)?1:0);
				/* reg 0x208 bit[31:29] set to <pri> ?? */
			}
		}
	}
	else
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x214 )
			{
				/* reg 0x214 bit[19] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				/* reg 0x214 bit[1:0] set to <2'b00> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				/* reg 0x208 bit[31:29] set to <pri0> ?? */
			}
			else if( (smi+i)->reg == 0x314 )
			{
				/* reg 0x314 bit[19] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				/* reg 0x314 bit[1:0] set to <2'b00> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				/* reg 0x208 bit[31:29] set to <pri0> ?? */
			}
			else if( (smi+i)->reg == 0x414 )
			{
				/* reg 0x414 bit[19] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				/* reg 0x414 bit[1:0] set to <2'b00> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				/* reg 0x208 bit[31:29] set to <pri0> ?? */
			}
			else if( (smi+i)->reg == 0x514 )
			{
				/* reg 0x514 bit[19] set to <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				/* reg 0x514 bit[1:0] set to <2'b00> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				/* reg 0x208 bit[31:29] set to <pri0> ?? */
			}
		}
	}
}

void __init_ar8236_rx_rate_limit(var_register *smi, uint32_t len, st_rxRateLimit *rx)
{
	int i = 0;

	if( rx->rxStatus )
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x110 )
			{
				/* reg 0x110 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				if( 0 == rx->rxCpuRate )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((rx->rxCpuRate)/32);
				}
			}
			else if( (smi+i)->reg == 0x210 )
			{
				/* reg 0x210 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				if( 0 == rx->rxEthRate[0] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((rx->rxEthRate[0])/32);
				}
			}
			else if( (smi+i)->reg == 0x310 )
			{
				/* reg 0x310 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				if( 0 == rx->rxEthRate[1] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((rx->rxEthRate[1])/32);
				}
			}
			else if( (smi+i)->reg == 0x410 )
			{
				/* reg 0x410 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				if( 0 == rx->rxEthRate[2] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((rx->rxEthRate[2])/32);
				}
			}
			else if( (smi+i)->reg == 0x510 )
			{
				/* reg 0x510 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				if( 0 == rx->rxEthRate[3] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((rx->rxEthRate[3])/32);
				}
			}
		}
	}
	else
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x110 )
			{
				/* reg 0x110 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x210 )
			{
				/* reg 0x210 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x310 )
			{
				/* reg 0x310 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x410 )
			{
				/* reg 0x410 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x510 )
			{
				/* reg 0x510 bit[12:0] <ING_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
		}
	}
}

void __init_ar8236_tx_rate_limit(var_register *smi, uint32_t len, st_txRateLimit *tx)
{
	int i = 0;

	if( tx->txStatus )
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x120 )
			{
				/* reg 0x120 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				if( 0 == tx->txCpuRate )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((tx->txCpuRate)/32);
				}
			}
			else if( (smi+i)->reg == 0x220 )
			{
				/* reg 0x220 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				if( 0 == tx->txEthRate[0] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((tx->txEthRate[0])/32);
				}
			}
			else if( (smi+i)->reg == 0x320 )
			{
				/* reg 0x320 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				if( 0 == tx->txEthRate[1] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((tx->txEthRate[1])/32);
				}
			}
			else if( (smi+i)->reg == 0x420 )
			{
				/* reg 0x420 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				if( 0 == tx->txEthRate[2] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((tx->txEthRate[2])/32);
				}
			}
			else if( (smi+i)->reg == 0x520 )
			{
				/* reg 0x520 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				if( 0 == tx->txEthRate[3] )
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|((tx->txEthRate[3])/32);
				}
			}
		}
	}
	else
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x120 )
			{
				/* reg 0x120 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x220 )
			{
				/* reg 0x220 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x320 )
			{
				/* reg 0x320 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x420 )
			{
				/* reg 0x420 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
			else if( (smi+i)->reg == 0x520 )
			{
				/* reg 0x520 bit[12:0] <EG_PRI_RATE> set to <0x1FFF> ?? */
				(smi+i)->value = (((smi+i)->value)&(0xffffffff<<13))|0x1fff;
			}
		}
	}
}

void __init_ar8236_port_ctl(var_register *smi, uint32_t len, st_portCtl *pc)
{
	int i = 0;

	if( pc->status )
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x104 )
			{
				if( pc->cpSts )
				{
					/* reg 0x104 bit[2:0] set to <3'b100> <Forward Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
				}
				else
				{
					/* reg 0x104 bit[2:0] set to <3'b000> <Disable Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 0);
				}
			}
			else if( (smi+i)->reg == 0x204 )
			{
				if( pc->upSts[0] )
				{
					/* reg 0x204 bit[2:0] set to <3'b100> <Forward Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
				}
				else
				{
					/* reg 0x204 bit[2:0] set to <3'b000> <Disable Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 0);
				}
			}
			else if( (smi+i)->reg == 0x304 )
			{
				if( pc->upSts[1] )
				{
					/* reg 0x304 bit[2:0] set to <3'b100> <Forward Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
				}
				else
				{
					/* reg 0x304 bit[2:0] set to <3'b000> <Disable Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 0);
				}
			}
			else if( (smi+i)->reg == 0x404 )
			{
				if( pc->upSts[2] )
				{
					/* reg 0x404 bit[2:0] set to <3'b100> <Forward Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
				}
				else
				{
					/* reg 0x404 bit[2:0] set to <3'b000> <Disable Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 0);
				}
			}
			else if( (smi+i)->reg == 0x504 )
			{
				if( pc->upSts[3] )
				{
					/* reg 0x504 bit[2:0] set to <3'b100> <Forward Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
				}
				else
				{
					/* reg 0x504 bit[2:0] set to <3'b000> <Disable Mode> */
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
					(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 0);
				}
			}
		}
	}
	else
	{
		/* 开启所有端口*/
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x104 )
			{
				/* reg 0x104 bit[2:0] set to <3'b100> <Forward Mode> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			}
			else if( (smi+i)->reg == 0x204 )
			{
				/* reg 0x204 bit[2:0] set to <3'b100> <Forward Mode> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			}
			else if( (smi+i)->reg == 0x304 )
			{
				/* reg 0x304 bit[2:0] set to <3'b100> <Forward Mode> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			}
			else if( (smi+i)->reg == 0x404 )
			{
				/* reg 0x404 bit[2:0] set to <3'b100> <Forward Mode> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			}
			else if( (smi+i)->reg == 0x504 )
			{
				/* reg 0x504 bit[2:0] set to <3'b100> <Forward Mode> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 0, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 1, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 2, 1);
			}
		}
	}
}

void __init_ar8236_vtu(var_register *smi, uint32_t len, st_cnuVlan *vlan)
{
	int i = 0;
	int j = 0;

	uint16_t vid[4] = {0};
	uint8_t vm[4] = {0};

	if( vlan->VlanEnable )
	{
		for( i=0;i<4;i++)
		{
			/* 防止端口VLAN 为0 */
			if( 0 == vlan->vid[i] )
			{
				vid[i] = 1;
			}
			else
			{
				vid[i] = vlan->vid[i];
			}
		}
		/* 计算vtuEntry */
		for( i=0;i<4;i++)
		{
			if( 0 == vid[i] ) continue;
			vm[i] = (0x1|(1<<(i+1)));
			for(j=i+1;j<4;j++)
			{
				if( vid[j] == vid[i] )
				{
					vid[j] = 0;
					vm[i] |= (1<<(1+j));
				}
			}			
		}
		/* insert into vtu */
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0 )
			{
				break;
			}			
		}
		/* 先清除VTU */
		(smi+i)->reg = 0x44;
		(smi+i++)->value = 0;
		(smi+i)->reg = 0x40;
		(smi+(i++))->value = 1;
		for( j=0;j<4;j++ )
		{
			if( 0 != vid[j] )
			{
				/* 11(VT_VALID) <1'b1>, indicated entry is valid */		
				/* 6:0(VID_MEM) <6'b111111> */
				(smi+i)->reg = 0x44;					
				(smi+(i++))->value = (1<<11)|vm[j];				
				/* 27:16(VID) <vid>, VLAN ID to be added or purged */
				/* 3(VT_BUSY) must be <1'b1> */
				/* 2:0(VT_FUNC) <3'b010>, load an entry */
				(smi+i)->reg = 0x40;				
				(smi+(i++))->value = (vid[j]<<16)|(1<<3)|(0x2);
				/* ? VTU 不能连续操作?*/
				//(smi+i)->reg = 0x608;
				//(smi+(i++))->value = (0x2<<16);
				//(smi+i)->reg = 0x608;
				//(smi+(i++))->value = (0x2<<16);
				//break;
			}
		}
	}
	else
	{
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0 )
			{
				break;
			}
		}
		(smi+i)->reg = 0x44;
		(smi+i)->value = 0;
		(smi+i+1)->reg = 0x40;
		(smi+i+1)->value = 1;
		//vtuEntry++;
	}
	//return vtuEntry;
}

void __init_ar8236_vlan(var_register *smi, uint32_t len, st_cnuVlan *vlan)
{
	int i = 0;

	if( vlan->VlanEnable )
	{
		/* 启用VLAN */
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x104 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b10>, egress should transmit frames with VLAN */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 1);
			}
			else if( (smi+i)->reg == 0x204 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b01>, egress should transmit frames without VLAN */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x304 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b01>, egress should transmit frames without VLAN */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x404 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b01>, egress should transmit frames without VLAN */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x504 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b01>, egress should transmit frames without VLAN */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x108 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x208 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid> *//* 防止端口VLAN 为0 */
				if( 0 == vlan->vid[0] )
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|((vlan->vid[0])<<16);
				}
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x308 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid> *//* 防止端口VLAN 为0 */
				if( 0 == vlan->vid[1] )
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|((vlan->vid[1])<<16);
				}
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x408 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid> *//* 防止端口VLAN 为0 */
				if( 0 == vlan->vid[2] )
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|((vlan->vid[2])<<16);
				}
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x508 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid> *//* 防止端口VLAN 为0 */
				if( 0 == vlan->vid[3] )
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				}
				else
				{
					(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|((vlan->vid[3])<<16);
				}
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x10c )
			{
				/* 31:30(802.1Q_MODE) <2'b11>, 802.1Q secure */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 1);
				/* 28:27(ING_VLAN_MODE) <2'b01>, only frame with tag can be received */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b111110>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0x20c )
			{
				/* 31:30(802.1Q_MODE) <2'b01>, 802.1Q fallback */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b10>, only frame untagged can be received */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 1);
				/* 22:16(PORT_VID_MEM) <6'b000001>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 0);
			}
			else if( (smi+i)->reg == 0x30c )
			{
				/* 31:30(802.1Q_MODE) <2'b01>, 802.1Q fallback */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b10>, only frame untagged can be received */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 1);
				/* 22:16(PORT_VID_MEM) <6'b000001>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 0);
			}
			else if( (smi+i)->reg == 0x40c )
			{
				/* 31:30(802.1Q_MODE) <2'b01>, 802.1Q fallback */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b10>, only frame untagged can be received */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 1);
				/* 22:16(PORT_VID_MEM) <6'b000001>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 0);
			}
			else if( (smi+i)->reg == 0x50c )
			{
				/* 31:30(802.1Q_MODE) <2'b01>, 802.1Q fallback */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b10>, only frame untagged can be received */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 1);
				/* 22:16(PORT_VID_MEM) <6'b000001>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 0);
			}			
			else if( (smi+i)->reg == 0 )
			{
				break;
			}
		}
		/* ?? VTU ?? */
		__init_ar8236_vtu(smi, len, vlan);
	}
	else
	{
		/* 禁用VLAN */
		for( i=0;i<len;i++)
		{
			if( (smi+i)->reg == 0x104 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b00>, egress should transmit frames unmodified */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x204 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b00>, egress should transmit frames unmodified */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x304 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b00>, egress should transmit frames unmodified */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x404 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b00>, egress should transmit frames unmodified */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x504 )
			{
				/* 9:8(EG_VLAN_MODE) <2'b00>, egress should transmit frames unmodified */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 8, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 9, 0);
			}
			else if( (smi+i)->reg == 0x108 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x208 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x308 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x408 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x508 )
			{
				/* 12(FORCE_DEFAULT_VID_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 12, 0);
				/* 27:16(PORT_DEFAULT_VID) <vid=1> */
				(smi+i)->value = (((smi+i)->value)&(~(0xfff<<16)))|(0x1<<16);
				/* 28(FORCE_PORT_VLAN_EN) <1'b0> */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
			}
			else if( (smi+i)->reg == 0x10c )
			{
				/* 31:30(802.1Q_MODE) <2'b00>, 802.1Q disable */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b00>, all frame can be received in,include untagged and tagged */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b111110>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0x20c )
			{
				/* 31:30(802.1Q_MODE) <2'b00>, 802.1Q disable */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b00>, all frame can be received in,include untagged and tagged */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b111101>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0x30c )
			{
				/* 31:30(802.1Q_MODE) <2'b00>, 802.1Q disable */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b00>, all frame can be received in,include untagged and tagged */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b111011>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0x40c )
			{
				/* 31:30(802.1Q_MODE) <2'b00>, 802.1Q disable */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b00>, all frame can be received in,include untagged and tagged */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b110111>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0x50c )
			{
				/* 31:30(802.1Q_MODE) <2'b00>, 802.1Q disable */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 30, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 31, 0);
				/* 28:27(ING_VLAN_MODE) <2'b00>, all frame can be received in,include untagged and tagged */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 27, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 28, 0);
				/* 22:16(PORT_VID_MEM) <6'b101111>, Port Base VLAN Member */
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 16, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 17, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 18, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 19, 1);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 20, 0);
				(smi+i)->value = __uint32t_set_bit((smi+i)->value, 21, 1);
			}
			else if( (smi+i)->reg == 0 )
			{
				break;
			}					
		}
		/* VLAN Table ?? */
		__init_ar8236_vtu(smi, len, vlan);	
	}
}

int __prepare_ar8236_register(uint32_t clt_index, uint32_t cnu_index, var_register *smi, uint32_t len)
{
	st_dbsProfile profile;
	st_stormFilter stormFilterSettings;
	st_portPri portBasedPriSettings;
	st_rxRateLimit rx;
	st_txRateLimit tx;
	st_portCtl pc;
	st_cnuVlan vlan;
	//uint8_t *p = NULL;

	if( !__is_cnu_valid(clt_index, cnu_index) )
	{
		return CMM_FAILED;
	}
	
	if( __tmDbsGetProfile(clt_index, cnu_index, &profile) != CMM_SUCCESS )
	{
		return CMM_DB_ACCESS_ERROR;
	}
	if( profile.col_row_sts == 0 )
	{
		return CMM_FAILED;
	}	

	memcpy(smi, ar8236_default_settings, sizeof(ar8236_default_settings));	
	
	/* 初始化AR8236 为MII PHY MODE */
	__init_ar8236_mii_phy_mode(smi, len);
	/* 解决CNU 端口自环问题的相关设置*/
	__init_ar8236_port_loop(smi, len);
	/* 初始化硬件IGMP SNOOPING 功能*/
	__init_ar8236_hw_igmp(smi, len);
	/* strom filter */
	stormFilterSettings.BroadStormEn = profile.col_sfbSts;
	stormFilterSettings.MultiStormEn = profile.col_sfmSts;
	stormFilterSettings.UniStormEn = profile.col_sfuSts;
	stormFilterSettings.StormRateLevel = profile.col_sfRate;
	__init_ar8236_strom_filter(smi, len, &stormFilterSettings);
	/* port pri */
	portBasedPriSettings.portPriEn = profile.col_portPriSts;
	portBasedPriSettings.pri[0] = profile.col_eth1pri;
	portBasedPriSettings.pri[1] = profile.col_eth2pri;
	portBasedPriSettings.pri[2] = profile.col_eth3pri;
	portBasedPriSettings.pri[3] = profile.col_eth4pri;
	__init_ar8236_port_pri(smi, len, &portBasedPriSettings);
	/* rx */
	rx.rxStatus = profile.col_rxLimitSts;
	rx.rxCpuRate = profile.col_cpuPortRxRate;
	rx.rxEthRate[0] = profile.col_eth1rx;
	rx.rxEthRate[1] = profile.col_eth2rx;
	rx.rxEthRate[2] = profile.col_eth3rx;
	rx.rxEthRate[3] = profile.col_eth4rx;
	__init_ar8236_rx_rate_limit(smi, len, &rx);
	/* tx */
	tx.txStatus = profile.col_txLimitSts;
	tx.txCpuRate = profile.col_cpuPortTxRate;
	tx.txEthRate[0] = profile.col_eth1tx;
	tx.txEthRate[1] = profile.col_eth2tx;
	tx.txEthRate[2] = profile.col_eth3tx;
	tx.txEthRate[3] = profile.col_eth4tx;
	__init_ar8236_tx_rate_limit(smi, len, &tx);
	/* port status */
	pc.status = profile.col_psctlSts;
	pc.cpSts = profile.col_cpuPortSts;
	pc.upSts[0] = profile.col_eth1sts;
	pc.upSts[1] = profile.col_eth2sts;
	pc.upSts[2] = profile.col_eth3sts;
	pc.upSts[3] = profile.col_eth4sts;
	__init_ar8236_port_ctl(smi, len, &pc);
	/* vlan */
	vlan.VlanEnable = profile.col_vlanSts;
	vlan.vid[0] = profile.col_eth1vid;
	vlan.vid[1] = profile.col_eth2vid;
	vlan.vid[2] = profile.col_eth3vid;
	vlan.vid[3] = profile.col_eth4vid;
	__init_ar8236_vlan(smi, len, &vlan);	

	return CMM_SUCCESS;
}

size_t __get_cnu_pib_length(uint32_t clt_index, uint32_t cnu_index)
{
	FILE *fp;
	size_t len = 0;
	uint8_t PIB_PATH[64] = { 0 };
	DB_INTEGER_V iValue;

	if( !__is_cnu_valid(clt_index, cnu_index) )
	{
		return 0;
	}
	/* 获取该CNU  的BASE PIB */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_BASE;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		return 0;
	}
	else
	{
		if( 0 == iValue.len )
		{
			return 0;
		}
		else
		{
			sprintf( PIB_PATH, "/usr/mnt/config/template/%d", iValue.integer);
		}
	}
	/* 判断文件是否存在*/
	if( access(PIB_PATH, 0) )
	{
		fprintf(stderr, "ERROR: file %s does not existed\n", PIB_PATH);
		return 0;
	}
	else if( (fp = fopen(PIB_PATH, "rb")) < 0 )
	{
		return 0;
	}
	else
	{
		fseek(fp, 0L, SEEK_END);
		/* 文件的实际长度*/
		len = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		/* 回收文件句柄*/
		fclose(fp);
		return len;
	}	
}

uint32_t __prepare_atheros_cnu_source_pib(uint32_t clt_index, uint32_t cnu_index, uint8_t *data, size_t len)
{
	FILE *fp;
	uint8_t PIB_PATH[64] = { 0 };
	DB_INTEGER_V iValue;

	if( NULL == data )
	{
		return CMM_FAILED;
	}
	
	/* 获取该CNU  的BASE PIB */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_BASE;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		return CMM_FAILED;
	}
	else
	{
		if( 0 == iValue.len )
		{
			return CMM_FAILED;
		}
		else
		{
			sprintf( PIB_PATH, "/usr/mnt/config/template/%d", iValue.integer);
		}
	}
	/* 判断文件是否存在*/
	if( access(PIB_PATH, 0) )
	{
		fprintf(stderr, "ERROR: file %s does not existed\n", PIB_PATH);
		return CMM_FAILED;
	}
	else if( (fp = fopen(PIB_PATH, "rb")) < 0 )
	{
		return CMM_FAILED;
	}
	else
	{
		if( fread(data, 1, len, fp) < len )
		{
			/* 回收文件句柄*/
			fclose(fp);
			return CMM_FAILED;
		}
		else
		{
			/* 回收文件句柄*/
			fclose(fp);
			/* CheckSum32 set 0 ? */
			memset(data+0x8, 0x0, 4);
			return CMM_SUCCESS;
		}
	}	
}

uint32_t __init_atheros_cnu_pib_mac(uint32_t clt_index, uint32_t cnu_index, uint8_t *data)
{
	uint8_t macaddr[6] = {0};
	DB_TEXT_V textValue;
	
	if( NULL == data )
	{
		return CMM_FAILED;
	}
	/* 获取CNU 的MAC 地址*/
	textValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	textValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	textValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_MAC;
	textValue.ci.colType = DBS_TEXT;
	if( CMM_SUCCESS == dbsGetText(dbsdev, &textValue) )
	{
		if( DBS_NULL == textValue.ci.colType )
		{
			return CMM_FAILED;
		}
		else if( 0 == textValue.len )
		{
			return CMM_FAILED;
		}
		else if( boardapi_macs2b(textValue.text, macaddr) != 0 )
		{
			return CMM_FAILED;
		}
		else
		{
			/* 修改MAC 地址*/
			memcpy(data+0xc, macaddr, 6);
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_FAILED;
	}
}

uint32_t __init_atheros_cnu_pib_device_type(uint32_t clt_index, uint32_t cnu_index, uint8_t *data)
{
	uint8_t DType[64] = { 0 };
	DB_INTEGER_V iValue;

	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* 获取CNU 的设备类型*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_MODEL;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == iValue.ci.colType )
		{
			return CMM_FAILED;
		}
		else if( 0 == iValue.len )
		{
			return CMM_FAILED;
		}
		else
		{
			strcpy(DType, boardapi_getCnuHfid(iValue.integer));
			/* 修改设备类型*/	
			memcpy(data+0x74, DType, 64);
			return CMM_SUCCESS;
		}
	}
}

uint32_t __init_atheros_cnu_pib_mac_limit(uint32_t clt_index, uint32_t cnu_index, uint8_t *data)
{
	uint8_t b1_value = 0;
	DB_INTEGER_V iValue;

	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* 获取该CNU  的配置 */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == iValue.ci.colType )
		{
			return CMM_FAILED;
		}
		else if( 0 == iValue.len )
		{
			return CMM_FAILED;
		}
		else
		{
			/* 填充数据表中的相关配置信息*/
			if( iValue.integer )
			{
				b1_value = iValue.integer;
				/* 修改MAC LIMIT NUM*/
				memcpy(data+0x1fdd, &b1_value, 1);				
			}
			return CMM_SUCCESS;
		}
	}
}

uint32_t __init_atheros_cnu_pib_aging(uint32_t clt_index, uint32_t cnu_index, uint8_t *data)
{
	uint8_t b1_sts = 0;
	uint16_t b2_l = 0;
	uint16_t b2_r = 0;	
	st_dbsProfile profile;
	uint16_t id = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;

	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* 获取该CNU  的配置 */	
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id, &profile) )
	{
		return CMM_FAILED;
	}
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	else
	{
		if( (15 == profile.col_loagTime) && (4 == profile.col_reagTime) )
		{
			b1_sts = 0;			
		}
		else
		{
			b1_sts = 1;
		}
		b2_l = profile.col_loagTime;
		b2_r = profile.col_reagTime;
		/*  转换字节序*/
		b2_l = ihtons(b2_l);
		b2_r = ihtons(b2_r);
		/* 修改配置*/
		memcpy(data+0x1fde, &b1_sts, 1);
		memcpy(data+0x1fdf, &b2_l, 2);
		memcpy(data+0x1fe1, &b2_r, 2);
		return CMM_SUCCESS;
	}
}

uint32_t __init_atheros_cnu_pib_qos(uint32_t clt_index, uint32_t cnu_index, uint8_t *data)
{
	uint8_t b1 = 0;
	uint32_t b4 = 0;
	st_dbsProfile profile;
	uint16_t id = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;

	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* 获取该CNU  的配置 */	
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id, &profile) )
	{
		return CMM_FAILED;
	}
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	else
	{
		/* 修改配置*/
		/* col_tbaPriSts: offset = 0x1f55, len = 1 */
		b1 = (profile.col_tbaPriSts?1:0);
		memcpy(data+0x1f55, &b1, 1);
		/* col_cosPriSts: offset = 0x0220, len = 1 */
		b1 = (profile.col_cosPriSts?1:0);
		memcpy(data+0x0220, &b1, 1);
		/* col_tosPriSts: offset = 0x0221, len = 1 */
		b1 = (profile.col_tosPriSts?1:0);
		memcpy(data+0x0221, &b1, 1);
		/* pri cap: offset = 0x0224, len = 4 */
		b4 |= ((profile.col_cos0pri & 0x00000003)<<0);
		b4 |= ((profile.col_cos1pri & 0x00000003)<<2);
		b4 |= ((profile.col_cos2pri & 0x00000003)<<4);
		b4 |= ((profile.col_cos3pri & 0x00000003)<<6);
		b4 |= ((profile.col_cos4pri & 0x00000003)<<8);
		b4 |= ((profile.col_cos5pri & 0x00000003)<<10);
		b4 |= ((profile.col_cos6pri & 0x00000003)<<12);
		b4 |= ((profile.col_cos7pri & 0x00000003)<<14);
		b4 |= ((profile.col_tos0pri & 0x00000003)<<16);
		b4 |= ((profile.col_tos1pri & 0x00000003)<<18);
		b4 |= ((profile.col_tos2pri & 0x00000003)<<20);
		b4 |= ((profile.col_tos3pri & 0x00000003)<<22);
		b4 |= ((profile.col_tos4pri & 0x00000003)<<24);
		b4 |= ((profile.col_tos5pri & 0x00000003)<<26);
		b4 |= ((profile.col_tos6pri & 0x00000003)<<28);
		b4 |= ((profile.col_tos7pri & 0x00000003)<<30);
		/*  转换字节序*/
		b4 = ihtonl(b4);
		memcpy(data+0x0224, &b4, 4);
		
		return CMM_SUCCESS;
	}
}

uint32_t __init_atheros_cnu_pib_crc(uint8_t *data, size_t len)
{
	uint32_t crc = 0;
	
	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* CheckSum32 set */
	crc = __tm_checksum_32_pib(data, len, 0);
	memcpy((data+0x8), &crc, sizeof(uint32_t));
	return CMM_SUCCESS;
}

uint32_t __make_atheros_cnu_pib(uint32_t clt_index, uint32_t cnu_index, uint8_t *data, size_t len)
{
	FILE *fp;
	uint8_t PIB_PATH[64] = { 0 };
	uint8_t macaddr[6] = {0};
	DB_TEXT_V textValue;

	if( NULL == data )
	{
		return CMM_FAILED;
	}

	/* 获取CNU 的MAC 地址*/
	textValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	textValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	textValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_MAC;
	textValue.ci.colType = DBS_TEXT;
	if( CMM_SUCCESS == dbsGetText(dbsdev, &textValue) )
	{
		if( DBS_NULL == textValue.ci.colType )
		{
			return CMM_FAILED;
		}
		else if( 0 == textValue.len )
		{
			return CMM_FAILED;
		}
		else if( boardapi_macs2b(textValue.text, macaddr) != 0 )
		{
			return CMM_FAILED;
		}
	}
	else
	{
		return CMM_FAILED;
	}
	
	/* 将数据写入PIB 文件*/
	__tm_gen_pib_file_path(PIB_PATH, macaddr);

	if( (fp = fopen(PIB_PATH, "wb+")) < 0 )
	{
		return CMM_FAILED;
	}
	else if(fwrite(data, len, 1, fp) != 1)
	{
		fclose(fp);
		return CMM_FAILED;
	}
	else
	{
		fclose(fp);
		return CMM_SUCCESS;
	}
}

uint32_t __calc_user_mod_crc(uint32_t clt_index, uint32_t cnu_index, uint32_t *crc_code)
{
	var_register ar8236_cur_settings[MAX_SMI_REG_SETTINGS_NUMS];	
	uint8_t *p = NULL;
	size_t len = 0;

	if( CMM_SUCCESS != __prepare_ar8236_register(
									clt_index, 
									cnu_index, 
									ar8236_cur_settings, 
									MAX_SMI_REG_SETTINGS_NUMS) 
	)
	{
		return CMM_FAILED;
	}
	
	len = __gen_atheros_mdio_module(ar8236_cur_settings, MAX_SMI_REG_SETTINGS_NUMS, &p);
	
	if( len < 0 )
	{
		return CMM_FAILED;
	}
	
	if( NULL != p )
	{
		/* 计算MOD CRC */
		if( 0 == len )
		{
			*crc_code = 0;
		}
		else
		{
			*crc_code = __tm_checksum_32_mod(p, len);
		}
	}
	else
	{
		*crc_code = 0;
	}

	return CMM_SUCCESS;
}

uint32_t __calc_user_pib_crc(uint32_t clt_index, uint32_t cnu_index, uint32_t *crc_code)
{
	uint32_t ret = 0;
	uint8_t *p = NULL;
	size_t len = 0;

	/* 获取当前CNU  PIB  文件长度*/
	len = __get_cnu_pib_length(clt_index, cnu_index);
	if( len > 0 )
	{
		/* 创建缓存*/
		p = (uint8_t *)malloc(len);
		if( NULL == p )
		{
			printf("__calc_user_pib_crc malloc null point\n");
			return CMM_FAILED;
		}
		/* 导入原始PIB 内容*/
		ret += __prepare_atheros_cnu_source_pib(clt_index, cnu_index, p, len);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__prepare_atheros_cnu_source_pib: failed\n");
		}
		/* 插入配置*/
		ret += __init_atheros_cnu_pib_mac(clt_index, cnu_index, p);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__init_atheros_cnu_pib_mac: failed\n");
		}
		ret += __init_atheros_cnu_pib_device_type(clt_index, cnu_index, p);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__init_atheros_cnu_pib_device_type: failed\n");
		}
		ret += __init_atheros_cnu_pib_mac_limit(clt_index, cnu_index, p);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__init_atheros_cnu_pib_mac_limit: failed\n");
		}
		ret += __init_atheros_cnu_pib_aging(clt_index, cnu_index, p);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__init_atheros_cnu_pib_aging: failed\n");
		}
		ret += __init_atheros_cnu_pib_qos(clt_index, cnu_index, p);
		if( CMM_SUCCESS != ret )
		{
			printf("__calc_user_pib_crc->__init_atheros_cnu_pib_qos: failed\n");
		}
		/* 计算CRC */
		if( 0 == ret )
		{
			*crc_code = __tm_checksum_32_pib(p, len, 0);
		}
		else
		{
			*crc_code = 0;
		}		
		free(p);
		return ret?CMM_FAILED:CMM_SUCCESS;		
	}
	else
	{
		printf("__calc_user_pib_crc->__get_cnu_pib_length: len <= 0\n");
		return CMM_FAILED;
	}
}

uint32_t __gen_user_mod(uint32_t clt_index, uint32_t cnu_index)
{
	var_register ar8236_cur_settings[MAX_SMI_REG_SETTINGS_NUMS];
	st_dbsCnu cnu;
	uint8_t *p = NULL;
	size_t len = 0;
	uint8_t macaddr[6] = {0};
	uint8_t MOD_PATH[64] = { 0 };
	FILE *fp = NULL;

	if( __tmDbsGetCnu(clt_index, cnu_index, &cnu) != CMM_SUCCESS )
	{
		return CMM_DB_ACCESS_ERROR;
	}
	if( !__is_cnu_valid(clt_index, cnu_index) )
	{
		return CMM_FAILED;
	}

	if( boardapi_macs2b(cnu.col_mac, macaddr) != 0 )
	{
		return CMM_FAILED;
	}

	if( CMM_SUCCESS != __prepare_ar8236_register(
									clt_index, 
									cnu_index, 
									ar8236_cur_settings, 
									MAX_SMI_REG_SETTINGS_NUMS) 
	)
	{
		return CMM_FAILED;
	}

	len = __gen_atheros_mdio_module(ar8236_cur_settings, MAX_SMI_REG_SETTINGS_NUMS, &p);
	
	if( len < 0 )
	{
		return CMM_FAILED;
	}

	__tm_gen_mod_file_path(MOD_PATH, macaddr);
	if( (fp = fopen(MOD_PATH, "wb+")) < 0 )
	{
		perror("\r\n  create mod Error!");
		return CMM_MME_ERROR;
	}
	
	if( NULL != p )
	{
		if(fwrite(p, len, 1, fp) != 1)
		{
			perror("\r\n  Pib Write Error!");
			fclose(fp);
			return CMM_MME_ERROR;
		}
	}

	fclose(fp);
	return CMM_SUCCESS;
}

uint32_t __gen_user_pib(uint32_t clt_index, uint32_t cnu_index)
{
	uint32_t ret = 0;
	uint8_t *p = NULL;
	size_t len = 0;

	/* 获取当前CNU  PIB  文件长度*/
	len = __get_cnu_pib_length(clt_index, cnu_index);
	if( len > 0 )
	{
		/* 创建缓存*/
		p = (uint8_t *)malloc(len);
		if( NULL == p )
		{
			return CMM_FAILED;
		}
		/* 导入原始PIB 内容*/
		ret += __prepare_atheros_cnu_source_pib(clt_index, cnu_index, p, len);
		/* 插入配置*/
		ret += __init_atheros_cnu_pib_mac(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_device_type(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_mac_limit(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_aging(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_qos(clt_index, cnu_index, p);
		/* 插入CRC 校验码*/
		ret += __init_atheros_cnu_pib_crc(p, len);
		/* 制作PIB 文件*/
		ret += __make_atheros_cnu_pib(clt_index, cnu_index, p, len);
		free(p);
		return ret?CMM_FAILED:CMM_SUCCESS;		
	}
	else
	{
		return CMM_FAILED;
	}
}

uint32_t __gen_mod_crc(uint32_t clt_index, uint32_t cnu_index, uint32_t *crc_code)
{
	return __calc_user_mod_crc(clt_index, cnu_index, crc_code);
}

uint32_t __gen_pib_crc(uint32_t clt_index, uint32_t cnu_index, uint32_t *crc_code)
{
	return __calc_user_pib_crc(clt_index, cnu_index, crc_code);
}

uint32_t __gen_pib(uint32_t clt_index, uint32_t cnu_index)
{
	return __gen_user_pib(clt_index, cnu_index);
} 

uint32_t __gen_mod(uint32_t clt_index, uint32_t cnu_index)
{
	return __gen_user_mod(clt_index, cnu_index);
} 

BOOLEAN __is_cnu_in_wlist(uint32_t clt_index, uint32_t cnu_index)
{
	DB_INTEGER_V v;
	v.ci.tbl = DBS_SYS_TBL_ID_CNU;
	v.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	v.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	v.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS == dbsGetInteger(dbsdev, &v) )
	{
		if( v.ci.colType == DBS_INTEGER)
		{
			return ((v.integer)?BOOL_TRUE:BOOL_FALSE);
		}
	}
	return BOOL_FALSE;
}

void __tm_reg_force_regist(uint32_t clt_index, uint32_t cnu_index)
{
	stRegEvent tmEvent;
	tmEvent.clt = clt_index;
	tmEvent.cnu = cnu_index;
	tmEvent.event = REG_CNU_FORCE_REGISTRATION;
	
	//printf("\r\n  __tm_reg_force_regist(%d/%d)\n", clt_index, cnu_index);
	tm2reg_send_notification(&tmEvent);
}

void __tm_reg_del_cnu(uint32_t clt_index, uint32_t cnu_index)
{
	stRegEvent tmEvent;
	tmEvent.clt = clt_index;
	tmEvent.cnu = cnu_index;
	tmEvent.event = REG_CNU_DELETE;
	
	//printf("\r\n  __tm_reg_del_cnu(%d/%d)\n", clt_index, cnu_index);
	tm2reg_send_notification(&tmEvent);
}

int __tm_delete_cnu(uint32_t clt_index, uint32_t cnu_index)
{
	if( 0 == dbsDestroyRowProfile(dbsdev, (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index ))
	{
		if( 0 == dbsDestroyRowCnu(dbsdev, (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index ))
		{
			return CMM_SUCCESS;
		}
	}
	return CMM_FAILED;
}

uint32_t __cnu_join_wlist(uint16_t clt_index, uint16_t cnu_index)
{
	if( CMM_SUCCESS != __dbsCnuEnable(clt_index, cnu_index) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_join_wlist->__dbsCnuEnable failed");
		return CMM_FAILED;
	}

	if( CMM_SUCCESS != __tmDbsSetCnuAuthEnable(clt_index, cnu_index) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_join_wlist->__tmDbsSetCnuAuthEnable failed");
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

uint32_t __cnu_leave_wlist(uint16_t clt_index, uint16_t cnu_index)
{
	uint32_t wlsts = 0;
	
	/* 获取白名单功能使能状态*/
	if( CMM_SUCCESS != __tmDbsGetWlsts(&wlsts) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_leave_wlist->__tmDbsGetWlsts communication failed");
		return CMM_FAILED;
	}

	if(wlsts)
	{
		if( CMM_SUCCESS != __dbsCnuDisable(clt_index, cnu_index) )
		{
			dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_leave_wlist->__dbsCnuDisable failed");
			return CMM_FAILED;
		}
	}
	else
	{
		if( CMM_SUCCESS != __dbsCnuEnable(clt_index, cnu_index) )
		{
			dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_leave_wlist->__dbsCnuEnable failed");
			return CMM_FAILED;
		}
	}

	if( CMM_SUCCESS != __tmDbsSetCnuAuthDisable(clt_index, cnu_index) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "__cnu_leave_wlist->__tmDbsSetCnuAuthDisable failed");
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

uint32_t tm_get_profile(st_dbsProfile *profile)
{
	return dbsGetProfile(dbsdev, profile->id, profile);
}

uint32_t tm_write_profile(st_dbsProfile *profile)
{
	if( CMM_SUCCESS == dbsUpdateProfile(dbsdev, profile->id, profile) )
	{
		__tm_reg_force_regist((profile->id)/64+1, (profile->id)%64);
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

uint32_t tm_get_wlist_status(uint32_t *status)
{
	return __tmDbsGetWlsts(status);
}

uint32_t tm_enable_wlist(void)
{
	int i = 0;
	int j = 0;
	int status = 0;

	/* 如果已经启用则直接返回*/
	if( CMM_SUCCESS != __tmDbsGetWlsts(&status) )
	{
		fprintf(stderr, "tm_enable_wlist->__tmDbsGetWlsts failed");
		return CMM_FAILED;
	}
	if(status)
	{
		return CMM_SUCCESS;
	}

	for( i=1; i<=MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		for( j=1; j<=MAX_CNUS_PER_CLT; j++ )
		{
			if( !__is_cnu_valid(i, j) )
			{
				continue;
			}
			if( !__is_cnu_in_wlist(i, j) )
			{
				/* 关闭CABLE 端口*/
				if( CMM_SUCCESS == __dbsCnuDisable(i, j) )
				{
					/* 通知*//* REG_CNU_FORCE_REGISTRATION */
					__tm_reg_force_regist(i, j);
				}
				else
				{
					fprintf(stderr, "tm_enable_wlist->__dbsCnuDisable(%d, %d) failed", i, j);
				}
			}
		}
	}
	
	return __tmDbsWlistEnable();
}

uint32_t tm_disable_wlist(void)
{
	int i = 0;
	int j = 0;
	int status = 0;

	/* 如果已经关闭则直接返回*/
	if( CMM_SUCCESS != __tmDbsGetWlsts(&status) )
	{
		fprintf(stderr, "tm_disable_wlist->__tmDbsGetWlsts failed");
		return CMM_FAILED;
	}
	if(!status)
	{
		return CMM_SUCCESS;
	}

	for( i=1; i<=MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		for( j=1; j<=MAX_CNUS_PER_CLT; j++ )
		{
			if( !__is_cnu_valid(i, j) )
			{
				continue;
			}			
			if( !__is_cnu_in_wlist(i, j) )
			{				
				/* 打开CABLE 端口*/
				if( CMM_SUCCESS == __dbsCnuEnable(i, j) )
				{
					/* 通知*//* REG_CNU_FORCE_REGISTRATION */
					__tm_reg_force_regist(i, j);
				}
				else
				{
					fprintf(stderr, "tm_disable_wlist->__dbsCnuEnable(%d, %d) failed", i, j);
				}
			}
		}
	}
	return __tmDbsWlistDisable();
}

uint16_t tm_del_cnu(stTmUserInfo *szTmUser)
{
	/* 如果在线则不允许删除 */
	if( __tmDbsGetCnuOnlineSts(szTmUser->clt, szTmUser->cnu) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_WARNING, "tm_del_cnu try to delete online cnu from nelib");
		return TM_CNUONLINE_DELERROR;
	}

	if( CMM_SUCCESS == __tm_delete_cnu(szTmUser->clt, szTmUser->cnu) )
	{
		/* 通知*//* REG_CNU_DELETE */
		__tm_reg_del_cnu(szTmUser->clt, szTmUser->cnu);
		return CMM_SUCCESS;
	}
	else
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "tm_del_cnu->__tm_delete_cnu failed");
		return CMM_FAILED;
	}
}

uint32_t tm_del_cnu_from_wlist(stTmUserInfo *szTmUser)
{
	/* 检查是否为有效的CNU 设备*/
	if( !__is_cnu_valid(szTmUser->clt, szTmUser->cnu) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "tm_del_user try to delete illegal cnu from wlist");
		return CMM_FAILED;
	}	

	if( __is_cnu_in_wlist(szTmUser->clt, szTmUser->cnu) )
	{
		/* 如果是白名单用户，需要从白名单中删除*/
		if( CMM_SUCCESS != __cnu_leave_wlist(szTmUser->clt, szTmUser->cnu) )
		{
			dbs_sys_log(dbsdev, DBS_LOG_ALERT, "tm_del_user leave from failed");
			return CMM_FAILED;
		}
		/* 通知*//* REG_CNU_FORCE_REGISTRATION */
		__tm_reg_force_regist(szTmUser->clt, szTmUser->cnu);
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_SUCCESS;
	}
}

uint32_t tm_add_cnu_into_wlist(stTmUserInfo *szTmNewUser)
{
	/* 检查是否为有效的CNU 设备*/
	if( !__is_cnu_valid(szTmNewUser->clt, szTmNewUser->cnu) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "tm_new_user try to add illegal cnu into wlist");
		return CMM_FAILED;
	}	
	
	if( __is_cnu_in_wlist(szTmNewUser->clt, szTmNewUser->cnu) )
	{
		/* 如果已经是白名单用户*/
		return CMM_SUCCESS;
	}
	else
	{
		/* 否则将用户添加至白名单*/
		if( CMM_SUCCESS != __cnu_join_wlist(szTmNewUser->clt, szTmNewUser->cnu) )
		{
			dbs_sys_log(dbsdev, DBS_LOG_ALERT, "tm_new_user join cnu into wlist failed");
			return CMM_FAILED;
		}
		/* 通知*//* REG_CNU_FORCE_REGISTRATION */
		__tm_reg_force_regist(szTmNewUser->clt, szTmNewUser->cnu);
		return CMM_SUCCESS;
	}
}

uint32_t tm_destroy_conf(stAutoGenConfig *pConf)
{
	assert( NULL != pConf );
	uint8_t CONF_PATH[64] = {0};
	st_dbsCnu cnu;
	uint8_t macaddr[6] = {0};
	
	if( pConf->cnu == 0 )
	{
		return CMM_FAILED;
	}
	else
	{
		/*  标识CNU */
		/* 判断CNU 索引是否有效*/
		if( (pConf->cnu < 1)||(pConf->cnu > MAX_CNUS_PER_CLT) )
		{
			return CMM_FAILED;
		}
		else
		{
			/* 从数据库获取该CNU的信息*/
			if( CMM_SUCCESS != __tmDbsGetCnu(pConf->clt, pConf->cnu, &cnu) )
			{
				return CMM_DB_ACCESS_ERROR;
			}
			if( boardapi_macs2b(cnu.col_mac, macaddr) != 0 )
			{
				return CMM_FAILED;
			}
			
			if( pConf->flag == TM_AUTO_GEN_PIB )
			{
				__tm_gen_pib_file_path(CONF_PATH, macaddr);
				/* 判断文件是否存在*/
				if( access(CONF_PATH, 0) )
				{
					return CMM_FAILED;
				}
				remove(CONF_PATH);
				return CMM_SUCCESS;
			}
			else if( pConf->flag == TM_AUTO_GEN_MOD )
			{
				__tm_gen_mod_file_path(CONF_PATH, macaddr);
				/* 判断文件是否存在*/
				if( access(CONF_PATH, 0) )
				{
					return CMM_FAILED;
				}
				remove(CONF_PATH);
				return CMM_SUCCESS;
			}
			else if( pConf->flag == TM_AUTO_GEN_ALL )
			{
				__tm_gen_pib_file_path(CONF_PATH, macaddr);
				/* 判断文件是否存在*/
				if( access(CONF_PATH, 0) )
				{
					return CMM_FAILED;
				}
				remove(CONF_PATH);
				__tm_gen_mod_file_path(CONF_PATH, macaddr);
				/* 判断文件是否存在*/
				if( access(CONF_PATH, 0) )
				{
					return CMM_FAILED;
				}
				remove(CONF_PATH);
				return CMM_SUCCESS;
			}
			else
			{
				return CMM_FAILED;
			}
		}
	}
	return CMM_FAILED;
}

uint32_t tm_gen_conf(stAutoGenConfig *pConf)
{
	assert( NULL != pConf );
	
	switch(pConf->flag)
	{
		case TM_AUTO_GEN_PIB:
		{
			return __gen_pib(pConf->clt, pConf->cnu);
		}
		case TM_AUTO_GEN_MOD:
		{
			return __gen_mod(pConf->clt, pConf->cnu);
		}
		default:
		{
			return CMM_FAILED;
		}
	}
}

uint32_t tm_gen_crc(stAutoGenCrc *pCrc)
{
	assert( NULL != pCrc );
	uint32_t crc_code = 0;
	
	pCrc->crc = 0;
	switch(pCrc->flag)
	{
		case TM_AUTO_GEN_PIB:
		{
			if( __gen_pib_crc(pCrc->clt, pCrc->cnu, &crc_code) == 0 )
			{
				pCrc->crc = crc_code;
			}
			else
			{
				return CMM_FAILED;
			}
			break;
		}
		case TM_AUTO_GEN_MOD:
		{			
			if( __gen_mod_crc(pCrc->clt, pCrc->cnu, &crc_code) == 0 )
			{
				pCrc->crc = crc_code;
			}
			else
			{
				return CMM_FAILED;
			}
			break;
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	return CMM_SUCCESS;
}

uint32_t tm_dump_register(uint16_t clt_index, uint16_t cnu_index)
{
	var_register ar8236_cur_settings[MAX_SMI_REG_SETTINGS_NUMS];

	if( CMM_SUCCESS != __prepare_ar8236_register(
									clt_index, 
									cnu_index, 
									ar8236_cur_settings, 
									MAX_SMI_REG_SETTINGS_NUMS) 
	)
	{
		return CMM_FAILED;
	}
	else
	{
		__dump_reg_settings(ar8236_cur_settings, MAX_SMI_REG_SETTINGS_NUMS);
		return CMM_SUCCESS;
	}
}

uint32_t tm_dump_mod(uint16_t clt_index, uint16_t cnu_index)
{
	var_register ar8236_cur_settings[MAX_SMI_REG_SETTINGS_NUMS];
	uint8_t *p = NULL;
	size_t len = 0;

	if( CMM_SUCCESS != __prepare_ar8236_register(
									clt_index, 
									cnu_index, 
									ar8236_cur_settings, 
									MAX_SMI_REG_SETTINGS_NUMS) 
	)
	{
		return CMM_FAILED;
	}
	else
	{
		len = __gen_atheros_mdio_module(ar8236_cur_settings, MAX_SMI_REG_SETTINGS_NUMS, &p);
		if( NULL != p )
		{
			if( len > 0 )
			{
				/* 打印参数块*/
				__dump_atheros_mod(p, len);
				return CMM_SUCCESS;
			}			
		}
		return CMM_FAILED;
	}
}

uint32_t tm_dump_cnu_pib(uint16_t clt_index, uint16_t cnu_index)
{
	uint32_t ret = 0;
	uint8_t *p = NULL;
	size_t len = 0;

	/* 获取当前CNU  PIB  文件长度*/
	len = __get_cnu_pib_length(clt_index, cnu_index);
	if( len > 0 )
	{
		/* 创建缓存*/
		p = (uint8_t *)malloc(len);
		if( NULL == p )
		{
			return CMM_FAILED;
		}
		/* 导入原始PIB 内容*/
		ret += __prepare_atheros_cnu_source_pib(clt_index, cnu_index, p, len);
		/* 插入配置*/
		ret += __init_atheros_cnu_pib_mac(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_device_type(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_mac_limit(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_aging(clt_index, cnu_index, p);
		ret += __init_atheros_cnu_pib_qos(clt_index, cnu_index, p);
		/* 插入CRC 校验码*/
		ret += __init_atheros_cnu_pib_crc(p, len);
		if( 0 == ret )
		{
			/* 打印参数块*/
			__dump_atheros_pib(p, len);
		}		
		/* 回收缓冲区*/
		free(p);
		return ret?CMM_FAILED:CMM_SUCCESS;		
	}
	else
	{
		return CMM_FAILED;
	}
}

uint32_t tm_dump_clt_pib(uint16_t clt_index)
{
	uint32_t ret = 0;
	return ret?CMM_FAILED:CMM_SUCCESS;
}

