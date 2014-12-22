/******************************************************
*
*     ©keithhedger Tue 16 Dec 12:35:56 GMT 2014
*     kdhedger68713@gmail.com
*
*     globals.cpp
* 
******************************************************/

#include "globals.h"

char			pathname[MAXPATHNAMELEN];
char			*configFilePath;

Display*		display;
Window			rootWin;
int				displayWidth;
int				displayHeight;
GC				gc;

Visual*			visual=NULL;
int				depth=0;

int				screen;
Region			rg;
XdbeBackBuffer	buffer;
XdbeSwapInfo	swapInfo;
Drawable		drawOnThis;
bool			useBuffer=false;
bool			useDBOveride=true;

int				done=0;
long			mainDelay;

uint			runCounter=0;
bool			useWindow=true;
int				offSetY=0;
bool			watchConfig=false;

char*			prefix;

bool			treeNeedsUpdate=false;
bool			flyerNeedsUpdate=false;
bool			figureNeedsUpdate=false;
bool			fallingNeedsUpdate=false;
bool			lampsNeedsUpdate=false;

bool			usingRootWindow=false;

//settled
settled			windowSnow[MAXWINDOWS];
settled			bottomSnow;

int				maxBottomHeight;
int				maxWindowHeight;
int				settleRate;
int				blackColor;
int				whiteColor;
bool			clearOnMaxHeight;
int				windowYOffset;
int				bottomYOffset;

//falling
objects			floating[MAXFLOAT];
movement		moving[MAXFALLINGOBJECTS];
int				fallingNumber=1;
int				fallingDelay=1;
int				swirlingDirection;
int				numberOfFalling=100;
bool			showFalling=true;
int				fallingSpread=1000;
int				fallSpeed=1;
int				minFallSpeed=1;
int				maxXStep=4;
int				fallingAnimSpeed=8;
int				fallingCount=0;

//wind
int				gustDuration=3000;
int				gustEvent=10000;
int				gustSpeed=40;
int				realGustSpeed;

bool			useGusts=true;
int				windSpeed=0;
int				gustOffSet=0;
int				gustPortion=1;
double			gustInc;
bool			doingGusts=false;
double			incr=1.01;

int				gustDirection=0;
int				gustCountdown=0;

//flyers
objects			flyers[MAXFLYER];
movement		flyersMove[MAXNUMBEROFFLYERS];
int				flyersSpeed=1;
int				flyersStep=8;
bool			showFlyers=false;
int				flyersMaxY=400;
int				flyerSpread=500;
int				flyerCount=0;
int				flyerAnimSpeed=8;
int				flyerNumber;
int				numberOfFlyers=1;

//figure
Pixmap			figurePixmap[4];
int				figureSpeed=100;
int				figureX=100;
int				figureY=100;
int				figureW;
int				figureH;
int				figureOnOff=0;
int				figureNumber=1;

//lamps
Pixmap			lampsPixmap[4];
int				lampSpeed=100;
int				lampX=0;
int				lampY=0;
int				lampWidth;
int				lampHeight;
int				lampCount;
int				lampSet=1;
int				lampsOnOff=0;
int				lampAnim;
int				lastLampAnim;
int				lampSection;
int				lampCountdown;
bool			*lampState;
int				lampCycleDelay;

//trees
Pixmap			treePixmap[2];
int				treeWidth;
int				treeHeight;
int				treeNumber=1;
int				treelampSpeed=100;
int				starSpeed=100;
int				treeX=0;
int				treeY=0;
int				treeLampSet=1;
int				treeOnOff=0;
bool			showTree=true;

//tinsel`
Pixmap			tinselPixmap[2];
bool			showTinsel=false;

//star
Pixmap			starPixmap[4];
bool			showStar=false;
int				starOnOff=0;

//tree lamps
Pixmap			treeLampsPixmap[MAXNUMBEROFTREELIGHTS][2];
bool			showTreeLamps=false;
int				treeLampCount=0;
