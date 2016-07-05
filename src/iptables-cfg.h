#ifndef _IPTABLES_CFG_H
#define _IPTABLES_CFG_H 1

#include <ncurses.h>

#define EXEC_NAME "iptables-cfg"
#define EXEC_VER "0.0.1"

typedef struct
{
	WINDOW* win;
	FIELD* field;
} DIALOG;

DIALOG* newdlg(int height, int width, char* msg);
void deldlg(DIALOG* dlg);

#endif
