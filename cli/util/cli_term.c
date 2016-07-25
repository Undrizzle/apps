/****************************************************************************
 �ļ���  : CLI_Term.c
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����:  1��ʵ�������в����ն˵��������
            2���ն����������е��ַ����༭����Ĵ���
 ��ע   :
 �����б�:
            ��
 �޸���ʷ:
        1. �޸���   :  may2250
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
        2. ...
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_term.h"
#include "cli_io.h"
#include "cli_interpret.h"
#include "../cli_comm.h"
#include <public.h>
#include <dbsapi.h>

ST_CLI_TERM m_stCliTerm;

extern T_DBS_DEV_INFO *dbsdev;

static struct termios stored_settings;
ULONG  CLI_SendToTerm ( )
{
    m_stCliTerm.iSendLen = (int   )STB_StrLen(m_stCliTerm.szSendBuf);
    if (m_stCliTerm.iSendLen == 0
     || m_stCliTerm.nTermStatus == TERM_SLEEPING)
        return TBS_SUCCESS;

    printf("%s", m_stCliTerm.szSendBuf);

    m_stCliTerm.iSendLen = 0 ;
    m_stCliTerm.szSendBuf[ 0 ] = '\0' ;
    return TBS_SUCCESS ;
}

int Cli_StdGetCh()
{
    return getchar();
}

/*********************************************************************/
/* �������� : CLI_SendToTerm()                                       */
/* �������� : �����ն�ѡ�����뺯��                                   */
/* ����     : �ɹ���ʧ��                                             */
/*********************************************************************/
ULONG  CLI_ReceiveFromTerm(ULONG  ulSecond)
{
    ULONG  ulRet = TBS_FAILED;

    switch (m_stCliTerm.nTermType)
    {
        case CLI_TERM_FOR_SERIAL:
            if (ulSecond == 0)
                break;

            m_stCliTerm.szRecvBuf[0] = Cli_StdGetCh();
            m_stCliTerm.iRecvLen = 1 ;
            ulRet = TBS_SUCCESS ;
            break;

        case CLI_TERMT_FOR_MML:
            if (m_stCliTerm.szRecvBuf[0] != '\0')
                ulRet = TBS_SUCCESS;
            else
            {
                CLI_DELAY(20);
                ulRet = TBS_FAILED;
            }
            break;

        case CLI_TERM_NOT_INITIAL:
            break;

        default:
            break;
    }

    if (m_stCliTerm.ulTimeLeft == 0 || m_stCliTerm.nTermStatus == TERM_SLEEPING)
    {
        return TBS_FAILED;
    }

    if (ulRet == TBS_SUCCESS)
    {
        m_stCliTerm.ulTimeLeft = DEADLINE_SHORT;
    }

    m_stCliTerm.ulDispLineCount = 0;

    return ulRet;
}

/*********************************************************************/
/* �������� : CLI_TermTaskProc()                                     */
/* �������� : �ն���������ĺ���                                   */
/* ������� : pTermStruct:�ն���������                                */
/* ������� : �ն���������                                           */
/* ����     : ��                                                     */
/* �ϲ㺯�� : �ն���������                                         */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
_VOID    CLI_TermTaskProc()
{
    if (!IS_TERM_NORMAL(m_stCliTerm))
        return ;

     /*��1�����ն˽����ַ�*/
     m_stCliTerm.iEditStatus = COMMAND_INPUT;
    if (CLI_ReceiveFromTerm(m_stCliTerm.ulTimeLeft) != TBS_SUCCESS)
        return;

    if (!IS_TERM_NORMAL(m_stCliTerm))
    {
        DBG_Trace("\r\n CLI_TermTaskProc::Received but Term Abnormal!");
        return;
    }

    /* ��2�������������*/
    if (CLI_CmdBufProcess() != TBS_SUCCESS)
        return;

    /* ��3��ִ���������յ�����*/
    ////CLI_ComStatusStart();
    CLI_CmdExecute();
    ////CLI_ComStatusEnd();

    if (!IS_TERM_NORMAL(m_stCliTerm))
    {
        DBG_Trace("\r\n CLI_TermTaskProc::Cmd executed but Term Abnormal!");
        return;
    }

    /* ��4�������¼�뻷�����*/
    CLI_CmdRecord();

    CLI_DisplayPrompt();

    return;
}

/*********************************************************************/
/* �������� : CLI_TermTask()                                         */
/* �������� : ������������                                         */
/* ������   : liuzequn    */
/* �޸���   : may2250                                                  */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
_VOID    CLI_TermTask( ULONG  ulArg1, ULONG  ulArg2)
{
    m_stCliTerm.nTermType = CLI_TERM_FOR_SERIAL;
    int logcount=1;
	
    /* ��CMM ģ���ѯ�û���¼*/
	#if 0
    int i=0;
    T_szUserst userst;
    bzero((unsigned char *)&userst, sizeof(T_szUserst));
    if( msg_get_userinfo(&userst) != TBS_SUCCESS )
    {
	    return;
    }
    #endif
	
    CLI_TermLogin();

    for ( ; ; )
    {
        while(m_stCliTerm.nTermStatus < TERM_LOGED || m_stCliTerm.ulTimeLeft == 0)
        {
            logcount++;
	     CLI_TermDataInit();
            CLI_TermLogout();
	     if(logcount>3)
		 	return;
            m_stCliTerm.nTermType = CLI_TERM_FOR_SERIAL;
            CLI_TermLogin();
        }
        CLI_TermTaskProc();

        if (!IS_TERM_NORMAL(m_stCliTerm))
            return;
    }
}

/*********************************************************************/
/* �������� : CLI_PeriodTask()                                       */
/* �������� : �������ڲ���ʱ����                                     */
/*********************************************************************/
_VOID    CLI_PeriodProc(_VOID   )
{
    if (m_stCliTerm.ulTimeLeft == 1)
    {
        DBG_Out( "r\n CLI: Term Timeout!");
    }

    if (m_stCliTerm.ulTimeLeft > 0)
    {
        m_stCliTerm.ulTimeLeft--;
    }

    return;
}


/*********************************************************************/
/* �������� : CLI_TermLogin()                                        */
/* �������� : �ն˵�¼�������                                       */
/* ������� :                                                        */
/* ������� : �ն��������ݡ��ú����е�¼�ɹ����ı����������״̬��ͬ */
/*            ʱ�ı�����¼���û���Ϣ��״̬                           */
/* ����     : ��                                                     */
/* �ϲ㺯�� : �ն�����ִ����������                                   */
/*********************************************************************/
_VOID    CLI_TermLogin(void)
{
	//char   szUsername[USERNAME_LEN] = UNLOGON_USER_NAME;
	char loginUser[16] = {0};
	char   szPassword[USERNAME_LEN] = "";
	ULONG  ulFailCount;

	int i = 0;
	st_dbsCliRoles roles;
	st_dbsCliRole role;

	for( i=1; i<=3; i++ )
	{
		if( 0 == dbsGetCliRole(dbsdev, i, &role) )
		{
			roles.role[i-1].id = role.id;
			strcpy(roles.role[i-1].col_user, role.col_user);
			strcpy(roles.role[i-1].col_pwd, role.col_pwd);
		}
		else
		{
			fprintf(stderr, "ERROR: CLI_TermLogin->dbsGetCliRole[%d] !\n", i);
			return;
		}
	}
    
	/* ��Ϊ���û�����������չ�������״̬���ж�,*/
	/* �ʴ˴�����һ�������������е�״̬         */
	CLI_SetTermStatus(TERM_ACTIVE);
	m_stCliTerm.ulTimeLeft   = DEADLINE_SHORT;
	ulFailCount = 0;

    for (; ;)
    {
    	 
	 //while(IO_GetCharByPeek() !='\0')
	 //	;
	 //printf("\nUsername:");
	 STB_StrCpy(m_stCliTerm.szSendBuf,"\nUsername:");
	 CLI_SendToTerm();
	 if (IO_GetString((char *)&loginUser,16,FALSE) != TBS_SUCCESS) 
                return; 

	 if(strcmp(loginUser," admin")==0)
	{
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[0].col_pwd)==0)
		{
			/* �ɹ���¼���һЩ�û����ն����ݳ�ʼ�� */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 2;
        		m_stCliWS.ulLevel = 2;
		 	m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);
			
        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else if(strcmp(loginUser," operator")==0)
	 {
	 	//printf("\nPassword: ");
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[1].col_pwd)==0)
		{
			/* �ɹ���¼���һЩ�û����ն����ݳ�ʼ�� */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 1;
        		m_stCliWS.ulLevel = 1;
			m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);

        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else if(strcmp(loginUser," user")==0)
	 {
	 	//printf("\nPassword: ");
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[2].col_pwd)==0)
		{
			/* �ɹ���¼���һЩ�û����ն����ݳ�ʼ�� */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 0;
        		m_stCliWS.ulLevel = 0;
			m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);

        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else
	 {
		printf("\n no such username! Please enter (admin operator or user)...\n");
	 }
	 return;
	 
    }
}

/*********************************************************************/
/* �������� : CLI_TermLogout()                                       */
/* �������� : �ն�ע���������                                       */
/* ����     : ��                                                     */
/* �ϲ㺯�� : �ն�����ִ����������                                   */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
_VOID    CLI_TermLogout()
{
    /* �������������: 1��δ�ɹ���¼��ע��; 2����¼���ע��  */
    /* ����������û����Ƿ�Ϊ��ʼ�����û������ж�            */
    if (!STB_StriCmp(m_stCliTerm.szCurUserName, UNLOGON_USER_NAME))
    {
        if (m_stCliTerm.nTermType == CLI_TERM_FOR_SERIAL)
            return ;
    }
    else
    {
    }

    /* �����ն����񷵻ص���ģʽ */
    CLI_ReturnRootMode();

    IO_Print("\nLogout");
    //CLI_DELAY(100);
    //CLI_CommDestroy();
    CLI_TermDataInit();
    CLI_ResetEnviroment();
    CLI_SetTermStatus(TERM_SLEEPING);
    
    return;
}


/*********************************************************************/
/* �������� : CLI_ReturnRootMode()                                   */
/* �������� : ����ǰ�ն����񷵻ص���ģʽ                             */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
ULONG  CLI_ReturnRootMode()
{
    CLI_SetWSStatus((ULONG )CLI_STAT_ENV_RUNCFG) ;

    if (m_stCliWS.ulCurrentMode != CTM_GENL)
    {
        STB_StrCpy(m_stCliTerm.szCommandBuf, CMD_CM "root") ;
        CLI_CmdExecute() ;
    }
    CLI_SetWSStatus(0) ;
    return TBS_SUCCESS;
}

/*==================================================================*/
/*      ������      :CLI_SetTermStatus                              */
/*      ��������    :�����ն�״̬                                   */
/*      �������    :nTermStatus ���õ�״̬                        */
/*      ����ֵ      :��                                             */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_SetTermStatus(UCHAR nTermStatus)
{
/*    char  szInfo[TELNET_MAX_STATUS][15] =
            {"SLEEPING","ACTIVE","LOGED","EXEC-CMD","WAIT-RESPONSE"};
*/

    DBG_Trace(
        "\r\n CLI: Term status terned from [%s] to [%s]",
        szInfo[m_stCliTerm.nTermStatus], szInfo[nTermStatus]);

    if (m_stCliTerm.nTermStatus == TERM_WAIT_RESPONSE && nTermStatus == TERM_LOGED)
    {
        m_stCliTerm.nTermStatus = nTermStatus;
        CLI_DisplayPrompt();
        return;
    }

    m_stCliTerm.nTermStatus = nTermStatus;
    return ;
}


/* �ַ����༭���� */

/*==================================================================*/
/*      ������      :CLI_EditString                                 */
/*      ��������    :ɨ����ջ�����,���༭�ַ����༭������          */
/*      ����ֵ      :ULONG 0:  ���յ�һ���������в�׼���ύ������   */
/*                         1:  ֻ���յ�һ�ַ���׼�����������ַ�     */
/*      ���ú���    :CLI_CharProcess ( )                            */
/*      ��������    :                                               */
/*==================================================================*/
ULONG  CLI_EditString( )
{
    ULONG   ulTmp ;
    UCHAR   ucTmp ;

    /* ���յ����ַ����Ĵ��� */
    if ( m_stCliTerm.nTermType == CLI_TERM_FOR_SERIAL )
    {
        for ( ulTmp = 0 ; ulTmp < (ULONG )m_stCliTerm.iRecvLen ; ulTmp ++ )
        {
            if (m_stCliTerm.szRecvBuf[ ulTmp ] == '\0')
                return TBS_FAILED ;
            ucTmp = (UCHAR)m_stCliTerm.szRecvBuf[ ulTmp ] ;

            if ( ! CLI_CharProcess ( ucTmp ) )
                return TBS_SUCCESS ;
        }
    }
    return TBS_FAILED ;
}

/*==================================================================*/
/*      ������      :CLI_CharProcess                                */
/*      ��������    :�Ե����ַ����д������ɾ��,����,�����ַ������ */
/*      �������    :UCHAR ucTmp  ��������ַ�                      */
/*      ����ֵ      :ULONG 0:  ���յ�һ���������в�׼���ύ������   */
/*                         1:  ֻ���յ�һ�ַ���׼�����������ַ�     */
/*==================================================================*/
ULONG  CLI_CharProcess ( UCHAR ucTmp )
{
    //BEGIN of ���������ͷ���Ź���
    if (m_stCliTerm.ucInputStatus == KEY_ESC)
    {
        if (ucTmp == '[')
        {
            m_stCliTerm.ucInputStatus = '[';
            return TBS_FAILED;
        }
        else if (ucTmp == KEY_ESC)
        {
            ucTmp = KEY_CTRL_C;
        }
        else
            m_stCliTerm.ucInputStatus = '\0';
    }
    else if (m_stCliTerm.ucInputStatus == '[')
    {
        switch(ucTmp)
        {
            case 'a':
            case 'A':
                ucTmp = KEY_MOVE_P;
                break;
            case 'b':
            case 'B':
                ucTmp = KEY_MOVE_N;
                break;
            case 'd':
            case 'D':
                ucTmp = KEY_MOVE_L;
                break;
            case 'c':
            case 'C':
                ucTmp = KEY_MOVE_R;
                break;
            default:
                break;
        }
        m_stCliTerm.ucInputStatus = '\0';
    }
    //END of ���������ͷ���Ź���

    if (ucTmp > 0x20 && ucTmp <= 0x7E
     && ucTmp != KEY_MOVE_L && ucTmp != KEY_MOVE_R
     && ucTmp != KEY_MOVE_P && ucTmp != KEY_MOVE_N)
    {
        CLI_CharInsert ( (char )ucTmp ) ;
        if (m_stCliTerm.iEditStatus != STRING_INPUT && (ucTmp == '?' ))
            return TBS_SUCCESS ;
    }
    else
    {
        switch (ucTmp)
        {
            case KEY_CTRL_C:   /* ��ֹ�ַ� */
            case KEY_CTRL_Z:
                m_stCliTerm.szEditBuf[0] = (char )ucTmp ;
                m_stCliTerm.szEditBuf[1] = '\0' ;
                m_stCliTerm.iCurrentPos = 0 ;
                return TBS_SUCCESS ;

            case KEY_ESC:
                m_stCliTerm.ucInputStatus = KEY_ESC;
                break;

            case KEY_ENTER: /* �س����з�*/
            case KEY_RETURN:
                /* ���������༭״̬����Ҫ��������������������Ӧ���� */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    CLI_DealWitchCommand();
                }
                m_stCliTerm.iCurrentPos = 0 ;
                return TBS_SUCCESS ;

            case KEY_SPACE: /* �ո� */
                /* ���������༭״̬������������Զ����벹ȫ */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    CLI_PatchCmdWord();
                }
                else   /* ����ֱ�Ӳ���ո� */
                {
                    CLI_CharInsert (' ');
                }
                break;
            case KEY_TABLE:
                /* �����TABLE�����������ո� */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    if (ISSPACEBAR(m_stCliTerm.szEditBuf[m_stCliTerm.iCurrentPos-1]))
                    {
                        CLI_CharInsert('?');
                        return TBS_SUCCESS;
                    }
                    else if (m_stCliTerm.iCurrentPos == (int   )STB_StrLen(m_stCliTerm.szEditBuf))
                    {
                        CLI_PatchCmdWord();
                    }
                    else
                    {
                        CLI_CharInsert (' ');
                    }
                }
                else   /* ����ֱ�Ӳ���ո� */
                {
                    CLI_CharInsert ('\t');
                }
                break;

            /* ����Ϊ�༭�����ַ��Ĵ���*/
            case KEY_MOVE_L:
                /* ���������,����������,���� */
                if ( m_stCliTerm.iCurrentPos == 0
                 || (m_stCliTerm.szInputCmd[0] != 0 && m_stCliTerm.iCurrentPos <= (int   )strlen(m_stCliTerm.szInputCmd)))
                {
                    CLI_Bell ( ) ;
                }
                else
                {
                    int    iLentgh = strlen(m_stCliTerm.szEditBuf);

                    // ���������༭״̬������ʱɾ�����һ���ո񣬼���
                    // �û��Ĳ���ʧ��(�����겻����󣬽��������ܾ�����)
                    if (m_stCliTerm.iCurrentPos == iLentgh
                     && m_stCliTerm.szEditBuf[iLentgh - 1] == 0x20
                     && m_stCliTerm.iEditStatus != STRING_INPUT)
                    {
                        CLI_DealWithBackSpace();
                    }
                    else
                    {
                        /* �������һ�� */
                        m_stCliTerm.iCurrentPos -- ;
                        CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos + 1 ) ;
                    }
                }
                break;
            case KEY_MOVE_R:
                if ( (ULONG )m_stCliTerm.iCurrentPos < (ULONG )STB_StrLen ( m_stCliTerm.szEditBuf ) )
                {
                    /* ���������һ�� */
                    m_stCliTerm.iCurrentPos ++ ;
                    CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos - 1 ) ;
                }
                /* ���������ĩ,���� */
                else
                    CLI_Bell () ;
                break;
            case KEY_CTRL_W:
            case KEY_MOVE_P: //DOSKEY������һ������
                // ����Ľ�����������ʱҲ���������DOSKEY����
                if ( m_stCliTerm.iEditStatus == COMMAND_INPUT )
                {
                    CLI_PrevCmdDisp( );
                }
                else   /*������༭״̬���ܽ���DOSKEY����*/
                {
                    CLI_Bell();
                }
                break;
            case KEY_CTRL_S:
            case KEY_MOVE_N: //DOSKEY������һ������
                // ����Ľ�����������ʱҲ���������DOSKEY����
                if ( m_stCliTerm.iEditStatus == COMMAND_INPUT )
                {
                    CLI_NextCmdDisp( );
                }
                else   /*������༭״̬���ܽ���DOSKEY����*/
                {
                    CLI_Bell();
                }
                break;
            default:
                CLI_EditSpecialKey ( (char )ucTmp ) ;
                break;

        }
    }
    return TBS_FAILED;

}


/*==================================================================*/
/*      ������      :CLI_CharInsert                                 */
/*      ��������    :���ַ����༭����ĵ�ǰ���λ�ò���һ�ַ�       */
/*      �������    :char  ch  ��������ַ�                          */
/*      ����ֵ      :��                                             */
/*      ���ú���    :CLI_MoveToCurrentPos ( )                       */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_CharInsert( char  ch )
{
    int     iTmp ;

    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) + 1 >= (ULONG )m_stCliTerm.iMaxLen )
    {
        DBG_Out( "\r\n �༭����������.");
        CLI_Bell();
        return ;
    }


    /* ��������ַ������ƶ�һ��,Ϊ��������ַ����������λ*/
    iTmp = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;
    if (m_stCliTerm.szEditBuf[iTmp -1] == '?')
        return;

    while ( iTmp >= m_stCliTerm.iCurrentPos )
    {
        m_stCliTerm.szEditBuf[ iTmp + 1 ]
            = m_stCliTerm.szEditBuf[ iTmp ] ;
        iTmp-- ;
    }

    /* �ڵ�ǰ���λ�ó�����ַ� */
    m_stCliTerm.szEditBuf[ m_stCliTerm.iCurrentPos ] = ch ;

    /* �����û��˵���Ļ��ʾ,ˢ�µ�ǰ����Ժ����ʾ�ַ�,             */
    /* ��ˢ�·��ͻ�����                                             */
    if (m_stCliTerm.nTermType != CLI_TERMT_FOR_MML)  // ��͸�����������
    {
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ch ;
    }
    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) - 1 > (ULONG )m_stCliTerm.iCurrentPos )
    {
        iTmp = m_stCliTerm.iCurrentPos + 1 ;
        while ( m_stCliTerm.szEditBuf[ iTmp ] )
            m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                = m_stCliTerm.szEditBuf[ iTmp ++ ] ;
    }
    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;

    /* ���λ������һ�� */
    m_stCliTerm.iCurrentPos ++ ;

    /* ��������ĩ�Ƶ���ǰ�������λ��  */
    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) > (ULONG )m_stCliTerm.iCurrentPos )
        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;

    return ;
}

/*==================================================================*/
/*      ������      :CLI_EditSpecialKey                             */
/*      ��������    :�Խ��յ��������ַ����б༭����                 */
/*      �������    :char  ch  ������������ַ�                      */
/*      ����ֵ      :��                                             */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_EditSpecialKey( char  ch )
{
    if ( ISESC( ch ) )
    {
        CLI_Bell ( ) ;
    }

    else if ( ISBACKSPACE( ch ) )
    {
        /*--------------------------------------------------*/
        /* ����û�������Backspace��,���֮���д���         */
        /*--------------------------------------------------*/
        CLI_DealWithBackSpace ( ) ;
    }

    else if ( ISDELETE( ch ) )
    {
        /*--------------------------------------------------*/
        /* ����û�������Delete��,���֮���д���            */
        /*--------------------------------------------------*/
        CLI_DealWithDelete ( ) ;

    }

    else if ( ISTAB( ch ) )
    {
        CLI_Bell ( ) ;
    }

    else if ( ISCTRLCHAR( ch ) )
    {
        CLI_CharInsert ( '^' ) ;
        CLI_CharInsert ( (char )(ISCTRLCHAR( ch )) ) ;
    }
    else
    {
        //CLI_Bell ( ) ;
    }
    return ;
}



/*==================================================================*/
/*      ������      :CLI_DealWithBackSpace                          */
/*      ��������    :��Backspace���Ĵ���                            */
/*      ����ֵ      :��                                             */
/*      ���ú���    :CLI_MoveToCurrentPos ( )                       */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_DealWithBackSpace ( )
{
    ULONG  ulTmp ;


    /*--------------------------------------------------------------*/
    /* ���ǰ���ַ���ɾ��,��澯                                    */
    /*--------------------------------------------------------------*/
    if ( m_stCliTerm.iCurrentPos == 0
     || (m_stCliTerm.szInputCmd[0] != 0 && m_stCliTerm.iCurrentPos <= (int   )strlen(m_stCliTerm.szInputCmd)))
    {
        CLI_Bell ( ) ;
        return;
    }


    /* �ն�������vt100�Ĵ��� */
    {
        m_stCliTerm.iCurrentPos -- ;
        CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos + 1 ) ;
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        /* ����༭������ɾ�����ǰ��һ�ַ�,��ˢ�·��ͻ�����    */
        do
        {
            if ( m_stCliTerm.szEditBuf[ ulTmp + 1 ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;

            else
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;

            m_stCliTerm.szEditBuf[ ulTmp ]
                = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            ulTmp ++ ;

        } while ( m_stCliTerm.szEditBuf[ ulTmp - 1 ] ) ;
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;
        /*------------------------------------------------------*/
        /* ������ʾ��ǰ������ַ���,ˢ�¹��λ��              */
        /*------------------------------------------------------*/
        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) + 1 ) ;

    }
    return ;
}


/*==================================================================*/
/*      ������      :CLI_DealWithDelete                              */
/*      ��������    :��Delete���Ĵ���                               */
/*      ����ֵ      :��                                             */
/*      ���ú���    :CLI_MoveToCurrentPos ( )                       */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_DealWithDelete ( )
{
    ULONG  ulTmp ;

    /* �������ַ�����ɾ��  */
    if ( m_stCliTerm.szEditBuf[ m_stCliTerm.iCurrentPos ] )
    {
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        /* ����༭������ɾ�������һ�ַ�,��ˢ�·��ͻ�����    */
        do
        {
            if ( m_stCliTerm.szEditBuf[ ulTmp + 1 ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            else
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = ' ' ;

            m_stCliTerm.szEditBuf[ ulTmp ]
                = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            ulTmp ++ ;

        } while ( m_stCliTerm.szEditBuf[ ulTmp - 1 ] ) ;
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;

        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) + 1 ) ;

    }
    /* �������ַ�����ɾ��,�򾯸�  */
    else
        CLI_Bell ( ) ;

    return ;
}


/*==================================================================*/
/*      ������      :CLI_PrevCmdDisp                                */
/*      ��������    :DOSKEY���ܵ���һ�������Զ���ȡ                 */
/*      ����ֵ      :��                                             */
/*      ���ú���    :CLI_MoveToCurrentPos ( )                       */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_PrevCmdDisp ( )
{
    ULONG  ulTmp ;

    /* �ò�����Ҫ��ʷ�����д������� */
    if ( m_stCliTerm.iHistoryPos > 0 )
    {
        /* ��������Ƶ���Ļ��ʾ�ĵ�һ����  */
        if ( m_stCliTerm.iCurrentPos )
        {
            ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
            m_stCliTerm.iCurrentPos = 0 ;
            CLI_MoveToCurrentPos ( ulTmp ) ;
        }
        /* ����û��ĵ�ǰ���� */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
            ulTmp = 0 ;
            while ( m_stCliTerm.szEditBuf[ ulTmp ++ ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;
            m_stCliTerm.szSendBuf [ m_stCliTerm.iSendLen ] = '\0' ;

            CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;
        }

        /* �õ�ǰ����ǰ����ʷ����ˢ�±༭������  */
        STB_StrCpy ( m_stCliTerm.szEditBuf,
                 m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos - 1 ] ) ;
        m_stCliTerm.iHistoryPos -- ;

        /* ������ʾ�µ����� */
        STB_StrCat ( m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;

        /* ˢ�µ�ǰ�Ĺ��λ�� */
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;

    }
    /* ��ǰ����֮ǰ����ʷ����,���� */
    else
        CLI_Bell ( ) ;

    return ;
}


/*==================================================================*/
/*      ������      :CLI_NextCmdDisp                                */
/*      ��������    :DOSKEY���ܵ���һ�������Զ���ȡ                 */
/*      ����ֵ      :��                                             */
/*      ���ú���    :CLI_MoveToCurrentPos ( )                       */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_NextCmdDisp ( )
{
    ULONG  ulTmp ;

    /* �ò�����Ҫ��ʷ�����д������� */
    if (    m_stCliTerm.iHistoryPos + 1 < HISTORY_SIZE
         && m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos + 1 ][0] )
    {

        /* ��������Ƶ���Ļ��ʾ�ĵ�һ����  */
        if ( m_stCliTerm.iCurrentPos )
        {
            ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
            m_stCliTerm.iCurrentPos = 0 ;
            CLI_MoveToCurrentPos ( ulTmp ) ;
        }

        /* ����û��ĵ�ǰ���� */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
            ulTmp = 0 ;
            while ( m_stCliTerm.szEditBuf[ ulTmp ++ ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;
            m_stCliTerm.szSendBuf [ m_stCliTerm.iSendLen ] = '\0' ;

            CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;
        }

        /* �õ�ǰ��������ʷ����ˢ�±༭������  */
        STB_StrCpy (m_stCliTerm.szEditBuf,
                m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos + 1 ] ) ;
        m_stCliTerm.iHistoryPos ++ ;

        /* ������ʾ�µ����� */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            STB_StrCat ( m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf ) ;
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
        }

        /* ˢ�µ�ǰ�Ĺ��λ�� */
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;

    }
    /* ��ǰ����֮������ʷ����,���� */
    else
        CLI_Bell ( ) ;

    return ;
}

/*==================================================================*/
/*      ������      :CLI_MoveToCurrentPos                           */
/*      ��������    :������Ƶ���ǰλ��                             */
/*      �������    :ULONG  iPos ����ƶ�����ʼλ��                    */
/*      ����ֵ      :��                                             */
/*      ���ú���    :��                                             */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_MoveToCurrentPos ( ULONG  iPos )
{
    int     iTmp ;
    char  szTmp[4] ;
	int nInputCmdLen = STB_StrLen(m_stCliTerm.szInputCmd);

    /* ����������ƶ� */
    iTmp = ((int)iPos + m_stCliTerm.iPromptLen - nInputCmdLen) / 80
           - (m_stCliTerm.iCurrentPos + m_stCliTerm.iPromptLen - nInputCmdLen) / 80 ;
    /* ����������ƶ� */
    if ( iTmp > 0)
    {
        STB_StrCat( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", iTmp ) ;
        STB_StrCat(m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat(m_stCliTerm.szSendBuf, "A" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    /* ����������ƶ�   */
    else if ( iTmp < 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", 0 - iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "B" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }

    /* ����������ƶ�  */
    iTmp = ( (int   )iPos + m_stCliTerm.iPromptLen - nInputCmdLen) % 80
        - ( m_stCliTerm.iCurrentPos + m_stCliTerm.iPromptLen -nInputCmdLen ) % 80 ;

    /* ����������ƶ� */
    if ( iTmp > 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "D" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    /* ����������ƶ�   */
    else if ( iTmp < 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", 0 - iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "C" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    return ;
}


/*==================================================================*/
/*      ������      :CLI_Bell                                       */
/*      ��������    :������յ����ַ����ܴ���,�򾯸�                */
/*      ����ֵ      :��                                             */
/*      ���ú���    :��                                             */
/*      ��������    :                                               */
/*==================================================================*/
_VOID    CLI_Bell ( )
{
//    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = 0x07 ;
//    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;
    return ;
}


/*==================================================================*/
/*      ������      :CLI_PatchCommand                               */
/*      ��������    :������������ճ������                           */
/*      ����ֵ      :ULONG TBS_FAILED ʧ�� TBS_SUCCESS  �ɹ�       */
/*      ��������    :                                               */
/*==================================================================*/
ULONG  CLI_PatchCommand()
{
    char  *s8Patch;
    ULONG  ulRet = TBS_FAILED;

    s8Patch = CLI_GetCommandPatch( (ULONG )m_stCliTerm.iCurrentPos );

    while(*s8Patch)
    {
        if (((ULONG )STB_StrLen(m_stCliTerm.szEditBuf) + 1) < (ULONG )m_stCliTerm.iMaxLen)
        {
            CLI_CharInsert (*(s8Patch++));

            if(*(s8Patch - 1) == '?')
                ulRet = TBS_SUCCESS ;
        }
        else
        {
            DBG_Out(
                    "\r\n Edit buffer overflow");
            return ulRet;
        }
    }

    return ulRet;
}

/*==================================================================*/
/*      ������      :CLI_PatchCmdWord                               */
/*      ��������    :��ȫ�û������������                           */
/*      ����ֵ      :ULONG  TBS_SUCCESS  �ɹ�                       */
/*      ��������    :                                               */
/*==================================================================*/
ULONG  CLI_PatchCmdWord()
{
    ULONG  ulTmp;
    char  *pacNewInput;

    ulTmp = (ULONG )m_stCliTerm.iCurrentPos;
    if ( ulTmp != 0 )
    {
        if ( !ISSPACEBAR(m_stCliTerm.szEditBuf[ulTmp-1])
            && (ulTmp == STB_StrLen(m_stCliTerm.szEditBuf)) )
        {
            CLI_SetCommandString( m_stCliTerm.szEditBuf);
            CLI_SetWSStatus(CLI_STAT_ENV_SILENT);

            if ((m_stCliWS.ulStatus & CLI_STAT_ENV_NEWLINE) != 0)
            {
                //������ڽ���״̬�£����Ѿ�������ո���ǰ����������Ѿ����ǽ�����ʾ�Ĳ���
                pacNewInput = m_stCliTerm.szEditBuf + strlen(m_stCliTerm.szInputCmd);
                if (strchr(pacNewInput, 0x20) != 0)
                {
                    m_stCliWS.ulExpectEle = 0;
                }
            }

            CLI_IntpKernelProc();

            CLI_SetWSStatus(0);
            CLI_PatchCommand( );
            return TBS_SUCCESS;
        }
    }

    return TBS_SUCCESS;
}

/*==================================================================*/
/*      ������      :CLI_DealWitchCommand                           */
/*      ��������    :�����������������                             */
/*      ����ֵ      :ULONG  TBS_SUCCESS  �ɹ�                       */
/*      ��������    :                                               */
/*==================================================================*/
ULONG  CLI_DealWitchCommand()
{
    char  *pacNewInput;

    //���е��˴����ض����û����»س��Ľ������ʱΪ��ϵͳ�������
    //һ�������ֵ����룬ǿ�ƽ����������󣬴˴�����Ҫ�Թ����ʾ
    //���д�����Ϊ����Ҫ������ǰ���б༭
    m_stCliTerm.iCurrentPos = STB_StrLen(m_stCliTerm.szEditBuf);

    if (m_stCliTerm.iCurrentPos != 0) //�����Ч����Ϊ�գ��Ͳ�Ҫ�˷�ʱ����
    {
        CLI_SetCommandString( m_stCliTerm.szEditBuf);
        CLI_SetWSStatus(CLI_STAT_ENV_SILENT);
        m_stCliWS.ulStatus |= CLI_STAT_ENV_ENTER ;

        if ((m_stCliWS.ulStatus & CLI_STAT_ENV_NEWLINE) != 0)
        {
            //������ڽ���״̬�£����Ѿ�������ո���ǰ����������Ѿ����ǽ�����ʾ�Ĳ���
            pacNewInput = m_stCliTerm.szEditBuf + strlen(m_stCliTerm.szInputCmd);
            if (strchr(pacNewInput, 0x20) != 0)
            {
                m_stCliWS.ulExpectEle = 0;
            }
        }
        CLI_IntpKernelProc();

        CLI_SetWSStatus(0);
        CLI_PatchCommand( );
    }

    return TBS_SUCCESS;

}

/*==============================================================*/
/*      ������      :CLI_CmdBufProcess                          */
/*      ��������    :�����������д���                       */
/*      ����ֵ      :ULONG  0:  ���յ�һ����������              */
/*                          1:  ֻ���յ�һ�ַ�                  */
/*      ���ú���    :CLI_EditString ( )                         */
/*                   CLI_SendToTerm ( )                         */
/*                   CLI_MoveToCurrentPos ( )                   */
/*      ��������    :                                           */
/*==============================================================*/
ULONG  CLI_CmdBufProcess (  )
{
    ULONG  ulTmp ;

    /*----------------------------------------------------------*/
    /* �༭�����ַ���                                           */
    /*----------------------------------------------------------*/
    ulTmp = CLI_EditString () ;

    /*----------------------------------------------------------*/
    /* ���Խ��յ����ַ���                                       */
    /*----------------------------------------------------------*/
    if(m_stCliTerm.bRecurCmd)
    {
        m_stCliTerm.bRecurCmd = FALSE;
        if ( m_stCliTerm.iSendLen )
        {
            CLI_DisplayPrompt() ;

            /* �ƶ�����굱ǰλ�� */
            CLI_MoveToCurrentPos ( (ULONG )STB_StrLen(m_stCliTerm.szEditBuf) ) ;
            CLI_SendToTerm (  ) ;
        }
    }
    else if ( m_stCliTerm.iSendLen )
    {
        CLI_SendToTerm ( ) ;
    }

    /*----------------------------------------------------------*/
    /* ���յ�һ������������                                     */
    /*----------------------------------------------------------*/
    if ( ! ulTmp )
    {
        /*------------------------------------------------------*/
        /* ������ƶ��������е�ĩβ                             */
        /*------------------------------------------------------*/
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;
        if (m_stCliTerm.nTermType != CLI_TERMT_FOR_MML)
            CLI_MoveToCurrentPos ( ulTmp ) ;

        /*------------------------------------------------------*/
        /* �������ύ���������,���������༭������          */
        /*------------------------------------------------------*/
        STB_StrCpy ( m_stCliTerm.szCommandBuf, m_stCliTerm.szEditBuf ) ;
        m_stCliTerm.szEditBuf[0] = '\0' ;
        m_stCliTerm.iCurrentPos = 0 ;
        if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML)
        {
            m_stCliTerm.szRecvBuf[0] = '\0';
            m_stCliTerm.iRecvLen = 0;
        }

        return TBS_SUCCESS ;
    }

    return TBS_FAILED ;
}

/*==============================================================*/
/*      ������      :CLI_DisplayPrompt                           */
/*      ��������    :��ʾ��ʾ��                                 */
/*      ���ú���    :CLI_SendToTerm ( )                         */
/*      ��������    :                                           */
/*==============================================================*/
_VOID    CLI_DisplayPrompt ( )
{
    char  szNewLine[] = "\r\n \r\n" ;
    char  *szPrompt;
    char  *szPortID;
    char  szHostName[ 80 ] = "";
    char  szUsrname[USERNAME_LEN]="";
    char  szTmp[USERNAME_LEN]="";
	
    if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML || m_stCliTerm.nTermStatus == TERM_WAIT_RESPONSE)
        return ;


    STB_StrCpy ( szHostName, m_szHostName) ;

    szHostName[HOSTNAME_LENGTH - 1] = 0;

    /* ����ȡ�����������������ַ�����ʱ�������´��� */
    if ((UCHAR)szHostName[HOSTNAME_LENGTH - 2] > 127)
    {
        int i,CnNum = 0;
        for (i = 0; i < HOSTNAME_LENGTH - 2; i++)
        {
            /*ͳ�������ַ�����*/
            if ((UCHAR)szHostName[i] > 127)
            {
                CnNum++;
            }
        }
        if(CnNum % 2 == 0)
        {
            szHostName[HOSTNAME_LENGTH - 2] = '\0';
        }
    }

    CLI_SendToTerm( ) ;
    szPrompt = CLI_GetCurrentPrompt();
    STB_StrCat(m_stCliTerm.szSendBuf,szNewLine) ;
    STB_StrCat(m_stCliTerm.szSendBuf,szHostName);
    memcpy(szTmp,m_stCliTerm.szCurUserName+1,USERNAME_LEN-1);

    sprintf(szUsrname,">>%s",szTmp);
    STB_StrCat(m_stCliTerm.szSendBuf,szUsrname);
    if (NULL != szPrompt)
        STB_StrCat ( m_stCliTerm.szSendBuf, szPrompt);

    szPortID = CLI_GetModeParameter(m_stCliWS.ulCurrentMode,2);
    if (NULL != szPortID)
    {
        STB_Sprintf(m_stCliTerm.szCommandBuf, m_stCliTerm.szSendBuf, szPortID);
        STB_StrCpy(m_stCliTerm.szSendBuf,m_stCliTerm.szCommandBuf) ;
        m_stCliTerm.szCommandBuf[0] = '\0';
    }

    m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    m_stCliTerm.iPromptLen = m_stCliTerm.iSendLen - (int   )STB_StrLen(szNewLine) ;

    DBG_ASSERT(m_stCliTerm.iSendLen < 80);

    /* ���� ? ǰ�������� */
    if(m_stCliTerm.szEditBuf[0] != '\0')
    {
        STB_StrCat(m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf);
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }

    CLI_SendToTerm( ) ;

    return ;
}

/*********************************************************************/
/* �������� : CLI_SetPromptLen()                                      */
/* �������� : ���õ�ǰ��ʾ���ĳ���                                   */
/*********************************************************************/
ULONG  CLI_SetPromptLen(ULONG  ulStrLen)
{
    m_stCliTerm.iPromptLen = (int   )ulStrLen ;
    return TBS_SUCCESS ;
}

/*-----------------------------------------------------------------------------
 ��������    : CLI_GetTermLevel();
 ����        : ��õ�ǰ�û����û�Ȩ��
 �������    : ��.
 ����ֵ      : �û���������, ö��CLI_OPERAT_LEVEL_T
 ��������˵��:
 ����ʹ��ʾ��:
-----------------------------------------------------------------------------*/
ULONG  CLI_GetTermLevel( )
{
    DBG_ASSERT( m_stCliTerm.ucUserLevel < CLI_AL_NULL );

    return m_stCliTerm.ucUserLevel;
}


/*********************************************************************/
/* �������� : CLI_GetHostName()                                      */
/* �������� : �����ṩ����������ȡ����                               */
/* ������� :                                                        */
/* ������� :                                                        */
/* ����     : �������ַ���                                           */
/*********************************************************************/
BOOL CLI_IsTermBusy()
{
    if (m_stCliTerm.nTermStatus > TERM_LOGED)
        return TRUE;
    return FALSE;
}
/*==================================================================*/
/*      ������      :CLI_GetTermData                                */
/*      ��������    :��ȡ��ǰ�������������                         */
/*      �������    :ULONG  ulPid  ���̺�                           */
/*                                                                  */
/*      �������    :��                                             */
/*      ����ֵ      :PTerm_Data_S ��������ָ��                      */
/*      ���ú���    :CLI_GetTaskData ( )                            */
/*      ��������    :                                               */
/*==================================================================*/
ST_CLI_TERM* CLI_GetTermData ( _VOID    )
{
    return &m_stCliTerm;
}


/*==================================================================*/
/*      ������      :CLI_TermInit                                   */
/*      ��������    :��ʼ���ն�                                     */
/*      ��ע        :���������ĳ�ʼ�����ڸú����н���               */
/*==================================================================*/
_VOID    CLI_TermDataInit( )
{
    ULONG          ulTmp;

    /* ��ʼ����������   */
    m_stCliTerm.nTermStatus = TERM_SLEEPING;
    m_stCliTerm.szRecvBuf[0]    = '\0' ;
    m_stCliTerm.iRecvLen        = 0 ;
    m_stCliTerm.szSendBuf[0]    = '\0' ;
    m_stCliTerm.iSendLen        = 0 ;
    for ( ulTmp = 0 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
        m_stCliTerm.szHistory[ulTmp][0] = '\0' ;
    m_stCliTerm.iHistoryPos     = 0 ;
    m_stCliTerm.szEditBuf[0]    = '\0' ;
    m_stCliTerm.iCurrentPos     = 0 ;
    m_stCliTerm.iPromptLen      = 1 ;
    m_stCliTerm.szCommandBuf[0] = '\0' ;
    m_stCliTerm.szInputCmd[0] = '\0';
    m_stCliTerm.iEditStatus     = COMMAND_INPUT;
    m_stCliTerm.iMaxLen         = CLI_MAX_CMDBUF;
    m_stCliTerm.ulDispLineCount = 0;
    STB_StrCpy (m_stCliTerm.szCurUserName, UNLOGON_USER_NAME );
    m_stCliTerm.ucUserLevel     = 0 ;
    m_stCliTerm.ucScroll        = SCROLL_MANUAL;
    m_stCliTerm.ulTimeLeft      = DEADLINE_LONG;
    m_stCliTerm.bRecurCmd      = FALSE ;
    m_stCliTerm.bCommandExecOk = FALSE ;
    m_stCliTerm.ucInputStatus   = 0;


    /* ��ʼ��telnet �������ݲ���  */
    m_stCliTerm.nTermType = CLI_TERM_NOT_INITIAL;

{
     struct termios new_settings;
     tcgetattr(0,&stored_settings);
     new_settings = stored_settings;
     new_settings.c_lflag &= (~ECHO);
     /* Disable canonical mode, and set buffer size to 1 byte */
     new_settings.c_lflag &= (~ICANON);
     new_settings.c_cc[VTIME] = 0;
     new_settings.c_cc[VMIN] = 1;
     tcsetattr(0,TCSANOW,&new_settings);
}

    return ;
}

_VOID CLI_CmdRecord ( )
{
    ULONG  ulTmp ;
    char   szCommandBuf[CLI_MAX_CMDBUF] ;

    /* ȥ������ո� */
    _TrimString( szCommandBuf, m_stCliTerm.szCommandBuf ) ;

    /* �����������ǿ��򱣴�������ʷ���������         */
    if ( szCommandBuf[0] == '\0'
        || szCommandBuf[0] == KEY_CTRL_Z
        || szCommandBuf[0] == KEY_CTRL_C
        || strchr (szCommandBuf, '?') != NULL)
    {
        if (szCommandBuf[STB_StrLen(szCommandBuf) - 1] == '?')
        {
             szCommandBuf[STB_StrLen(szCommandBuf) - 1] = '\0' ;
             STB_StrCpy ( m_stCliTerm.szEditBuf, szCommandBuf ) ;
             m_stCliTerm.iCurrentPos = (int)STB_StrLen (m_stCliTerm.szEditBuf) ;
        }

        m_stCliTerm.bCommandExecOk = FALSE ;
        m_stCliTerm.szCommandBuf[0] = '\0' ;
        m_stCliWS.szCmdString[0] = '\0';
        return ;
    }

    /* ��Ѱ��һ�����е���ʷ�������                       */
    for ( ulTmp = 0 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
    {
        if ( ! ( m_stCliTerm.szHistory[ulTmp][0] ) )
            break ;
    }
    m_stCliTerm.iHistoryPos = (int   )ulTmp;

    if (ulTmp == 0 || STB_StriCmp(szCommandBuf, m_stCliTerm.szHistory[ulTmp - 1]))
    {

        /* �����ʷ�����������,���������ʷ�������        */
        if ( ulTmp == HISTORY_SIZE )
        {
            for ( ulTmp = 1 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
                STB_StrCpy ( m_stCliTerm.szHistory[ ulTmp - 1 ],
                         m_stCliTerm.szHistory[ ulTmp ] ) ;
            ulTmp = HISTORY_SIZE - 1 ;
        }

        /* ����ǰ����д�뵽�ҵ��Ŀ�����ʷ���������ȥ         */
        STB_StrCpy ( m_stCliTerm.szHistory[ ulTmp ], szCommandBuf ) ;
        m_stCliTerm.iHistoryPos = (int   )ulTmp + 1 ;
    }

    m_stCliTerm.bCommandExecOk = FALSE ;

    /* ����������,�ȴ�������һ������                        */
    m_stCliTerm.szCommandBuf[0] = '\0' ;
    m_stCliWS.szCmdString[0] = '\0';

    return ;
}



/*==================================================================*/
/*      ������      :CLI_TermDestroy                                */
/*      ��������    :�����ն�                                       */
/*==================================================================*/
_VOID    CLI_TermDestroy( )
{
     stored_settings.c_lflag |= ECHO;
     /* Disable canonical mode, and set buffer size to 1 byte */
     stored_settings.c_lflag |= ICANON;
     stored_settings.c_cc[VTIME] = 1;
     stored_settings.c_cc[VMIN] = 1;
     tcsetattr(0,TCSANOW,&stored_settings);
}

/* CTRL _ C ʲô���鶼���� */
void CLI_ProcForCtrlC(int n)
{
    m_stCliTerm.szEditBuf[0] = '\0' ;
    m_stCliTerm.iCurrentPos = 0 ;

    CLI_DisplayPrompt();

    fflush(0);
}



#ifdef __cplusplus
}
#endif


