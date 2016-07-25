#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <assert.h>
#include <public.h>
#include "boardapi.h"

/********************************************************************************************
*	函数名称:boardapi_checkCpuEndian
*	函数功能:判断处理器的字节序是大端还是小端
*	return true: little-endian, return false: big-endian
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_checkCpuEndian(void)
{
	union w
	{
		int a;
		char b;
	}c;
	c.a = 1;
	return (1 == c.b);
}

/********************************************************************************************
*	函数名称:boardapi_getMacAddress
*	函数功能:获取CBAT的MAC地址
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getMacAddress(void)
{
	int fd = 0;
	static char sMAC[32] = {0};
	struct ifreq req;

	strcpy(req.ifr_name, "eth0");
	
	if( ( fd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		perror("boardapi_getMacAddress socket error !\n");     
		strcpy(sMAC, "30:71:B2:00:00:00");
	}
	else if( ioctl(fd, SIOCGIFHWADDR, &req) < 0 )
	{
		perror("boardapi_getMacAddress ioctl error !\n");
		strcpy(sMAC, "30:71:B2:00:00:00");
	}
	else
	{
		sprintf(sMAC, "%02X:%02X:%02X:%02X:%02X:%02X", 
			req.ifr_hwaddr.sa_data[0], req.ifr_hwaddr.sa_data[1], req.ifr_hwaddr.sa_data[2], 
			req.ifr_hwaddr.sa_data[3], req.ifr_hwaddr.sa_data[4], req.ifr_hwaddr.sa_data[5]);
	}
	close(fd);
	return sMAC;
}

/********************************************************************************************
*	函数名称:boardapi_isValidUnicastMacb
*	函数功能:判断是否为有效的单播MAC
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_isValidUnicastMacb(uint8_t *bin)
{
	uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t MC[6] = {0x00,0xb0,0x52,0x00,0x00,0x01};
	
	if( NULL == bin )
	{
		return BOOL_FALSE;
	}
	/* 不允许00:00:00:00:00:00 */
	else if( memcmp(bin, MA, 6) == 0 )
	{
		return BOOL_FALSE;
	}
	/* 不允许FF:FF:FF:FF:FF:FF */
	else if( memcmp(bin, MB, 6) == 0 )
	{
		return BOOL_FALSE;
	}
	/* 不允许00:b0:52:00:00:01 */
	else if( memcmp(bin, MC, 6) == 0 )
	{
		return BOOL_FALSE;
	}
	else
	{
		return BOOL_TRUE;
	}
}

/********************************************************************************************
*	函数名称:boardapi_macs2b
*	函数功能:将字符串形式的MAC地址转换为6位二进制格式
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_macs2b(const char *str, uint8_t *bin)
{
	int result, i;
	unsigned int mac[6];
	
	if((str == NULL) || (bin == NULL))
	{
		return CMM_FAILED;
	}
	
	if( strlen(str) == 0 )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = 0;
		}
		return CMM_SUCCESS;
	}

	if( 6 == sscanf(str,"%2X:%2X:%2X:%2X:%2X:%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else if( 6 == sscanf(str,"%2X-%2X-%2X-%2X-%2X-%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else if( 6 == sscanf(str,"%2X%2X%2X%2X%2X%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_FAILED;
	}	
}

/********************************************************************************************
*	函数名称:boardapi_mac2Uppercase
*	函数功能:将字符串形式的MAC地址转换为大写形式的字符串MAC地址
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_mac2Uppercase(char *strmac)
{
	char bmac[6] = {0};
	uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

	assert( NULL != strmac );
	
	/* check if cnu mac address is valid*/
	if( CMM_SUCCESS != boardapi_macs2b(strmac, bmac) )
		return CMM_FAILED;
	else if( memcmp(bmac, MA, 6) == 0 )
		return CMM_FAILED;
	else if( memcmp(bmac, MB, 6) == 0 )
		return CMM_FAILED;
	else
		sprintf(strmac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			bmac[0], bmac[1], bmac[2], bmac[3], bmac[4], bmac[5]
		);	
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:boardapi_getDeviceModelStr
*	函数功能:获取字符串表示的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getDeviceModelStr(uint32_t model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return "WEC-3501I X7";
		}
		case WEC_3501I_E31:
		{
			return "WEC-3501I E31";
		}
		case WEC_3501I_C22:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 6402";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC-3501I C22";
				
			}			
		}
		case WEC_3501I_S220:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 6401";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC-3501I S220";				
			}
		}
		case WEC_3501I_S60:
		{
			return "WEC-3501I S60";
		}
		case WEC_3501I_Q31:
		{
			return "WEC-3501I Q31";
		}
		case WEC_3601I:
		{
			return "WEC-3601I";
		}
		case WEC_3602I:
		{
			return "WEC-3602I";
		}
		case WEC_3604I:
		{
			return "WEC-3604I";
		}
		case WEC_3702I:
		{
			return "WEC-3702I L2";
		}
		case WEC_3703I:
		{
			return "WEC-3703I L3";
		}
		case WEC_3704I:
		{
			return "WEC-3704I L4";
		}
		case WEC_602:
		{
			return "WEC-3702I C2";
		}
		case WEC_604:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 64";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC-3702I C4";			
			}			
		}
		case WEC9720EK_C22:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 7403";
				case CUSTOM_LOGO_ALCOTEL:
					return "EOC-MO350-2G";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC9720EK C22";
			}			
		}
		case WEC9720EK_E31:
		{
			return "WEC9720EK E31";
		}
		case WEC9720EK_Q31:
		{
			return "WEC9720EK Q31";
		}
		case WEC9720EK_S220:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 7404";
				case CUSTOM_LOGO_ALCOTEL:
					return "EOC-MI350-2G";
				default:
					return "WEC9720EK S220";
			}			
		}
		case WEC9720EK_SD220:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 7401";
				case CUSTOM_LOGO_ALCOTEL:
					return "EOC-MI350D-2G";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC9720EK SD220";
			}			
		}
		case WEC701_C2:
		{
			return "WEC701 C2";
		}
		case WEC701_C4:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 74";
				case CUSTOM_LOGO_ALCOTEL:
					return "EOC-S100-4F";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC701 C4";
			}			
		}
		case WEC_3501I_XD25:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 6403";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC-3501I XD25";
			 }
		}
		case WEC9720EK_XD25:
		{
			switch(CUSTOM_LOGO_ID)
			{				
				case CUSTOM_LOGO_PX:
					return "PX 7402";
				case CUSTOM_LOGO_ALCOTEL:
					return "EOC-MO350D-2G";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC9720EK XD25";
			}			
		}
		case WR1004JL:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 7405";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WR1004JL";
		        }
		}
		case WR1004SJL:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 7406";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WR1004SJL";
			}
		}
		case WR1004JLD:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WR1004JLD";
			}
		}
		case WEC_3702I_E4:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 64E";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC-3702I E4";
			}
		}
		case WEC701_E4:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 74E";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC701 E4";
			}
		}
		case WEC701_L4:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 74L";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC701 L4";
			}
		}
		case WEC701_W4:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_PX:
					return "PX 74W";
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC701 W4";
			}
		}
		case WEC701W_C4:
		{
			switch(CUSTOM_LOGO_ID)
			{
				case CUSTOM_LOGO_DEMO:
				case CUSTOM_LOGO_PREVAIL:
				default:
					return "WEC701W C4";
			}
		}
		default:
		{
			return "UNKNOWN";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getCltStandardStr
*	函数功能:get clt serial type
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getCltStandardStr(void)
{
	const char *pStandardStr = NULL;

	
	switch(CUSTOM_LOGO_ID)
	{				
		case CUSTOM_LOGO_PX:
			pStandardStr = "AR7400";
			break;
		case CUSTOM_LOGO_DEMO:
		case CUSTOM_LOGO_PREVAIL:
		default:
			pStandardStr = "AR7410";
			break;
	}
	return pStandardStr;	
}

/********************************************************************************************
*	函数名称:boardapi_getMenufactoryStr
*	函数功能:get clt serial type
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getMenufactoryStr(void)
{
	const char *pMenufactoryStr = NULL;

	
	switch(CUSTOM_LOGO_ID)
	{				
		case CUSTOM_LOGO_PX:
			pMenufactoryStr = "PX";
			break;
		case CUSTOM_LOGO_PREVAIL:
			pMenufactoryStr = "Prevail";
			break;
		default:
			pMenufactoryStr = " ";
			break;
	}
	return pMenufactoryStr;	
}

/********************************************************************************************
*	函数名称:boardapi_getCnuHfid
*	函数功能:根据设备型号获取烧录在PIB中存储的HFID
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getCnuHfid(uint32_t devType)
{
	const char *user_HFID = NULL;
	
	switch(devType)
	{
		case WEC_3602I:
			user_HFID = "WEC-3602I";
			break;
		case WEC_3702I:
			user_HFID = "WEC-3702I";
			break;
		case WEC_3703I:
			user_HFID = "WEC-3703I";
			break;
		case WEC_602:
			user_HFID = "WEC-602";
			break;
		case WEC_604:
			user_HFID = "WEC-604";
			break;
		case WEC701_C2:
			user_HFID = "WEC701-C2";
			break;
		case WEC701_C4:
			user_HFID = "WEC701-C4";
			break;
		case WEC_3702I_E4:
			user_HFID = "WEC-3702I-E4";
			break;
		case WEC701_E4:
			user_HFID = "WEC701-E4";
			break;
		case WEC701_L4:
			user_HFID = "WEC701-L4";
			break;
		case WEC701_W4:
			user_HFID = "WEC701-W4";
			break;
		case WEC701W_C4:
			user_HFID = "S744_8306RA";
		default :
			user_HFID = "Intellon Enabled Product";
			break;
	}
	return user_HFID;
}

/********************************************************************************************
*	函数名称:boardapi_isCnuSupported
*	函数功能:根据输入的设备型号判断系统是否支持该设备
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_isCnuSupported(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC_3702I:		/* WEC-3702I L2 */
		case WEC_3703I:		/* WEC-3702I L3 */
		case WEC_602:		/* WEC-3702I C2 */
		case WEC_604:		/* WEC-3702I C4 */
		case WEC701_C2:		/* WEC701 C2 */
		case WEC701_C4:		/* WEC701 C4 */
		{
			return BOOL_TRUE;
		}		
		default:
			return BOOL_FALSE;
	}
}

int boardapi_isCnuTrusted(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC_3702I:		/* WEC-3702I L2 */
		case WEC_3703I:		/* WEC-3702I L3 */
		case WEC_602:		/* WEC-3702I C2 */
		case WEC_604:		/* WEC-3702I C4 */
		case WEC701_C2:		/* WEC701 C2 */
		case WEC701_C4:		/* WEC701 C4 */
		case WEC_3702I_E4:	/* WEC-3702I E4 */
		case WEC701_E4:		/* WEC701 E4 */
		case WEC701_L4:
		case WEC701_W4:
		case WEC701W_C4:
		{
			return BOOL_TRUE;
		}		
		default:
			return BOOL_FALSE;
	}
}

int boardapi_getCnuSwitchType(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC_3702I:		/* WEC-3702I L2 */
		case WEC_3703I:		/* WEC-3702I L3 */
		case WEC_602:		/* WEC-3702I C2 */
		case WEC_604:		/* WEC-3702I C4 */
		case WEC701_C2:		/* WEC701 C2 */
		case WEC701_C4:		/* WEC701 C4 */
		{
			return CNU_SWITCH_TYPE_AR8236;
		}		
		default:
		{
			return CNU_SWITCH_TYPE_RTL8306E;
		}
	}
}

int boardapi_isKTCnu(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC701W_C4:
			return BOOL_TRUE;
		default:
			return BOOL_FALSE;
	}
}

int boardapi_isAr7400Device(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		case WEC701W_C4:
		{
			return BOOL_TRUE;
		}		
		default:
			return BOOL_FALSE;
	}
}

int boardapi_isAr6400Device(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC_3702I:		/* WEC-3702I L2 */
		case WEC_3703I:		/* WEC-3702I L3 */
		case WEC_602:		/* WEC-3702I C2 */
		case WEC_604:		/* WEC-3702I C4 */
		case WEC_3702I_E4:
		{
			return BOOL_TRUE;
		}		
		default:
			return BOOL_FALSE;
	}
}

/********************************************************************************************
*	函数名称:boardapi_getModNameStr
*	函数功能:根据模块ID获取字符串表示的模块名称
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getModNameStr(uint16_t mid)
{
	switch(mid)
	{
		case MID_SNMP:
		{
			return "MID_SNMP";
		}
		case MID_CLI:
		{
			return "MID_CLI";
		}
		case MID_HTTP:
		{
			return "MID_HTTP";
		}
		case MID_CMM:
		{
			return "MID_CMM";
		}
		case MID_ALARM:
		{
			return "MID_ALARM";
		}
		case MID_LLOG:
		{
			return "MID_LLOG";
		}
		case MID_DBA:
		{
			return "MID_DBA";
		}
		case MID_REGISTER:
		{
			return "MID_REG";
		}
		case MID_MMEAD:
		{
			return "MID_MMEAD";
		}
		case MID_SQL:
		{
			return "MID_SQL";
		}
		case MID_TEMPLATE:
		{
			return "MID_TM";
		}
		case MID_DBS:
		{
			return "MID_DBS";
		}
		case MID_DBS_TESTER:
		{
			return "DBS_TESTER";
		}
		case MID_SYSMONITOR:
		{
			return "MID_MON";
		}
		case MID_TM_TESTER:
		{
			return "TM_TESTER";
		}
		case MID_SYSEVENT:
		{
			return "MID_EVENT";
		}
		case MID_DSDT_TESTER:
		{
			return "DSDT_TESTER";
		}
		case MID_AT91BTN:
		{
			return "T_AT91BTN";
		}
		case MID_ATM:
		{
			return "T_ATM";
		}
		case MID_SYSINDI:
		{
			return "T_SYSINDI";
		}
		case MID_SYSLED:
		{
			return "T_SYSLED";
		}
		case MID_WDTIMER:
		{
			return "T_WDT";
		}		
		default:
		{
			return "MID_OTHER";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_mapDevModel
*	函数功能:将CBAT中定义的设备型号印射为NMS定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_mapDevModel(int model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return 1;
		}
		case WEC_3501I_E31:
		{
			return 2;
		}
		case WEC_3501I_Q31:
		{
			return 3;
		}
		case WEC_3501I_C22:
		{
			return 4;
		}
		case WEC_3501I_S220:
		{
			return 5;
		}
		case WEC_3501I_S60:
		{
			return 6;
		}
		case WEC_3702I:
		{
			return 7;
		}
		case WEC_3703I:
		{
			return 8;
		}
		case WEC_602:
		{
			return 9;
		}
		case WEC_604:
		{
			return 10;
		}
		case WEC_3801I:
		{
			return 11;
		}
		case WEC_3501I_XD25:
		{
			return 12;
		}
		case WEC9720EK_C22:
		{
			return 20;
		}
		case WEC9720EK_E31:
		{
			return 21;
		}
		case WEC9720EK_Q31:
		{
			return 22;
		}
		case WEC9720EK_S220:
		{
			return 23;
		}
		case WEC9720EK_SD220:
		{
			return 24;
		}
		case WEC9720EK_XD25:
		{
			return 25;
		}
		case WR1004JL:
		{
			return 26;
		}
		case WR1004SJL:
		{
			return 27;
		}
		case WR1004JLD:
		{
			return 28;
		}
		case WEC701_M0:
		{
			return 36;
		}
		case WEC701_C2:
		{
			return 40;
		}
		case WEC701_C4:
		{
			return 41;
		}
		case WEC_3702I_E4:
		{
			return 42;
		}
		case WEC701_E4:
		{
			return 43;
		}
		case WEC701_L4:
		{
			return 44;
		}
		case WEC701_W4:
		{
			return 45;
		}
		case WEC701W_C4:
		{
			return 46;
		}
		default:
		{
			return 256;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_umapDevModel
*	函数功能:将NMS定义的设备型号印射为CBAT中定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_umapDevModel(int model)
{
	switch(model)
	{
		case 1:
		{
			return WEC_3501I_X7;
		}
		case 2:
		{
			return WEC_3501I_E31;
		}
		case 3:
		{
			return WEC_3501I_Q31;
		}
		case 4:
		{
			return WEC_3501I_C22;
		}
		case 5:
		{
			return WEC_3501I_S220;
		}
		case 6:
		{
			return WEC_3501I_S60;
		}
		case 7:
		{
			return WEC_3702I;
		}
		case 8:
		{
			return WEC_3703I;
		}
		case 9:
		{
			return WEC_602;
		}
		case 10:
		{
			return WEC_604;
		}
		case 11:
		{
			return WEC_3801I;
		}
		case 12:
		{
			return WEC_3501I_XD25;
		}
		case 20:
		{
			return WEC9720EK_C22;
		}
		case 21:
		{
			return WEC9720EK_E31;
		}
		case 22:
		{
			return WEC9720EK_Q31;
		}
		case 23:
		{
			return WEC9720EK_S220;
		}
		case 24:
		{
			return WEC9720EK_SD220;
		}
		case 25:
		{
			return WEC9720EK_XD25;
		}
		case 26:
		{
			return WR1004JL;
		}
		case 27:
		{
			return WR1004SJL;
		}
		case 28:
		{
			return WR1004JLD;
		}
		case 36:
		{
			return WEC701_M0;
		}
		case 40:
		{
			return WEC701_C2;
		}
		case 41:
		{
			return WEC701_C4;
		}
		case 42:
		{
			return WEC_3702I_E4;
		}
		case 43:
		{
			return WEC701_E4;
		}
		case 44:
		{
			return WEC701_L4;
		}
		case 45:
		{
			return WEC701_W4;
		}
		case 46:
		{
			return WEC701W_C4;
		}
		default:
		{
			return WEC_INVALID;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmTypeStr
*	函数功能:获取字符串表示的告警类型
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmTypeStr(uint16_t alarmType)
{
	switch(alarmType)
	{
		case ALARM_STS_NOMINAL:
		{
			return "aasNominal";
		}
		case ALARM_STS_HIHI:
		{
			return "aasHIHI";
		}
		case ALARM_STS_HI:
		{
			return "aasHI";
		}
		case ALARM_STS_LO:
		{
			return "aasLO";
		}
		case ALARM_STS_LOLO:
		{
			return "aasLOLO";
		}
		case ALARM_STS_MAJOR:
		{
			return "MAJOR";
		}
		case ALARM_STS_MINOR:
		{
			return "MINOR";
		}		
		default:
		{
			return "aasNominal";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevelByCode
*	函数功能:根据告警码获取该告警的等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevelByCode(uint32_t alarmCode)
{
	switch(alarmCode)
	{
		/*Emergency*/
		case 200903:		/* 环境温度告警*/
		case 200906:		/* 噪声过高告警*/
		case 200907:		/* 链路衰减告警*/
		case 200908:		/* 物理层速率告警*/				
		case 200923:		/* can not find clt */
		{			
#ifdef CFG_USE_PLATFORM_WR1004JL
			return DBS_LOG_NOTICE;
#endif
#ifdef CFG_USE_PLATFORM_WR1004SJL 
			return DBS_LOG_NOTICE;
#endif
#ifdef CFG_USE_PLATFORM_WR1004JLD
			return DBS_LOG_NOTICE;
#endif
			return DBS_LOG_EMERG;

		}
		/*DBS_LOG_ALERT*/
		case 200904:		/* CBAT管理CPU负载过高告警以及恢复*/
		case 200905:		/* CBAT内存利用率过高告警*/
		case 200921:		/* CBAT down */
		{
			return DBS_LOG_ALERT;
		}
		/*DBS_LOG_CRIT*/
		case 200915:		/* auto-config pib */
		case 200916:		/* auto-config mod */
		{
			return DBS_LOG_CRIT;
		}
		/*DBS_LOG_ERR*/
		case 200910:		/* 自动配置失败事件*/
		case 200911:		/* INDEX重复告警以及恢复*/
		{
			return DBS_LOG_ERR;
		}
		/*DBS_LOG_WARNING*/
		case 200912:		/* 非法CNU注册告警*/
		case 200913:		/* CNU用户数量超限告警*/
		case 200917:		/* abort auto config */
		case 200922:		/* clt heartbeat loss */
		{
			return DBS_LOG_WARNING;
		}
		/*DBS_LOG_NOTICE*/
		case 200909:		/*  升级状态告警标识OID */
		case 200914:		/* block cnu */
		case 200918:		/* kick off cnu */
		case 200919:		/* force re-registration */
		case 200920:		/* CBAT ColdStart */
		{
			return DBS_LOG_NOTICE;
		}
		/*DBS_LOG_INFO*/
		case 200901:		/* CLT上下线*/
		case 200902:		/* CNU上下线*/
		case 200000:		/* 心跳TRAP-CBAT状态*/
		case 200001:		/* 心跳TRAP-CNU状态*/
		{
			return DBS_LOG_INFO;
		}
		/*DBS_LOG_DEBUG*/
		default:
		{
			return DBS_LOG_DEBUG;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevelStr
*	函数功能:获取字符串表示的告警等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmLevelStr(uint16_t alarmLevel)
{
	switch(alarmLevel)
	{
		case DBS_LOG_EMERG:
		{
			return "Emergency";
		}
		case DBS_LOG_ALERT:
		{
			return "Alert";
		}
		case DBS_LOG_CRIT:
		{
			return "Cratical";
		}
		case DBS_LOG_ERR:
		{
			return "Error";
		}
		case DBS_LOG_WARNING:
		{
			return "Warnning";
		}
		case DBS_LOG_NOTICE:
		{
			return "Notice";
		}
		case DBS_LOG_INFO:
		{
			return "Informational";
		}
		default:
		{
			return "Debugging";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevel
*	函数功能:根据告警码获取该告警的等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevel(st_dbsAlarmlog *alarm)
{
	switch(alarm->alarmCode)
	{
		/*Emergency*/
		case 200903:		/* 环境温度告警*/
		{
			if( ALARM_STS_NOMINAL == alarm->alarmType )
			{
				return DBS_LOG_NOTICE;
			}
			else if( ALARM_STS_HIHI == alarm->alarmType )
			{
				return DBS_LOG_ALERT;
			}
			else if( ALARM_STS_HI == alarm->alarmType )
			{
				return DBS_LOG_WARNING;
			}
			else if( ALARM_STS_LO == alarm->alarmType )
			{
				return DBS_LOG_WARNING;
			}
			else if( ALARM_STS_LOLO == alarm->alarmType )
			{
				return DBS_LOG_ALERT;
			}
			else
			{
				return DBS_LOG_WARNING;
			}
			break;
		}
		case 200906:		/* 噪声过高告警*/
		case 200907:		/* 链路衰减告警*/
		case 200908:		/* 物理层速率告警*/				
		case 200923:		/* can not find clt */
		{
#ifdef CFG_USE_PLATFORM_WR1004JL 
			return DBS_LOG_NOTICE;
#endif
#ifdef CFG_USE_PLATFORM_WR1004SJL 
			return DBS_LOG_NOTICE;
#endif
#ifdef CFG_USE_PLATFORM_WR1004JLD 
			return DBS_LOG_NOTICE;
#endif
			return DBS_LOG_EMERG;
		
		}
		/*DBS_LOG_ALERT*/
		case 200904:		/* CBAT管理CPU负载过高告警以及恢复*/
		case 200905:		/* CBAT内存利用率过高告警*/
		case 200921:		/* CBAT down */
		{
			return DBS_LOG_ALERT;
		}
		/*DBS_LOG_CRIT*/
		case 200915:		/* auto-config pib */
		case 200916:		/* auto-config mod */
		{
			return DBS_LOG_CRIT;
		}
		/*DBS_LOG_ERR*/
		case 200910:		/* 自动配置失败事件*/
		case 200911:		/* INDEX重复告警以及恢复*/
		{
			return DBS_LOG_ERR;
		}
		/*DBS_LOG_WARNING*/
		case 200912:		/* 非法CNU注册告警*/
		case 200913:		/* CNU用户数量超限告警*/
		case 200917:		/* abort auto config */
		case 200922:		/* clt heartbeat loss */
		{
			return DBS_LOG_WARNING;
		}
		/*DBS_LOG_NOTICE*/
		case 200909:		/*  升级状态告警标识OID */
		case 200914:		/* block cnu */
		case 200918:		/* kick off cnu */
		case 200919:		/* force re-registration */
		case 200920:		/* CBAT ColdStart */
		{
			return DBS_LOG_NOTICE;
		}
		/*DBS_LOG_INFO*/
		case 200901:		/* CLT上下线*/
		case 200902:		/* CNU上下线*/
		case 200000:		/* 心跳TRAP-CBAT状态*/
		case 200001:		/* 心跳TRAP-CNU状态*/
		{
			return DBS_LOG_INFO;
		}
		/*DBS_LOG_DEBUG*/
		default:
		{
			return DBS_LOG_DEBUG;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_setMTParameters
*	函数功能:烧录NVM参数的接口函数
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_setMTParameters(stMTmsgInfo *para)
{
	char mac[6];

	/* parse mac address */
	if( CMM_SUCCESS != boardapi_macs2b(para->mac, mac) )
	{
		printf("	ERROR: boardapi_setMTParameters mac address invalid\n");
		return CMM_FAILED;
	}
	else
	{
		sprintf(para->mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return nvm_set_mt_parameters(para);
	}	
} 

/********************************************************************************************
*	函数名称:boardapi_getCltDsdtPortid
*	函数功能:根据CLT索引号找到其对应的交换端口
*	作者:frank
*	时间:2013-08-19
*********************************************************************************************/
uint32_t boardapi_getCltDsdtPortid(uint32_t cltid)
{	
#ifdef CFG_USE_PLATFORM_WEC9720EK
	switch(cltid)
	{
		case 1:
		{
			return PORT_CABLE1_PORT_ID;
		}
		default:
		{
			return PORT_CABLE_PORT_NULL;
		}
	}
#endif

#ifdef CFG_USE_PLATFORM_WR1004JL
	switch(cltid)
	{
		case 1:
		{
			return PORT_CABLE1_PORT_ID;
		}
		default:
		{
			return PORT_CABLE_PORT_NULL;
		}
	}
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL
	switch(cltid)
	{
		case 1:
		{
			return PORT_CABLE1_PORT_ID;
		}
		case 2:
		{
			return PORT_CABLE2_PORT_ID;
		}
		case 3:
		{
			return PORT_CABLE3_PORT_ID;
		}
		case 4:
		{
			return PORT_CABLE4_PORT_ID;
		}
		default:
		{
			return PORT_CABLE_PORT_NULL;
		}
	}
#endif

#ifdef CFG_USE_PLATFORM_WR1004JLD
	switch(cltid)
	{
		case 1:
		{
			return PORT_CABLE1_PORT_ID;
		}
		case 2:
		{
			return  PORT_CABLE2_PORT_ID;
		}
		default:
		{
			return PORT_CABLE_PORT_NULL;
		}
	}
#endif

}  

/********************************************************************************************
*	函数名称:boardapi_isDsdtPortValid
*	函数功能:定义端口的有效属性
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
uint32_t boardapi_isDsdtPortValid(uint32_t portid)
{
	uint32_t ret = 0;
	uint32_t onused[CBAT_SW_PORT_NUM];

	if( portid >= CBAT_SW_PORT_NUM )
	{
		return 0;
	}

#ifdef CFG_USE_PLATFORM_WEC9720EK
	onused[0] = BOOL_TRUE;
	onused[1] = BOOL_TRUE;
	onused[2] = BOOL_FALSE;
	onused[3] = BOOL_FALSE;
	onused[4] = BOOL_FALSE;
	onused[5] = BOOL_TRUE;
	onused[6] = BOOL_TRUE;
#endif

#ifdef CFG_USE_PLATFORM_WR1004JL
	onused[0] = BOOL_TRUE;
	onused[1] = BOOL_TRUE;
	onused[2] = BOOL_TRUE;
	onused[3] = BOOL_TRUE;
	onused[4] = BOOL_TRUE;
	onused[5] = BOOL_TRUE;
	onused[6] = BOOL_FALSE;
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL
	onused[0] = BOOL_TRUE;
	onused[1] = BOOL_TRUE;
	onused[2] = BOOL_TRUE;
	onused[3] = BOOL_TRUE;
	onused[4] = BOOL_TRUE;
	onused[5] = BOOL_TRUE;
	onused[6] = BOOL_TRUE;
#endif

#ifdef CFG_USE_PLATFORM_WR1004JLD
	onused[0] = BOOL_TRUE;
	onused[1] = BOOL_TRUE;
	onused[2] = BOOL_TRUE;
	onused[3] = BOOL_TRUE;
	onused[4] = BOOL_TRUE;
	onused[5] = BOOL_TRUE;
	onused[6] = BOOL_TRUE;
#endif

	ret = onused[portid];
	return ret;
}

/********************************************************************************************
*	函数名称:boardapi_isDsdtPortValid
*	函数功能:定义端口的名称
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char *boardapi_getDsdtPortName(uint32_t portid)
{
	static char ret[32] = {0};
	char portName[CBAT_SW_PORT_NUM][32];

	if( portid >= CBAT_SW_PORT_NUM )
	{
		strcpy(ret, "NULL");
		return ret;
	}

#ifdef CFG_USE_PLATFORM_WEC9720EK
	strcpy(portName[0], "ETH1");
	strcpy(portName[1], "ETH2");
	strcpy(portName[2], "NULL");
	strcpy(portName[3], "NULL");
	strcpy(portName[4], "NULL");
	strcpy(portName[5], "MP");
	strcpy(portName[6], "CLT/1");
#endif

#ifdef CFG_USE_PLATFORM_WR1004JL
	strcpy(portName[0], "ORT");
	strcpy(portName[1], "ETH1");
	strcpy(portName[2], "ETH2");
	strcpy(portName[3], "ONU/1");
	strcpy(portName[4], "CLT/1");
	strcpy(portName[5], "MP");
	strcpy(portName[6], "NULL");
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL
	strcpy(portName[0], "ONU/ETH");
	strcpy(portName[1], "CLT/1");
	strcpy(portName[2], "CLT/2");
	strcpy(portName[3], "CLT/3");
	strcpy(portName[4], "CLT/4");
	strcpy(portName[5], "MP");
	strcpy(portName[6], "ORT");
#endif

#ifdef CFG_USE_PLATFORM_WR1004JLD
	strcpy(portName[0], "ONU/1");
	strcpy(portName[1], "CLT/2");
	strcpy(portName[2], "ETH1");
	strcpy(portName[3], "ETH2");
	strcpy(portName[4], "ORT");
	strcpy(portName[5], "MP");
	strcpy(portName[6], "CLT/1");
#endif

	strcpy(ret, portName[portid]);
	return ret;
}
  
