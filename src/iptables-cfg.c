#include <ncurses.h>

#include "iptables-cfg.h"

int main()
{
	initscr();
	printw("iptables-cfg");
	refresh();
	getch();
	endwin();

	return 0;
}
