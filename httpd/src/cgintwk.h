#ifndef __CGI_NTWK_H__
#define __CGI_NTWK_H__

#include <stdio.h>
#include <fcntl.h>

/********************** Global Types ****************************************/



#if defined(__cplusplus)
extern "C" {
#endif

//void writePageHeader(FILE *fs);
void cgiNtwkView(char *query, FILE *fs);
void cgiPortPropetyView(char *query, FILE *fs);
void cgiPortPropetyViewAll(char *query, FILE *fs);
void cgiPortStatsView(char *query, FILE *fs) ;
void cgiPortStatsViewAll(char *query, FILE *fs);
void cgiOptlogView(char *query, FILE *fs) ;
void cgiSyslogView(char *query, FILE *fs) ;
void cgiAlarmlogView(char *query, FILE *fs) ;
void cgiAlarmlogDetailView(char *query, FILE *fs) ;
void cgiTopologyView(char *query, FILE *fs);
void cgiTemplateView(char *query, FILE *fs);
void cgiTemplateEdit(char *query, FILE *fs);
void cgiTemplateMgmt(char *query, FILE *fs);
//void cgiCltProfileView(char *query, FILE *fs) ;
void cgiCltProfile(char *query, FILE *fs);
void cgiCltMgmt(char *query, FILE *fs);
//void cgiCnuProfileView(char *query, FILE *fs) ;
void cgiCnuProfile(char *query, FILE *fs);
void cgiCnuProfileExt(char *query, FILE *fs);
void cgiCnuWifi(char *query, FILE *fs);
void cgiCnuMgmt(char *query, FILE *fs);
void cgiLinkDiag(char *query, FILE *fs) ;
void cgiLinkDiagResult(char *query, FILE *fs);
void cgiPortPropetyAllView(char *query, FILE *fs);

#if defined(__cplusplus)
}
#endif

#endif
