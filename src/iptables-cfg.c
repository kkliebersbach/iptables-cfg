#include <form.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "iptables-cfg.h"

int row, col;

int main()
{
	initscr();
	cbreak(); /* Disable line buffering. */
	noecho(); /* Stop echoing characters to the screen. */
	getmaxyx(stdscr, row, col);

	FILE* ipv4_config = fopen(IPV4_CONF, "w+");
	/* Request TCP and UDP ports to open. */
	ipvx_ports_t ipv4_tcp_ports = req_ipvx_ports(IPV4, TCP);
	ipvx_ports_t ipv4_udp_ports = req_ipvx_ports(IPV4, UDP);
	/* Construct configuration file. */
	fprintf(ipv4_config, IPVX_CONF_START);
	for (int i = 0; i < ipv4_tcp_ports.count; i++)
	{
		append_ipvx_rule(ipv4_config, ipv4_tcp_ports.ports[i], TCP);
	}
	for (int i = 0; i < ipv4_udp_ports.count; i++)
	{
		append_ipvx_rule(ipv4_config, ipv4_udp_ports.ports[i], UDP);
	}
	fprintf(ipv4_config, IPV4_CONF_END);
	fclose(ipv4_config);

	FILE* ipv6_config = fopen(IPV6_CONF, "w+");
	ipvx_ports_t ipv6_tcp_ports = req_ipvx_ports(IPV6, TCP);
	ipvx_ports_t ipv6_udp_ports = req_ipvx_ports(IPV6, UDP);
	fprintf(ipv6_config, IPVX_CONF_START);
	for (int i = 0; i < ipv6_tcp_ports.count; i++)
	{
		append_ipvx_rule(ipv6_config, ipv6_tcp_ports.ports[i], TCP);
	}
	for (int i = 0; i < ipv6_udp_ports.count; i++)
	{
		append_ipvx_rule(ipv6_config, ipv6_udp_ports.ports[i], UDP);
	}
	fprintf(ipv6_config, IPV6_CONF_END);
	fclose(ipv6_config);

	if(req_confirm("Apply new rules?")) /* Show confirmation dialog. */
	{
		/* Execute iptables-restore to apply configuration files. */
		char iptables_restore[64];
		sprintf(iptables_restore, IPV4_COM_RESTORE, IPV4_CONF);
		system(iptables_restore);
		char ip6tables_restore[64];
		sprintf(ip6tables_restore, IPV6_COM_RESTORE, IPV6_CONF);
		system(ip6tables_restore);

		if(req_confirm("Persist changes? (requires netfilter-persistent)"))
		{
			system(COM_NETFILTER);
		}
	}

	endwin();
	return 0;
}

WINDOW* new_dialog(int height, int width, char* text)
{
	int text_length = strlen(text);
	width = (text_length > width - 2) ? text_length + 2 : width;
	WINDOW* win = newwin(height, width, row / 2 - height / 2,\
						col / 2 - width / 2);

	box(win, 0, 0); /* Draw a box around the border of the window. */
	mvwprintw(win, height / 2 - 1, width / 2 - text_length / 2, text);
	wrefresh(win);

	return win;
}

void del_dialog(WINDOW* win)
{
	/* Reset the window border. */
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}

void append_ipvx_rule(FILE* config, long port, ipvx_protocol_t protocol)
{
	const char* protocol_str = IPVX_PROTOCOLS[protocol];
	fprintf(config, IPVX_CONF_RULE, protocol_str, protocol_str, port);
}

int req_confirm(char* text)
{
	int height = 8, width = 32;
	WINDOW* dialog_win = new_dialog(height, width, text);
	getmaxyx(dialog_win, height, width); /* Update height and width. */
	keypad(dialog_win, TRUE); /* Enable function keys. */
	curs_set(0); /* Hide cursor. */

	int no_row = height / 2, no_col = width - 4;
	mvwprintw(dialog_win, no_row, no_col, "No");
	int yes_row = height / 2, yes_col = 2;
	mvwprintw(dialog_win, yes_row, yes_col, "Yes");

	int ch, confirm = FALSE;
	do
	{
		/* Update text highlight to display selection. */
		mvwchgat(dialog_win, no_row, no_col, 2, confirm ? A_NORMAL : A_REVERSE, 0, NULL);
		mvwchgat(dialog_win, yes_row, yes_col, 3, confirm ? A_REVERSE : A_NORMAL, 0, NULL);
		wrefresh(dialog_win);

		switch(ch = wgetch(dialog_win))
		{
		case 'y':
			return TRUE;
			break;
		case 'n':
			return FALSE;
			break;
		case KEY_LEFT:
			confirm = TRUE;
			break;
		case KEY_RIGHT:
			confirm = FALSE;
			break;
		default:
			break;
		}
	}
	while(ch != KEY_ENTER_ASCII);

	del_dialog(dialog_win);
	curs_set(1); /* Show cursor. */

	return confirm;
}

ipvx_ports_t req_ipvx_ports(ipvx_t version, ipvx_protocol_t protocol)
{
	char text[52];
	sprintf(text, "Enter the %s ports to open over the IPv%d protocol:", \
			strupper(IPVX_PROTOCOLS[protocol]), version);
	int height = 8, width = 56;
	WINDOW* dialog_win = new_dialog(height, width, text);
	getmaxyx(dialog_win, height, width); /* Update height and width. */
	WINDOW* form_win = newwin(height / 2 - 1, width - 2, \
							row / 2, col / 2 - width / 2 + 1);
	keypad(form_win, TRUE); /* Enable function keys. */

	FIELD* field[2] = { new_field(1, width - 2, 0, 0, 0, 0), NULL };
	set_field_back(field[0], A_UNDERLINE);
	/* Prevent next field from automatically being selected. */
	field_opts_off(field[0], O_AUTOSKIP);
	field_opts_off(field[0], O_STATIC); /* Make field length dynamic. */
	set_max_field(field[0], 1024);
	/* Set default TCP ports. */
	if (protocol == TCP) set_field_buffer(field[0], 0, PORTS_DEFAULT_TCP);

	FORM* form = new_form(field);
	int form_row, form_col;
	scale_form(form, &form_row, &form_col);
	set_form_win(form, form_win);
	set_form_sub(form, derwin(form_win, form_row, form_col, 2, 2));

	post_form(form);
	wrefresh(form_win);

	/* Read characters until the Enter key is pressed. */
	int ch;
	while((ch = wgetch(form_win)) != KEY_ENTER_ASCII)
	{
		switch(ch)
		{
		/* Delete the previous character if the Backspace key was pressed. */
		case KEY_BACKSPACE:
			form_driver(form, REQ_DEL_PREV);
			break;
		/* Check if arrow key was pressed and move cursor accordingly. */
		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;
		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;
		/* Pass the character to the form driver otherwise. */
		default:
			form_driver(form, ch);
			break;
		}
		wrefresh(form_win);
	}
	/* Send focus to the next field so that the buffer is updated. */
	form_driver(form, REQ_NEXT_FIELD);

	/* Create structure to return values in and allocate the ports array. */
	ipvx_ports_t ports = { 0, malloc(sizeof(long)) };
	/* Get field length so suitable memory can be allocated to store it. */
	int field_len;
	dynamic_field_info(field[0], NULL, &field_len, NULL);
	/* Copy field buffer into new memory space so that it can be accessed. */
	char* ports_str = malloc(field_len * sizeof(char));
	strcpy(ports_str, field_buffer(field[0], 0));
	/* Tokenize the buffer on each space character and parse each token as a
	   long, before appending it to the ports array and reallocating it. */
	char* token = strtok(ports_str, PORTS_TOK);
	while(token != NULL)
	{
		ports.ports[ports.count++] = strtol(token, NULL, 10);
		ports.ports = realloc(ports.ports, (ports.count + 1) * sizeof(long));
		token = strtok(NULL, PORTS_TOK);
	}
	free(ports_str);

	unpost_form(form);
	free_form(form);
	free_field(field[0]);
	delwin(form_win);
	del_dialog(dialog_win);

	return ports;
}
