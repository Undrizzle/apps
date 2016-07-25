#include <assert.h>
#include <dbsapi.h>
#include <sqlite3.h>

//static T_UDP_SK_INFO dbsapiFd;
//static uint16_t dbsSrcMod = 0;
//static uint8_t gBuf_dbsApi[MAX_UDP_SIZE] = {0};

int __dbsCommunicate(T_DBS_DEV_INFO *dev)
{
	assert( NULL != dev );
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);
	uint16_t msgType = req->HEADER.usMsgType;
	
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	
	int ret = 0;
	int sendn = 0;
	
	T_DB_MSG_HEADER_ACK *r = NULL;
	struct sockaddr_in from;
	
	int FromAddrSize = 0;
	//int rev_len = 0;

	sendn = sendto(dev->channel.sk, dev->buf, dev->blen, 0, (struct sockaddr *)&(dev->channel.skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->sendto !\n", dev->srcmod);
		return -1;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(dev->channel.sk, &fdsr);

		// timeout setting
		tv.tv_sec = 3;
		tv.tv_usec = 0;

		maxsock = dev->channel.sk;
		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->select[msgType=%d] !\n", dev->srcmod, msgType);
			return -1;
		}
		
		// check whether a new connection comes
		if (FD_ISSET(dev->channel.sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			dev->blen = recvfrom(dev->channel.sk, dev->buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

			if ( -1 == dev->blen )
			{
				fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->recvfrom !\n", dev->srcmod);
				return -1;
			}
			else
			{
				r = (T_DB_MSG_HEADER_ACK *)(dev->buf);
				if( msgType != r->usMsgType )
				{
					fprintf(stderr, "WARNNING: MOD[%d] dbsCommunicate: msgType[%d!=%d], [continue] !\n", 
						dev->srcmod, r->usMsgType, msgType);
					continue;
				}
				else if( MID_DBS != r->usSrcMID )
				{
					fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate: usSrcMID[%d!=MID_DBS], [continue] !\n", 
						dev->srcmod, r->usSrcMID);
					continue;
				}
				else if( r->result )
				{
					if( msgType == DB_GET_SYSLOG )
					{
						return r->result;
					}
					else if( msgType == DB_GET_OPTLOG )
					{
						return r->result;
					}
					else if( msgType == DB_GET_ALARMLOG )
					{
						return r->result;
					}
					else
					{
						fprintf(stderr, "\r\n  ERROR: MOD[%d] dbsCommunicate: result [%d] !\n", dev->srcmod, r->result);
						return r->result;
					}
				}
				else
				{
					return r->result;
				}
			}			
		}
		else
		{
			fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->FD_ISSET !\n", dev->srcmod);
			return -1;
		}
	}
}

BOOLEAN __isDbsModuleOK(T_DBS_DEV_INFO *dev, uint16_t mid)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);
	uint16_t *ack_ms = (uint16_t *)(ack->BUF);	
	
	if( mid > MAX_MODULE_NUMS )
	{
		fprintf(stderr, "WARNING: dbsRegisterModule mid[%d] is out of range !\n", mid);
		return BOOL_FALSE;
	}	

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_MODULE_STS;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	
	*req_mid = mid;
	
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( CMM_SUCCESS == __dbsCommunicate(dev) )
	{
		return (*ack_ms)?BOOL_TRUE:BOOL_FALSE;
	}
	else
	{
		return BOOL_FALSE;
	}	
}

int __dbsRegisterModule(T_DBS_DEV_INFO *dev)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_REGISTER_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = dev->srcmod;
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int __dbsDestroyModule(T_DBS_DEV_INFO *dev)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = dev->srcmod;
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int dbsSelectCnuIndexByMacAddress(T_DBS_DEV_INFO *dev, char *mac, stCnuNode *index)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint8_t *req_cell = (uint8_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	stCnuNode *ack_cell = (stCnuNode *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_SELECT_CNU_INDEX_BY_MAC;
	req->HEADER.ulBodyLength = strlen(mac);

	memcpy(req_cell, mac, req->HEADER.ulBodyLength);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(index, ack_cell, sizeof(stCnuNode));	
	}
	return ack->HEADER.result;
}

int dbsGetInteger(T_DBS_DEV_INFO *dev, DB_INTEGER_V *v)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	assert( NULL != v );
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	DB_COL_INFO *ci = (DB_COL_INFO *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);
	DB_COL_VAR *ack_cell = (DB_COL_VAR *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_INFO);

	ci->tbl = v->ci.tbl;
	ci->row = v->ci.row;
	ci->col = v->ci.col;
	ci->colType = SQLITE_INTEGER;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( 0 == __dbsCommunicate(dev) )
	{
		v->ci.colType = ack_cell->ci.colType;
		
		if( SQLITE_NULL == v->ci.colType )
		{
			v->len = 0;
			v->integer = 0;
		}
		else
		{
			v->len = ack_cell->len;
			v->integer = *(uint32_t *)(ack_cell->data);
		}
	}
	return ack->HEADER.result;
}

int dbsGetText(T_DBS_DEV_INFO *dev, DB_TEXT_V *v)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	DB_COL_INFO *ci = (DB_COL_INFO *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	DB_COL_VAR *ack_cell = (DB_COL_VAR *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_INFO);	

	ci->tbl = v->ci.tbl;
	ci->row = v->ci.row;
	ci->col = v->ci.col;
	ci->colType = SQLITE3_TEXT;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		v->ci.colType = ack_cell->ci.colType;
		if( SQLITE_NULL == v->ci.colType )
		{
			v->len = 0;
			v->text[0] = '\0';
		}
		else
		{
			if( ack_cell->len >= DBS_COL_MAX_LEN )
			{
				v->len = 0;
				v->text[0] = '\0';
				return CMM_FAILED;
			}
			else
			{
				v->len = ack_cell->len;
				strncpy(v->text, ack_cell->data, ack_cell->len);
				v->text[v->len] = '\0';
			}
		}		
	}
	return ack->HEADER.result;
}

int dbsGetCliRole(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCliRole *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsCliRole *ack_cell = (st_dbsCliRole *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLI_ROLE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCliRole));	
	}
	return ack->HEADER.result;
}

int dbsGetClt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsClt *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsClt *ack_cell = (st_dbsClt *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsClt));	
	}
	return ack->HEADER.result;
}

int dbsGetCltconf(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCltConf *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsCltConf *ack_cell = (st_dbsCltConf *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCltConf));	
	}
	return ack->HEADER.result;
}

int dbsGetCnu(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnu *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsCnu *ack_cell = (st_dbsCnu *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CNU;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCnu));	
	}
	return ack->HEADER.result;
}

int dbsGetDepro(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnuDefaultProfile *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsCnuDefaultProfile *ack_cell = (st_dbsCnuDefaultProfile *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_DEPRO;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCnuDefaultProfile));	
	}
	return ack->HEADER.result;
}

int dbsGetTemplate(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsTemplate  *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsTemplate *ack_cell = (st_dbsTemplate *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_TEMPLATE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsTemplate));	
	}
	return ack->HEADER.result;
}


int dbsGetNetwork(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsNetwork *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsNetwork *ack_cell = (st_dbsNetwork *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_NETWORK;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsNetwork));	
	}
	return ack->HEADER.result;
}

int dbsGetProfile(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsProfile *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsProfile *ack_cell = (st_dbsProfile *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsProfile));	
	}
	return ack->HEADER.result;
}

int dbsGetSnmp(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSnmp *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsSnmp *ack_cell = (st_dbsSnmp *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SNMP;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSnmp));	
	}
	return ack->HEADER.result;
}

int dbsGetSwmgmt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSwmgmt *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsSwmgmt *ack_cell = (st_dbsSwmgmt *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SWMGMT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSwmgmt));	
	}
	return ack->HEADER.result;
}

int dbsGetSysinfo(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSysinfo *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);	
	st_dbsSysinfo *ack_cell = (st_dbsSysinfo *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SYSINFO;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSysinfo));	
	}
	return ack->HEADER.result;
}

int dbsUpdateInteger(T_DBS_DEV_INFO *dev, DB_INTEGER_V *v)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	assert( NULL != v );
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	DB_COL_VAR *req_cell = (DB_COL_VAR *)(req->BUF);

	req_cell->ci.tbl = v->ci.tbl;
	req_cell->ci.row = v->ci.row;
	req_cell->ci.col = v->ci.col;
	req_cell->ci.colType = SQLITE_INTEGER;
	req_cell->len = sizeof(uint32_t);
	*(uint32_t *)(req_cell->data) = v->integer;	

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_VAR) + req_cell->len;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;	

	return __dbsCommunicate(dev);
}

int dbsSetDsdtRgmiiDelay(T_DBS_DEV_INFO *dev, st_dsdtRgmiiTimingDelay *dsdtRgmiiTimingDelay)
{
	assert( NULL != dev );
	
	DB_INTEGER_V iv;
	
	iv.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iv.ci.row = 1;
	iv.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD;
	iv.ci.colType = SQLITE_INTEGER;
	iv.len = sizeof(uint32_t);
	iv.integer = dsdtRgmiiTimingDelay->rxdelay;
	if( CMM_SUCCESS != dbsUpdateInteger(dev, &iv) )
	{
		return CMM_FAILED;
	}

	iv.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iv.ci.row = 1;
	iv.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD;
	iv.ci.colType = SQLITE_INTEGER;
	iv.len = sizeof(uint32_t);
	iv.integer = dsdtRgmiiTimingDelay->txdelay;
	return dbsUpdateInteger(dev, &iv);
}

int dbsUpdateText(T_DBS_DEV_INFO *dev, DB_TEXT_V *v)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	DB_COL_VAR *req_cell = (DB_COL_VAR *)(req->BUF);
	
	if( strlen(v->text) >= DBS_COL_MAX_LEN )
	{
		return CMM_FAILED;
	}

	req_cell->ci.tbl = v->ci.tbl;
	req_cell->ci.row = v->ci.row;
	req_cell->ci.col = v->ci.col;
	req_cell->ci.colType = SQLITE3_TEXT;
	req_cell->len = strlen(v->text);
	strcpy(req_cell->data, v->text);	

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_VAR) + req_cell->len;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(dev);
}


int dbsUpdateCliRole(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCliRole *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsCliRole *req_cell = (st_dbsCliRole *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLI_ROLE;
	req->HEADER.ulBodyLength = sizeof(st_dbsCliRole);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateCliRole: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCliRole));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateClt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsClt *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsClt *req_cell = (st_dbsClt *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLT;
	req->HEADER.ulBodyLength = sizeof(st_dbsClt);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateClt: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsClt));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateCltconf(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCltConf *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsCltConf *req_cell = (st_dbsCltConf *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(st_dbsCltConf);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateCltconf: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCltConf));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateCnu(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnu *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsCnu *req_cell = (st_dbsCnu *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CNU;
	req->HEADER.ulBodyLength = sizeof(st_dbsCnu);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateCnu: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCnu));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateDepro(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnuDefaultProfile *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsCnuDefaultProfile *req_cell = (st_dbsCnuDefaultProfile *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_DEPRO;
	req->HEADER.ulBodyLength = sizeof(st_dbsCnuDefaultProfile);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateDepro: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCnuDefaultProfile));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateNetwork(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsNetwork *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsNetwork *req_cell = (st_dbsNetwork *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_NETWORK;
	req->HEADER.ulBodyLength = sizeof(st_dbsNetwork);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateNetwork: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsNetwork));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateProfile(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsProfile *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsProfile *req_cell = (st_dbsProfile *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_PROFILE;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateProfile: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsProfile));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateTemplate(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsTemplate *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsTemplate *req_cell = (st_dbsTemplate *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_TEMPLATE;
	req->HEADER.ulBodyLength = sizeof(st_dbsTemplate);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateTemplate: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsTemplate));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateSnmp(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSnmp *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsSnmp *req_cell = (st_dbsSnmp *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SNMP;
	req->HEADER.ulBodyLength = sizeof(st_dbsSnmp);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateSnmp: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSnmp));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateSwmgmt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSwmgmt *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsSwmgmt *req_cell = (st_dbsSwmgmt *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SWMGMT;
	req->HEADER.ulBodyLength = sizeof(st_dbsSwmgmt);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateSwmgmt: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSwmgmt));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsUpdateSysinfo(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSysinfo *row)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsSysinfo *req_cell = (st_dbsSysinfo *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SYSINFO;
	req->HEADER.ulBodyLength = sizeof(st_dbsSysinfo);

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( dev->blen > MAX_UDP_SIZE )
	{
		dbs_sys_log(dev, DBS_LOG_ALERT, "dbsUpdateSysinfo: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSysinfo));
		req_cell->id = id;	
		return __dbsCommunicate(dev);
	}
}

int dbsDestroyRowClt(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CLT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsDestroyRowCltconf(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsDestroyRowCnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CNU;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsDestroyRowProfile(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateSuProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_SU_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateDewlProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_WL_DE_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateDeblProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_BL_DE_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateSuProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_SU2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateDewlProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_WL_DE2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);	
}

int dbsCreateDeblProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_BL_DE2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int dbsLogCount(T_DBS_DEV_INFO *dev, uint16_t tbl, uint32_t *n)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	assert( NULL != n );

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *v = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_LOG_COUNT;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*v = tbl;
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	*n = 0;
	
	if( 0 == __dbsCommunicate(dev) )
	{
		*n = *(uint32_t *)(ack->BUF);
	}
	return ack->HEADER.result;
}

int dbsGetSyslog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsSyslog *log)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);
	st_dbsSyslog *ack_data = (st_dbsSyslog *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_SYSLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(log, ack_data, sizeof(st_dbsSyslog));
	}
	return ack->HEADER.result;
}

int dbsGetOptlog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsOptlog *log)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);
	st_dbsOptlog *ack_data = (st_dbsOptlog *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_OPTLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(log, ack_data, sizeof(st_dbsOptlog));
	}
	return ack->HEADER.result;
}

int dbsGetAlarmlog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsAlarmlog *log)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)(dev->buf);
	st_dbsAlarmlog *ack_data = (st_dbsAlarmlog *)(ack->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ALARMLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(dev) )
	{
		memcpy(log, ack_data, sizeof(st_dbsAlarmlog));
	}
	return ack->HEADER.result;
}

int dbs_sys_log(T_DBS_DEV_INFO *dev, uint32_t priority, const char *message)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	time_t b_time;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	st_dbsSyslog *pLog = (st_dbsSyslog *)(req->BUF);

	assert( NULL != message );

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_SYSLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsSyslog);

	time(&b_time);
	pLog->time = b_time;
	pLog->who = dev->srcmod;
	pLog->level = priority;
	
	if( strlen(message) < DBS_COL_MAX_LEN )
	{
		strcpy(pLog->log, message);
	}
	else
	{
		pLog->level = DBS_LOG_ERR;
		sprintf(pLog->log, "module write syslog length[%d]", strlen(message));
	}	

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(dev);
}

int dbs_opt_log(T_DBS_DEV_INFO *dev, st_dbsOptlog *log)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);		
	st_dbsOptlog *pLog = (st_dbsOptlog *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_OPTLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsOptlog);

	memcpy(pLog, log, req->HEADER.ulBodyLength);
	
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int dbs_alarm_log(T_DBS_DEV_INFO *dev, st_dbsAlarmlog *log)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);		
	st_dbsAlarmlog *pLog = (st_dbsAlarmlog *)(req->BUF);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_ALARMLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsAlarmlog);

	memcpy(pLog, log, req->HEADER.ulBodyLength);
	
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int dbsRegisterModuleById(T_DBS_DEV_INFO *dev, uint16_t mid)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	if( mid > MAX_MODULE_NUMS )
	{
		return -1;
	}

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_REGISTER_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = mid;
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

int dbsDestroyModuleById(T_DBS_DEV_INFO *dev, uint16_t mid)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	if( mid > MAX_MODULE_NUMS )
	{
		return -1;
	}

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = mid;
	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(dev);
}

void dbsWaitModule(T_DBS_DEV_INFO *dev, uint32_t MF)
{	
	assert( NULL != dev );
	
	int i = 0;
	BOOLEAN ms = BOOL_FALSE;

	for( i=0; i<MAX_MODULE_NUMS; i++ )
	{
		if( MF&(1<<i) )
		{
			do
			{				
				ms = __isDbsModuleOK(dev, i+1);
				if( BOOL_FALSE == ms )
				{
					//fprintf(stderr, "WARNING: MID[%d] dbsWaitModule MID[%d] Failed, Continue !\n", dev->srcmod, i+1);
					usleep(200000);
				}
				else
				{
					//fprintf(stderr, "INFO: MID[%d] dbsWaitModule MID[%d] Success !\n", dev->srcmod, i+1);
					usleep(2000);
				}
			}
			while( BOOL_FALSE == ms );
		}
	}
	
}

BOOLEAN dbsGetModuleStatus(T_DBS_DEV_INFO *dev, uint16_t mid)
{
	return __isDbsModuleOK(dev, mid);
}

int dbsMsgDebug(T_DBS_DEV_INFO *dev, uint32_t status)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = (status?DB_MSG_DEBUG_ENABLE:DB_MSG_DEBUG_DISABLE);
	req->HEADER.ulBodyLength = 0;

	dev->blen = sizeof(req->HEADER);
	
	return __dbsCommunicate(dev);
}

int dbsSQLDebug(T_DBS_DEV_INFO *dev, uint32_t status)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = (status?DB_SQL_DEBUG_ENABLE:DB_SQL_DEBUG_DISABLE);
	req->HEADER.ulBodyLength = 0;

	dev->blen = sizeof(req->HEADER);
	
	return __dbsCommunicate(dev);
}

int dbsFflush(T_DBS_DEV_INFO *dev)
{
	assert( NULL != dev );
	
	bzero(dev->buf, MAX_UDP_SIZE);
	dev->blen = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(dev->buf);

	req->HEADER.usSrcMID = dev->srcmod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_ONLINE_BACKUP;
	req->HEADER.ulBodyLength = 0;

	dev->blen = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(dev);
}

T_DBS_DEV_INFO * dbsNoWaitOpen(uint16_t mid)
{
	BOOLEAN ms = BOOL_FALSE;
	T_DBS_DEV_INFO *dev = NULL;	

	dev = (T_DBS_DEV_INFO *)malloc(sizeof(T_DBS_DEV_INFO));

	if( NULL == dev )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsNoWaitOpen no memery\n", mid);
		return NULL;
	}
	
	/*创建外部SOCKET  接口*/
	if( ( dev->channel.sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsNoWaitOpen socket error\n", mid);
		free(dev);
		return NULL;
	}
	
	dev->channel.skaddr.sin_family = PF_INET;
	dev->channel.skaddr.sin_port = htons(DBS_LISTEN_PORT);
	dev->channel.skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	dev->srcmod = mid;
	dev->blen = 0;
	bzero(dev->buf, MAX_UDP_SIZE);

	do
	{
		ms = __isDbsModuleOK(dev, MID_DBS);
		if( BOOL_FALSE == ms )
		{
			usleep(300000);
		}
	}
	while( BOOL_FALSE == ms );

	if( CMM_SUCCESS == __dbsRegisterModule(dev) )
	{
		//fprintf(stderr, "INFO: MOD[%d] dbsOpen success !\n", dev->srcmod);
		return dev;
	}
	else
	{
		fprintf(stderr, "ERROR: MOD[%d] register error\n", mid);
		free(dev);
		return NULL;
	}
}

T_DBS_DEV_INFO * dbsOpen(uint16_t mid)
{
	BOOLEAN ms = BOOL_FALSE;
	T_DBS_DEV_INFO *dev = NULL;	

	dev = (T_DBS_DEV_INFO *)malloc(sizeof(T_DBS_DEV_INFO));

	if( NULL == dev )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsOpen no memery\n", mid);
		return NULL;
	}

	/*创建外部SOCKET  接口*/
	if( ( dev->channel.sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsNoWaitOpen socket error\n", mid);
		free(dev);
		return NULL;
	}
	
	dev->channel.skaddr.sin_family = PF_INET;
	dev->channel.skaddr.sin_port = htons(DBS_LISTEN_PORT);
	dev->channel.skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	dev->srcmod = mid;
	dev->blen = 0;
	bzero(dev->buf, MAX_UDP_SIZE);

	/* wait for dbs starting */
	sleep(3);
	do
	{
		ms = __isDbsModuleOK(dev, MID_DBS);
		if( BOOL_FALSE == ms )
		{
			usleep(300000);
		}
	}
	while( BOOL_FALSE == ms );

	if( CMM_SUCCESS == __dbsRegisterModule(dev) )
	{
		//fprintf(stderr, "INFO: MOD[%d] dbsOpen success !\n", dbsSrcMod);
		return dev;
	}
	else
	{
		fprintf(stderr, "ERROR: MOD[%d] register error\n", mid);
		free(dev);
		return NULL;
	}
}

int dbsClose(T_DBS_DEV_INFO *dev)
{
	assert( NULL != dev );

	if( dev->channel.sk <= 0 )
	{
		fprintf(stderr, "WARNING: MOD[%d] dbs has been closed already\n", dev->srcmod);
		free(dev);
		dev = NULL;
		return CMM_FAILED;
	}

	if( CMM_SUCCESS != __dbsDestroyModule(dev) )
	{
		fprintf(stderr, "WARNING: MOD[%d] dbsDestroyModule failed\n", dev->srcmod);
	}

	close(dev->channel.sk);
	free(dev);
	dev = NULL;
	
	return CMM_SUCCESS;
}

