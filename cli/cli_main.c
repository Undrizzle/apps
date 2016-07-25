/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cli_main.c
 �ļ����� :


 �����б� :


 �޶���¼ :
          1 ���� : frank
            ���� : 2007-10-30
            ���� :

**********************************************************************/


/******************************************************************************
 *                                GLOBAL                                      *
 ******************************************************************************/
#include <assert.h>
#include <stdio.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include "util/cli_term.h"

/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

extern void CLI_TermTaskProc();
extern unsigned long  CLI_ProcessInit();
extern void CLI_ProcessDestory();

void ShowBrand(void)
{
	printf("\n");
	printf("*********************************************************************\n");
	printf("*    EoC CLI system. Copyright (c) 2013-2014                        *\n");
	/* demo version info */
	//printf("*    Hangzhou Prevail Optoelectronic Equipment Co.,LTD.                  *\n");
	printf("*    Without the owner's prior written consent,                     *\n");
	printf("*    no decompiling or reverse-engineering shall be allowed.        *\n");
	printf("*********************************************************************\n");
	printf("\n");
	printf("\n");
}

int main()
{
	if (CLI_FAILED(CLI_ProcessInit()))
	{
		printf("\n CLI Init Failed!!!");
		CLI_ProcessDestory();
		return 1;
	}
	ShowBrand();
	CLI_TermTask(0, 0);
	CLI_ProcessDestory();
	printf("\nCLI Exit!\n");
	return 0;
}



/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



