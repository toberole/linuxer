//
// Created by linux on 19-1-19.
//

#ifndef LINUXER_XHTTPD_ENGINE_H
#define LINUXER_XHTTPD_ENGINE_H

void send_status(int status, char *desc);

void send_header(char *header, char *header_value);

int send_msg(char *msg, char *title, int flag);

int send_file(char *filename);

#endif //LINUXER_XHTTPD_ENGINE_H
