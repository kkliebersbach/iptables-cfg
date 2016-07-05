#include <form.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "iptables-cfg.h"

int row, col;

int main()
{
	initscr();
	cbreak();
	noecho();
	getmaxyx(stdscr, row, col);

	ipvx_ports ipv4_ports = req_ipvx_ports(4);
	ipvx_ports ipv6_ports = req_ipvx_ports(6);

	endwin();
	return 0;
}

WINDOW* new_dialog(int height, int width, char* text)
{
	WINDOW* win = newwin(height, width, row / 2 - height / 2, col / 2 - width / 2);

	box(win, 0, 0);
	mvwprintw(win, height / 2 - 1, width / 2 - strlen(text) / 2, text);
	wrefresh(win);

	return win;
}

void del_dialog(WINDOW* win)
{
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}

ipvx_ports req_ipvx_ports(int version)
{
	char text[48];
	sprintf(text, "Enter the ports to open over the IPv%d protocol:", version);
	int height = 8, width = 56;
	WINDOW* dialog_win = new_dialog(height, width, text);
	WINDOW* form_win = newwin(height / 2 - 1, width - 2, row / 2, col / 2 - width / 2 + 1);
	keypad(form_win, TRUE);

	FIELD* field[2] = { new_field(1, width - 2, 0, 0, 0, 0), NULL };
	set_field_back(field[0], A_UNDERLINE);
	field_opts_off(field[0], O_AUTOSKIP);
	field_opts_off(field[0], O_STATIC);
	set_max_field(field[0], 1024);

	FORM* form = new_form(field);
	int form_row, form_col;
	scale_form(form, &form_row, &form_col);
	set_form_win(form, form_win);
	set_form_sub(form, derwin(form_win, form_row, form_col, 2, 2));

	post_form(form);
	wrefresh(form_win);

	int ch;
	while((ch = wgetch(form_win)) != KEY_ENTER_ASCII)
	{
		switch(ch)
		{
		case KEY_BACKSPACE:
			form_driver(form, REQ_DEL_PREV);
			break;
		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;
		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;
		default:
			form_driver(form, ch);
			break;
		}
		wrefresh(form_win);
	}
	form_driver(form, REQ_NEXT_FIELD);

	ipvx_ports ports = { 0, malloc(sizeof(long)) };
	int field_len;
	dynamic_field_info(field[0], NULL, &field_len, NULL);
	char* ports_str = malloc(field_len * sizeof(char));
	strcpy(ports_str, field_buffer(field[0], 0));
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
