/******************************************************
*
*     ©keithhedger Tue 16 Dec 12:35:56 GMT 2014
*     kdhedger68713@gmail.com
*
*     globals.h
* 
******************************************************/

#ifndef _GLOBALS_
#define _GLOBALS_

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <Imlib2.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdbe.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXPATHNAMELEN 4096
#define MAXNUMBEROFFLYERS 100
#define MAXFLYER 10
#define MAXNUMBEROFTREELIGHTS 10
#define MAXFLOAT 10
#define MAXFALLINGOBJECTS 5000
#define	MAXFALLINGANIMATION 48
#define MAXSWIRL 2
#define MAXWINDOWS 100

#define VERSION "0.1.7"
#define _SELECTPIXMAP(a,b) (a+(2*b))//a=ONPIXMAP b=xxxOnOff

struct					objects
{
	Pixmap*				pixmap[MAXFALLINGANIMATION];
	Pixmap*				mask[MAXFALLINGANIMATION];
	int					h[MAXFALLINGANIMATION];
	int					w[MAXFALLINGANIMATION];
	int					anims;
};

struct					movement
{
	objects				*object;
	int					x;
	int					y;
	int					deltaX;
	int					deltaY;
	int					stepX;
	int					stepY;
	bool				use;
	int					imageNum;
	int					countDown;
	int					maxW;
	int					maxH;
	bool				direction;
};

struct					settled
{
	int					*lasty;
	Pixmap				pixmap;
	Pixmap				mask;
	bool				keepSettling;
	int					maxHeight;
	GC					maskgc;
	Window				wid;
	int					width;
	int					x,y;
	bool				showing;
};

enum {ONPIXMAP=0,ONMASK,OFFPIXMAP,OFFMASK};
enum {LEFT=-1000,CENTRE=-2000,RIGHT=-3000,TOP=-4000,BOTTOM=-5000};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {LAMPFLASH=0,INVERTLAMPCHASE,LAMPCHASE,LAMPRANDOM,LAMPCYCLE,NUMOPTIONS};

extern char				pathname[MAXPATHNAMELEN];
extern char				*configFilePath;

extern Display*			display;
extern Window			rootWin;
extern int				displayWidth;
extern int				displayHeight;
extern GC				gc;
//extern GC				gcpm;

extern Visual*			visual;
extern int				depth;
extern Imlib_Image		image;
extern int				screen;
extern Region			rg;
extern XdbeBackBuffer	buffer;
extern XdbeSwapInfo		swapInfo;
extern Drawable			drawOnThis;
extern bool				useBuffer;
extern bool				useDBOveride;

extern int				done;
extern long				mainDelay;

extern uint				runCounter;
extern bool				useWindow;
extern int				offSetY;
extern bool				watchConfig;
extern uint				runCounter;
extern bool				useWindow;
extern int				offSetY;
extern bool				watchConfig;

extern char				*prefix;

extern bool				treeNeedsUpdate;
extern bool				flyerNeedsUpdate;
extern bool				figureNeedsUpdate;
extern bool				fallingNeedsUpdate;
extern bool				lampsNeedsUpdate;

extern bool				usingRootWindow;

//settled
extern settled			windowSnow[MAXWINDOWS];
extern settled			bottomSnow;

extern int				maxBottomHeight;
extern int				maxWindowHeight;
extern int				settleRate;
extern int				blackColor;
extern int				whiteColor;
extern bool				clearOnMaxHeight;
extern int				windowYOffset;
extern int				bottomYOffset;

//falling
extern objects			floating[MAXFLOAT];
extern movement			moving[MAXFALLINGOBJECTS];
extern int				fallingNumber;
extern int				fallingDelay;
extern int				swirlingDirection;
extern int				numberOfFalling;
extern bool				showFalling;
extern int				fallingSpread;
extern int				fallSpeed;
extern int				minFallSpeed;
extern int				maxXStep;
extern int				fallingAnimSpeed;
extern int				fallingCount;

//wind
extern int				gustDuration;
extern int				gustEvent;
extern int				gustSpeed;
extern int				realGustSpeed;

extern bool				useGusts;
extern int				windSpeed;
extern int				gustOffSet;
extern int				gustPortion;
extern double			gustInc;
extern bool				doingGusts;
extern double			incr;

extern int				gustDirection;
extern int				gustCountdown;

//flyers
extern objects			flyers[MAXFLYER];
extern movement			flyersMove[MAXNUMBEROFFLYERS];
extern int				flyersSpeed;
extern int				flyersStep;
extern bool				showFlyers;
extern int				flyersMaxY;
extern int				flyerSpread;
extern int				flyerCount;
extern int				flyerAnimSpeed;
extern int				flyerNumber;
extern int				numberOfFlyers;

//figure
extern Pixmap			figurePixmap[4];
extern int				figureSpeed;
extern int				figureX;
extern int				figureY;
extern int				figureW;
extern int				figureH;
extern int				figureOnOff;
extern int				figureNumber;

//lamps
extern Pixmap			lampsPixmap[4];
extern int				lampSpeed;
extern int				lampX;
extern int				lampY;
extern int				lampWidth;
extern int				lampHeight;
extern int				lampCount;
extern int				lampSet;
extern int				lampsOnOff;
extern int				lampAnim;
extern int				lastLampAnim;
extern int				lampSection;
extern int				lampCountdown;
extern bool				*lampState;
extern int				lampCycleDelay;

//trees
extern Pixmap			treePixmap[2];
extern int				treeWidth;
extern int				treeHeight;
extern int				treeNumber;
extern int				treelampSpeed;
extern int				starSpeed;
extern int				treeX;
extern int				treeY;
extern int				treeLampSet;
extern int				treeOnOff;
extern bool				showTree;

//tinsel
extern Pixmap			tinselPixmap[2];
extern bool				showTinsel;

//star
extern Pixmap			starPixmap[4];
extern bool				showStar;
extern int				starOnOff;

//tree lamps
extern Pixmap			treeLampsPixmap[MAXNUMBEROFTREELIGHTS][2];
extern bool				showTreeLamps;
extern int				treeLampCount;

#endif
