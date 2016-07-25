#ifndef __WEC_BOARD_API_H__
#define __WEC_BOARD_API_H__

#include <public.h>
#include "nvm-utils.h"
#include "hexdump.h"
#include "md5.h"

/********************************************************************************************
*	��������:boardapi_checkCpuEndian
*	��������:�жϴ��������ֽ����Ǵ�˻���С��
*	return true: little-endian, return false: big-endian
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_checkCpuEndian(void);

/********************************************************************************************
*	��������:boardapi_getMacAddress
*	��������:��ȡCBAT��MAC��ַ
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char * boardapi_getMacAddress(void);

/********************************************************************************************
*	��������:boardapi_isValidUnicastMacb
*	��������:�ж��Ƿ�Ϊ��Ч�ĵ���MAC
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_isValidUnicastMacb(uint8_t *bin);

/********************************************************************************************
*	��������:boardapi_macs2b
*	��������:���ַ�����ʽ��MAC��ַת��Ϊ6λ�����Ƹ�ʽ
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_macs2b(const char *str, uint8_t *bin);

/********************************************************************************************
*	��������:boardapi_mac2Uppercase
*	��������:���ַ�����ʽ��MAC��ַת��Ϊ��д��ʽ���ַ���MAC��ַ
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_mac2Uppercase(char *strmac);

/********************************************************************************************
*	��������:boardapi_getDeviceModelStr
*	��������:��ȡ�ַ�����ʾ���豸�ͺ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char * boardapi_getDeviceModelStr(uint32_t model);

/********************************************************************************************
*	��������:boardapi_getCltStandardStr
*	��������:get clt serial type
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
const char *boardapi_getCltStandardStr(void);

/********************************************************************************************
*	��������:boardapi_getMenufactoryStr
*	��������:get clt serial type
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
const char *boardapi_getMenufactoryStr(void);

/********************************************************************************************
*	��������:boardapi_getModNameStr
*	��������:����ģ��ID��ȡ�ַ�����ʾ��ģ������
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char * boardapi_getModNameStr(uint16_t mid);

/********************************************************************************************
*	��������:boardapi_getCnuHfid
*	��������:�����豸�ͺŻ�ȡ��¼��PIB�д洢��HFID
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
const char *boardapi_getCnuHfid(uint32_t devType);

/********************************************************************************************
*	��������:boardapi_isCnuSupported
*	��������:����������豸�ͺ��ж�ϵͳ�Ƿ�֧�ָ��豸
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_isCnuSupported(uint32_t DevType);
int boardapi_isKTCnu(uint32_t DevType);
int boardapi_isAr6400Device(uint32_t DevType);
int boardapi_isAr7400Device(uint32_t DevType);
int boardapi_getCnuSwitchType(uint32_t DevType);
int boardapi_isCnuTrusted(uint32_t DevType);

/********************************************************************************************
*	��������:boardapi_mapDevModel
*	��������:��CBAT�ж�����豸�ͺ�ӡ��ΪNMS������豸�ͺ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_mapDevModel(int model);

/********************************************************************************************
*	��������:boardapi_umapDevModel
*	��������:��NMS������豸�ͺ�ӡ��ΪCBAT�ж�����豸�ͺ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_umapDevModel(int model);

/********************************************************************************************
*	��������:boardapi_getAlarmTypeStr
*	��������:��ȡ�ַ�����ʾ�ĸ澯����
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmTypeStr(uint16_t alarmType);

/********************************************************************************************
*	��������:boardapi_getAlarmLevelByCode
*	��������:���ݸ澯���ȡ�ø澯�ĵȼ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevelByCode(uint32_t alarmCode);

/********************************************************************************************
*	��������:boardapi_getAlarmLevelStr
*	��������:��ȡ�ַ�����ʾ�ĸ澯�ȼ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmLevelStr(uint16_t alarmLevel);

/********************************************************************************************
*	��������:boardapi_getAlarmLevel
*	��������:���ݸ澯���ȡ�ø澯�ĵȼ�
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevel(st_dbsAlarmlog *alarm);

/********************************************************************************************
*	��������:boardapi_setMTParameters
*	��������:��¼NVM�����Ľӿں���
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
int boardapi_setMTParameters(stMTmsgInfo *para);

/********************************************************************************************
*	��������:boardapi_getCltDsdtPortid
*	��������:����CLT�������ҵ����Ӧ�Ľ����˿�
*	����:frank
*	ʱ��:2013-08-19
*********************************************************************************************/
uint32_t boardapi_getCltDsdtPortid(uint32_t cltid);

/********************************************************************************************
*	��������:boardapi_isDsdtPortValid
*	��������:����˿ڵ���Ч����
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
uint32_t boardapi_isDsdtPortValid(uint32_t portid);

/********************************************************************************************
*	��������:boardapi_isDsdtPortValid
*	��������:����˿ڵ�����
*	����:frank
*	ʱ��:2010-08-19
*********************************************************************************************/
char *boardapi_getDsdtPortName(uint32_t portid);

#endif 