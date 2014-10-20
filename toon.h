/* toon.h - functions for drawing cartoons on the root window
 * Copyright (C) 1999-2001  Robin Hogan
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#define _(string) string

#define TOON_UNASSOCIATED -2
#define TOON_HERE -1
#define TOON_LEFT 0
#define TOON_RIGHT 1
#define TOON_UP 2
#define TOON_DOWN 3
#define TOON_UPLEFT 4
#define TOON_UPRIGHT 5
#define TOON_DOWNLEFT 6
#define TOON_DOWNRIGHT 7

#define TOON_FORCE 1
#define TOON_MOVE 0
#define TOON_STILL -1

#define TOON_OK 1
#define TOON_PARTIALMOVE 0
#define TOON_BLOCKED -1
#define TOON_SQUASHED -2

/* General configuration options */
#define TOON_DEFAULTS 0L

#define TOON_NOEDGEBLOCK (1L<<0)
#define TOON_EDGEBLOCK (1L<<1)
#define TOON_SIDEBOTTOMBLOCK (1L<<2)
#define TOON_NOSOLIDPOPUPS (1L<<4)
#define TOON_SOLIDPOPUPS (1L<<5)
#define TOON_NOSHAPEDWINDOWS (1L<<6)
#define TOON_SHAPEDWINDOWS (1L<<7)
#define TOON_SQUISH (1L<<8)
#define TOON_NOSQUISH (1L<<9)

#define TOON_NOCATCHSIGNALS (1L<<16)
#define TOON_CATCHSIGNALS (1L<<17)
#define TOON_EXITGRACEFULLY (1L<<18)

/* Configuration for individual toon types */
#define TOON_NOCYCLE (1L<<0)
#define TOON_INVULNERABLE (1L<<1)
#define TOON_NOBLOCK (1L<<2)

#define TOON_MESSAGE_LENGTH 128
#define TOON_DEFAULTMAXRELOCATE 8

/*** STRUCTURES ***/

/* The ToonData structure describes the properties of a type of toon,
 * such as walker, climber etc. */

typedef struct __ToonData ToonData;

struct __ToonData {
  unsigned long int conf; /* bitmask of toon properties such as cycling etc */
  char **image;
  char *filename; /* Name of image file name */
  ToonData *master; /* If pixmap data is duplicated from another toon, this is it */
  Pixmap pixmap, mask; /* pointers to X structures */
  unsigned int nframes; /* number of frames in image */
  unsigned int ndirections; /* number directions in image (1 or 2) */
  unsigned int width, height; /* width & height of individual frame/dir */
  unsigned int acceleration, speed, terminal_velocity;
  int loop; /* Number of times to repeat cycle */
  char exists;
};

/* The Toon structure describes the properties of a particular toon,
 * such as its location and speed */

typedef struct {
  int x, y, u, v; /* new position and velocity */
  unsigned int genus, type, frame, direction;
  int x_map, y_map;
  unsigned int width_map, height_map;
    /* properties of the image mapped on the screen */
  unsigned int associate; /* toon is associated with a window */
  int xoffset, yoffset; /* location relative to window origin */
  unsigned int wid; /* window associated with */
  unsigned int cycle; /* Number of times frame cycle has repeated */
  char pref_direction;
  char pref_climb;
  char hold;
  char active;
  char terminating;
  char mapped;
  char squished;
} Toon;

/*** INTERNAL STRUCTURES AND FUNCTION PROTOTYPES ***/
typedef struct {
  int solid;
  unsigned int wid;
  XRectangle pos;
} __ToonWindowData;
typedef int __ToonErrorHandler();
void __ToonSignalHandler(int sig);
int __ToonError(Display *toon_display, XErrorEvent *error);
void __ToonExitGracefully(int sig);

/* OK, OK, global variables are gross but it makes the interface easier,
 * and we're unlikely to want to two threads in the same program writing
 * to two different screens */
extern Display *toon_display;
extern Window toon_root; /* the window to draw to */
extern Window toon_parent; /* the parent window of all the client windows */
extern Window toon_root_override; /* override both toon_root and toon_parent */
extern int toon_x_offset, toon_y_offset; /* toon_root relative to toon_parent */
extern int toon_display_width, toon_display_height;
extern GC toon_drawGC;
extern Region toon_windows;
extern unsigned int toon_nwindows;
extern __ToonWindowData *toon_windata;
extern ToonData **toon_data;
extern int toon_ngenera;
extern int toon_ntypes;
extern int toon_errno;
extern char toon_edge_block;
extern char toon_shaped_windows;
extern char toon_solid_popups;
extern int toon_signal;
extern char toon_error_message[TOON_MESSAGE_LENGTH];
extern char toon_message[TOON_MESSAGE_LENGTH];
extern int toon_max_relocate_up;
extern int toon_max_relocate_down;
extern int toon_max_relocate_left;
extern int toon_max_relocate_right;
extern int toon_button_x;
extern int toon_button_y;
extern char toon_expose;
extern char toon_squish;
extern Window toon_squish_window;

/*** FUNCTION PROTOTYPES ***/

/* HANDLE STRANGE WINDOW MANAGERS (toon_root.c) */
Window ToonGetRootWindow(Display *display, int screen,
			 Window *clientparent);

/* SIGNAL AND ERROR HANDLING FUNCTIONS (toon_signal.c) */
int ToonSignal();
char *ToonErrorMessage();

/* STARTUP FUNCTIONS (toon_init.c) */
Display *ToonOpenDisplay(char *display_name);
int ToonInit(Display *display);
int ToonConfigure(unsigned long int code);
int ToonInstallData(ToonData **data, int ngenera, int ntypes);
#define ToonSetRoot(id) toon_root_override = (Window) id

/* FINISHING UP (toon_end.c) */
int ToonFinishUp();
int ToonFreeData();
int ToonCloseDisplay();

/* DRAWING FUNCTIONS (toon_draw.c) */
int ToonDraw(Toon *toon,int n);
int ToonErase(Toon *toon,int n);
void ToonFlush();

/* QUERY FUNCTIONS (toon_query.c) */
#define ToonDisplayWidth() (toon_display_width)
#define ToonDisplayHeight() (toon_display_height)
int ToonBlocked(Toon *toon, int direction);
int ToonOffsetBlocked(Toon *toon, int xoffset, int yoffset);
int ToonCheckBlocked(Toon *toon, int type, int gravity);
int ToonWindowsMoved();

/* ASSIGNMENT FUNCTIONS (toon_set.c) */
void ToonMove(Toon *toon, int xoffset, int yoffset);
void ToonSetPosition(Toon *toon, int x, int y);
void ToonSetGenusAndType(Toon *toon, int genus, int type, int direction,
			 int gravity);
#define ToonSetType(toon, type, direction, gravity) \
     ToonSetGenusAndType(toon, (toon)->genus, type, direction, gravity)
void ToonSetVelocity(Toon *toon, int u, int v);

/* CORE FUNCTIONS (toon_core.c) */
int ToonAdvance(Toon *toon, int mode);
int ToonLocateWindows();
int ToonSleep(unsigned long usecs);

/* HANDLING TOON ASSOCIATIONS WITH MOVING WINDOWS (toon_associate.c) */
void ToonSetAssociation(Toon *toon, int direction);
void ToonSetMaximumRelocate(int up, int down, int left, int right);
int ToonRelocateAssociated(Toon *toon, int n);
int ToonCalculateAssociations(Toon *toon, int n);


