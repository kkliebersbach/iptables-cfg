#ifndef _IPTABLES_CFG_H
#define _IPTABLES_CFG_H 1

#include <ncurses.h>

#define KEY_ENTER_ASCII 10 /* ASCII Enter keycode. */

#define PORTS_DEFAULT "22 25 80 443"
#define PORTS_TOK " "

#define IPVX_CONF_START \
"*filter\n\
:INPUT DROP [0:0]\n\
:FORWARD ACCEPT [0:0]\n\
:OUTPUT ACCEPT [0:0]\n\
-A INPUT -i lo -j ACCEPT\n\
-A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT\n"
#define IPV4_CONF_END \
"-A INPUT -p icmp -m icmp --icmp-type 8 -j ACCEPT\n\
COMMIT\n"
#define IPV6_CONF_END \
"-A INPUT -p ipv6-icmp -j ACCEPT\n\
COMMIT\n"

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
