// Set the locale
const unsigned int locale = LC_ALL;

// Define input modes
enum InputModeType
{
	MODE_NORMAL = OFFSET(0),
	MODE_INSERT = OFFSET(1),
};

#define KEY_LIST(...) (int[]) { __VA_ARGS__, 0 }

const struct Keybind normalkeymap[] =
{
	  /* Keybinds */            /* Function */
	{ KEY_LIST('h', KEY_LEFT),  moveleft      },
	{ KEY_LIST('l', KEY_RIGHT), moveright     },
	{ KEY_LIST('j', KEY_DOWN),  movedown      },
	{ KEY_LIST('k', KEY_UP),    moveup        },
	{ KEY_LIST('i'),            setmodeinsert },
	{ KEY_LIST('x'),            delunder      },
	{ KEY_LIST('q'),            quit          },
	{ NULL, NULL }
};

const struct Keybind insertkeymap[] =
{
	  /* Keybinds */           /* Function */
	{ KEY_LIST(27),            quitmode   },
	{ KEY_LIST(KEY_DOWN),      movedown   },
	{ KEY_LIST(KEY_LEFT),      moveleft   },
	{ KEY_LIST(KEY_RIGHT),     moveright  },
	{ KEY_LIST(KEY_UP),        moveup     },
	{ KEY_LIST(10),            insnewline },
	{ KEY_LIST(KEY_BACKSPACE), delbefore  },
	{ KEY_LIST(KEY_DL),        delunder   },
	{ NULL, NULL }
};

// The current ted mode (The default one is the Normal mode)
enum InputModeType currmode = MODE_NORMAL;  

const struct InputMode modes[] = 
{
	  /* Enum value */ /* Mode name */ /* Keymap */  /* Fallthrough handler function */
	{ MODE_NORMAL,     "Normal",       normalkeymap, fthandlenormal         },
	{ MODE_INSERT,     "Insert",       insertkeymap, fthandleinput          },
};
