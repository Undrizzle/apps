#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgicmd.h"
#include "cgimain.h"
#include "cgintwk.h"
//#include "cgisec.h"
//#include "cgists.h"

#define WEB_GENERAL_CMD_NUM   21
#define WEB_PORT_MAP_CMD_NUM  0
#define WEB_RIP_CMD_NUM  0
#define WEB_WIRELESS_CMD_NUM  0
#define WEB_VOICE_CMD_NUM     0
#define WEB_DDNS_CMD_NUM      0
#define WEB_IPSEC_CMD_NUM     0
#define WEB_CERT_CMD_NUM     0
#define WEB_PORT_MIRROR_CMD_NUM     0
#define WEB_CHINATELCOM_CMD_NUM    0


#define WEB_CMD_NUM_MAX    ( WEB_GENERAL_CMD_NUM + WEB_PORT_MAP_CMD_NUM + WEB_RIP_CMD_NUM + \
                             WEB_WIRELESS_CMD_NUM + WEB_VOICE_CMD_NUM + WEB_DDNS_CMD_NUM + \
                             WEB_IPSEC_CMD_NUM + WEB_CERT_CMD_NUM + WEB_PORT_MIRROR_CMD_NUM )

CGI_CMD WebCmdTable[WEB_CMD_NUM_MAX] = {
   { "showNetworkInfo", cgiNtwkView },
   { "wecOptlog", cgiOptlogView },
   { "wecSyslog", cgiSyslogView },
   { "wecAlarmlog", cgiAlarmlogView },
   { "alarmlogDetail", cgiAlarmlogDetailView },
   { "wecTopology", cgiTopologyView },   
   { "wecTemplateMgmt", cgiTemplateMgmt },      
   { "editCltPro", cgiCltProfile },
   { "cltManagement", cgiCltMgmt },
   { "editCnuWifi", cgiCnuWifi },
   { "editCnuPro", cgiCnuProfile },
   { "rtl8306eConfigView", cgiCnuProfileExt },
   { "cnuManagement", cgiCnuMgmt },   
   { "portStatsDetail", cgiPortStatsView },
   { "wecPortStats", cgiPortStatsViewAll },
   { "wecPortPropety", cgiPortPropetyViewAll },
   { "portPropety", cgiPortPropetyView },
   { "linkDiagResult", cgiLinkDiagResult },
   { "wecLinkDiag", cgiLinkDiag },   
};

void do_cmd_cgi(char *path, FILE *fs) {
   int i = 0;
   char fileName[WEB_BUF_SIZE_MAX], *query;
   char* cp = NULL;
   char* ext = NULL;
   char empty = 0;

   cp = strchr(path, '?');
   if ( cp != NULL )
      query = cp + 1; // reuse big buffer in httpd.c
   else
      query = &empty;

   fileName[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(fileName, path);

      // printf ("WEB_CMD_NUM_MAX = %d \n", WEB_CMD_NUM_MAX) ;

      for ( ; i < WEB_CMD_NUM_MAX; i++ ) {

         if ( WebCmdTable[i].cgiCmdName == NULL )
            continue ;

         if ( strstr(fileName, WebCmdTable[i].cgiCmdName) != NULL )
            break;
      }

      if ( i >= WEB_CMD_NUM_MAX ) {

         // write header
         fprintf(fs, "<html><head>\n");
         fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
         fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
         fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

         // write body
         fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
         fprintf(fs, "<b><font color='blue'>This page is not supported.</font></b><br><br>\n") ;

         fprintf(fs, "<tr>\n");
         fprintf(fs, "<tr>\n");
         fprintf(fs, "<br><br>\n");

         return;
      }

         // printf ("DBG: Proper Command \n");
      (*(WebCmdTable[i].cgiCmdHdlr))(query, fs);
   }
}
