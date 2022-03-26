#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define OFFSET(x) (1 << x)	

// Function and struct definitions
typedef void (*KeybindFunction)();

struct Keybind
{
	const int *keycodes;
	KeybindFunction function;
};

typedef void (*InputModeFallthroughHandler)(int keycode);

struct InputMode
{
	int mode;
	const char *name;
	const struct Keybind *keymap;
	InputModeFallthroughHandler fallthrough;
};

int clampi(int x, int min, int max);
void delbefore();
void delunder();
void drawui();
void fthandleinput();
void fthandlenormal();
int getcharwidth(wchar_t c);
int getcurrmode();
void insnewline();
bool ismodeactive(int mode);
void movecursor(int x, int y);
void movedown();
void moveleft();
void moveright();
void moveup();
void quit();
void quitmode();
void setmode(int mode);
void setmodeinsert();
void setmodenormal();

// Include the configuration header file
#include "config.h"

int cursorx = 0;
int cursory = 0;

const int modescount = sizeof(modes) / sizeof(struct InputMode);

int
clampi(int x, int min, int max)
{
	if (x <= min) return min;
	if (x >= max) return max;
	return x;
}

void
delbefore()
{
	movecursor(cursorx - 1, cursory);
	delch();
}

void
delunder()
{
	delch();
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

void
fthandleinput(int keycode)
{
	addch(keycode);
	// Move the cursor by the width of the inserted character
	int charwidth = getcharwidth(keycode);
	movecursor(cursorx + charwidth, cursory);
}

void
fthandlenormal(int keycode)
{
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
insnewline()
{
	addch('\n');
	movecursor(0, cursory + 1);
}

bool
ismodeactive(int mode)
{
	return currmode == mode;
}

void
movecursor(int x, int y)
{
	mvchgat(cursory, cursorx, 1, A_NORMAL, 0, NULL);  // Reset attributes for previous position
	cursorx = clampi(x, 0, COLS - 1);
	cursory = clampi(y, 0, LINES - 2);                // -2, so the cursor can't enter the status bar
	move(cursory, cursorx);
	mvchgat(cursory, cursorx, 1, A_REVERSE, 0, NULL); // Set new attribute for new position
}

void movedown()  { movecursor(cursorx, cursory + 1); }
void moveleft()  { movecursor(cursorx - 1, cursory); }
void moveright() { movecursor(cursorx + 1, cursory); }
void moveup()    { movecursor(cursorx, cursory - 1); }

void 
setmode(int mode)
{
	currmode = mode;
}

void
setmodeinsert()
{
	setmode(MODE_INSERT);
}

void
setmodenormal()
{
	setmode(MODE_NORMAL);
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
quitmode()
{
	if (getch() == -1) { 
		setmode(MODE_NORMAL); 
		return; 
	}
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
			// Get current mode
			int currmodeidx = getcurrmode();
			if (currmodeidx != -1)
			{
				// Iterate current mode's keymap
				const struct Keybind *currkb;
				for (currkb = modes[currmodeidx].keymap; currkb && currkb->keycodes != NULL; ++currkb)
				{
					// Iterate keycodes in the keymap
					const int *currkeycode;
					for (currkeycode = currkb->keycodes; currkeycode && *currkeycode != 0; ++currkeycode)
					{
						// If the current keycode matches the pressed key...
						if (*currkeycode == key)
						{
							// Run the keybind function if it is valid.
							if (currkb->function)
							{
								(*currkb->function)();
								goto break_loop;
							}
						}
					}
				}

				// If no key from the keymap was pressed,
				// call the fallthrough handler function.
				(*modes[currmodeidx].fallthrough)(key);

break_loop:
				;
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
