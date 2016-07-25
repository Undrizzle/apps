#include <sys/queue.h>

#define SNMP_CACHE_DEFAULT_TIMEOUT 5


#define IS_CACH_TIMEOUT(lasttime) \
do{ \
    if((lasttime) == 0)  \
        (lasttime) = time(NULL) - SNMP_CACHE_DEFAULT_TIMEOUT; \
    if (time(NULL)-(lasttime) < SNMP_CACHE_DEFAULT_TIMEOUT) return 0; \
    else (lasttime) = time(NULL); \
}while(0);

struct eoc_link_node {
	LIST_ENTRY(eoc_link_node) nodes;
	char *data;
};
	
void eoc_addList (char *l, char *data, int len, int refBytes);

typedef LIST_HEAD( ,eoc_link_node) eoc_list_t;
#pragma pack (push, 1)
typedef struct _onlinecnu_table_data {
	unsigned long master_index;
	unsigned long cnu_index;
	unsigned char mac[6];
	unsigned char model[256];
	unsigned long online;
	unsigned long ports;
	unsigned long author_state;
	unsigned char sw_ver[256];
	unsigned long reset;
	unsigned long rfatt;
	unsigned long rfdownq;
	unsigned long rfupq;
	unsigned long phydownrate;
	unsigned long phyuprate;
	unsigned long confsaving;
	unsigned long confresult;
	
	time_t cachTime;
}onlinecnu_table_data;

typedef struct _cnu_table_data {
	unsigned long master_index;
	unsigned long cnu_index;
	unsigned char mac[6];
	unsigned long devtype;
	unsigned long online;
	unsigned long reset;
	unsigned char sw_ver[256];
	unsigned long rfatt;
	unsigned long rfdownq;
	unsigned long rfupq;
	unsigned long phydownrate;
	unsigned long phyuprate;
	unsigned long usertype;
	unsigned long tempid;
	unsigned long accessenable;
	unsigned long autoupenable;
	unsigned long onused;
	
	time_t cachTime;
}cnu_table_data;

typedef struct _cnuport_table_data {
	unsigned long master_index;
	unsigned long cnu_index;
	unsigned long port_index;
	unsigned long port_enable;
	unsigned long port_service;
	unsigned long vlantag_enable;
	unsigned long vlanmarkid;

	time_t cachTime;
}cnuport_table_data;

/* �Զ��������������Ϣ�ֶ�*/
typedef struct
{
	unsigned long isValid;			/* ��ʶ�������ֶ��Ƿ���Ч*/
	unsigned long MacLimitEn;		/* 0=Disabled; 1=Enabled;*/
	unsigned long MacLimitNum;	/* Limit number of Source Addresses allowed to transmit through Slave device;*/
}stPibMacLimit_m;

typedef struct
{
	unsigned long isValid;			/* ��ʶ�������ֶ��Ƿ���Ч*/
	unsigned long BroadStormEn;	/* �㲥�������ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	unsigned long UniStormEn;		/* δ֪�������ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	unsigned long MultiStormEn;		/* δ֪�鲥���ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	unsigned long StormRateLevel;	/* Storm Filter�������Ƶȼ���1��11��*/
}stModStormFilter_m;

typedef struct
{
	unsigned long isValid;		/* ��ʶ�������ֶ��Ƿ���Ч*/
	unsigned long VlanEnable;	/* VLAN����ʹ�ܡ�0�����ã�1�����á�*/
	unsigned long vid[4];		/* �ö˿ڵ�PVID��1��4094��*/
}stModVlan_m;

typedef struct
{
	unsigned long isValid;	/* ��ʶ�������ֶ��Ƿ���Ч*/
	unsigned long pri[4];		/* Port 0�˿����ȼ���0��3��*/	
}stModPortPri_m;

typedef struct
{
	unsigned long isValid;		/* ��ʶ�������ֶ��Ƿ���Ч*/
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate limit for ingress. 
	** if these bits are set to 0, no frame should be received in from this port.*/
	unsigned long rxRate[4];	/* Ingress Rate Limit.*/
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate
	** limit for egress. If these bits are set to13'h0, no egress frame should 
	** be send out from this port.*/
	unsigned long txRate[4];	/* Egress Rate Limit.*/
}stModRateLimit_m;

typedef struct
{
	unsigned long isValid;		/* ��ʶ�������ֶ��Ƿ���Ч*/
	unsigned long portStatus[4];	/* Port 0�˿�״̬ʹ�ܡ�0�����ã�1�����á�*/
}stModPortEnable_m;

/* ���ݱ������Ϣ�ֶ�*/
typedef struct
{
	unsigned long id;			/* template id*/
	unsigned long idle;			/* ��ʶ������ģ���Ƿ��Ѿ���������*/
	unsigned long enable;		/* ��ʶ������ģ���Ƿ�����*/
	unsigned long DevType;		/* ��ʶ������ģ����ݵ�Ӳ������*/
	unsigned long base;			/* ��ʶ������ģ���BASE PIB */
	unsigned long btime;		/* ��ʶ������ģ��ı���ʱ��*/
	unsigned char tname[128];	/* ������ģ�����������*/	
}st_tm_head_m;

/* PIBҵ�������ֶ�*/
typedef struct
{
	stPibMacLimit_m pib_spec;
}st_pib_spec_m;

/* �Զ��������������Ϣ�ֶ�*/
typedef struct
{
	stModStormFilter_m stormFilter;
	stModVlan_m vlan;
	stModPortPri_m portPri;
	stModRateLimit_m rateLimit;
	stModPortEnable_m portAuth;
}st_mod_spec_m;

typedef struct _template_table_data
{
	st_tm_head_m tm_info;	
	st_pib_spec_m pib;	
	st_mod_spec_m mod;	

	time_t cachTime;
}template_table_data;

typedef struct writeMethod_struct_tag
{
    unsigned char ucVar_expect_type; /*destionation OID type we defined, from vp*/
    unsigned short usAccessType; /*if this var is writable, from vp*/    

    unsigned char ucDataType;  /*the type of data to be written:1 long, 0 charpointer*/
    union
    {
	    struct /*if destination type is long, we specify its min and max value*/
	    {
	        long lMin;
	        long lMax;
	        long *pulDest; /*pointer to room where new value(long) will be saved, if it is NULL, we set value to CMM*/
	    } longRange;
	    struct /*if destination type is char*, we specify its varaible size*/
	    {
	        unsigned long ulValueSize; /*destination room size if pszDest isn't NULL, or Node size defined in config.xml if pszDest is NULL*/
	        long ulReserve;
	        char *pszDest; /*pointer to room where new value(char*) will be saved, if it is NULL, we set value to CMM*/
	    }charpSize;
    }range;

    char **pv;
    int pvLen;
    
    char *pszNodeWholePath; /*Pointer to the node path for which we will set new value to CMM*/
}writeMethod_struct_data;

#pragma pack (pop)


