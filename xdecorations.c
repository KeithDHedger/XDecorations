#if 0
	©Keith Hedger Mon 20 Oct 14:21:11 BST 2014 kdhedger68713@gmail.com
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License,or
*   (at your option) any later version.
* 
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
* 
*   You should hargve received a copy of the GNU General Public License
*   along with this program; if not,write to the Free Software
*   Foundation,Inc.,59 Temple Place,Suite 330,Boston,MA  02111-1307  USA

*   toon_root.c Copyright (C) 1999-2001  Robin Hogan
#endif

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/xpm.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

Display*	display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
int			centerX;
int			centerY;
GC			gc;

int			done=0;
long		mainDelay=50000;

Pixmap		snowManPixmap[2];
Pixmap		snowManMaskPixmap[2];

Pixmap		lampsPixmap[2];
Pixmap		lampsMaskPixmap[2];

Pixmap		treePixmap[2];
Pixmap		treeMaskPixmap[2];

Pixmap		starPixmap[2];
Pixmap		starMaskPixmap[2];

Pixmap		treeLampsPixmap[8];
Pixmap		treeLampsMaskPixmap[8];

Pixmap		tinselPixmap;
Pixmap		tinselMaskPixmap;

int			onOff=0;

uint		runCounter=0;

int			snowManSpeed=10;
int			snowManX=100;
int			snowManY=100;
int			snowManW;
int			snowManH;
int			snowManCount=0;
int			snowMan=0;
int			snowManonOff=0;

int			lampSpeed=10;
int			lampX=0;
int			lampY=26;
int			lampWidth;
int			lampHeight;
int			lampCount=0;
int			showLamps=1;
int			lampOffset=0;

int			treeWidth;
int			treeHeight;
int			treeNumber=1;
int			treelampSpeed=8;
int			starSpeed=6;
int			treeX=100;
int			treeY=100;
int			treeLampSet=1;
int			showTinsel=1;
int			showStar=1;
int			showTree=1;

int			treeOnOff=0;
int			starOnOff=0;

const char*	prefix="Xmas";

void signalHandler()
{
	done=1;
}

int randInt(int maxVal)
{
	return rand() % maxVal;
}

void uSsleep(unsigned long usec)
{
	struct timeval t;
	t.tv_usec=usec%(unsigned long)1000000;
	t.tv_sec=usec/(unsigned long)1000000;
	select(0,(void* )0,(void* )0,(void* )0,&t);
}

void initSnowMan(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;
	
	asprintf(&lampseton,"%s/%sFigureOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sFigureOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&snowManPixmap[0],&snowManMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&snowManPixmap[1],&snowManMaskPixmap[1],&attrib);
	if(rc!=0)
		snowMan=0;
	snowManW=attrib.width;
	snowManH=attrib.height;

	free(lampseton);
	free(lampsetoff);
}

void initLamps(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;
	
	asprintf(&lampseton,"%s/%sLampsOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sLampsOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&lampsPixmap[0],&lampsMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&lampsPixmap[1],&lampsMaskPixmap[1],&attrib);
	if(rc!=0)
		showLamps=0;
	lampWidth=attrib.width;
	lampHeight=attrib.height;

	lampCount=(displayWidth/lampWidth);
	lampOffset=(displayWidth-(lampWidth*lampCount))/2;

	free(lampseton);
	free(lampsetoff);
}

void initTree(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	attrib.valuemask=0;

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tree1.xpm",&treePixmap[0],&treeMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tree2.xpm",&treePixmap[1],&treeMaskPixmap[1],&attrib);

	treeWidth=attrib.width;
	treeHeight=attrib.height;

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsBlue0.xpm",&treeLampsPixmap[0],&treeLampsMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsBlue1.xpm",&treeLampsPixmap[4],&treeLampsMaskPixmap[4],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsGreen0.xpm",&treeLampsPixmap[1],&treeLampsMaskPixmap[1],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsGreen1.xpm",&treeLampsPixmap[5],&treeLampsMaskPixmap[5],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsPurple0.xpm",&treeLampsPixmap[2],&treeLampsMaskPixmap[2],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsPurple1.xpm",&treeLampsPixmap[6],&treeLampsMaskPixmap[6],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsRed0.xpm",&treeLampsPixmap[3],&treeLampsMaskPixmap[3],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsRed1.xpm",&treeLampsPixmap[7],&treeLampsMaskPixmap[7],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Star0.xpm",&starPixmap[0],&starMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Star1.xpm",&starPixmap[1],&starMaskPixmap[1],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tinsel.xpm",&tinselPixmap,&tinselMaskPixmap,&attrib);
	if(rc!=0)
		showTree=0;
}

void drawSnowMan(void)
{
	int rc;
	if (snowMan==1)
		{
			rc=XSetClipMask(display,gc,snowManMaskPixmap[snowManonOff]);
			rc=XSetClipOrigin(display,gc,snowManX,snowManY);
			rc=XCopyArea(display,snowManPixmap[snowManonOff],rootWin,gc,0,0,snowManW,snowManH,snowManX,snowManY);
		}
}

void drawLamps(void)
{
	int rc;
	int loop;
	int	CurrentlampX=lampX;
	rc=XSetClipMask(display,gc,lampsMaskPixmap[onOff]);

	for (loop=0; loop<lampCount; loop++)
		{
			rc=XSetClipOrigin(display,gc,CurrentlampX+lampOffset,lampY);
			rc=XCopyArea(display,lampsPixmap[onOff],
			             rootWin,
			             gc,
			             0,0,lampWidth,lampHeight,
			             CurrentlampX+lampOffset,lampY);
			CurrentlampX+=lampWidth;
		}
}

updateLamps(void)
{
	onOff=(onOff+1) & 1;
}

void drawTreeLamps(void)
{
	int rc;
	int lastlamp;
	int lampset;

	if (treeLampSet==2)
		lampset=4;
	else
		lampset=0;

	rc=XSetClipMask(display,gc,treeMaskPixmap[treeNumber-1]);
	rc=XSetClipOrigin(display,gc,treeX,treeY);
	rc=XCopyArea(display,treePixmap[treeNumber-1],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	rc=XSetClipMask(display,gc,treeLampsMaskPixmap[treeOnOff+lampset]);
	rc=XCopyArea(display,treeLampsPixmap[treeOnOff+lampset],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if (showStar==1)
		{
			rc=XSetClipMask(display,gc,starMaskPixmap[starOnOff]);
			rc=XCopyArea(display,starPixmap[starOnOff],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if (showTinsel==1)
		{
			rc=XSetClipMask(display,gc,tinselMaskPixmap);
			rc=XCopyArea(display,tinselPixmap,
			             rootWin,
			             gc,
			             0,0,treeWidth,treeHeight,
			             treeX,treeY);
		}
}

updateTreeLamps(void)
{
	treeOnOff++;

	if (treeOnOff==4)
		treeOnOff=0;
}

updateStar(void)
{
	starOnOff=(starOnOff+1) & 1;
}

updateSnowMan(void)
{
	snowManonOff=(snowManonOff+1) & 1;
}

int main(int argc,char* argv[])
{
	XGCValues xgcv;
	int ax;
	char* arg;
	XEvent ev;
	int Exposed;
	int ConfigureNotified;
	int i;
	int x,y;
	Window root;
	int winX,winY;
	unsigned int winHeight,winWidth;
	unsigned int depth;
	int screen;
	Window		parentWindow;
//command line options.
	for (ax=1; ax<argc; ax++)
		{
			arg=argv[ax];

			if (strcmp(arg,"-lampspeed")==0)
				{
					lampSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-lampy")==0)
				{
					lampY=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treelampspeed")==0)
				{
					treelampSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treelampset")==0)
				{
					treeLampSet=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treenumber")==0)
				{
					treeNumber=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treex")==0)
				{
					treeX=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treey")==0)
				{
					treeY=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-starspeed")==0)
				{
					starSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-noxmastree")==0)
				{
					showTree=0;
				}
			else if (strcmp(arg,"-nolamps")==0)
				{
					showLamps=0;
				}
			else if (strcmp(arg,"-nostar")==0)
				{
					showStar=0;
				}
			else if (strcmp(arg,"-notinsel")==0)
				{
					showTinsel=0;
				}
			else if (strcmp(arg,"-holiday")==0)
				{
					prefix=argv[++ax];
				}
			else if (strcmp(arg,"-snowman")==0)
				{
					snowMan=1;
				}
			else if (strcmp(arg,"-snowmanx")==0)
				{
					snowManX=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-snowmany")==0)
				{
					snowManY=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-snowmanspeed")==0)
				{
					snowManSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
		}
	srand((int)time((long* )NULL));

	signal(SIGKILL,signalHandler);
	signal(SIGINT,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGHUP,signalHandler);
	signal(SIGQUIT,signalHandler);
	display=XOpenDisplay(NULL);
	if (display==NULL)
		{
			exit(1);
		}
	screen=DefaultScreen(display);
	rootWin=ToonGetRootWindow(display,screen,&parentWindow);

	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	centerX=displayWidth / 2;
	centerY=displayHeight / 2;

	initLamps();
	initTree();
	initSnowMan();

	gc=XCreateGC(display,rootWin,0,NULL);
	XGetGCValues(display,gc,0,&xgcv);
	XSetFillStyle(display,gc,FillStippled);

	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);

	while (!done)
		{
			Exposed=0;
			ConfigureNotified=0;
			while (XPending(display))
				{
					XNextEvent(display,&ev);

				}
			uSsleep(mainDelay);

			runCounter++;

			if (showLamps==1)
				{
					if ((runCounter % lampSpeed)==0)
						updateLamps();
					drawLamps();

				}

			if (showTree==1)
				{
					if ((runCounter % treelampSpeed)==0)
						updateTreeLamps();
					drawTreeLamps();

					if ((runCounter % starSpeed)==0 && showStar==1)
						updateStar();
				}

			if (snowMan==1)
				{
					if ((runCounter % snowManSpeed)==0)
						updateSnowMan();
					drawSnowMan();

				}

		}
	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	exit(0);
}
