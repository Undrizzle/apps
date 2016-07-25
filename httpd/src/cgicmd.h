#ifndef __CGI_CMD_H__
#define __CGI_CMD_H__

#include <stdio.h>
#include <fcntl.h>
#include "httpd.h"

/********************** Global Types ****************************************/

typedef void (*CGI_CMD_HDLR) (char *path, FILE *fs);

typedef struct {
   char *cgiCmdName;
   CGI_CMD_HDLR cgiCmdHdlr;
} CGI_CMD, *PCGI_CMD;

	
void do_cmd_cgi(char *path, FILE *fs);
void cgiRtRouteCfg(char *path, FILE *fs);
void cgiArpView(char *path, FILE *fs);
void cgiBackupSettings(char *path, FILE *fs);
void cgiLogView(char *path, FILE *fs);
void cgiScVrtSrv(char *path, FILE *fs);
void cgiScPrtTrg(char *path, FILE *fs);
void cgiScOutFlt(char *path, FILE *fs);
void cgiScInFlt(char *path, FILE *fs);
void cgiScAccCntr(char *path, FILE *fs);
void cgiScSrvCntr(char *path, FILE *fs);
void cgiScMacFlt(char *path, FILE *fs);
void cgiQosCls(char *path, FILE *fs);
void cgiScDmzHost(char *path, FILE *fs);
void cgiWanCfg(char *path, FILE *fs);
void cgiStsAtm(char *path, FILE *fs);
void cgiStsWan(char *path, FILE *fs);
void bcmUpdateStsAtm(int reset);
void cgiAdslCfgTestMode(char *query, FILE *fs);
void cgiAdslCfgTones(char *query, FILE *fs);
void cgiEngInfo(char *query, FILE *fs);
void cgiQosQueue(char *path, FILE *fs);
void cgiQosMgmt(char *path, FILE *fs);


#endif
