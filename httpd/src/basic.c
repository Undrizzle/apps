#include "httpd.h"

struct mime_handler mime_handlers[] = {
	{ "**.htm", "text/html", do_file, do_auth },
	{ "**.html", "text/html", do_ej, do_auth },
	{ "**.cgi", "text/html", do_cgi, do_auth },	
	{ "**.cmd", "text/html", do_cmd_cgi, do_auth },	
	{ "**.js", "text/js", do_file, do_auth },
	{ "**.gif", "image/gif", do_file, do_auth },
	{ "**.jpg", "image/jpeg", do_file, do_auth },
	{ "**.jpeg", "image/gif", do_file, do_auth },
	{ "**.png", "image/png", do_file, do_auth },
	{ "**.ico", "image/ico", do_file, do_auth },
	{ "**.css", "text/css", do_file, do_auth },
	//{ "**.conf", "config/conf", do_cmd_cgi, do_auth },
	//{ "**.tst", "text/html", do_test_cgi, do_auth },
	//{ "**.au", "audio/basic", do_file, do_auth },
	//{ "**.wav", "audio/wav", do_file, do_auth },
	//{ "**.avi", "video/x-msvideo", do_file, do_auth },
	//{ "**.mov", "video/quicktime", do_file, do_auth },
	//{ "**.mpeg", "video/mpeg", do_file, do_auth },
	//{ "**.vrml", "model/vrml", do_file, do_auth },
	//{ "**.midi", "audio/midi", do_file, do_auth },
	//{ "**.mp3", "audio/mpeg", do_file, do_auth },
	//{ "**.pac", "application/x-ns-proxy-autoconfig", do_file, do_auth },
	{ NULL, NULL, NULL, NULL }
};
