/*
 * milli_httpd - pretty small HTTP server
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * $Id: httpd.h,v 1.2 2001/09/29 23:00:32 mhuang Exp $
 */

#ifndef _httpd_h_
#define _httpd_h_

#include <stdio.h>

/* Basic authorization userid and passwd limit */
#define AUTH_MAX 64

/* Web status */
#define WEB_STS_ERROR      -1
#define WEB_STS_OK          0
#define WEB_STS_REBOOT      1
#define WEB_STS_RESTORE     2
#define WEB_STS_UPLOAD      3
#define WEB_UPLOAD_IMAGE    1       // for image upload
#define WEB_UPLOAD_SETTINGS 2       // for setting upload

/* Regular file handler */
extern void do_file(char *path, FILE *stream);

/* Authentication file handler */
extern void do_auth(char *userid, char *passwd, char *realm,
                    char *sptPasswd, char *usrPasswd);

/* Embedded JavaScript handler */
extern void do_ej(char *path, FILE *stream);

/* CGI file handler */
extern void do_cgi(char *path, FILE *stream);

/* Test CGI file handler */
extern void do_test_cgi(char *path, FILE *stream);

/* Test CGI file handler */
extern void do_cmd_cgi(char *path, FILE *stream);

/* Generic MIME type handler */
struct mime_handler {
	char *pattern;
	char *mime_type;
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm,
                char *sptPasswd, char *usrPasswd);
};

struct cgi_do_file_list{
	char *path;
	size_t len;
	void *data;
};

extern struct mime_handler mime_handlers[];
extern struct cgi_do_file_list cgi_do_file_table[];

/* CGI helper function */
extern void parse_cgi(char *path, void (*set)(char *name, char *value));

/* CGI upload handler */
//extern int  do_upload_pre(FILE *stream, int upload_len, int upload_type);
//extern void do_upload_post(void);

/* Embedded JavaScript function handler */
struct ej_handler {
	char *pattern;
	void (*output)(int argc, char **argv, FILE *stream);
};

extern struct ej_handler ej_handlers[];

#endif /* _httpd_h_ */
