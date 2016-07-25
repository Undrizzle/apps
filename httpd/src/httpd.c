/* milli_httpd - pretty small HTTP server
** A combination of
** micro_httpd - really small HTTP server
** and
** mini_httpd - small HTTP server
**
** Copyright © 1999,2000 by Jef Poskanzer <jef@acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/un.h> 
#include <signal.h>

#include "httpd.h"
#include <clidefs.h>
/* modify from original (PT) */
#include "cgimain.h"
#include "http2dbs.h"
#include "http2cmm.h"
#include "upload.h"
#include "syscall.h"
#include "cgimain.h"
#include "jsonmain.h"


#define SERVER_NAME "micro_httpd"
#define SERVER_URL "http://www.acme.com/software/micro_httpd/"
#define SERVER_PORT 80
#define PROTOCOL "HTTP/1.1"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define MONLOOP_TIMEOUT 1

/* A multi-family sockaddr. */
typedef union {
    struct sockaddr sa;
    struct sockaddr_in sa_in;
} usockaddr;

/* Globals. */
static FILE *conn_fp;
//static char auth_flag = 0;
//static char auth_sts = 0;
static char auth_userid[AUTH_MAX];
static char auth_passwd[AUTH_MAX];
static char auth_sptPasswd[AUTH_MAX];
static char auth_usrPasswd[AUTH_MAX];
static char auth_realm[AUTH_MAX];

/* Forwards. */
static int initialize_listen_socket( usockaddr* usaP );
static int initialize_api_socket( usockaddr* usaP );
static int auth_check( char* dirname, char* authorization, struct in_addr clntAddr );
static void send_authenticate( char* realm );
static void send_error( int status, char* title, char* extra_header, char* text );
static void send_headers( int status, char* title, char* extra_header, char* mime_type );
static int b64_decode( const char* str, unsigned char* space, int size );
static int match( const char* pattern, const char* string );
static int match_one( const char* pattern, int patternlen, const char* string );
static int handle_request(struct in_addr clntAddr);
static int early_auth(int fd, struct in_addr clntAddr);

struct cgi_do_file_list cgi_do_file_table[] = 
{
	{ "/webs/css/redmond/jquery-ui-1.10.3.custom.min.css", 0, NULL },
	{ "/webs/css/redmond/demos.css", 0, NULL },	
	{ "/webs/css/redmond/images/animated-overlay.gif", 0, NULL },
	{ "/webs/css/redmond/images/ui-bg_flat_0_aaaaaa_40x100.png", 0, NULL },	
	{ "/webs/css/redmond/images/ui-bg_flat_55_fbec88_40x100.png", 0, NULL },	
	{ "/webs/css/redmond/images/ui-bg_glass_75_d0e5f5_1x400.png", 0, NULL },	
	{ "/webs/css/redmond/images/ui-bg_glass_85_dfeffc_1x400.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-bg_glass_95_fef1ec_1x400.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-bg_gloss-wave_55_5c9ccc_500x100.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-bg_inset-hard_100_f5f8f9_1x100.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-bg_inset-hard_100_fcfdfd_1x100.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_217bc0_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_2e83ff_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_469bdd_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_6da8d5_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_cd0a0a_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_d8e7f3_256x240.png", 0, NULL },
	{ "/webs/css/redmond/images/ui-icons_f9bd01_256x240.png", 0, NULL },	
	{ "/webs/js/jquery-1.9.1.js", 0, NULL },
	{ "/webs/js/jquery-ui-1.10.3.custom.min.js", 0, NULL },	
	{ "/webs/util.js", 0, NULL },
	{ "/webs/stylemain.css", 0, NULL },
	{ "/webs/colors.css", 0, NULL },
	{ "/webs/favicon.ico", 0, NULL },
	{ "/webs/logo_corp.gif", 0, NULL },
	{ "/webs/delete.gif", 0, NULL },
	{ "/webs/footer.html", 0, NULL },
	{ "/webs/frmload.gif", 0, NULL },
	{ "/webs/help.gif", 0, NULL },
	{ "/webs/ico_Editor.gif", 0, NULL },
	{ "/webs/index.html", 0, NULL },
	{ "/webs/info.html", 0, NULL },
	{ "/webs/logo.html", 0, NULL },
	{ "/webs/main.html", 0, NULL },
	{ "/webs/menu.html", 0, NULL },
	{ "/webs/menuBcm.js", 0, NULL },
	{ "/webs/menuTitle.js", 0, NULL },
	{ "/webs/menuTree.js", 0, NULL },
	{ "/webs/net_down.gif", 0, NULL },
	{ "/webs/net_up.gif", 0, NULL },
	{ "/webs/show.gif", 0, NULL },
	{ "/webs/suppress.png", 0, NULL },
	{ "/webs/true.png", 0, NULL },
	{ "/webs/upload.html", 0, NULL },
	{ "/webs/uploadinfo.html", 0, NULL },
	{ "/webs/util.js", 0, NULL },
	{ "/webs/wait.gif", 0, NULL },
	{ "/webs/wecAlarmlog.html", 0, NULL },
	{ "/webs/wecCliUsers.html", 0, NULL },
	{ "/webs/wecNewCnu.html", 0, NULL },
	{ "/webs/wecOptResult2.html", 0, NULL },
	{ "/webs/wecOptlog.html", 0, NULL },
	//{ "/webs/wecPortPropety.html", 0, NULL },
	//{ "/webs/wecPortStas.html", 0, NULL },
	{ "/webs/wecPreView.html", 0, NULL },
	{ "/webs/wecReboot.html", 0, NULL },
	{ "/webs/wecRebootInfo.html", 0, NULL },
	{ "/webs/wecRestoreDefault.html", 0, NULL },
	{ "/webs/wecRestoreInfo.html", 0, NULL },
	{ "/webs/wecSaveDb.html", 0, NULL },
	{ "/webs/wecSnmpCfg.html", 0, NULL },
	{ "/webs/wecSyslog.html", 0, NULL },
	{ "/webs/wecWebUsers.html", 0, NULL },
	{ "/webs/wecWlistCtrl.html", 0, NULL },
	{ "/webs/wecLogout.html", 0, NULL },
	{ "/webs/upgrade1.html", 0, NULL },
	{ "/webs/upgrade2.html", 0, NULL },
	{ "/webs/upgrade3.html", 0, NULL },
	{ NULL, 0, NULL }
};

void cgi_do_file_table_free(void)
{
	struct cgi_do_file_list *handler;
	for (handler = &cgi_do_file_table[0]; handler->path; handler++)
	{
		handler->len = 0;
		if(NULL != handler->data)
		{
			free(handler->data);
			handler->data = NULL;
		}
	}
}

void cgi_do_file_table_init(void)
{
	FILE *fp;
	size_t malloc_total = 0;
	struct cgi_do_file_list *handler;
	
	for (handler = &cgi_do_file_table[0]; handler->path; handler++)
	{
		if( access(handler->path, 0) )
		{
			fprintf(stderr, "WARNNING: can not find %s\n", handler->path);
			continue;
		}
		if (!(fp = fopen(handler->path, "r")))
		{
			fprintf(stderr, "WARNNING: can not open %s\n", handler->path);
			continue;
		}
		
		fseek(fp, 0L, SEEK_END);
		handler->len = ftell(fp);
		if( handler->len == 0)
		{
			fprintf(stderr, "WARNNING: file %s length = 0\n", handler->path);
			fclose(fp);
			continue;
		}
		
		handler->data = malloc(handler->len);
		if( NULL == handler->data)
		{
			fprintf(stderr, "WARNNING: init_cgi_do_file_table malloc error\n");
			handler->len = 0;
			fclose(fp);
			continue;
		}
		fseek(fp, 0L, SEEK_SET);
		if (fread (handler->data, 1, handler->len, fp) != handler->len)
		{			
			fprintf(stderr, "WARNNING: fread file %s error\n", handler->path);
			handler->len = 0;
			free(handler->data);
			handler->data = NULL;
			fclose(fp);
			continue;
		}
		malloc_total += handler->len;
		fclose(fp);
	}

	fprintf(stderr, "INFO: init_cgi_do_file_table %ul Byte\n", malloc_total);
}

#if 0
// used in syscall.c for web remote upload
static char gIfName[32];
char *getIfName(void)
{
    return gIfName;
}
#endif

// defined in upload.c
//extern void sigUserCfgUpdate(char *xmlFileName);
// defined in cgimain.c
extern WEB_NTWK_VAR glbWebVar;

static int
initialize_listen_socket( usockaddr* usaP )
{
    int listen_fd;
    int i;

    memset( usaP, 0, sizeof(usockaddr) );
    usaP->sa.sa_family = AF_INET;
    usaP->sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
    usaP->sa_in.sin_port = htons( SERVER_PORT );
    listen_fd = socket( usaP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
    {
        perror( "socket" );
        return -1;
    }
    (void) fcntl( listen_fd, F_SETFD, 1 );
    i = 1;
    if ( setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i) ) < 0 )
    {
        perror( "setsockopt" );
        return -1;
    }
	
#if 0
	int tcp_size = 0;
	socklen_t optlen;
	tcp_size = 256 * 1024;
	optlen = sizeof(tcp_size);
	if( setsockopt(listen_fd,SOL_SOCKET,SO_RCVBUF, (char *)&tcp_size, optlen) < 0 )
	{
		perror( "\nERROR: setsockopt[SO_RCVBUF]\n" );
		return -1;
	}
	optlen = sizeof(tcp_size);
	if( getsockopt(listen_fd, SOL_SOCKET, SO_RCVBUF, &tcp_size, &optlen) < 0 )
	{
		perror( "\nERROR: getsockopt[SO_RCVBUF]\n" );
		return -1;
	}
	else
	{
		printf("\nINDO: SO_RCVBUF = %d Byte\n", tcp_size);
	}

	tcp_size = 128 * 1024;
	optlen = sizeof(tcp_size);
	if( setsockopt(listen_fd,SOL_SOCKET,SO_SNDBUF, (char *)&tcp_size, optlen) < 0 )
	{
		perror( "\nERROR: setsockopt[SO_SNDBUF]\n" );
		return -1;
	}
	optlen = sizeof(tcp_size);
	if( getsockopt(listen_fd, SOL_SOCKET, SO_SNDBUF, &tcp_size, &optlen) < 0 )
	{
		perror( "\nERROR: getsockopt[SO_SNDBUF]\n" );
		return -1;
	}
	else
	{
		printf("\nINDO: SO_SNDBUF = %d Byte\n", tcp_size);
	}
#endif	

	
    if ( bind( listen_fd, &usaP->sa, sizeof(struct sockaddr_in) ) < 0 )
    {
        perror( "bind" );
        return -1;
    }
    if ( listen( listen_fd, 10 ) < 0 )
    {
        perror( "listen" );
        return -1;
    }
    return listen_fd;
}

#if 0
static int initialize_api_socket( usockaddr* usaP )
{
    int api_fd;
    struct sockaddr_un addr; 
    struct timeval tv;

    api_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    tv.tv_sec=0;
    tv.tv_usec=10000;
    setsockopt(api_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/var/cfm_socketapi"); 
    bind(api_fd, (struct sockaddr *) &addr, sizeof(addr));

    return api_fd;
}
#endif

/* modify from original (PT) */

static int
auth_check( char* dirname, char* authorization, struct in_addr clntAddr )
{
    char authinfo[500];
    char* authpass;
    int l;

    /* Is this directory unprotected? */
    if ( !strlen(auth_passwd) )
        /* Yes, let the request go through. */
        return 1;

    //fprintf(stderr, "\nauthorization = %s\n", authorization);
    /* Basic authorization info? */
    if ( strncmp( authorization, "Basic ", 6 ) != 0 ) {
        send_authenticate( dirname );
        return 0;
    }

    /* Decode it. */
    l = b64_decode( &(authorization[6]), (unsigned char*)authinfo, sizeof(authinfo) );
    authinfo[l] = '\0';
    /* Split into user and password. */
    authpass = strchr( authinfo, ':' );
    if ( authpass == (char*) 0 ) {
        /* No colon?  Bogus auth info. */
        send_authenticate( dirname );
        return 0;
    }
    *authpass++ = '\0';
    //fprintf(stderr, "authinfo = %s, authpass = %s\n", authinfo, authpass);
    // is client address in Access Control List ?
    if ( TRUE ) {
        if ( TRUE ) {
            // is http enabled from lan ?
            if ( TRUE ) {
                if ( ( strcmp(authinfo, auth_userid) == 0 &&
                    strcmp(authpass, auth_passwd) == 0 ) ||
                    ( strcmp(authinfo, "user") == 0 &&
                    strcmp(authpass, auth_usrPasswd) == 0 ) ){
                        cgiSetVar("curUserName", authinfo);
                        return 1;
                    }
            }
        } else {
            // is http enabled from wan ?
            if ( TRUE ) {
                if ( strcmp(authinfo, "support") == 0 &&
                    strcmp(authpass, auth_sptPasswd) == 0 ) {
                        cgiSetVar("curUserName", authinfo);
                        return 1;
                    }
            }
        }
    }

    send_authenticate( dirname );
    return 0;
}

static void
send_authenticate( char* realm )
{
    char header[10000];

    (void) snprintf(
        header, sizeof(header), "WWW-Authenticate: Basic realm=\"%s\"", realm );
    send_error( 401, "Unauthorized", header, "Authorization required." );
}


static void
send_error( int status, char* title, char* extra_header, char* text )
{
    send_headers( status, title, extra_header, "text/html" );
    (void) fprintf( conn_fp, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title, status, title );
    (void) fprintf( conn_fp, "%s\n", text );
    (void) fprintf( conn_fp, "<HR>\n<ADDRESS><A HREF=\"%s\">%s</A></ADDRESS>\n</BODY></HTML>\n", SERVER_URL, SERVER_NAME );
    (void) fflush( conn_fp );
}


static void
send_headers( int status, char* title, char* extra_header, char* mime_type )
{
    time_t now;
    char timebuf[100];

    (void) fprintf( conn_fp, "%s %d %s\r\n", PROTOCOL, status, title );
    (void) fprintf( conn_fp, "Server: %s\r\n", SERVER_NAME );
    (void) fprintf( conn_fp,"Cache-Control: no-cache\r\n") ;
    now = time( (time_t*) 0 );
    (void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    (void) fprintf( conn_fp, "Date: %s\r\n", timebuf );
    if ( extra_header != (char*) 0 )
        (void) fprintf( conn_fp, "%s\r\n", extra_header );
    if ( mime_type != (char*) 0 )
        (void) fprintf( conn_fp, "Content-Type: %s\r\n", mime_type );
    (void) fprintf( conn_fp, "Connection: close\r\n" );
    (void) fprintf( conn_fp, "\r\n" );
}


/* Base-64 decoding.  This represents binary data as printable ASCII
** characters.  Three 8-bit binary bytes are turned into four 6-bit
** values, like so:
**
**   [11111111]  [22222222]  [33333333]
**
**   [111111] [112222] [222233] [333333]
**
** Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
*/

static int b64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

/* Do base-64 decoding on a string.  Ignore any non-base64 bytes.
** Return the actual number of bytes generated.  The decoded size will
** be at most 3/4 the size of the encoded, and may be smaller if there
** are padding characters (blanks, newlines).
*/
static int
b64_decode( const char* str, unsigned char* space, int size )
{
    const char* cp = NULL;
    int space_idx = 0, phase = 0;
    int d = 0, prev_d = 0;
    unsigned char c;

    space_idx = 0;
    phase = 0;
    for ( cp = str; *cp != '\0'; ++cp )
    {
        d = b64_decode_table[(int)*cp];
        if ( d != -1 )
        {
            switch ( phase )
            {
            case 0:
                ++phase;
                break;
            case 1:
                c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
                if ( space_idx < size )
                    space[space_idx++] = c;
                ++phase;
                break;
            case 2:
                c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
                if ( space_idx < size )
                    space[space_idx++] = c;
                ++phase;
                break;
            case 3:
                c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
                if ( space_idx < size )
                    space[space_idx++] = c;
                phase = 0;
                break;
            }
            prev_d = d;
        }
    }
    return space_idx;
}


/* Simple shell-style filename matcher.  Only does ? * and **, and multiple
** patterns separated by |.  Returns 1 or 0.
*/
int
match( const char* pattern, const char* string )
{
    const char* or;

    for (;;)
    {
        or = strchr( pattern, '|' );
        if ( or == (char*) 0 )
            return match_one( pattern, strlen( pattern ), string );
        if ( match_one( pattern, or - pattern, string ) )
            return 1;
        pattern = or + 1;
    }
}


static int
match_one( const char* pattern, int patternlen, const char* string )
{
    const char* p;

    for ( p = pattern; p - pattern < patternlen; ++p, ++string )
    {
        if ( *p == '?' && *string != '\0' )
            continue;
        if ( *p == '*' )
        {
            int i, pl;
            ++p;
            if ( *p == '*' )
            {
                /* Double-wildcard matches anything. */
                ++p;
                i = strlen( string );
            }
            else
                /* Single-wildcard matches anything but slash. */
                i = strcspn( string, "/" );
            pl = patternlen - ( p - pattern );
            for ( ; i >= 0; --i )
                if ( match_one( p, pl, &(string[i]) ) )
                    return 1;
            return 0;
        }
        if ( *p != *string )
            return 0;
    }
    if ( *string == '\0' )
        return 1;
    return 0;
}

void
do_file(char *path, FILE *stream)
{
	FILE *fp;
	int c;
	size_t len = 0;
	char * p = NULL;
	struct cgi_do_file_list *handler;
	
	for (handler = &cgi_do_file_table[0]; handler->path; handler++)
	{
		if( strcmp(handler->path, path) == 0 )
		{
			if(( 0 != handler->len ) && ( NULL != handler->data ) )
			{
				//find
				len = handler->len;
				p = (char *)handler->data;
				while(len--)
				{
					fputc(*p++, stream);
				}
				//fwrite(handler->data, handler->len, 1, stream);
				fflush(stream);
				return;
			}
		}
	}
	
	fprintf(stderr, "do_file(%s)\n", path);
	if (!(fp = fopen(path, "r")))
		return;
	while ((c = getc(fp)) != EOF)
		fputc(c, stream);
	fclose(fp);
}

static int
early_auth(int fd, struct in_addr clntAddr)
{
    if ( TRUE ) {
        return 0;
    }
    return 1;
}

static int
handle_request(struct in_addr clntAddr)
{
	char line[10000], method[10000], path[10000], protocol[10000], authorization[10000];
	char filename[10000], *query;
	char *cp;
	char *file;
	int len, i;
	struct mime_handler *handler;
	int cl = 0;
	char boundary[32];
	int upload_type = 0;
	char *extra_header = 0;
	char host[256];
	//char addr[32];
	int no_auth = 0;
	int icount = 0;

	/* Initialize the request variables. */
	strcpy(authorization, "");

	/* Parse the first line of the request. */
	while (fgets (line, sizeof (line), conn_fp) == (char *) 0)
	{
		//send_error( 400, "Bad Request", (char*) 0, "No request found." );
		// return WEB_STS_ERROR;		
		if(icount>5)
		{
			send_error( 400, "Bad Request", (char*) 0, "No request found." );
			return WEB_STS_ERROR;
		}
		icount++;
		//fprintf(stderr, "\nfgets no request found, continue[%d].", icount);
		//fflush(stderr);
		usleep(500000);
		continue;
	}
  
	if ( sscanf( line, "%[^ ] %[^ ] %[^ ]", method, path, protocol ) != 3 )
	{
		send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
		return WEB_STS_ERROR;
	}

	/* Parse the rest of the request headers. */
	while (fgets (line, sizeof (line), conn_fp) != (char *) 0)
	{
		if ( strcmp( line, "\n" ) == 0 || strcmp( line, "\r\n" ) == 0 )
			break;
		else if ( strncasecmp( line, "Authorization:", 14 ) == 0 )
		{
			cp = &line[14];
			cp += strspn( cp, " \t" );
			strncpy(authorization, cp, sizeof(authorization));
		}
		else if ((cp = strstr(line, "Content-Length:")) != NULL)
		{
			cp += strlen("Content-Length: ");
			cl = atoi(cp);
		}
		else if ((cp = strstr(line, "Referer:")) != NULL)
		{
			// search for 'upload.html' or 'updatesettings.html' to see where upload from 
			if ((cp = strstr(line, "upload.")) != NULL)
				upload_type = WEB_UPLOAD_IMAGE;
			else if ((cp = strstr(line, "updatesettings.")) != NULL)
				upload_type = WEB_UPLOAD_SETTINGS;
			else if ((cp = strstr(line, "grade")) != NULL)
				upload_type = WEB_UPLOAD_IMAGE;
		}
		else if ((cp = strstr(line, "boundary=")))
		{
			for( cp = cp + 9; *cp && *cp != '\r' && *cp != '\n'; cp++ );
			*cp = '\0';
			strncpy(boundary, &cp[9], sizeof(boundary));
		}
		else if ((cp = strstr(line, "Host:")) != NULL)
		{
			char *cp2;
			cp += strlen("Host: ");
			if( (cp2 = strstr(cp, "\r\n")) != NULL )
				*cp2 = '\0';
			else
				if( (cp2 = strstr(cp, "\n")) != NULL )
				*cp2 = '\0';
			strncpy( host, cp, sizeof(host) );
		}
	}
	/* modify from original (PT) */
	/* support post for software upgrade */
	if ( strcasecmp( method, "post" ) == 0 )
	{
		if ( strcasecmp( path, "/upload.cgi" ) == 0 || strcasecmp( path, "/uploadsettings.cgi" ) == 0 )
		{
			return (do_upload_pre(conn_fp, cl, upload_type));
		}
		if ( strcasecmp( path, "/upgrade1.cgi" ) == 0 )
		{
			return (do_upgrade_pre1(conn_fp, cl, upload_type));
		}
		if ( strcasecmp( path, "/upgrade2.cgi" ) == 0 )
		{
			return (do_upgrade_pre2(conn_fp, cl, upload_type));
		}
		if ( strcasecmp( path, "/upgrade3.cgi" ) == 0 )
		{
			return (do_upgrade_pre3(conn_fp, cl, upload_type));
		}
		else if ( strstr(path, ".json") != NULL )
		{
			return do_json(path, conn_fp, cl);
		}
	}
	/* end modify */

	if ( strcasecmp( method, "get" ) != 0 && strcasecmp(method, "post") != 0 )
	{
		send_error( 501, "Not Implemented", (char*) 0, "That method is not implemented." );
		return WEB_STS_ERROR;
	}
	if ( path[0] != '/' )
	{
		send_error( 400, "Bad Request", (char*) 0, "Bad filename." );
		return WEB_STS_ERROR;
	}
	file = &(path[1]);
	len = strlen( file );
	if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 )
	{
		send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
		return WEB_STS_ERROR;
	}

	if( strstr(file, "ppppassword") )
		no_auth = 1;       

	if ( file[0] == '\0' || file[len-1] == '/' )
#ifdef WEB_POPUP
		(void) snprintf( &file[len], sizeof(path) - len - 1, "index.html" );
#else
		(void) snprintf( &file[len], sizeof(path) - len - 1, "main.html" );
#endif
	path[sizeof(path) - 1] = '\0';
	/* modify from original (PT) */
	/* extract filename from file which has the following format
	filename?query. For ex: example.cgi?foo=1&bar=2 */
	if ( (query = strstr(file, "?")) != NULL )
	{
		for ( i = 0, cp = file; cp != query; i++, cp++ )
			filename[i] = *cp;
		filename[i] = '\0';
	} else
		strcpy(filename, file);
	/* end modify */

	for (handler = &mime_handlers[0]; handler->pattern; handler++)
	{
		/* use filename to match instead of use file */
		if (match(handler->pattern, filename)) 
		{
			if (handler->auth && no_auth == 0) 
			{
				handler->auth(auth_userid, auth_passwd, auth_realm, auth_sptPasswd, auth_usrPasswd);				
				if (!auth_check(auth_realm, authorization, clntAddr))
				{
					break;
				}
			}

			send_headers( 200, "Ok", extra_header, handler->mime_type );

			if (handler->output)
			{
				/* modify from original (PT) */
				strcpy(filename, "/webs/");
				strcat(filename, file);

				/* Handle post. Make post appears to be the same as get to handlers. (Needed by certificate UI code.)*/
#define FILENAME_SIZE 10000
				if ( strcasecmp( method, "post" ) == 0 )
				{
					int fnsize = strlen(filename);
					int remsize;
					int nread;
					//printf("----\n%s----\n", filename);
					remsize = FILENAME_SIZE - (fnsize+1) -1;
					nread = fread(filename+(fnsize+1), 1, remsize, conn_fp);
					if (nread > 0)
					{
						if (strstr(filename, "?"))
						{
							filename[fnsize]='&';
						}
						else
						{
							filename[fnsize]='?';
						}
						filename[fnsize+1+nread]=0;
					}
				}

				handler->output(filename, conn_fp);
			/* end modify */
			}
			break;
		}
	}

	if (!handler->pattern)
	{
		fprintf(stderr, "filename = %s\n", filename);
		send_error( 404, "Not Found", (char*) 0, "File not found." );
		return WEB_STS_OK;
	}        

	if ( strcmp(file, "wecRebootInfo.cgi") == 0 )
		return WEB_STS_REBOOT;  
	else if ( strcmp(file, "restoreinfo.cgi") == 0 )
		return WEB_STS_RESTORE;
	else
		return WEB_STS_OK;
}

// SIGUSR1 handler
void sigUserHandler(int sig) 
{
   
      
}

//SIGCHLD handler
void sigChldHandler(int sig)
{
	int pid;
	int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
#ifdef BRCM_DEBUG
	    printf("Child %d terminated\n", pid);
#endif
	}
}

/* modify from original (PT) */
int main(void)
{
    usockaddr usa;
    //int listen_fd, api_fd;
    int listen_fd;
    int conn_fd;
    socklen_t sz = sizeof(usa);
    //FILE *pid_fp;
    /* modify from original (PT) */
    int nready;
    //int done = 0;
    int ret = 0;
    int retFlag = WEB_STS_OK, rebootFlag = WEB_STS_OK;
    extern void destroy(void);
    fd_set rset;
    fd_set errset;
    struct timeval tv;
    // register SIGUSR1 handler
    signal(SIGUSR1, sigUserHandler);
	// register SIGCHLD handler
    signal(SIGCHLD, sigChldHandler);

    /* end modify */

    /* Ignore broken pipes */
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    /* Initialize listen socket */
    if ((listen_fd = initialize_listen_socket(&usa)) < 0) {
        fprintf(stderr, "can't bind to any address\n" );
        exit(errno);
    }

#if 0    
    /* Initialize socket API socket */
    if ((api_fd = initialize_api_socket(&usa)) < 0) {
        fprintf(stderr, "api socket error\n" );
        exit(errno);
    }
#endif

    http2dbs_init();
    http2cmm_init();
    httpd2sysmonitor_init();
    cgi_do_file_table_init();
	
    //sysSetMonitorFd(listen_fd);

    /* Daemonize and log PID */
    /* Comment out daemon() to remove zombie process */
    /*if (daemon(1, 1) == -1) {
    perror("daemon");
    exit(errno);
    }
    if (!(pid_fp = fopen("/var/run/httpd_pid", "w"))) {
        perror("/var/run/httpd_pid");
	 http2dbs_destroy();
	 http2cmm_destroy();
	 httpd2sysmonitor_destroy();
	 cgi_do_file_table_free();
        return errno;
    }
    fprintf(pid_fp, "%d\n", getpid());
    fclose(pid_fp);*/

    /* modify from original (PT) */
    cgiGetAllInfo();
    /* end modify */

    /* Loop forever handling requests */
    for (;;) {
        FD_ZERO(&rset);
        //FD_SET(api_fd,&rset);
        FD_SET(listen_fd,&rset);
        FD_ZERO(&errset);
        FD_SET(listen_fd,&errset);	 

        nready = select(sizeof(int)*8,&rset,NULL,&errset,NULL);
        
        if ( FD_ISSET(listen_fd, &errset) ) {
            //BcmWan_monitorInterfaces(&done);
            // Set the flags for static information to force the request
            // from browser to fetch the latest information.
        }
        else if ( nready == -1 ) {
            //perror("select");
            //printf("!!! HTTP server is received request with nready == -1 !!!\n");
            usleep(100);
            continue;
        } else if ( nready != 1 ) {
            perror("select");
        }

        if ( FD_ISSET(listen_fd,&rset)) {
            if ((conn_fd = accept(listen_fd, &usa.sa, &sz)) < 0) {
                perror("accept");
                shutdown(listen_fd, 2);
                close(listen_fd);
		  http2dbs_destroy();
		  http2cmm_destroy();
		  httpd2sysmonitor_destroy();
		  cgi_do_file_table_free();
                return errno;
            }

            tv.tv_sec=0;
            tv.tv_usec=500000;
            setsockopt(conn_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
#if 0
            // get the interface name..
            gIfName[0] = '\0';
            bcmGetIntfNameSocket(conn_fd, gIfName);
#endif
            retFlag = early_auth(conn_fd, usa.sa_in.sin_addr);
            if ( retFlag != 0) {
                close(conn_fd);   // go away
                continue;
            }
            if (!(conn_fp = fdopen(conn_fd, "r+"))) {
                perror("fdopen");
                shutdown(listen_fd, 2);
                close(listen_fd);
		  http2dbs_destroy();
		  http2cmm_destroy();
		  httpd2sysmonitor_destroy();
		  cgi_do_file_table_free();
                return errno;
            }
            retFlag = handle_request(usa.sa_in.sin_addr);

            // keep the reboot or restore flag to be used
            // when there is no activity in listen_fd
            if ( retFlag == WEB_STS_RESTORE ||
                retFlag == WEB_STS_REBOOT ||
                retFlag == WEB_STS_UPLOAD )
            {
            	  //printf("set rebootFlag = %d\n", retFlag);
		  rebootFlag = retFlag;
	     }
            
            if ( rebootFlag == WEB_STS_UPLOAD )
            {
                //sysWakeupMonitorTask();
                glbWebVar.upgStep = 1;
		  cgiWriteUpgPage(0, conn_fp, &glbWebVar);
                //do_upload_post();
		  rebootFlag = WEB_STS_OK;
	     }

            ret = fflush(conn_fp);
            fclose(conn_fp);
            close(conn_fd);


            // Wake up monitor task in case something happened while processing
            // the web pages. This is not required since events happening
            // during the processing of the HTTP request are posted when the
            // loops returns to wait on the select call.
            //sysWakeupMonitorTask();

        } else { // if no activity in listen_fd descriptor
            if ( rebootFlag == WEB_STS_RESTORE ) {
                //BcmDb_restoreDefault();
                shutdown(listen_fd, 2);
                close(listen_fd);
                //destroy();
            } else if ( rebootFlag == WEB_STS_REBOOT ) {
                if( 0 == cgiReboot() )
		  {
		  	shutdown(listen_fd, 2);
			close(listen_fd);
			//destroy();
			http2cmm_destroy();
			httpd2sysmonitor_destroy();
			http2dbs_destroy();
			cgi_do_file_table_free();
			return WEB_STS_OK;
		  }
            } 
	     else if ( rebootFlag == WEB_STS_UPLOAD )
            {
			//do_upload_post();
			printf("do_upload_post(2)");
	     }                
        } // FD_ISSET
    } // for

    shutdown(listen_fd, 2);
    close(listen_fd);
    http2dbs_destroy();
    http2cmm_destroy();
    httpd2sysmonitor_destroy();
    cgi_do_file_table_free();
	
    return WEB_STS_OK;
}




