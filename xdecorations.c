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
Window		Parent;
int			displayWidth;
int			displayHeight;
int			centerX;
int			centerY;
GC			gc;

int			done=0;
long		mainDelay=50000;

Pixmap		snowManPixmap[2];
Pixmap		snowManMaskPixmap[2];

Pixmap		LampPixmap[2];
Pixmap		LampMaskPixmap[2];

Pixmap		TreePixmap[2];
Pixmap		TreeMaskPixmap[2];

Pixmap		StarPixmap[2];
Pixmap		StarMaskPixmap[2];

Pixmap		TreeLampsPixmap[8];
Pixmap		TreeLampsMaskPixmap[8];

Pixmap		TinselPixmap;
Pixmap		TinselMaskPixmap;

int			OnOff=0;

uint		RunCounter=0;

int			snowManSpeed=10;
int			snowManX=100;
int			snowManY=100;
int			snowManW;
int			snowManH;
int			snowManCount=0;
int			snowMan=0;
int			snowManOnOff=0;

int			LampSpeed=10;
int			LampX=0;
int			LampY=26;
int			LampWidth;
int			LampHeight;
int			LampCount=0;
int			Lamps=1;
int			lampOffset=0;

int			TreeWidth;
int			TreeHeight;
int			TreeNumber=1;
int			TreeLampSpeed=8;
int			StarSpeed=6;
int			TreeX=100;
int			TreeY=100;
int			TreeLampSet=1;
int			Tinsel=1;
int			Star=1;
int			Tree=1;

const char*	prefix="Xmas";

void SigHandler()
{
	done=1;
}

int RandInt(int maxVal)
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
	
	asprintf(&lampseton,"%s/%sDecOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sDecOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&snowManPixmap[0],&snowManMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&snowManPixmap[1],&snowManMaskPixmap[1],&attrib);
	if(rc!=0)
		snowMan=0;
	snowManW=attrib.width;
	snowManH=attrib.height;

	free(lampseton);
	free(lampsetoff);
}

void InitLamps(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;
	
	asprintf(&lampseton,"%s/%sLampsOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sLampsOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&LampPixmap[0],&LampMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&LampPixmap[1],&LampMaskPixmap[1],&attrib);
	if(rc!=0)
		Lamps=0;
	LampWidth=attrib.width;
	LampHeight=attrib.height;

	LampCount=(displayWidth/LampWidth);
	lampOffset=(displayWidth-(LampWidth*LampCount))/2;

	free(lampseton);
	free(lampsetoff);
}

void InitTree(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	attrib.valuemask=0;

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tree1.xpm",&TreePixmap[0],&TreeMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tree2.xpm",&TreePixmap[1],&TreeMaskPixmap[1],&attrib);

	TreeWidth=attrib.width;
	TreeHeight=attrib.height;

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsBlue0.xpm",&TreeLampsPixmap[0],&TreeLampsMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsBlue1.xpm",&TreeLampsPixmap[4],&TreeLampsMaskPixmap[4],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsGreen0.xpm",&TreeLampsPixmap[1],&TreeLampsMaskPixmap[1],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsGreen1.xpm",&TreeLampsPixmap[5],&TreeLampsMaskPixmap[5],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsPurple0.xpm",&TreeLampsPixmap[2],&TreeLampsMaskPixmap[2],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsPurple1.xpm",&TreeLampsPixmap[6],&TreeLampsMaskPixmap[6],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsRed0.xpm",&TreeLampsPixmap[3],&TreeLampsMaskPixmap[3],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/LightsRed1.xpm",&TreeLampsPixmap[7],&TreeLampsMaskPixmap[7],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Star0.xpm",&StarPixmap[0],&StarMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Star1.xpm",&StarPixmap[1],&StarMaskPixmap[1],&attrib);

	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tinsel.xpm",&TinselPixmap,&TinselMaskPixmap,&attrib);
	if(rc!=0)
		Tree=0;
}

void drawSnowMan(void)
{
	int rc;
	if (snowMan==1)
		{
			rc=XSetClipMask(display,gc,snowManMaskPixmap[snowManOnOff]);
			rc=XSetClipOrigin(display,gc,snowManX,snowManY);
			rc=XCopyArea(display,snowManPixmap[snowManOnOff],rootWin,gc,0,0,snowManW,snowManH,snowManX,snowManY);
		}
}

void DrawLamps(void)
{
	int rc;
	int loop;
	int	CurrentLampX=LampX;
	rc=XSetClipMask(display,gc,LampMaskPixmap[OnOff]);

	for (loop=0; loop<LampCount; loop++)
		{
			rc=XSetClipOrigin(display,gc,CurrentLampX+lampOffset,LampY);
			rc=XCopyArea(display,LampPixmap[OnOff],
			             rootWin,
			             gc,
			             0,0,LampWidth,LampHeight,
			             CurrentLampX+lampOffset,LampY);
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

	if (TreeLampSet==2)
		lampset=4;
	else
		lampset=0;

	rc=XSetClipMask(display,gc,TreeMaskPixmap[TreeNumber-1]);
	rc=XSetClipOrigin(display,gc,TreeX,TreeY);
	rc=XCopyArea(display,TreePixmap[TreeNumber-1],rootWin,gc,0,0,TreeWidth,TreeHeight,TreeX,TreeY);

	rc=XSetClipMask(display,gc,TreeLampsMaskPixmap[TreeOnOff+lampset]);
	rc=XCopyArea(display,TreeLampsPixmap[TreeOnOff+lampset],rootWin,gc,0,0,TreeWidth,TreeHeight,TreeX,TreeY);

	if (Star==1)
		{
			rc=XSetClipMask(display,gc,StarMaskPixmap[StarOnOff]);
			rc=XCopyArea(display,StarPixmap[StarOnOff],rootWin,gc,0,0,TreeWidth,TreeHeight,TreeX,TreeY);
		}

	if (Tinsel==1)
		{
			rc=XSetClipMask(display,gc,TinselMaskPixmap);
			rc=XCopyArea(display,TinselPixmap,
			             rootWin,
			             gc,
			             0,0,TreeWidth,TreeHeight,
			             TreeX,TreeY);
		}
}

UpdateTreeLamps(void)
{
	TreeOnOff++;

	if (TreeOnOff==4)
		TreeOnOff=0;
}

UpdateStar(void)
{
	StarOnOff=(StarOnOff+1) & 1;
}

updateSnowMan(void)
{
	snowManOnOff=(snowManOnOff+1) & 1;
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

//command line options.
	for (ax=1; ax<argc; ax++)
		{
			arg=argv[ax];

			if (strcmp(arg,"-lampspeed")==0)
				{
					LampSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-lampy")==0)
				{
					LampY=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treelampspeed")==0)
				{
					TreeLampSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treelampset")==0)
				{
					TreeLampSet=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treenumber")==0)
				{
					TreeNumber=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treex")==0)
				{
					TreeX=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-treey")==0)
				{
					TreeY=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-starspeed")==0)
				{
					StarSpeed=strtol(argv[++ax],(char* *)NULL,0);
				}
			else if (strcmp(arg,"-noxmastree")==0)
				{
					Tree=0;
				}
			else if (strcmp(arg,"-nolamps")==0)
				{
					Lamps=0;
				}
			else if (strcmp(arg,"-nostar")==0)
				{
					Star=0;
				}
			else if (strcmp(arg,"-notinsel")==0)
				{
					Tinsel=0;
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

	signal(SIGKILL,SigHandler);
	signal(SIGINT,SigHandler);
	signal(SIGTERM,SigHandler);
	signal(SIGHUP,SigHandler);
	signal(SIGQUIT,SigHandler);
	display=XOpenDisplay(NULL);
	if (display==NULL)
		{
			exit(1);
		}
	screen=DefaultScreen(display);
	rootWin=ToonGetRootWindow(display,screen,&Parent);

	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	centerX=displayWidth / 2;
	centerY=displayHeight / 2;

	InitLamps();
	InitTree();
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

			RunCounter++;

			if (Lamps==1)
				{
					if ((RunCounter % LampSpeed)==0)
						UpdateLamps();
					DrawLamps();

				}

			if (Tree==1)
				{
					if ((RunCounter % TreeLampSpeed)==0)
						UpdateTreeLamps();
					DrawTreeLamps();

					if ((RunCounter % StarSpeed)==0 && Star==1)
						UpdateStar();
				}

			if (snowMan==1)
				{
					if ((RunCounter % snowManSpeed)==0)
						updateSnowMan();
					drawSnowMan();

				}

		}
	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	exit(0);
}
