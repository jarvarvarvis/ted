// Set the locale
const unsigned int locale = LC_ALL;

// Define input modes
enum InputModeType
{
	MODE_NORMAL = OFFSET(0),
	MODE_INSERT = OFFSET(1),
};

enum InputModeType currmode = MODE_NORMAL;  // The current ted mode (The default one is the Normal mode)
const struct InputMode modes[] = 
{
	  /* Enum value */ /* Mode name */ /* Keycode */ /* Handler function */
	{ MODE_NORMAL,     "Normal",       0,            handlenormal         },
	{ MODE_INSERT,     "Insert",       'i',          handleinput          },
};

// Cursor movement
int cursorx = 0; // Cursor X position
int cursory = 0; // Cursor Y position

const struct CursorMovement cmovements[] =
{
	  /* X */ /* Y */  /* Keycode */ /* Alternative Keycode */ /* Relative? */
	{ -1,      0,      'h',          KEY_LEFT,                 true  }, // Left
	{ +1,      0,      'l',          KEY_RIGHT,                true  }, // Right
	{  0,     -1,      'k',          KEY_UP,                   true  }, // Up
	{  0,     +1,      'j',          KEY_DOWN,                 true  }, // Down
	// {  ?,      0,      'a',          -1,                       false }, // Start of line
	// {  ?,      0,      'e',          -1,                       false }, // End of line
};
