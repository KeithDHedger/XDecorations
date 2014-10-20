/*

*/


//#ifdef VMS
//#include <socket.h>
//# if defined(__SOCKET_TYPEDEFS) && !defined(CADDR_T)
//#  define CADDR_T
//# endif
//#endif
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/xpm.h>

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
//#if defined(__STDC__) || defined(VMS)
#include <stdlib.h>
//#else
//long strtol();
//char *getenv();
//#endif

#include "pixmaps/LampsOn.XPM"
#include "pixmaps/LampsOff.XPM"
#include "pixmaps/LightsBlue0.xpm"
#include "pixmaps/LightsBlue1.xpm"
#include "pixmaps/LightsGreen0.xpm"
#include "pixmaps/LightsGreen1.xpm"
#include "pixmaps/LightsPurple0.xpm"
#include "pixmaps/LightsPurple1.xpm"
#include "pixmaps/LightsRed0.xpm"
#include "pixmaps/LightsRed1.xpm"
#include "pixmaps/Star0.xpm"
#include "pixmaps/Star1.xpm"
#include "pixmaps/Tinsel.xpm"
#include "pixmaps/Tree1.xpm"
#include "pixmaps/Tree2.xpm"


//#include "xsnow.h"
//#include "toon.h"
//typedef int ErrorHandler();
//#if !defined(GRAB_SERVER)
//#define GRAB_SERVER	0
//#endif
Display *display;
int screen;
Window rootWin;
//char toon_message[TOON_MESSAGE_LENGTH] = "";
Window Parent;
int display_width, display_height;
int center_x, center_y;
GC gc;
char *display_name = NULL;
//Pixel black, white;
int done = 0;
//int errorVal = 0;
//int flake;
//int SmoothWhirl = 1;
//int NoTrees = 0;
//int NoSanta = 0;
//int Rudolf = 1;
//int NoKeepSnow = 0;
//int NoKeepSBot = 0;
//int NoKeepSWin = 0;
int NoPopuphandling = 1;
//int NoWind = 0;
unsigned int borderWidth = 0;
//int SnowOffset = 0;
//int UseFillForClear = 0;
//Snow *snowflakes;
//int MaxSnowFlakeHeight = 0;  /* Biggest flake */
//int maxSnowflakes = INITIALSNOWFLAKES;
//int curSnowflakes = 0;
long snowDelay = 50000; /* useconds */
//int MaxXStep = MAXXSTEP;
//int MaxYStep = MAXYSTEP;
//int WhirlFactor = WHIRLFACTOR;
//int WhirlAdapt;
//int MaxWinSnowDepth = INITWINSNOWDEPTH;
//int MaxScrSnowDepth = INITSCRSNOWDEPTH;
/* The default Santa 0,1,2 */
//int SantaSize = 2;
//int SantaSpeed = -1;  /* Not initialized yet */
//#define	MAXSANTA_SPEED 30
//int SantaX;
//int SantaY;
//int SantaXStep;
//int SantaYStep;
//int SantaVisible;
//int CurrentSanta;
//Pixmap SantaPixmap[PIXINANIMATION];
//Pixmap SantaMaskPixmap[PIXINANIMATION];
/*
		My Stuff!
*/
Pixmap LampPixmap[2];
Pixmap LampMaskPixmap[2];

Pixmap TreePixmap[2];
Pixmap TreeMaskPixmap[2];

Pixmap StarPixmap[2];
Pixmap StarMaskPixmap[2];

Pixmap TreeLampsPixmap[8];
Pixmap TreeLampsMaskPixmap[8];

Pixmap TinselPixmap;
Pixmap TinselMaskPixmap;

//Pixmap SnowManPixmap[2];
//Pixmap SnowManMaskPixmap[2];

//int SantaWidth;
//int SantaHeight;   
//Region Wr = NULL;
//Region PrevWr = NULL;
//Region WDR = NULL;
//Region CDR = NULL;   
Region rscrr = NULL;
Region snscr = NULL;
//Region sar = NULL;
//Region SubR = NULL;
int stilltddr = 0;
/* Wind stuff */
//int diff=0;
//int wind = 0;
//int direction=0;
//int WindTimer=30;
//int current_snow_height;
//int geta=0;
//XRectangle AddRect;
unsigned int RunCounter = 0;
//int SantaUpdateFactor = 3;
/*
		My Stuff!
*/
int LampSpeed = 10;
int LampX=0;
int LampY=26;
int LampWidth;
int LampHeight;
int	LampCount=0;
int Lamps=1;

int TreeWidth;
int TreeHeight;
int TreeNumber=1;
int TreeLampSpeed=8;
int StarSpeed=6;
int TreeX=100;
int TreeY=100;
int TreeLampSet=1;
int Tinsel=1;
int Star=1;
int Tree=1;
//int ShowSnow=1;

/* Forward decls */
//void Usage();
void SigHandler();
void SigHupHandler();
void InitSnowflake();
void UpdateSnowflake();
void DrawSnowflake();
void EraseSnowflake();
void PaintSnowAtBottom();
void DrawTannenbaum();
void InitSantaPixymaps();
/*
		My Stuff
*/
void InitLamps(void);
void DrawLamps(void);
void InitTree(void);
void DrawTreeLamps(void);

//void ResetSanta();
//void DrawSanta();
//void EraseSanta();
//void UpdateSanta();
void uSsleep();
//int MWR(int);
//void MASR();
//void RCSR();
//int DDLT();     
//void HEAr(int X,int Y,int W, int H);
Pixel AllocNamedColor();
void sig_alarm();
/* Colo(u)rs */
char *snowColor = "snow";
char *slcColor = "black";
char *blackColor = "black";
char *redColor = "red";
char *whiteColor = "white";
char *bgColor = "none";
char *trColor = "chartreuse";
/* The author thoroughly recommends a cup of tea */
/* with a dash of green Chartreuse. Jum!         */
char *greenColor = "chartreuse"; 
Pixel redPix;
Pixel whitePix;
Pixel greenPix;
Pixel blackPix;
Pixel snowcPix;
Pixel bgPix;
Pixel trPix;
Pixel slcPix;
/* GC's */
//GC SnowGC[SNOWFLAKEMAXTYPE+1];
//GC ClearSnowGC;
//GC SleighGCs[3];
GC SantaGC,RudolfGC,FurGC;
//GC TreesGC;
//int rc;
/* ------------------------------------------------------------------ */ 
int main(ac, av)
int ac;
char *av[];
{
		XGCValues xgcv;
		int ax;
		char *arg;
//		SnowMap *rp;
//		TannenbaumMap *tp;
		XEvent ev;
		int Exposed;
		int ConfigureNotified;
		int i; 
		int x,y;
		Window root; /* used in getgeometry call */
		int winX, winY;
		unsigned int winHeight, winWidth;
		unsigned int depth;
		/*
			 Process command line options.
		*/
		for (ax=1; ax<ac; ax++) {
			arg = av[ax];
			if (strcmp(arg, "-display") == 0) {
				display_name = av[++ax];
			}
//			else if (strcmp(arg, "-sc") == 0) {
//				snowColor = av[++ax];
//			}
//			else if (strcmp(arg, "-bg") == 0) {
//				bgColor = av[++ax];
//				/* If solid colour used as bg we can use XFillArea for clear */
//				/* This results in a big performance improvement */
//				/*** Uncomment this if sure all Xservers are faster with
//					 XFillRectangle than with XClearArea ***/
//				/** December 2000: well, this is NOT true for all X-servers!
//						It does work this way on SGI, but on SUN Solaris 2.5.1 
//						UseFillForClear costs 20% *more* resources! **/
//				/*** UseFillForClear = 1; ***/
//			}
//			else if (strcmp(arg, "-solidbg") == 0) {
//				/*	
//						If solid colour used as bg we can use XFillArea for clear
//						This results in a big performance improvement, but not on all
//						X-servers! (Yes on SGI, NO on SUN (Solaris 2.5.1) (Dec 2000)
//				*/
//				UseFillForClear = 1;
//			}
//			else if (strcmp(arg, "-tc") == 0) {
//				trColor = av[++ax];
//			}
//			else if (strcmp(arg, "-slc") == 0) {
//				/* Obsolete option, Santa is a colour pic now */
//				/*slcColor = av[++ax];*/
//				fprintf(stderr,"** The option -slc (sleigh color) is obsoleten");
//			}
//			else if (strcmp(arg, "-delay") == 0) {
//				snowDelay = strtol(av[++ax], (char **)NULL,0);
//				snowDelay = snowDelay * 1000;  /* ms to us */
//			}
//			else if (strcmp(arg, "-santaupdatefactor") == 0) {
//				SantaUpdateFactor = strtol(av[++ax], (char **)NULL,0);
//			}
//			else if (strcmp(arg, "-snowflakes") == 0) {
//				maxSnowflakes = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-unsmooth") == 0) {
//				SmoothWhirl = 0;
//			}
//			else if (strcmp(arg, "-whirl") == 0) {
//				WhirlFactor = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-nowind") == 0) {
//				NoWind = 1;
//			}
//			else if (strcmp(arg, "-windtimer") == 0) {
//				WindTimer = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-xspeed") == 0) {
//				MaxXStep = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-yspeed") == 0) {
//				MaxYStep = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-wsnowdepth") == 0) {
//				MaxWinSnowDepth = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-ssnowdepth") == 0) {
//				MaxScrSnowDepth = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-offset") == 0) {
//				SnowOffset = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-notrees") == 0) {
//				NoTrees = 1;     
//			}
//			else if (strcmp(arg, "-nosanta") == 0) {
//				NoSanta = 1;     
//			}
//			else if (strcmp(arg, "-santa") == 0) {
//				SantaSize = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-norudolf") == 0) {
//				Rudolf = 0;
//			}
//			else if (strcmp(arg, "-santaspeed") == 0) {
//				SantaSpeed = strtol(av[++ax], (char **)NULL, 0);
//			}
//			else if (strcmp(arg, "-nokeepsnow") == 0) {
//				NoKeepSnow = 1;
//				NoKeepSWin = 1;
//				NoKeepSBot = 1;
//			}
//			else if (strcmp(arg, "-nokeepsnowonwindows") == 0) {
//				NoKeepSWin = 1;
//			}
//			else if (strcmp(arg, "-nokeepsnowonscreen") == 0) {
//				NoKeepSBot = 1;
//			}
//			/* And now for some confusion */
//			else if (strcmp(arg, "-popup") == 0) {
//				NoPopuphandling = 1;
//			}
//			else if (strcmp(arg, "-nopopup") == 0) {
//				NoPopuphandling = 0;
//			}
//			else if (strcmp(arg, "-nonopopup") == 0) {
//				NoPopuphandling = 1;
//			}
//			else if (strcmp(arg, "-nononopopup") == 0) {
//				NoPopuphandling = 0;
//			}
//			else if (strcmp(arg, "-version") == 0) {
//				printf("nThis is %snn", VERSION);
//				exit(0);
//			}
			else if (strcmp(arg, "-lampspeed") == 0) {
				LampSpeed = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-lampy") == 0) {
				LampY = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-treelampspeed") == 0) {
				TreeLampSpeed = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-treelampset") == 0) {
				TreeLampSet = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-treenumber") == 0) {
				TreeNumber = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-treex") == 0) {
				TreeX = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-treey") == 0) {
				TreeY = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-starspeed") == 0) {
				StarSpeed = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-noxmastree") == 0) {
				Tree = 0;
			}
			else if (strcmp(arg, "-nolamps") == 0) {
				Lamps = 0;
			}
			else if (strcmp(arg, "-nostar") == 0) {
				Star = 0;
			}
			else if (strcmp(arg, "-notinsel") == 0) {
				Tinsel = 0;
			}
//			else if (strcmp(arg, "-nosnow") == 0) {
//				ShowSnow = 0;
//			}

//			else {
//				Usage();
//			}
		}
//		printf("%s\n",VERSION);
		/* Some people... */
//		if (maxSnowflakes > MAXSNOWFLAKES) {
//			fprintf(stderr,"** Maximum number of snowflakes is %dn", MAXSNOWFLAKES);
//			exit(1);
//		}
//		if ((SantaSize < 0) || (SantaSize > MAXSANTA)) {
//			fprintf(stderr,"** Maximum Santa is %dn",MAXSANTA);
//			exit(1);
//		}
//		if (SantaUpdateFactor == 0) {
//			fprintf(stderr,"** It is naughty (sic!) to specify zero for -santaupdatefactorn");
//			exit(1);
//		}
//		/* Eskimo warning */
//		if (strstr(snowColor,"yellow") != NULL)
//			printf("nWarning: don't eat yellow snow!nn");
//		if (SantaSpeed < 0) SantaSpeed = Speed[SantaSize];
#ifdef VMS
		srand((int)time((unsigned long *)NULL));
#else
		srand((int)time((long *)NULL));
#endif
			 
		signal(SIGKILL, SigHandler);
		signal(SIGINT, SigHandler);
		signal(SIGTERM, SigHandler);
#if debug
		signal(SIGHUP, SigHupHandler);
#else
		signal(SIGHUP, SigHandler);
#endif
		signal(SIGALRM, sig_alarm);
//		alarm(WindTimer);
		display = XOpenDisplay(display_name);
		if (display == NULL) {
			if (display_name == NULL) display_name = getenv("DISPLAY");
			(void) fprintf(stderr, "%s: cannot connect to X server %s\n", av[0],
				display_name ? display_name : "(default)");
				exit(1);
		}
		screen = DefaultScreen(display);
		rootWin = ToonGetRootWindow(display, screen, &Parent);

//		black = BlackPixel(display, screen);
//		white = WhitePixel(display, screen);
		display_width = DisplayWidth(display, screen);
		display_height = DisplayHeight(display, screen);
		center_x = display_width / 2;
		center_y = display_height / 2;
//		current_snow_height = display_height;
//		if (MaxScrSnowDepth> (display_height-SNOWFREE)) {
//			printf("** Maximum snow depth set to %dn", display_height-SNOWFREE);
//			MaxScrSnowDepth = display_height-SNOWFREE;
//		}
//		for (flake=0; flake<=SNOWFLAKEMAXTYPE; flake++) {
//			rp = &snowPix[flake];
//			rp->pixmap = XCreateBitmapFromData(display, rootWin,
//						rp->snowBits, rp->width, rp->height);
//			if (rp->height > MaxSnowFlakeHeight) MaxSnowFlakeHeight = rp->height;
//		}
//		snowflakes = (Snow *)malloc(sizeof(Snow) * MAXSNOWFLAKES);
//		tp = &tannenbaumPix[0];
//		tp->pixmap = XCreateBitmapFromData(display, rootWin,
//						tp->tannenbaumBits, tp->width, tp->height);
//		InitSantaPixymaps();
/*
		My Stuff!
*/
		InitLamps();
		InitTree();

//		redPix =   AllocNamedColor(redColor, black);
//		whitePix = AllocNamedColor(whiteColor, white);
//		greenPix = AllocNamedColor(greenColor, black);
//		blackPix = AllocNamedColor(blackColor, black);
//		snowcPix = AllocNamedColor(snowColor, white);   
//		trPix = AllocNamedColor(trColor, black);
		gc = XCreateGC(display, rootWin, 0, NULL);
		XGetGCValues(display, gc, 0, &xgcv);
		XSetForeground(display, gc, blackPix);
		XSetFillStyle(display, gc, FillStippled);
#if 0
		if(strcmp(bgColor,"none") != 0) {
			bgPix = AllocNamedColor(bgColor, white);
			XSetWindowBackground(display, rootWin, bgPix);
			XClearWindow(display, rootWin);
			XFlush(display);
			if (!UseFillForClear) {
				printf("\nNote: when using backgrounds of one color also specifying\n");
				printf("      -solidbg MAY greatly improve performance!\n");
			}
			else {
				ClearSnowGC = XCreateGC(display, rootWin, 0L, &xgcv);
				XCopyGC(display,gc,0,ClearSnowGC);
				XSetForeground(display,ClearSnowGC, bgPix);
			}
		}
		else if (UseFillForClear) {
			printf("\n");
			printf(
			 "Sorry, -solidbg can only be specified with a background color (-bg)");
			printf("\n");
			exit(1);
		}
		for (flake=0; flake<=SNOWFLAKEMAXTYPE; flake++) {
			SnowGC[flake] = XCreateGC(display, rootWin, 0L, &xgcv);
			XCopyGC(display,gc,0,SnowGC[flake]);
			XSetStipple(display, SnowGC[flake], snowPix[flake].pixmap);
			XSetForeground(display,SnowGC[flake],snowcPix);
			XSetFillStyle(display, SnowGC[flake], FillStippled);
		}
		TreesGC = XCreateGC(display, rootWin, 0L, &xgcv);
		XCopyGC(display,gc,0,TreesGC);
		XSetStipple(display, TreesGC, tannenbaumPix[0].pixmap);
		XSetForeground(display,TreesGC,trPix);
		XSetFillStyle(display, TreesGC, FillStippled);
		 
#endif		 
		 
		 
//		Wr = XCreateRegion();
//		PrevWr = XCreateRegion();
		snscr = XCreateRegion();
//		sar = XCreateRegion();
//		WDR = XCreateRegion();
//		CDR = XCreateRegion();                                                                                 
		rscrr = XCreateRegion();
//		AddRect.x = 0;
//		AddRect.y = display_height;
//		AddRect.width = display_width - 1;
//		AddRect.height = MaxYStep+MaxSnowFlakeHeight;
//		if (!NoKeepSBot)
//			XUnionRectWithRegion(&AddRect, snscr, snscr);
//		for (i=0; i<maxSnowflakes; i++) InitSnowflake(i);
////rc = MWR(1);
//		XUnionRegion(Wr,PrevWr, PrevWr);                                                                 
//		RCSR();
//		MASR();
	//	ResetSanta();   UpdateSanta();
		XSelectInput(display, rootWin, ExposureMask | SubstructureNotifyMask);
		 
		 
		while (!done) {
			Exposed = 0;
			ConfigureNotified = 0;
			while (XPending(display) || stilltddr) {
				XNextEvent(display, &ev);
#if 0
				if (stilltddr) {
					stilltddr = !DDLT();
				}
				if (!NoKeepSnow) {
					switch (ev.type) {
							case Expose:    
									#if debug
									printf("EXPOSE\n");
									#endif
								  Exposed = 1;
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
									#if debug
									printf("expose x:%d y:%d h:%d w:%d\n",ev.xexpose.x,ev.xexpose.y,ev.xexpose.height,ev.xexpose.width);
									#endif
								  break;
							case ConfigureNotify:
									#if debug
									printf("--------\nConfigureNotify: r=%d w=%d geo=(%d,%d,%d,%d) bw=%d root: %d\n\n",
														ev.xconfigure.event,
														ev.xconfigure.window,
														ev.xconfigure.x,
														ev.xconfigure.y,
														ev.xconfigure.width,
														ev.xconfigure.height,
														ev.xconfigure.border_width,
														(rootWin == ev.xconfigure.event)  
												);
									printf("ConfigureNotify calling DDLT\n");
									#endif
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
								  break;
							case ConfigureRequest:
									#if debug
									printf("ConfigureRequest\n\n");
									#endif
								  break;
							case CreateNotify:
									#if debug
									printf("CreateNotify\n\n");
									#endif
									DDLT(); 
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
								  break;
							case DestroyNotify:
									#if debug
									printf("DestroyNotify: %d %d\n\n",
														ev.xdestroywindow.event,
														ev.xdestroywindow.window);
									#endif
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
								  break;
							case MapNotify:
									#if debug
									printf("MapNotify: %d %d\n",
														ev.xmap.event,
														ev.xmap.window
												);
									#endif
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
									XGetGeometry(	ev.xmap.display, ev.xmap.window, 
																&root, &winX, &winY, &winWidth, &winHeight, &borderWidth, &depth);
									#if debug
									printf("MapNotify r=%d w=%d geo=(%d,%d,%d,%d) bw=%d d=%d\n\n", 
														root, ev.xmap.window,  winX,winY,winWidth,winHeight, borderWidth,depth);
									#endif
								  break;
							case UnmapNotify:
									#if debug
									printf("UnmapNotify: %d %d\n",
														ev.xunmap.event,
														ev.xunmap.window
												);
									#endif
									if (!DDLT()) {
										stilltddr = 1;
										continue; 
									}
								  break;
					} 
				} 
#endif 
			 
			}  
#if debug
#endif
			uSsleep(snowDelay);
			 
			 
			RunCounter++;
#if 0
			if (ShowSnow==1)
				for (i=0; i<maxSnowflakes; i++) UpdateSnowflake(i);
			if (!NoTrees ) {
				DrawTannenbaum(display_width-150, display_height-500);
				DrawTannenbaum(display_width-100, display_height-200);
				DrawTannenbaum(100, display_height-200);
				DrawTannenbaum(50, display_height-150);
				DrawTannenbaum(center_x, display_height-100);
				DrawTannenbaum(200,400);
			}
			if (!NoSanta) {
				if ((RunCounter % SantaUpdateFactor) == 0) UpdateSanta();
			}
#endif
/*
		My Stuff!
*/
			
			if (Lamps==1)
				{
				if ((RunCounter % LampSpeed) == 0)
					UpdateLamps();
				DrawLamps();

				}

			if (Tree==1)
				{
				if ((RunCounter % TreeLampSpeed) == 0)
					UpdateTreeLamps();
				DrawTreeLamps();
					
				if ((RunCounter % StarSpeed) == 0 && Star==1)
					UpdateStar();
				}


		
		}
//		XDestroyRegion(Wr);
		XDestroyRegion(snscr);
//		XDestroyRegion(sar);
//		XDestroyRegion(PrevWr);
//		XDestroyRegion(WDR);
		XDestroyRegion(rscrr);
		XClearWindow(display, rootWin);
		XCloseDisplay(display);
		exit(0);
}		/* End of the snow */
/* ------------------------------------------------------------------ */ 
#if 0
#define USEPRT(msg) fprintf(stderr, msg)
void
Usage()
{
		USEPRT("Usage: xsnow [options]\n\n");
		USEPRT("Options:\n");
		USEPRT("       -display     <displayname>\n");
		USEPRT("       -sc          <snowcolor>\n");
		USEPRT("       -tc          <tree color>\n");
		USEPRT("       -bg          <background color>\n");
		USEPRT("       -solidbg     (Performance improvement!)\n");
		USEPRT("       -snowflakes  <numsnowflakes>\n");
		USEPRT("       -delay       <delay in milliseconds>\n");
		USEPRT("       -unsmooth\n");
		USEPRT("       -whirl       <whirlfactor>\n");
		USEPRT("       -nowind\n");
		USEPRT("       -windtimer   <Time between windy periods in seconds>\n");
		USEPRT("       -xspeed      <max xspeed snowflakes>\n");
		USEPRT("       -yspeed      <max yspeed snowflakes>\n");
		USEPRT("       -wsnowdepth  <max snow depth on windows>\n");
		USEPRT("       -offset      <shift snow down>\n");
		USEPRT("       -ssnowdepth  <max snow depth at bottom of display>\n");
		USEPRT("       -notrees\n");
		USEPRT("       -nosanta\n");
		USEPRT("       -norudolf\n");
		USEPRT("       -santa       <santa>\n");
		USEPRT("       -santaspeed  <santa_speed>\n");
		USEPRT("       -santaupdatefactor  <factor>\n");
		USEPRT("       -nokeepsnow\n");
		USEPRT("       -nokeepsnowonwindows\n");
		USEPRT("       -nokeepsnowonscreen\n");
		USEPRT("       -nonopopup\n");
		USEPRT("       -version\n\n");
		USEPRT("Recommended: xsnow -bg SkyBlue3\n");
		exit(1);
}
#endif
/* ------------------------------------------------------------------ */ 
void SigHandler()
{
	done = 1;
}

void SigHupHandler()
{
	void (*Sig_Hup_ptr)() = SigHupHandler;
	XFlush(display);
	XSetForeground(display,FurGC,trPix);
	XSetFillStyle(display,FurGC, FillSolid);
	XSetRegion(display,FurGC,snscr);
	XFillRectangle(display,rootWin,FurGC,0,0,display_width,display_height);
	XFlush(display);
	signal(SIGHUP, Sig_Hup_ptr);
}
/* ------------------------------------------------------------------ */ 
	 
int RandInt(int maxVal)
{
	return rand() % maxVal;
}
/* ------------------------------------------------------------------ */ 
void uSsleep(unsigned long usec) 
{
#ifdef SVR3
		poll((struct poll *)0, (size_t)0, usec/1000);   
#else
#ifdef VMS
		void lib$wait();
		float t;
		t= usec/1000000.0;
		lib$wait(&t);
#else
		struct timeval t;
		t.tv_usec = usec%(unsigned long)1000000;
		t.tv_sec = usec/(unsigned long)1000000;
		select(0, (void *)0, (void *)0, (void *)0, &t);
#endif
#endif
}
/* ------------------------------------------------------------------ */ 
	 
//int
//SnowPtInRect(snx, sny, recx, recy, width, height)
//int snx;
//int sny;
//int recx;
//int recy;
//int width;
//int height;
//{
//		if (snx < recx) return 0;
//		if (snx > (recx + width)) return 0;
//		if (sny < recy) return 0;
//		if (sny > (recy + height)) return 0;
//		return 1;
//}
	 
//void
//InitSnowflake(rx)
//int rx;
//{
//		Snow *r;
//		r = &snowflakes[rx];
//	if (curSnowflakes < maxSnowflakes) {
//		r->whatFlake = RandInt(SNOWFLAKEMAXTYPE+1);
//		if (wind) {
//			if (direction == 1) 
//				r->intX = RandInt(display_width/3);
//			else
//				r->intX = display_width - RandInt(display_width/3);
//			r->intY =  RandInt(display_height);
//		}
//		else  {
//			r->intX = RandInt(display_width - snowPix[r->whatFlake].width);
//			r->intY =  RandInt(display_height/10);
//		}
//		r->yStep = RandInt(MaxYStep+1)+1;
//		r->xStep = RandInt(r->yStep/4+1);
//		if (RandInt(1000) > 500) r->xStep = -r->xStep;
//		r->active = 1;
//		r->visible = 1;
//		r->insnow = 0;
//	}
//}
	 
//void
//UpdateSnowflake(rx)
//int rx;
//{
//Snow *snow;
//int NewX;
//int NewY;
//int tmp_x;
//int TouchDown;
//int InVisible;
//int NoErase;
//	snow = &snowflakes[rx];
//	NoErase = 0;  
//	if (!snow->active) {
//		InitSnowflake(rx);
//		snow->insnow = 1; 
//	}
//	if (wind) {
//		tmp_x = abs(snow->xStep);
//		if( wind == 1 ){  
//				tmp_x = tmp_x + (RandInt(WhirlFactor+1) - WhirlFactor/2);
//		}else{            
//				tmp_x = tmp_x + (RandInt(20));
//		}
//		snow->xStep = tmp_x * direction;
//		if (snow->xStep > 50) snow->xStep = 50;
//		if (snow->xStep < -50) snow->xStep = -50;
//		if( wind == 1 ){
//				NewY = NewY+3;
//		}else{
//				NewY = NewY+10;
//		}
//	}
//	NewX = snow->intX + snow->xStep;
//	NewY = snow->intY + snow->yStep;
//	snow->active = (NewY < display_height);
//	if (wind) snow->active = 
//			(snow->active && (NewX > 0) && (NewX < display_width));
//	InVisible = XRectInRegion(Wr,
//								            NewX,NewY,
//								            snowPix[snow->whatFlake].width,
//								            snowPix[snow->whatFlake].height);
//	snow->visible = (InVisible != RectangleIn);
//	 
//	 
//	if (snow->visible) {
//		TouchDown = XPointInRegion(snscr,
//								               NewX+snowPix[snow->whatFlake].width/2,
//								               NewY+snowPix[snow->whatFlake].height);
//		if (TouchDown && snow->visible && !NoKeepSnow) {
//			NoErase = 1;
//			TouchDown = XRectInRegion(snscr,
//								                NewX,NewY,
//								                snowPix[snow->whatFlake].width,
//								                snowPix[snow->whatFlake].height);
//			if (TouchDown == RectanglePart) {
//				AddRect.x = NewX;
//				AddRect.y = NewY + snowPix[snow->whatFlake].height - 2;
//				AddRect.height = 2;
//				AddRect.width = snowPix[snow->whatFlake].width;               
//				if (XRectInRegion(sar,
//								          AddRect.x, AddRect.y,
//								          AddRect.width, AddRect.height) == RectangleIn) {
//					XUnionRectWithRegion(&AddRect, snscr,snscr);
//				}
//			}
//		}
//		if (SmoothWhirl) {
//			WhirlAdapt = RandInt(WhirlFactor); 
//			if (RandInt(1000) > 500) WhirlAdapt = -WhirlAdapt;
//			snow->xStep = snow->xStep + WhirlAdapt;
//		}
//		else {
//			snow->xStep = snow->xStep + RandInt(WhirlFactor);
//			if (RandInt(1000) > 500) snow->xStep = -snow->xStep;
//		}
//	}  
//	if (!wind) {
//		if (snow->xStep > MaxXStep) snow->xStep = MaxXStep;
//		if (snow->xStep < -MaxXStep) snow->xStep = -MaxXStep;
//	}
//	if (NoErase && !wind) snow->xStep = snow->xStep/2;
//	/* xsnow 1.29 method: */
//	if (!snow->insnow) EraseSnowflake(rx);
//	snow->intX = NewX;
//	snow->intY = NewY;
//	snow->insnow = NoErase;
//	if (snow->active && snow->visible) DrawSnowflake(rx);
//}
 
//void
//DrawSnowflake(rx)
//int rx;
//{
//		Snow *snow;
//		snow = &snowflakes[rx];
//		XSetTSOrigin(display, SnowGC[snow->whatFlake], snow->intX, snow->intY);
//		XFillRectangle(display, rootWin, SnowGC[snow->whatFlake],
//				 snow->intX, snow->intY,
//				 snowPix[snow->whatFlake].width, snowPix[snow->whatFlake].height);
//}
	 
//void
//EraseSnowflake(rx)
//int rx;
//{
//		Snow *snow;
//		snow = &snowflakes[rx];
//	 
//		if (UseFillForClear) {
//			XFillRectangle(display, rootWin, ClearSnowGC,
//								 snow->intX, snow->intY,
//								 snowPix[snow->whatFlake].width, 
//								 snowPix[snow->whatFlake].height);
//			}
//		else {
//			XClearArea(display, rootWin, 
//								 snow->intX, snow->intY,
//								 snowPix[snow->whatFlake].width, 
//								 snowPix[snow->whatFlake].height,
//								 False);
//		}
//}

//void
//PaintSnowAtBottom(depth)
//int depth;
//{
//int x,y;
//Snow *snow;
//	for (y=0; y < depth; y++) {
//		for (x=0; x<maxSnowflakes; x++) {
//			InitSnowflake(x);
//			snow = &snowflakes[x];
//			snow->intY = display_height - y;
//			DrawSnowflake(x);
//		}
//	}
//}
/* ------------------------------------------------------------------ */ 
	 
//void
//DrawTannenbaum(x,y)
//int x,y;
//{
//		XSetTSOrigin(display, TreesGC, x,y);
//		XFillRectangle(display, rootWin, TreesGC,
//				 x,y,
//				 tannenbaumPix[0].width, tannenbaumPix[0].height);
//}
/* ------------------------------------------------------------------ */ 
/*
		My Stuff!
*/

void InitLamps()
{
	int	rc;
	rc = XpmCreatePixmapFromData(display,rootWin,LampsOff_XPM, &LampPixmap[0], &LampMaskPixmap[0], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,LampsOn_XPM, &LampPixmap[1], &LampMaskPixmap[1], NULL);
	sscanf(*LampsOff_XPM,"%d %d", &LampWidth,&LampHeight);
	LampCount=(display_width/LampWidth)+1;
	LampCount--;
}

void InitTree()
{
	int	rc;
	rc = XpmCreatePixmapFromData(display,rootWin,Tree1_xpm, &TreePixmap[0], &TreeMaskPixmap[0], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,Tree2_xpm, &TreePixmap[1], &TreeMaskPixmap[1], NULL);
	sscanf(*Tree1_xpm,"%d %d", &TreeWidth,&TreeHeight);
	
	rc = XpmCreatePixmapFromData(display,rootWin,LightsBlue0_xpm, &TreeLampsPixmap[0], &TreeLampsMaskPixmap[0], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,LightsBlue1_xpm, &TreeLampsPixmap[4], &TreeLampsMaskPixmap[4], NULL);

	rc = XpmCreatePixmapFromData(display,rootWin,LightsGreen0_xpm, &TreeLampsPixmap[1], &TreeLampsMaskPixmap[1], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,LightsGreen1_xpm, &TreeLampsPixmap[5], &TreeLampsMaskPixmap[5], NULL);

	rc = XpmCreatePixmapFromData(display,rootWin,LightsPurple0_xpm, &TreeLampsPixmap[2], &TreeLampsMaskPixmap[2], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,LightsPurple1_xpm, &TreeLampsPixmap[6], &TreeLampsMaskPixmap[6], NULL);

	rc = XpmCreatePixmapFromData(display,rootWin,LightsRed0_xpm, &TreeLampsPixmap[3], &TreeLampsMaskPixmap[3], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,LightsRed1_xpm, &TreeLampsPixmap[7], &TreeLampsMaskPixmap[7], NULL);

	rc = XpmCreatePixmapFromData(display,rootWin,Star0_xpm, &StarPixmap[0], &StarMaskPixmap[0], NULL);
	rc = XpmCreatePixmapFromData(display,rootWin,Star1_xpm, &StarPixmap[1], &StarMaskPixmap[1], NULL);

	rc = XpmCreatePixmapFromData(display,rootWin,Tinsel_xpm, &TinselPixmap, &TinselMaskPixmap, NULL);

}

#if 0
void InitSantaPixymaps()
{
	int rc;
	char t[255];
	#if debug
	printf("InitSantaPixymaps: SantaSize=%d Rudolf=%d\n", SantaSize,Rudolf);
	#endif
	switch (SantaSize) {
		case 0:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*RegularSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*RegularSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;
		case 1:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*MediumSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*MediumSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;
		case 2:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*BigSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*BigSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;
		default:
			fprintf(stderr,"** Maximum Santa is %d\n",MAXSANTA);
			exit(1);
		break;
	}	
	#if debug
	printf("InitSantaPixymaps: SantaWidth: %d, SantaHeight: %d\n", SantaWidth,SantaHeight);
	#endif
}		
#endif 
//void
//ResetSanta()      
//{
//	SantaX = -SantaWidth;
//	SantaY = RandInt(display_height / 3)+40;
//	SantaXStep = SantaSpeed;
//	SantaYStep = 1;
//	CurrentSanta = 0;
//	SantaVisible = 1;
//}

/*
		My Stuff!
*/

int	OnOff=0;
/*
		My Stuff!
*/
void DrawLamps()
{	
	int rc;
	int loop;
	int	CurrentLampX=LampX;
	rc = XSetClipMask(display,gc,LampMaskPixmap[OnOff]);

	for (loop=0;loop<=LampCount;loop++)
		{
		rc = XSetClipOrigin(display,gc,CurrentLampX,LampY);
		rc = XCopyArea(display,LampPixmap[OnOff],
												rootWin,
												gc,
												0,0,LampWidth,LampHeight,
												CurrentLampX,LampY);
		CurrentLampX+=LampWidth;
		}
}

UpdateLamps(void)
{
	OnOff=(OnOff+1) & 1;
}

int	TreeOnOff=0;
int	StarOnOff=0;

void DrawTreeLamps(void)
{
	int rc;
	int lastlamp;
	int lampset;

	if (TreeLampSet<3)
		lampset=(TreeLampSet-1)*4;
	else
		lampset=0;

	rc = XSetClipMask(display,gc,TreeMaskPixmap[TreeNumber-1]);
	rc = XSetClipOrigin(display,gc,TreeX,TreeY);
	rc = XCopyArea(display,TreePixmap[TreeNumber-1],
												rootWin,
												gc,
												0,0,TreeWidth,TreeHeight,
												TreeX,TreeY);

	rc = XSetClipMask(display,gc,TreeLampsMaskPixmap[TreeOnOff+lampset]);
	rc = XCopyArea(display,TreeLampsPixmap[TreeOnOff+lampset],
												rootWin,
												gc,
												0,0,TreeWidth,TreeHeight,
												TreeX,TreeY);
	
	if (Star==1)
		{
		rc = XSetClipMask(display,gc,StarMaskPixmap[StarOnOff]);
		rc = XCopyArea(display,StarPixmap[StarOnOff],
												rootWin,
												gc,
												0,0,TreeWidth,TreeHeight,
												TreeX,TreeY);
		}

	if (Tinsel==1)
		{
		rc = XSetClipMask(display,gc,TinselMaskPixmap);
		rc = XCopyArea(display,TinselPixmap,
												rootWin,
												gc,
												0,0,TreeWidth,TreeHeight,
												TreeX,TreeY);
		}
}

UpdateTreeLamps(void)
{
	int lastlamp;
	
	if (TreeLampSet==3)
		lastlamp=8;
	else
		lastlamp=4;

	TreeOnOff++;
	if (TreeOnOff==lastlamp)
		TreeOnOff=0;
}

UpdateStar(void)
{
	StarOnOff=(StarOnOff+1) & 1;
}

#if 0
void
UpdateSanta()
{
int Visible;
int tmp_x;
static int SantaXWindXcel = 0;
	if (SantaVisible) EraseSanta();
	if (wind) {
		if (wind == 2) {
			if (direction > 0) SantaXWindXcel += 2;
			SantaXWindXcel++;
			if (SantaXWindXcel > 2*MAXSANTA_SPEED) SantaXWindXcel = 0;
			SantaX = SantaX + SantaXStep + direction*SantaXWindXcel; 
		}
		if( wind == 1 ) {  
			SantaXWindXcel -= 3;
			if (SantaXWindXcel < 0) SantaXWindXcel = 0;
			SantaX = SantaX + SantaXStep + direction*SantaXWindXcel/4;  
		}
	}
	else {
		SantaX = SantaX + SantaXStep;
	}
	if (RandInt(10) > 3) SantaY = SantaY + SantaYStep; 
	if (SantaY < 0) SantaY = 0;
	if (RandInt(100) > 80) SantaYStep = -SantaYStep;
	if (SantaX >= display_width) ResetSanta(); 
	Visible = XRectInRegion(Wr,
								          SantaX, SantaY,
								          SantaWidth,SantaHeight);
	SantaVisible = (Visible != RectangleIn);
	SantaVisible = (SantaVisible || (SantaX < 0));
	/* Santa inside snow layers? Then make a hole in the snow :)  */
	Visible = XRectInRegion(snscr,
								          SantaX, SantaY,
								          SantaWidth,SantaHeight);
	if (Visible != RectangleOut) {
		SubR = XCreateRegion();
		AddRect.x = SantaX;
		AddRect.y = SantaY;
		AddRect.width = SantaWidth;
		AddRect.height = SantaHeight;
		XUnionRectWithRegion(&AddRect, SubR,SubR);
		XSubtractRegion(SubR,Wr, SubR);
		XSubtractRegion(snscr,SubR, snscr);
		XDestroyRegion(SubR);
	}
	CurrentSanta++;
	if (CurrentSanta >= PIXINANIMATION) CurrentSanta = 0;
	if (SantaVisible) DrawSanta();
}
#endif
/*
	Draw Santa
*/
#if 0
void
DrawSanta() {
	int rc;
	rc = XSetClipMask(	display,
											gc,
											SantaMaskPixmap[CurrentSanta]);
	rc = XSetClipOrigin(display,
											gc,
											SantaX,SantaY);
	rc = XCopyArea(			display,
											SantaPixmap[CurrentSanta],
											rootWin,
											gc,
											0,0,SantaWidth,SantaHeight,
											SantaX,SantaY);
	/* Note: the fur in his hat is *imitation* white-seal fur, of course. */
	/* Santa is a big supporter of Greenpeace.                            */
}	/* DrawSanta */
/*
	Erase Santa
*/
void
EraseSanta()
{
	XClearArea(display, rootWin,
						 SantaX , SantaY,     
						 SantaWidth,SantaHeight,
						 False);
}
#endif
/* ------------------------------------------------------------------ */ 
#if 0
#if !GRAB_SERVER
int
XsnowErrors(dpy, err)
Display *dpy;
XErrorEvent *err;
{
		errorVal = err->error_code;
		return 0;
}
#endif /* GRAB_SERVER */
#endif
/* ------------------------------------------------------------------ */ 

#if 0	 
Pixel
AllocNamedColor(colorName, dfltPix)
char *colorName;
Pixel dfltPix;
{
	Pixel pix;
	XColor scrncolor;
	XColor exactcolor;
	if (XAllocNamedColor(display, DefaultColormap(display, screen),
		colorName, &scrncolor, &exactcolor)) {
		pix = scrncolor.pixel;
	}
	else {
		pix = dfltPix;
	}
	return pix;
}
#endif
/* ------------------------------------------------------------------ */ 
void sig_alarm()
{
return;
#if 0
		int rand=RandInt(100);
		void (*sig_alarm_ptr)() = sig_alarm;
		if (NoWind) return;
		if( rand > 80 ){
				geta -= 1;
		}else if( rand > 55 ){
				geta += 1;
		}
		if( rand > 65 ){
				if( RandInt(10) > 4 ){
						direction = 1;
				}else{
						direction = -1;
				}
				wind = 2;
				signal(SIGALRM, sig_alarm_ptr);
				alarm(abs(RandInt(5))+1);
		}else{
				if( wind == 2 ){
						wind = 1;
						signal(SIGALRM, sig_alarm_ptr);
						alarm(RandInt(3)+1);
				}else{
						wind = 0;
						signal(SIGALRM, sig_alarm_ptr);
						alarm(WindTimer);
				}
		}
#endif
}

#if 0
int MWR(int Force) {
		Window *children;
		unsigned int nChildren;
		Window dummy;
		XWindowAttributes wa;
		int wx;
		XRectangle CatchRect;
		XRectangle AllowRect;
		XRectangle WinRect;
		int winX, winY;
		int NouMoe;
		unsigned int winHeight, winWidth;
		unsigned int depth;
			 
			 
			 
#if GRAB_SERVER
		XGrabServer(display);
#else
		XSetErrorHandler(XsnowErrors);
#endif
		XDestroyRegion(Wr);
		Wr = XCreateRegion();
			 
		XQueryTree(display, Parent, &dummy, &dummy, &children, &nChildren); 
		#if debug
			printf("makewrgn: nChildren=%d \n", nChildren);
		#endif
			 
		for (wx=0; wx<nChildren; wx++) {
				if (XEventsQueued(display, QueuedAlready) && (! Force)) {
					XFree((char *)children); 
					#if debug
					printf("Returning after XFree: more events are queued\n");
					#endif
					return 0;
				}
				errorVal = 0;
				XGetWindowAttributes(display, children[wx], &wa);
				if (errorVal) {
					printf("XGetWindowAttributes err\n");
					continue;
				}
				#if debug
					if (wa.save_under) printf("POPUP discarded\n");
				#endif
				if (!NoPopuphandling && wa.save_under) continue;
				if (wa.map_state == IsViewable) {
						XGetGeometry(display, children[wx], &dummy, &winX, &winY,
								&winWidth, &winHeight, &borderWidth, &depth);
						/*if (winWidth == 1280) continue;  /* debug */
						#if debug
						printf("\nw x:%d y:%d w:%d h:%d - bw:%d d:%d \n", winX,winY,winWidth,winHeight,borderWidth,depth);
						#endif
						#if debug
						printf("MakeWrx: (x;%d\ty:%d\tx+w%d\ty+h%d)\tbw:%d d:%d\n",
									winX,winY,winX+winWidth,winY+winHeight,borderWidth,depth);
						#endif
						if (errorVal) continue;
						WinRect.x = winX;
						WinRect.y = winY;
						WinRect.height = winHeight + 2*borderWidth;
						WinRect.width = winWidth + 2*borderWidth;
						if (WinRect.x >= display_width) continue;
						if (WinRect.y >= display_height) continue;
						if (WinRect.y <= 0) continue;
						if ((WinRect.x + WinRect.width) < 0) continue;                                                                                                  
						XUnionRectWithRegion(&WinRect,Wr, Wr);
				}
		}
		XFree((char *)children);
#if GRAB_SERVER
		XUngrabServer(display);
#else
		XSetErrorHandler((ErrorHandler *)NULL);
#endif
		return 1;
}
#endif	
#if 0
void RCSR() {
	XDestroyRegion(rscrr);
	rscrr = XCreateRegion();
	XUnionRegion(Wr,rscrr, rscrr);
	XOffsetRegion(rscrr, 0,MaxYStep+MaxSnowFlakeHeight);	
	XSubtractRegion(Wr,rscrr, rscrr);
	if (NoKeepSWin) {
		XDestroyRegion(snscr);
		snscr = XCreateRegion();
		return;
	}
	XUnionRegion(rscrr,snscr, snscr);
	if (SnowOffset != 0) XOffsetRegion(snscr,0,SnowOffset);
}		
#endif
#if 0
void MASR() {
	XRectangle AllowRect;
	XDestroyRegion(sar);
	sar = XCreateRegion();
	if (!NoKeepSWin) {
		XUnionRegion(Wr,sar, sar);
		XOffsetRegion(sar, 0,-MaxWinSnowDepth);
		XSubtractRegion(sar,Wr, sar);
	}
	if (!NoKeepSWin) {
		AllowRect.x = 0;
		AllowRect.y = display_height - MaxScrSnowDepth;
		AllowRect.width = display_width - 1;
		AllowRect.height = MaxYStep+MaxSnowFlakeHeight+MaxScrSnowDepth;
		XUnionRectWithRegion(&AllowRect, sar, sar);
	}
	XUnionRegion(snscr,sar, sar);
}
#endif

#if 0
int DDLT() {
	XRectangle DiffRect;
	if (!MWR(0)) return 0;
	XClipBox( Wr, &DiffRect );
	#if debug
	printf("DDLT: Wr: (%d,%d,%d,%d)\n",
					DiffRect.x,DiffRect.y,DiffRect.width,DiffRect.height); 
	#endif
	if (! XEqualRegion(Wr,PrevWr) ) {
		#if debug
		printf("DDLT: ************* CHANGED!***********\n"); 
		#endif
		XXorRegion(Wr,PrevWr, WDR);
		XClipBox( WDR, &DiffRect );
		#if debug
		printf("DDLT: diffR: (%d,%d,%d,%d)\n",
						DiffRect.x,DiffRect.y,DiffRect.x+DiffRect.width,DiffRect.y+DiffRect.height); 
		#endif
	  if (0) XClearArea(display, rootWin,
	             				DiffRect.x,
	             				DiffRect.y,
	             				DiffRect.width,              
	             				DiffRect.height,
	             				False);
		HEAr(DiffRect.x,DiffRect.y,DiffRect.width,DiffRect.height);
		XDestroyRegion(PrevWr);
		PrevWr = XCreateRegion();
		XUnionRegion(Wr,PrevWr, PrevWr);
	}
	else {
		#if debug
		printf("DDLT: NOT CHANGED!\n\n");
		#endif
		/*return 1;*/
		;;
	}
	#if debug
	printf("DDLT: RCSR\n");
	#endif
	RCSR();
	#if debug
	printf("DDLT: MASR\n");
	#endif
	MASR();
	#if debug
	printf("DDLT: MASR done\n");
	#endif
	return 1;
}		
#endif
#if 0
void HEAr(int X,int Y,int W, int H) {
int AddX,AddY;
int ClearX, ClearY;
/*return;		/* DEBUG */
	AddX = X - MaxSnowFlakeHeight;
	AddY = Y - MaxWinSnowDepth;
	AddRect.height = H +  MaxWinSnowDepth;
	AddRect.width = W + 2*MaxSnowFlakeHeight;
	if (AddY < 0) {
	  AddRect.height = AddRect.height + AddY;
	  AddY = 0;
	}
	if ((AddY+AddRect.height) > display_height) {
	  AddRect.height = display_height - AddY;
	}
	AddRect.x = AddX;
	AddRect.y = AddY;
	SubR= XCreateRegion();
	  XUnionRectWithRegion(&AddRect, SubR,SubR);
	  AddY = AddY - MaxYStep-MaxSnowFlakeHeight;
	  AddRect.height = 
	    MaxWinSnowDepth+MaxYStep*2+MaxSnowFlakeHeight*2;
	  if (AddY < 0) {
	    AddRect.height = AddRect.height + AddY;
	    AddY = 0;
	  }
	  AddRect.y = AddY;
	  if ((AddY+AddRect.height) > display_height) {
	    AddRect.height = display_height - AddY;
	  }
	  XUnionRectWithRegion(&AddRect, SubR,SubR);
		#if debug
		printf("HEAr: clearing (%d,%d,%d,%d)\n", AddX,AddY,AddRect.width,AddRect.height);
		#endif
	  XClearArea(display, rootWin,
	             AddX,
	             AddY,
	             AddRect.width,              
	             AddRect.height,
	             False);
	  XSubtractRegion(snscr,SubR, snscr);
	XDestroyRegion(SubR);
}
#endif
