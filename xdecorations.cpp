#if 0
©Keith Hedger Mon 20 Oct 14:21:11 BST 2014 kdhedger68713@gmail.com
*   This program is free software;
you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation;
either version 2 of the License,or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY;
without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should hargve received a copy of the GNU General Public License
*   along with this program;
if not,write to the Free Software
*   Foundation,Inc.,59 Temple Place,Suite 330,Boston,MA  02111-1307  USA

*   toon_root.c Copyright (C) 1999-2001  Robin Hogan
#endif

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/xpm.h>
#include <X11/Xatom.h>

#include <X11/extensions/shape.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "toon.h"

#define MAXPATHNAMELEN 2048
#define MAXNUMBEROFFLYERS 10
#define MAXNUMBEROFTREELIGHTS 10

#define VERSION "0.1.2"

enum {ONPIXMAP=0,ONMASK,OFFPIXMAP,OFFMASK};
#define _SELECTPIXMAP(a,b) (a+(2*b))//a=ONPIXMAP b=xxxOnOff

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};


char		pathname[MAXPATHNAMELEN];
char*		configFilePath;

Display*	display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
GC			gc;

int			done=0;
long		mainDelay=20000;

uint		runCounter=0;

//flyers
Pixmap		flyersPixmap[MAXNUMBEROFFLYERS][2];
int			flyersSpeed=1;
int			flyersStep=8;
int			flyersWidth[MAXNUMBEROFFLYERS];
int			flyersHeight[MAXNUMBEROFFLYERS];
int			flyersX[MAXNUMBEROFFLYERS];
int			flyersY[MAXNUMBEROFFLYERS];
bool		showFlyers=false;
int			flyersMaxY=400;
int			flyersActive[MAXNUMBEROFFLYERS];
int			flyerSpread=500;
int			flyerCount=0;
Window		flyerWindow[MAXNUMBEROFFLYERS];

//figure
Pixmap		figurePixmap[4];
int			figureSpeed=100;
int			figureX=100;
int			figureY=100;
int			figureW;
int			figureH;
bool		showFigure=false;
int			figureOnOff=0;
int			figureNumber=1;

//lamps
Pixmap		lampsPixmap[4];
int			lampSpeed=100;
int			lampX=0;
int			lampY=16;
int			lampWidth;
int			lampHeight;
int			lampCount;
bool		showLamps=true;
int			lampSet=1;
int			lampsOnOff=0;

//trees
Pixmap		treePixmap[2];
int			treeWidth;
int			treeHeight;
int			treeNumber=1;
int			treelampSpeed=100;
int			starSpeed=100;
int			treeX;
int			treeY;
int			treeLampSet=1;
int			treeOnOff=0;
bool		showTree=false;

//tinsel`
Pixmap		tinselPixmap[2];
bool		showTinsel=false;

//star
Pixmap		starPixmap[4];
bool		showStar=false;
int			starOnOff=0;

//tree lamps
Pixmap		treeLampsPixmap[MAXNUMBEROFTREELIGHTS][2];
//Pixmap		treeLampsMaskPixmap[8];
bool		showTreeLamps=false;
int			treeLampCount=0;

char*		prefix;

bool		treeNeedsUpdate=false;
bool		flyerNeedsUpdate=false;
bool		figureNeedsUpdate=false;

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

args	xdecorations_rc[]=
{
//bools
	{"flyer",TYPEBOOL,&showFlyers},
	{"tree",TYPEBOOL,&showTree},
	{"tinsel",TYPEBOOL,&showTinsel},
	{"figure",TYPEBOOL,&showFigure},
	{"lamps",TYPEBOOL,&showLamps},
	{"star",TYPEBOOL,&showStar},
	{"treelamps",TYPEBOOL,&showTreeLamps},
//strings
	{"holiday",TYPESTRING,&prefix},
//ints
	{"delay",TYPEINT,&mainDelay},
	{"flyermaxy",TYPEINT,&flyersMaxY},
	{"spread",TYPEINT,&flyerSpread},
	{"lampdelay",TYPEINT,&lampSpeed},
	{"lampset",TYPEINT,&lampSet},
	{"flydelay",TYPEINT,&flyersSpeed},
	{"flystep",TYPEINT,&flyersStep},
	{"lampy",TYPEINT,&lampY},
	{"treelampdelay",TYPEINT,&treelampSpeed},
	{"treelampset",TYPEINT,&treeLampSet},
	{"treenumber",TYPEINT,&treeNumber},
	{"treex",TYPEINT,&treeX},
	{"treey",TYPEINT,&treeY},
	{"stardelay",TYPEINT,&starSpeed},
	{"figurex",TYPEINT,&figureX},
	{"figurey",TYPEINT,&figureY},
	{"figuredelay",TYPEINT,&figureSpeed},
	{"figurenumber",TYPEINT,&figureNumber},
	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
						case TYPEINT:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

void loadVarsFromFile(char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%as %as",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
										case TYPEINT:
											*(int*)dataptr[cnt].data=atoi(strarg);
											break;
										case TYPESTRING:
											if(*(char**)(dataptr[cnt].data)!=NULL)
												free(*(char**)(dataptr[cnt].data));
											sscanf(buffer,"%*s %a[^\n]s",(char**)dataptr[cnt].data);
											break;
										case TYPEBOOL:
											*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
											break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
					argname=NULL;
					strarg=NULL;
				}
			fclose(fd);
		}
}

void signalHandler()
{
	done=1;
}

int randInt(int maxVal)
{
	return rand() % maxVal;
}

void initFlyers(void)
{
	int				rc=0;
	int				j=0;
	XpmAttributes	attrib;

	attrib.valuemask=0;

	rc=0;
	for(j=0; j<MAXNUMBEROFFLYERS; j++)
		{
			rc=0;
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sFly%i.xpm",DATADIR,prefix,j+1);
			rc=XpmReadFileToPixmap(display,rootWin,pathname,&flyersPixmap[flyerCount][ONPIXMAP],&flyersPixmap[flyerCount][ONMASK],&attrib);
			if(rc==0)
				{
					flyersWidth[flyerCount]=attrib.width;
					flyersHeight[flyerCount]=attrib.height;
					flyersY[flyerCount]=(rand() % flyersMaxY);
					flyersX[flyerCount]=0-flyersWidth[j];
					if((rand() % flyerSpread)==0)
						flyersActive[flyerCount]=1;
					else
						flyersActive[flyerCount]=0;
					flyerCount++;
				}
		}

	if(flyerCount==0)
		showFlyers=false;
}

void initFigure(void)
{
	int				rc=0;
	XpmAttributes	attrib;

	attrib.valuemask=0;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sFigureOn%i.xpm",DATADIR,prefix,figureNumber);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&figurePixmap[ONPIXMAP],&figurePixmap[ONMASK],&attrib);
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sFigureOff%i.xpm",DATADIR,prefix,figureNumber);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&figurePixmap[OFFPIXMAP],&figurePixmap[OFFMASK],&attrib);

	if(rc==0)
		{
			figureW=attrib.width;
			figureH=attrib.height;
		}
	else
		showFigure=false;
}

void initLamps(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;
	rc=0;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sLampsOn%i.xpm",DATADIR,prefix,lampSet);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&lampsPixmap[ONPIXMAP],&lampsPixmap[ONMASK],&attrib);
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sLampsOff%i.xpm",DATADIR,prefix,lampSet);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&lampsPixmap[OFFPIXMAP],&lampsPixmap[OFFMASK],&attrib);
	if(rc==0)
		{
			lampWidth=attrib.width;
			lampHeight=attrib.height;
			lampCount=(displayWidth/lampWidth)+1;
		}
	else
		showLamps=false;
}

void initTree(void)
{
	int				rc=0;
	int				j=0;
	XpmAttributes	attrib;
	bool			gotsomelamps=false;

	attrib.valuemask=0;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sTree%i.xpm",DATADIR,prefix,treeNumber);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&treePixmap[ONPIXMAP],&treePixmap[ONMASK],&attrib);
	if(rc==0)
		{
			treeWidth=attrib.width;
			treeHeight=attrib.height;
		}
	else
		showTree=false;


	treeLampCount=0;
	for(j=0; j<MAXNUMBEROFTREELIGHTS; j++)
		{
			rc=0;
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sTreeLights%i.%i.xpm",DATADIR,prefix,treeLampSet,j+1);
			rc=XpmReadFileToPixmap(display,rootWin,pathname,&treeLampsPixmap[j][ONPIXMAP],&treeLampsPixmap[j][ONMASK],&attrib);
			if(rc==0)
				{
					treeLampCount++;
					gotsomelamps=true;
				}
		}
	if(gotsomelamps==false)
		showTreeLamps=false;

	rc=0;
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar0.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&starPixmap[ONPIXMAP],&starPixmap[ONMASK],&attrib);
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar1.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&starPixmap[OFFPIXMAP],&starPixmap[OFFMASK],&attrib);
	if(rc!=0)
		showStar=false;

	rc=0;
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tinsel.xpm",&tinselPixmap[ONPIXMAP],&tinselPixmap[ONMASK],&attrib);
	if(rc!=0)
		showTinsel=false;
}

void drawFlyers(void)
{
	if(showFlyers==false)
		return;

	for(int j=0; j<flyerCount; j++)
		{
			if(flyersActive[j]==1)
				{
					XCopyArea(display,flyersPixmap[j][0],flyerWindow[j],gc,0,0,flyersWidth[j],flyersHeight[j],0,0);
					XMoveWindow(display,flyerWindow[j],flyersX[j],flyersY[j]);
				}
		}
#if 0
	int rc;
	int	j;

	if(showFlyers==false)
		return;

	for(j=0; j<flyerCount; j++)
		{
			if(flyersActive[j]==1)
				{
					rc=XSetClipMask(display,gc,flyersPixmap[j][ONMASK]);
					rc=XSetClipOrigin(display,gc,flyersX[j],flyersY[j]);
					rc=XCopyArea(display,flyersPixmap[j][ONPIXMAP],rootWin,gc,0,0,flyersWidth[j],flyersHeight[j],flyersX[j],flyersY[j]);
				}
		}
#endif
}

void drawFigure(void)
{
	int rc;

	if(showFigure==false)
		return;

	rc=XSetClipMask(display,gc,figurePixmap[_SELECTPIXMAP(ONMASK,figureOnOff)]);
	rc=XSetClipOrigin(display,gc,figureX,figureY);
	rc=XCopyArea(display,figurePixmap[_SELECTPIXMAP(ONPIXMAP,figureOnOff)],rootWin,gc,0,0,figureW,figureH,figureX,figureY);
}

void drawLamps(void)
{
	int rc=0;
	int loop;
	int	CurrentlampX;

	if(showLamps==false)
		return;

	CurrentlampX=lampX;
	rc=XSetClipMask(display,gc,lampsPixmap[_SELECTPIXMAP(ONMASK,lampsOnOff)]);

	for (loop=0; loop<lampCount; loop++)
		{
			rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
			rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,lampsOnOff)],rootWin,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
			CurrentlampX+=lampWidth;
		}
}

void updateLamps(void)
{
	lampsOnOff=(lampsOnOff+1) & 1;
}

void drawTreeLamps(void)
{
	int rc;
	int lampset;

	if(showTree==false)
		return;

	lampset=(treeLampSet-1)*4;
	rc=XSetClipMask(display,gc,treePixmap[ONMASK]);
	rc=XSetClipOrigin(display,gc,treeX,treeY);
	rc=XCopyArea(display,treePixmap[ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if(showTreeLamps==true)
		{
			rc=XSetClipMask(display,gc,treeLampsPixmap[treeOnOff][ONMASK]);
			rc=XCopyArea(display,treeLampsPixmap[treeOnOff][ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showStar==true)
		{
			rc=XSetClipMask(display,gc,starPixmap[_SELECTPIXMAP(ONMASK,starOnOff)]);
			rc=XCopyArea(display,starPixmap[_SELECTPIXMAP(ONPIXMAP,starOnOff)],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showTinsel==true)
		{
			rc=XSetClipMask(display,gc,tinselPixmap[ONMASK]);
			rc=XCopyArea(display,tinselPixmap[ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}
}

void updateTreeLamps(void)
{
	treeOnOff++;

	if(treeOnOff==treeLampCount)
		treeOnOff=0;
}

void updateStar(void)
{
	starOnOff=(starOnOff+1) & 1;
}

void updateFigure(void)
{
	figureOnOff=(figureOnOff+1) & 1;
}

void updateFlyers(void)
{
	for(int j=0; j<flyerCount; j++)
		{
			if((flyersActive[j]==0) && ((rand() % flyerSpread)==0))
				flyersActive[j]=1;

			if(flyersActive[j]==1)
				{
					flyersX[j]+=flyersStep;

					if(flyersX[j]>displayWidth)
						{
							flyersActive[j]=0;
							flyersX[j]=0-flyersWidth[j];
							flyersY[j]=(rand() % flyersMaxY);
						}
				}
		}
}

void eraseRects(void)
{
	int	rc;
	int	j;

	if((showTree==true) && (treeNeedsUpdate==true))
		{
			rc=XClearArea(display,rootWin,treeX,treeY,treeWidth,treeHeight,False);
			updateTreeLamps();
		}

	if((showFigure==true) && (figureNeedsUpdate==true))
		{
			rc=XClearArea(display,rootWin,figureX,figureY,figureW,figureH,False);
			updateFigure();
		}

	if((showFlyers==true) && (flyerNeedsUpdate==true))
		{
			//for(j=0; j<flyerCount; j++)
			//	rc=XClearArea(display,rootWin,flyersX[j],flyersY[j],flyersWidth[j],flyersHeight[j],False);
			updateFlyers();
		}

	treeNeedsUpdate=false;
	figureNeedsUpdate=false;
	flyerNeedsUpdate=false;
}

void showUnShow(const char* arg1,const char* arg2,bool *value)
{
	const char*	ptr=NULL;

	ptr=strcasestr(arg1,arg2);
	if(ptr!=NULL)
		{
			if(strcasecmp(ptr,arg2)!=0)
				return;
			*value=true;
			if(strcasestr(arg1,"no-")!=NULL)
				*value=false;
		}
}

void doHelp(void)
{
	printf("XDecorations (c)2014 K. D. Hedger - Version %s\n",VERSION);
	printf("Released under the gpl-3.0 license\n\n");
	printf("Values are set to defaults then set to values contained in ~/.config/xdecorations.rc and then overridden on the command line\n\n");

	printf("-holiday STRING\n");
	printf("\tSet prefix for theme\n");
	printf("-delay INTEGER\n");
	printf("\tSet main delay\n");
	printf("-configfile FILEPATH\n");
	printf("\tSet new config file ( only the first of instance of this will be used )\n");
	printf("-writeconfig FILEPATH\n");
	printf("\tWrite out a new config file including currently loaded config file,defaults and command line options\n");
	printf("\tMust be last command on line\n\n");

	printf("-showflyer/-no-showflyer\n");
	printf("\tShow flying objects\n");
	printf("-showtree/-no-showtree\n");
	printf("\tShow tree\n");
	printf("-showlamps/-no-showlamps\n");
	printf("\tShow lamps\n");
	printf("-showfigure/-no-showfigure\n");
	printf("\tShow figure\n");
	printf("-showstar/-no-showstar\n");
	printf("\tShow star\n");
	printf("-showtinsel/-no-showtinsel\n");
	printf("\tShow tinsel\n");
	printf("-showtreelamps/-no-showtreelamps\n");
	printf("\tShow tree lamps\n");
	printf("\n");
	printf("-lampy INTEGER\n");
	printf("\tLamp Y position\n");
	printf("-lampdelay INTEGER\n");
	printf("\tLamp delay\n");
	printf("-lampset INTEGER\n");
	printf("\tLamp set\n");
	printf("-flyermaxy INTEGER\n");
	printf("\tLowest point on screen for flying objects\n");
	printf("-spread INTEGER\n");
	printf("\tRandom delay for flying objects\n");
	printf("-flydelay INTEGER\n");
	printf("\tFlying objects delay\n");
	printf("-flystep INTEGER\n");
	printf("\tAmount to move flying objects\n");
	printf("-treelampdelay INTEGER\n");
	printf("\tTree lamps delay\n");
	printf("-treelampset INTEGER\n");
	printf("\tLampset to use on tree\n");
	printf("-treenumber INTEGER\n");
	printf("\tThe tree to use\n");
	printf("-treex INTEGER\n");
	printf("\tAbsolute X position of tree\n");
	printf("-treey INTEGER\n");
	printf("\tAbsolute Y position of tree\n");
	printf("-stardelay INTEGER\n");
	printf("\tDelay for star\n");
	printf("-figurex INTEGER\n");
	printf("\tAbsolute X position of figure\n");
	printf("-figuredelay INTEGER\n");
	printf("\tDelay for figure\n");
	printf("-figurenumber INTEGER\n");
	printf("\tNumber of figure to use\n\n");

	exit(0);
}

void doMakeFlyerWindow(void)
{
	int rc;
	Hints   hints;
	Atom    property;
	Atom xa;
	XSetWindowAttributes attr;
	XVisualInfo vinfo;
	XpmAttributes	attrib;
	XWindowChanges wc;
	Atom xa_prop[5];

	XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

	attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);

	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.override_redirect = True;
	attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;

	hints.flags=2;
	hints.decorations=0;
	property=XInternAtom(display,"_MOTIF_WM_HINTS",True);

	xa=XInternAtom(display,"_NET_WM_STATE",False);
	if (xa!=None)
		{
			xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
			xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
			xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
			xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
		}

	for(int j=0;j<flyerCount;j++)
		{
			flyerWindow[j]=XCreateWindow(display,DefaultRootWindow(display),0,0,flyersWidth[j],flyersHeight[j],0,CopyFromParent,InputOutput,CopyFromParent,0,&attr);

			XChangeProperty(display,flyerWindow[j],xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,4);
			XChangeProperty(display,flyerWindow[j],property,property,32,PropModeReplace,(unsigned char *)&hints,5);

			XMapWindow(display,flyerWindow[j]);
			XShapeCombineMask (display,flyerWindow[j],ShapeBounding,0,0,flyersPixmap[j][1],ShapeSet);
		}
}

int main(int argc,char* argv[])
{
	int		argnum;
	const	char* argstr;
	XEvent	ev;
	Window	root;
	int		screen;
	Window	parentWindow;
	bool	redoconfig=true;
	bool	doneconfig=false;

	prefix=strdup("Xmas");
	asprintf(&configFilePath,"%s/.config/xdecorations.rc",getenv("HOME"));

	while(redoconfig==true)
		{
			loadVarsFromFile(configFilePath,xdecorations_rc);
			redoconfig=false;
//command line options.
			for (argnum=1; argnum<argc; argnum++)
				{
					argstr=argv[argnum];

					showUnShow(argstr,"showflyer",&showFlyers);//showFlyers=false
					showUnShow(argstr,"showtree",&showTree);//showTree=false
					showUnShow(argstr,"showlamps",&showLamps);//showLamps=true
					showUnShow(argstr,"showfigure",&showFigure);//showFigure=false
					showUnShow(argstr,"showstar",&showStar);//showStar=false
					showUnShow(argstr,"showtinsel",&showTinsel);//showTinsel=false
					showUnShow(argstr,"showtreelamps",&showTreeLamps);//showTreeLamps=false

					if(strcmp(argstr,"-configfile")==0)//~/.config/xdecorations.rc
						{
							if(doneconfig==false)
								{
									doneconfig=true;
									free(configFilePath);
									asprintf(&configFilePath,"%s",argv[++argnum]);
									redoconfig=true;
									continue;
								}
						}

					if(strcmp(argstr,"-holiday")==0)//Xmas
						{
							free(prefix);
							prefix=strdup(argv[++argnum]);
						}

					if(strcmp(argstr,"-delay")==0)//mainDelay=20000
						mainDelay=atol(argv[++argnum]);

					if(strcmp(argstr,"-lampy")==0)//lampY=16
						lampY=atol(argv[++argnum]);

					if(strcmp(argstr,"-lampdelay")==0)//lampSpeed=100
						lampSpeed=atol(argv[++argnum]);

					if(strcmp(argstr,"-lampset")==0)//lampSet=0
						lampSet=atol(argv[++argnum]);

					if(strcmp(argstr,"-flyermaxy")==0)//flyersMaxY=400
						flyersMaxY=atol(argv[++argnum]);

					if(strcmp(argstr,"-spread")==0)//flyerSpread=500
						flyerSpread=atol(argv[++argnum]);

					if(strcmp(argstr,"-flydelay")==0)//flyersSpeed=1
						flyersSpeed=atol(argv[++argnum]);

					if(strcmp(argstr,"-flystep")==0)//flyersStep=8
						flyersStep=atol(argv[++argnum]);

					if(strcmp(argstr,"-treelampdelay")==0)//treelampSpeed=100
						treelampSpeed=atol(argv[++argnum]);

					if(strcmp(argstr,"-treelampset")==0)//treeLampSet=1
						treeLampSet=atol(argv[++argnum]);

					if(strcmp(argstr,"-treenumber")==0)//treeNumber=1
						treeNumber=atol(argv[++argnum]);

					if(strcmp(argstr,"-treex")==0)//treeX=100
						treeX=atol(argv[++argnum]);

					if(strcmp(argstr,"-treey")==0)//treeY=100
						treeY=atol(argv[++argnum]);

					if(strcmp(argstr,"-stardelay")==0)//starSpeed
						starSpeed=atol(argv[++argnum]);

					if(strcmp(argstr,"-figurex")==0)//figureX=100
						figureX=atol(argv[++argnum]);

					if(strcmp(argstr,"-figurey")==0)//figureY=100
						figureY=atol(argv[++argnum]);

					if(strcmp(argstr,"-figuredelay")==0)//figureSpeed=100
						figureSpeed=atol(argv[++argnum]);

					if(strcmp(argstr,"-figurenumber")==0)//figureNumber=1
						figureNumber=atol(argv[++argnum]);

					if(strcmp(argstr,"-writeconfig")==0)//figureNumber=1
						{
							saveVarsToFile(argv[++argnum],xdecorations_rc);
							return(0);
						}
//print help
					if(strcmp(argstr,"-help")==0)
						doHelp();
				}
		}
	srand((int)time((long* )NULL));

	signal(SIGKILL,(sighandler_t)&signalHandler);
	signal(SIGTERM,(sighandler_t)signalHandler);
	signal(SIGQUIT,(sighandler_t)signalHandler);
	signal(SIGINT,(sighandler_t)signalHandler);
	signal(SIGHUP,(sighandler_t)signalHandler);

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	rootWin=ToonGetRootWindow(display,screen,&parentWindow);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);

	initLamps();
	initTree();
	initFigure();
	initFlyers();

	gc=XCreateGC(display,rootWin,0,NULL);
	XSetFillStyle(display,gc,FillSolid);

	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);
	figureNeedsUpdate=false;

	doMakeFlyerWindow();

	while (!done)
		{
			usleep(mainDelay);
			runCounter++;

			while(XPending(display))
				XNextEvent(display,&ev);

			if(showLamps==true)
				{
					if((runCounter % lampSpeed)==0)
						updateLamps();
				}

			if(showTree==true)
				{
					if((runCounter % treelampSpeed)==0)
						treeNeedsUpdate=true;

					if((runCounter % starSpeed)==0 && showStar==true)
						updateStar();
				}

			if(showFigure==true)
				{
					if((runCounter % figureSpeed)==0)
						figureNeedsUpdate=true;
				}

			if(showFlyers==true)
				{
					if((runCounter % flyersSpeed)==0)
						flyerNeedsUpdate=true;
				}

			eraseRects();
			drawFlyers();
			drawTreeLamps();
			drawFigure();
			drawLamps();
		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	return(0);
}




