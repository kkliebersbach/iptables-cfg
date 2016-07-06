#ifndef _IPTABLES_CFG_H
#define _IPTABLES_CFG_H 1

#include <ctype.h>
#include <ncurses.h>

#define KEY_ENTER_ASCII 10 /* ASCII Enter keycode. */

#define PORTS_DEFAULT_TCP "22 25 80 443"
#define PORTS_TOK " "

#define IPV4_CONF "/tmp/iptables.conf"
#define IPV6_CONF "/tmp/ip6tables.conf"
#define IPVX_CONF_RULE "-A INPUT -p %s -m %s --dport %ld -j ACCEPT\n"
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

#define IPV4_COM_RESTORE "sh -c \"iptables-restore << cat %s\""
#define IPV6_COM_RESTORE "sh -c \"ip6tables-restore << cat %s\""
#define COM_NETFILTER "sh -c \"netfilter-persistent save \""

typedef struct
{
	int count;
	long* ports;
}
ipvx_ports_t;

typedef enum { IPV4 = 4, IPV6 = 6 } ipvx_t;
typedef enum { TCP, UDP } ipvx_protocol_t;
const char* IPVX_PROTOCOLS[] = { "tcp", "udp" };

static inline char* strupper(const char* str)
{
	int length = strlen(str);
	char* new_str = malloc(length * sizeof(char));
	strcpy(new_str, str);
	for (int i = 0; i < length; i++)
	{
		new_str[i] = toupper(new_str[i]);
	}
	return new_str;
}

WINDOW* new_dialog(int height, int width, char* text);
void del_dialog(WINDOW* win);
void append_ipvx_rule(FILE* config, long port, ipvx_protocol_t protocol);
int req_confirm(char* text);
ipvx_ports_t req_ipvx_ports(ipvx_t version, ipvx_protocol_t protocol);

#endif
