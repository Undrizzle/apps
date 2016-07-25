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

/* 自定义参数块配置信息字段*/
typedef struct
{
	unsigned long isValid;			/* 标识该配置字段是否有效*/
	unsigned long MacLimitEn;		/* 0=Disabled; 1=Enabled;*/
	unsigned long MacLimitNum;	/* Limit number of Source Addresses allowed to transmit through Slave device;*/
}stPibMacLimit_m;

typedef struct
{
	unsigned long isValid;			/* 标识该配置字段是否有效*/
	unsigned long BroadStormEn;	/* 广播报文抑制功能使能【0：禁用，1：启用】*/
	unsigned long UniStormEn;		/* 未知单播抑制功能使能【0：禁用，1：启用】*/
	unsigned long MultiStormEn;		/* 未知组播抑制功能使能【0：禁用，1：启用】*/
	unsigned long StormRateLevel;	/* Storm Filter速率抑制等级【1～11】*/
}stModStormFilter_m;

typedef struct
{
	unsigned long isValid;		/* 标识该配置字段是否有效*/
	unsigned long VlanEnable;	/* VLAN功能使能【0：禁用，1：启用】*/
	unsigned long vid[4];		/* 该端口的PVID【1～4094】*/
}stModVlan_m;

typedef struct
{
	unsigned long isValid;	/* 标识该配置字段是否有效*/
	unsigned long pri[4];		/* Port 0端口优先级【0～3】*/	
}stModPortPri_m;

typedef struct
{
	unsigned long isValid;		/* 标识该配置字段是否有效*/
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
	unsigned long isValid;		/* 标识该配置字段是否有效*/
	unsigned long portStatus[4];	/* Port 0端口状态使能【0：禁用，1：启用】*/
}stModPortEnable_m;

/* 数据表基本信息字段*/
typedef struct
{
	unsigned long id;			/* template id*/
	unsigned long idle;			/* 标识该配置模板是否已经导入数据*/
	unsigned long enable;		/* 标识该配置模板是否启用*/
	unsigned long DevType;		/* 标识该配置模板兼容的硬件类型*/
	unsigned long base;			/* 标识该配置模板的BASE PIB */
	unsigned long btime;		/* 标识该配置模板的编译时间*/
	unsigned char tname[128];	/* 该配置模板的名称描述*/	
}st_tm_head_m;

/* PIB业务配置字段*/
typedef struct
{
	stPibMacLimit_m pib_spec;
}st_pib_spec_m;

/* 自定义参数块配置信息字段*/
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


