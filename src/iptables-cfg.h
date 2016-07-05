#ifndef _IPTABLES_CFG_H
#define _IPTABLES_CFG_H 1

#include <ncurses.h>

#define KEY_ENTER_ASCII 10
#define PORTS_TOK " "

typedef struct
{
	int count;
	long* ports;
}
ipvx_ports;

WINDOW* new_dialog(int height, int width, char* text);
void del_dialog(WINDOW* win);
ipvx_ports req_ipvx_ports(int version);

#endif
