#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "msg.h"

void request_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

void request_read_headers(int fd);

int request_parse_uri(char *uri, char *filename, char *cgiargs);

void request_handle(int fd, char *method, char *uri, char *version);

#endif // __REQUEST_H__
