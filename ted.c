#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define OFFSET(x) (1 << x)	

// Function and struct definitions
typedef void (*InputModeFunction)(int keycode);

struct InputMode
{
	int mode;
	const char *name;
	int keycode;
	InputModeFunction handler;
};

struct CursorMovement
{
	int x, y;
	int keycode, altkeycode;
	bool relative;
};

int clampi(int x, int min, int max);
void drawui();
int getcharwidth(wchar_t c);
int getcurrmode();
void handleinput();
void handlenormal();
bool ismodeactive(int mode);
void movecursor(int x, int y);
void quit();
void setmode(int mode);

// Include the configuration header file
#include "config.h"

const int modescount = sizeof(modes) / sizeof(struct InputMode);
const int cmovementscount = sizeof(cmovements) / sizeof(struct CursorMovement);

int
clampi(int x, int min, int max)
{
	if (x <= min) return min;
	if (x >= max) return max;
	return x;
}

void 
drawui()
{
	// Print mode string
	char mstr[32];
	int currmodeidx = getcurrmode();
	if (currmodeidx != -1)
	{
		sprintf(mstr, "[ %s mode ]", modes[currmodeidx].name);
		mvprintw(LINES - 1, 0, mstr);
	}

	// Print version string
	const char *vstr = "ted - Version " VERSION;
	mvprintw(LINES - 1, COLS - strlen(vstr), vstr);
}

// Initialize the fake window
static WINDOW *fakewin;

int
getcharwidth(wchar_t c)
{
	// If the fake window was not yet initialized, do it here
	if (fakewin == NULL)
	{
		fakewin = newwin(2, 10, 0, 0);
	}

	// Move to position (0, 0)
	wmove(fakewin, 0, 0);

	// Insert the character
	waddch(fakewin, c);

	// Get new x and y positions
	int newx = 0, newy = 0;
	getyx(fakewin, newy, newx);

	// Return x position
	return newx;
}

int
getcurrmode()
{
	for (int i = 0; i < modescount; ++i)
		if (modes[i].mode == currmode)
			return i;
	return -1;
}

void
handleinput(int keycode)
{
	if (keycode == 27) {
		if (getch() == -1) { 
			setmode(MODE_NORMAL); 
			return; 
		}
	} 
	else if (keycode == KEY_BACKSPACE)
	{
		movecursor(cursorx - 1, cursory);
		delch();
	}
	else if (keycode == 10) // Newline character for Enter key
	{
		addch('\n');
		movecursor(0, cursory + 1);
	}
	else
	{
		int cwidth = getcharwidth(keycode);
		addch(keycode);
		movecursor(cursorx + cwidth, cursory);
	}
}

void
handlenormal(int keycode)
{
	// Mode keybinds
	for (int i = 1; i < modescount; ++i)
	{
		if (modes[i].keycode == keycode)
		{
			setmode(modes[i].mode);
			return;
		}
	}

	// Quit when q is pressed
	if (keycode == 'q') quit();
	
	// Handle cursor movements
	for (int i = 0; i < cmovementscount; ++i)
	{
		if (keycode == cmovements[i].keycode ||
		    keycode == cmovements[i].altkeycode)
		{
			int x, y;
			if (cmovements[i].relative)
			{
				x = cursorx + cmovements[i].x;
				y = cursory + cmovements[i].y;
			}
			else 
			{
				x = cmovements[i].x;
				y = cmovements[i].y;
			}
			movecursor(x, y);
			break;
		}
	}
}

bool
ismodeactive(int mode)
{
	return currmode == mode;
}

void
movecursor(int x, int y)
{
	mvchgat(cursory, cursorx, 1, A_NORMAL, 0, NULL); // Reset attributes for previous position
	cursorx = clampi(x, 0, COLS);
	cursory = clampi(y, 0, LINES);
	move(cursory, cursorx);
	mvchgat(cursory, cursorx, 1, A_REVERSE, 0, NULL); // Set new attribute for new position
}

void 
setmode(int mode)
{
	currmode = mode;
}

void 
quit()
{
	// Delete the fake window
	delwin(fakewin);

	endwin();
	exit(EXIT_SUCCESS);
}

void 
run()
{
	int key = 0;
	while (1) 
	{
		// Handle keyboard input
		key = getch();	

		// Move the cursor back
		movecursor(cursorx, cursory);

		if (key != ERR)
		{
			// Call mode handler function
			int currmodeidx = getcurrmode();
			if (currmodeidx != -1)
			{
				(*modes[currmodeidx].handler)(key);
			}
		}

		// Draw UI
		drawui();

		// Refresh
		refresh();

		// Sleep for 15ms to not hog the CPU
		napms(15);
	}
}

int 
main(void)
{
	setlocale(locale, "");

	// Initialize ncurses
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	// Set the ESCDELAY global variable to 0 to remove the delay after the Escape
	// key is pressed.
	set_escdelay(0);

	// Hide the default screen cursor
	curs_set(0);

	// Make getch a non-blocking call
	nodelay(stdscr, TRUE);

	// Run ted
	run();

	// End
	quit();
}
