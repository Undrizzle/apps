#include <assert.h>
#include "auto_cmm.h"
#include "auto_dbs.h"

int __auto2cmm_comm(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
{
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t msgType = req->HEADER.usMsgType;
	T_REQ_Msg_CMM *ack = NULL;

	fd_set fdsr;
	//int maxsock;
	struct timeval tv;	
	int ret = 0;
	int sendn = 0;	
	struct sockaddr_in from;	
	int FromAddrSize = 0;
	int rev_len = 0;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		return CMM_FAILED;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 18;
		tv.tv_usec = 0;

		//¼ì²âsocket
		ret = select(sk->sk + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			return CMM_FAILED;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				return CMM_FAILED;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{
					fprintf(stderr, "WARNNING: sysMonitor2cmm: msgType[%d!=%d], [continue] !\n", 
						ack->HEADER.usMsgType, msgType);
					continue;
				}
				else if( ack->result != CMM_SUCCESS )
				{	
					return  ack->result ;
				}
				else
				{
					return ack->result;
				}
			}
		}
		else
		{
			return CMM_FAILED;
		}
	}
}

int auto2cmm_writeSwitchSettings(T_UDP_SK_INFO *sk, stCnuNode *node, st_rtl8306eSettings * rtl8306e)
{
    uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	int i = 0;
	uint8_t tmp;
	int opt_sts=CMM_SUCCESS;

	
	T_Msg_CMM* req = (T_Msg_CMM *)buf;
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);
	
	/*add by stan end  */
	req_data->node.clt = node->clt;
	req_data->node.cnu = node->cnu;
	


	/* add by stan for save dbs  begin */
	st_dbsProfile profile;

	//update db
	if(CMM_SUCCESS != dbsGetProfile(dbsdev, (node->clt-1) * 64 + node->cnu, &profile))
	{
		opt_sts = CMM_FAILED;
	}
	profile.col_vlanSts= rtl8306e->vlanConfig.vlan_enable; 
	
	profile.col_eth1VMode = rtl8306e->vlanConfig.vlan_port[0].egress_mode;
	profile.col_eth1vid = rtl8306e->vlanConfig.vlan_port[0].pvid;
	
	profile.col_eth2VMode = rtl8306e->vlanConfig.vlan_port[1].egress_mode;
	profile.col_eth2vid = rtl8306e->vlanConfig.vlan_port[1].pvid;

	profile.col_eth3VMode = rtl8306e->vlanConfig.vlan_port[2].egress_mode;
	profile.col_eth3vid = rtl8306e->vlanConfig.vlan_port[2].pvid;
	
	profile.col_eth4VMode = rtl8306e->vlanConfig.vlan_port[3].egress_mode;
	profile.col_eth4vid = rtl8306e->vlanConfig.vlan_port[3].pvid;

	profile.col_uplinkVMode = rtl8306e->vlanConfig.vlan_port[4].egress_mode;
	profile.col_uplinkvid = rtl8306e->vlanConfig.vlan_port[4].pvid;

		
     /* add by stan for save dbs  end */

	req->HEADER.usSrcMID = MID_AUTOCONFIG;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_WRITE;
	req->HEADER.ulBodyLength = sizeof(rtl8306eWriteInfo) + sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	/*add by stan */
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		printf("add by stan too len rtl8306 body + profile len\n");
		return CMM_FAILED;
	}

    memcpy(&(req_data->rtl8306eConfig), rtl8306e, sizeof(st_rtl8306eSettings));
	memcpy(req->BUF + sizeof(rtl8306eWriteInfo), &profile, sizeof(st_dbsProfile));

	
	
	
	return __auto2cmm_comm(sk, buf, len);

}



int auto2cmm_destroy(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int auto2cmm_init(T_UDP_SK_INFO *sk)
{
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_FAILED;
	}	

	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* Ä¿µÄ¶Ë¿ÚºÅ*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return CMM_SUCCESS;
} 


