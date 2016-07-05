#include <form.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "iptables-cfg.h"

int row, col, ctr_row, ctr_col;

int main()
{
	initscr();
	cbreak();
	getmaxyx(stdscr, row, col);
	ctr_row = row / 2;
	ctr_col = col / 2;

	DIALOG* dlg = newdlg(8, 32, EXEC_NAME);
	wgetch(dlg->win);
	deldlg(dlg);

	endwin();
	return 0;
}

DIALOG* newdlg(int height, int width, char* msg)
{
	int d_ctr_row = height / 2, d_ctr_col = width / 2;
	DIALOG* dlg = malloc(sizeof(DIALOG));
	dlg->win = newwin(height, width, row / 2 - d_ctr_row, col / 2 - d_ctr_col);

	box(dlg->win, 0, 0);
	mvwprintw(dlg->win, d_ctr_row - 1, d_ctr_col - strlen(msg) / 2, msg);
	wmove(dlg->win, d_ctr_row, 0);
	wrefresh(dlg->win);

	return dlg;
}

void deldlg(DIALOG* dlg)
{
	wborder(dlg->win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(dlg->win);
	delwin(dlg->win);
}
