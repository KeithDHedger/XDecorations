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
#include <X11/xpm.h>
#include <Imlib2.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdbe.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#include "toon.h"
#include "globals.h"
#include "draw.h"
#include "startstop.h"
#include "update.h"
#include "routines.h"

struct args
{
	const char*		name;
	int				type;
	void*			data;
};

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

args				xdecorations_rc[]=
{
//app
	{"theme",TYPESTRING,&prefix},
	{"delay",TYPEINT,&mainDelay},
	{"usewindow",TYPEBOOL,&useWindow},
	{"usebuffer",TYPEBOOL,&useDBOveride},
	{"offsety",TYPEINT,&offSetY},

//lamps
	{"lampset",TYPEINT,&lampSet},
	{"lampy",TYPEINT,&lampY},
	{"lampdelay",TYPEINT,&lampSpeed},
	{"lampflash",TYPEINT,&lampAnim},
	{"lampcycledelay",TYPEINT,&lampCycleDelay},

//flyers
	{"flyer",TYPEINT,&flyerNumber},
	{"maxflyers",TYPEINT,&numberOfFlyers},
	{"flyermaxy",TYPEINT,&flyersMaxY},
	{"spread",TYPEINT,&flyerSpread},
	{"flydelay",TYPEINT,&flyersSpeed},
	{"flystep",TYPEINT,&flyersStep},
	{"flyanimdelay",TYPEINT,&flyerAnimSpeed},

//trees
	{"tree",TYPEINT,&treeNumber},
	{"treex",TYPEINT,&treeX},
	{"treey",TYPEINT,&treeY},
	{"star",TYPEBOOL,&showStar},
	{"stardelay",TYPEINT,&starSpeed},
	{"tinsel",TYPEBOOL,&showTinsel},
	{"treelampset",TYPEINT,&treeLampSet},
	{"treelampdelay",TYPEINT,&treelampSpeed},

//figure
	{"figure",TYPEINT,&figureNumber},
	{"figurex",TYPEINT,&figureX},
	{"figurey",TYPEINT,&figureY},
	{"figuredelay",TYPEINT,&figureSpeed},

//falling
	{"falling",TYPEINT,&fallingNumber},
	{"falldelay",TYPEINT,&fallingDelay},
	{"maxfalling",TYPEINT,&numberOfFalling},
	{"fallingspread",TYPEINT,&fallingSpread},
	{"fallingspeed",TYPEINT,&fallSpeed},
	{"minfallspeed",TYPEINT,&minFallSpeed},
	{"fallinanimdelay",TYPEINT,&fallingAnimSpeed},
	{"maxxstep",TYPEINT,&maxXStep},

//wind
	{"wind",TYPEINT,&windSpeed},
	{"usegusts",TYPEBOOL,&useGusts},
	{"gustlen",TYPEINT,&gustDuration},
	{"gustdelay",TYPEINT,&gustEvent},
	{"gustspeed",TYPEINT,&gustSpeed},

//settling
	{"settleheight",TYPEINT,&maxBottomHeight},
	{"settlerate",TYPEINT,&settleRate},
	{"clearonmaxhight",TYPEBOOL,&clearOnMaxHeight},

	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	if(filepath[0]=='-')
		fd=stdout;
	else
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

int get_argb_visual(Visual** vis, int *depth)
{
	/* code from gtk project, gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals = 0, i;
	visual_template.screen = screen;
	visual_list = XGetVisualInfo (display,0,&visual_template, &nxvisuals);

	for (i = 0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth == 32 &&
			        (visual_list[i].red_mask   == 0xff0000 &&
			         visual_list[i].green_mask == 0x00ff00 &&
			         visual_list[i].blue_mask  == 0x0000ff ))
				{
					*vis = visual_list[i].visual;
					*depth = visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

void doHelp(void)
{
	printf("XDecorations (c)2014 K. D. Hedger - Version %s\n",VERSION);
	printf("Released under the gpl-3.0 license\n\n");
	printf("Values are set to defaults then set to values contained in ~/.config/xdecorations.rc and then overridden on the command line\n\n");

//app
	printf("APPLICATION OPTIONS\n");
	printf("-theme STRING\n");
	printf("\tSet theme name ( default 'Xmas' )\n");
	printf("-delay INTEGER\n");
	printf("\tSet main delay\n");
	printf("-configfile FILEPATH\n");
	printf("\tSet new config file ( using only one instance of this makes any sense )\n");
	printf("-writeconfig FILEPATH\n");
	printf("\tWrite out a new config file including currently loaded config file,defaults and command line options then exit\n");
	printf("\tOptions after this are ignored\n");
	printf("-noconfig\n");
	printf("\tDont't load the default config file ( ~/.config/xdecorations.rc )\n");
	printf("-usewindow/-no-usewindow\n");
	printf("\tUse a transparent window instead of the root window\n");
	printf("-usebuffer/-no-usebuffer\n");
	printf("\tUse double buffering\n\tUsing double buffering gives best graphical results but is slower\n");
	printf("\tThe default is to use a transparent window with double buffering\n\n");

	printf("-watchconfig/-no-watchconfig\n");
	printf("\tMonitor changes to last loaded config file and apply any changes in real time ( experimental be warned! )\n");

//lamps
	printf("LAMPS\n");
	printf("-lampset INTEGER\n");
	printf("\tThe number of the lamp set to use ( 0=no lamps )\n");
	printf("-lampy INTEGER\n");
	printf("\tLamp Y position\n");
	printf("-lampdelay INTEGER\n");
	printf("\tDelay for flashing lamps\n");
	printf("-lampflash INTEGER\n");
	printf("\tLamp flash type 0=flash, 1=inverted chase, 2=chase ,3=random, 4=cycle\n");
	printf("-lampcycledelay INTEGER\n");
	printf("\tDelay before cycling to the next flash type\n\n");

//flying
	printf("FLYERS\n");
	printf("-flyer\n");
	printf("\tFlying object set ( 0=disable flyers )\n");
	printf("-maxflyers INTEGER\n");
	printf("\tNumber of flying objects to use <%i\n",MAXNUMBEROFFLYERS);
	printf("\tIf the number of flyers are > 0 then the image is chosen randomly from the available flyers\n");
	printf("\tIf the number of flyers are < 0 then tthe image is chosen sequentially\n");
	printf("-flyermaxy INTEGER\n");
	printf("\tLowest point on screen for flying objects\n");
	printf("-spread INTEGER\n");
	printf("\tRandom delay for flying objects\n");
	printf("-flydelay INTEGER\n");
	printf("\tFlying objects delay\n");
	printf("-flystep INTEGER\n");
	printf("\tAmount to move flying objects\n");
	printf("-flyanimdelay INTEGER\n");
	printf("\tDelay for animation of flying objects\n\n");

//tree
	printf("TREES\n");
	printf("-tree INTEGER\n");
	printf("\tThe number of the tree to use ( 0=no tree )\n");
	printf("\tIf no trees are shown star/treelights/tinsel are also disabled\n");
	printf("-treex INTEGER\n");
	printf("\tAbsolute X position of tree\n");
	printf("-treey INTEGER\n");
	printf("\tAbsolute Y position of tree\n");
	printf("\tYou can also use the terms left/centre/right with treex to position the tree\n");
	printf("\tYou can also use the terms top/centre/bottom with treey to position the tree\n");
//star
	printf("-showstar/-no-showstar\n");
	printf("\tShow star\n");
	printf("-stardelay INTEGER\n");
	printf("\tDelay for star\n");
//tinsel
	printf("-showtinsel/-no-showtinsel\n");
	printf("\tShow tinsel\n");
//treelamps
	printf("-treelampset INTEGER\n");
	printf("\tLampset to use on tree\n");
	printf("-treelampdelay INTEGER\n");
	printf("\tTree lamps delay\n\n");

//figure
	printf("FIGURE\n");
	printf("-figure INTEGER\n");
	printf("\tThe number of figure to use ( 0=no figure )\n");
	printf("-figurex INTEGER\n");
	printf("\tAbsolute X position of figure\n");
	printf("-figurey INTEGER\n");
	printf("\tAbsolute Y position of figure\n");
	printf("\tYou can also use the terms left/centre/right with figurex to position the figure\n");
	printf("\tYou can also use the terms top/centre/bottom with figurey to position the figure\n");
	printf("-figuredelay INTEGER\n");
	printf("\tDelay for figure\n\n");

//falling
	printf("FALLING OBJECTS\n");
	printf("-falling INTEGER\n");
	printf("\tThe set number of the falling objects to use ( 0=no falling objects )\n");
	printf("-falldelay INTEGER\n");
	printf("\tDelay for falling objects\n");
	printf("-maxfalling INTEGER\n");
	printf("\tMaximum number of falling objects INTEGER<%i\n",MAXFALLINGOBJECTS);
	printf("-fallingspread INTEGER\n");
	printf("\tRandom deleay between new falling objects appearing\n");
	printf("-fallingspeed INTEGER\n");
	printf("\tDistance falling objects move in Y direction in one go\n");
	printf("-minfallspeed INTEGER\n");
	printf("\tMin distance falling objects move in Y direction in one go ( MUST be less than fallingspeed )\n");
	printf("-maxxstep INTEGER\n");
	printf("\tFalling objects max X step\n");
	printf("-fallinanimdelay INTEGER\n");
	printf("\tDelay for animation of falling objects\n\n");

//wind
	printf("WIND\n");
	printf("-wind INTEGER\n");
	printf("\tWind speed/direction ( +INTEGER wind>>> -INTEGER wind<<< 0 no wind )\n");
//gusts
	printf("-usegusts/-no-usegusts\n");
	printf("\tUse/don't use gusts of wind\n");
	printf("-gustlen INTEGER\n");
	printf("\tDuration of gusts of wind\n");
	printf("-gustdelay INTEGER\n");
	printf("\tRandom delay between gusts of wind\n");
	printf("-gustspeed INTEGER\n");
	printf("\tSpeed of gusts of wind\n\n");

//settling
	printf("-settleheight INTEGER\n");
	printf("\tHeight of settled snow etc, 0 = Don't let snow settle\n");
	printf("-settlerate INTEGER\n");
	printf("\tRate at which settling occurs - lower the number slower but better\n");
	printf("\tSettled snow etc is swept clean at the end of a gust\n");
	printf("\tYou should set maxxstep > 1 for a more relaistic settle after being swept clean\n");
	printf("-clearonmaxheight/-no-clearonmaxheight\n");
	printf("\tClear the snow etc when it reaches the maximum height set by settleheight\n");

	printf("\n");
	exit(0);
}

void setDefaults(void)
{
	prefix=strdup("Xmas");
	treeNumber=0;
	figureNumber=1;
	showFlyers=false;
	fallingNumber=1;
	fallingSpread=2000;
	lampSpeed=20;
	gustEvent=1500;
	gustDuration=100;
	gustSpeed=60;
	gustOffSet=0;
	lampsNeedsUpdate=true;
	lampSection=0;
	lampAnim=LAMPFLASH;
	lastLampAnim=LAMPFLASH;
	lampCycleDelay=30;
	lampCountdown=lampCycleDelay;
	mainDelay=25000;
	fallSpeed=1;
	minFallSpeed=1;
	flyerNumber=0;
	numberOfFlyers=20;
	maxBottomHeight=100;
	settleRate=2;
	clearOnMaxHeight=true;
}

void reloadConfig(void)
{
	int numflyobjects;
	int flysetnumber=flyerNumber;
	int numfallobjects=numberOfFalling;
	int fallsetnumber=fallingNumber;
	int	tmptree=treeNumber;
	int	tmptreelampset=treeLampSet;
	int	tmpfig=figureNumber;
	int tmplamps=lampSet;
	char*	tmptheme;
	bool	force=false;

	tmptheme=strdup(prefix);
	numflyobjects=abs(numberOfFlyers);

	loadVarsFromFile(configFilePath,xdecorations_rc);

	if(strcmp(tmptheme,prefix)!=0)
		force=true;

	if((numflyobjects<abs(numberOfFlyers)) || (flysetnumber!=flyerNumber) || (force==true))
		{
			destroyFlyers();
			initFlyers();
		}

	if((numfallobjects<numberOfFalling) || (fallsetnumber!=fallingNumber) || (force==true))
		{
			destroyFalling();
			initFalling();
		}

	if((treeNumber!=tmptree) || (tmptreelampset!=treeLampSet) || (force==true))
		{
			destroyTree();
			treeOnOff=0;
			initTree();
		}

	if((tmpfig!=figureNumber) || (force==true))
		{
			destroyFigure();
			figureOnOff=0;
			initFigure();
		}

	if((tmplamps!=lampSet) || (force==true))
		{
			destroyLamps();
			lampsOnOff=0;
			lastLampAnim=0;
			initLamps();
		}

	setGravity(&treeX,&treeY,treeWidth,treeHeight);
	setGravity(&figureX,&figureY,figureW,figureH);
	lastLampAnim=lampAnim;
	if(lampAnim==LAMPCYCLE)
		lastLampAnim=LAMPFLASH;
	free(tmptheme);
	numberOfFlyers=abs(numberOfFlyers);
}

int main(int argc,char* argv[])
{
	int		argnum;
	const	char* argstr;
	XEvent	ev;
	Window	parentWindow;
	int		rc=0;
	Hints	hints;
	Atom	xa;
	Atom	xa_prop[10];
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC,&now);
	srandom(now.tv_nsec);

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);

	setDefaults();

	if((argc>1) && (strcmp(argv[1],"-noconfig")!=0))
		{
			prefix=strdup("Xmas");
			asprintf(&configFilePath,"%s/.config/xdecorations.rc",getenv("HOME"));
			loadVarsFromFile(configFilePath,xdecorations_rc);
		}

//command line options.
	for (argnum=1; argnum<argc; argnum++)
		{
			argstr=argv[argnum];


//app
			if(strcmp(argstr,"-configfile")==0)//~/.config/xdecorations.rc
				{
					free(configFilePath);
					asprintf(&configFilePath,"%s",argv[++argnum]);
					loadVarsFromFile(configFilePath,xdecorations_rc);
				}

			if(strcmp(argstr,"-writeconfig")==0)//figureNumber=1
				{
					saveVarsToFile(argv[++argnum],xdecorations_rc);
					return(0);
				}

			showUnShow(argstr,"usewindow",&useWindow);//use/don't use window
			showUnShow(argstr,"usebuffer",&useDBOveride);//use/don'tdouble buffering
			showUnShow(argstr,"watchconfig",&watchConfig);//watch config file for changes
			showUnShow(argstr,"clearonmaxheight",&clearOnMaxHeight);//clear settletled snow when reaches max hite

			if(strcmp(argstr,"-theme")==0)//Xmas
				{
					free(prefix);
					prefix=strdup(argv[++argnum]);
				}

			if(strcmp(argstr,"-delay")==0)//mainDelay=20000
				mainDelay=atol(argv[++argnum]);

			if(strcmp(argstr,"-offsety")==0)//offSetY=0
				offSetY=atol(argv[++argnum]);

//lamps
			if(strcmp(argstr,"-lampset")==0)//lampSet=1
				lampSet=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampy")==0)//lampY=16
				lampY=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampdelay")==0)//lampSpeed=100
				lampSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampflash")==0)//lampAnim=LAMPFLASH
				lampAnim=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampcycledelay")==0)//lampCycleDelay=30
				lampCycleDelay=atol(argv[++argnum]);

//flyers
			if(strcmp(argstr,"-flyer")==0)//flyersMaxY=400
				flyerNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxflyers")==0)//flyersMaxY=400
				numberOfFlyers=atol(argv[++argnum]);
			if(strcmp(argstr,"-flyermaxy")==0)//flyersMaxY=400
				flyersMaxY=atol(argv[++argnum]);
			if(strcmp(argstr,"-spread")==0)//flyerSpread=500
				flyerSpread=atol(argv[++argnum]);
			if(strcmp(argstr,"-flydelay")==0)//flyersSpeed=1
				flyersSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-flystep")==0)//flyersStep=8
				flyersStep=atol(argv[++argnum]);
			if(strcmp(argstr,"-flyanimdelay")==0)//flyersStep=8
				flyerAnimSpeed=atol(argv[++argnum]);

//tree
			if(strcmp(argstr,"-tree")==0)//treeNumber=1
				treeNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-treex")==0)//treeX=100
				treeX=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-treey")==0)//treeY=100
				treeY=translateGravity(argv[++argnum]);

			showUnShow(argstr,"showstar",&showStar);//showStar=false
			if(strcmp(argstr,"-stardelay")==0)//starSpeed
				starSpeed=atol(argv[++argnum]);

			showUnShow(argstr,"showtinsel",&showTinsel);//showTinsel=false

			if(strcmp(argstr,"-treelampset")==0)//treeLampSet=1
				treeLampSet=atol(argv[++argnum]);
			if(strcmp(argstr,"-treelampdelay")==0)//treelampSpeed=100
				treelampSpeed=atol(argv[++argnum]);

//figure
			if(strcmp(argstr,"-figure")==0)//figure=1
				figureNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-figurex")==0)//figureX=100
				figureX=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-figurey")==0)//figureY=100
				figureY=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-figuredelay")==0)//figureSpeed=100
				figureSpeed=atol(argv[++argnum]);

//falling
			if(strcmp(argstr,"-falling")==0)//falling set=1
				fallingNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-falldelay")==0)//falling speed=10
				fallingDelay=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxfalling")==0)//numberOfFalling=100
				numberOfFalling=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallingspread")==0)//leaf spread=400
				fallingSpread=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallingspeed")==0)//leaf spread=400
				fallSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-minfallspeed")==0)//max xstep =4
				minFallSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxxstep")==0)//max xstep =4
				maxXStep=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallinanimdelay")==0)//max xstep =4
				fallingAnimSpeed=atol(argv[++argnum]);

//wind
			if(strcmp(argstr,"-wind")==0)//windspeed=4
				windSpeed=atol(argv[++argnum]);

			showUnShow(argstr,"usegusts",&useGusts);//use/don't use gusts of wind
			if(strcmp(argstr,"-gustlen")==0)//gustDuration=10
				gustDuration=atol(argv[++argnum]);
			if(strcmp(argstr,"-gustdelay")==0)//gustEvent=2000
				gustEvent=atol(argv[++argnum]);
			if(strcmp(argstr,"-gustspeed")==0)//gustSpeed=40
				gustSpeed=atol(argv[++argnum]);

//settling
			if(strcmp(argstr,"-settleheight")==0)//maxBottomHeight=100
				maxBottomHeight=atol(argv[++argnum]);
			if(strcmp(argstr,"-settlerate")==0)//settleRate=2
				settleRate=atol(argv[++argnum]);

//print help
			if(strcmp(argstr,"-help")==0)
				doHelp();
		}

	srand((int)time((long* )NULL));

	signal(SIGKILL,(sighandler_t)&signalHandler);
	signal(SIGTERM,(sighandler_t)signalHandler);
	signal(SIGQUIT,(sighandler_t)signalHandler);
	signal(SIGINT,(sighandler_t)signalHandler);
	signal(SIGHUP,(sighandler_t)signalHandler);

	if(useWindow==false)
		{
			rootWin=ToonGetRootWindow(display,screen,&parentWindow);
			visual=DefaultVisual(display,screen);
			useBuffer=false;
			drawOnThis=rootWin;
		}
	else
		{
			rc=get_argb_visual(&visual,&depth);
			if(rc==0)
				{
					XSetWindowAttributes attr;
					attr.colormap=XCreateColormap(display,DefaultRootWindow(display),visual,AllocNone);
					attr.border_pixel=0;
					attr.background_pixel=0;

					rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWColormap | CWBorderPixel | CWBackPixel,&attr);
					XSelectInput(display,rootWin,StructureNotifyMask);

					xa=XInternAtom(display,"_NET_WM_STATE",False);
					xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
					xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
					xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
					xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
					xa_prop[4]=XInternAtom(display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
					xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);

					xa=XInternAtom(display,"_NET_WM_STATE",False);
					if(xa!=None)
						XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,5);

					hints.flags=2;
					hints.decorations=0;
					XChangeProperty(display,rootWin,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

					rg=XCreateRegion();
					XMapWindow(display,rootWin);
					XSync(display, False);

					XMoveWindow(display,rootWin,0,0);
					XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);

					buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
					swapInfo.swap_window = rootWin;
					swapInfo.swap_action = XdbeBackground;
					if((XdbeSwapBuffers(display,&swapInfo,1)) && (useDBOveride==true))
						{
							useBuffer=true;
							drawOnThis=buffer;
						}
					else
						{
							printf("no double buffering\n");
							useBuffer=false;
							drawOnThis=rootWin;
						}
				}
			else
				{
					rootWin=ToonGetRootWindow(display,screen,&parentWindow);
					visual=DefaultVisual(display,screen);
					printf("Can't get ARGB, do you have a composite manager running\n");
					drawOnThis=rootWin;
					useBuffer=false;
				}
		}

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	initBottomSnow();
	initWindowSnow();
	initLamps();
	initTree();
	initFigure();
	initFlyers();
	initFalling();

	clearBottomSnow();

	getOpenwindows();

	while (!done)
		{
			while (XPending(display))
				XNextEvent(display,&ev);

			switch(ev.type)
				{
				case ClientMessage:
					if (ev.xclient.message_type == XInternAtom(display,"WM_PROTOCOLS",1) && (Atom)ev.xclient.data.l[0] == XInternAtom(display,"WM_DELETE_WINDOW",1))
						done=1;
					continue;

					break;
				}

			usleep(mainDelay);
			runCounter++;

			getOpenwindows();

			if(lampSet!=0)
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

			if(figureNumber!=0)
				{
					if((runCounter % figureSpeed)==0)
						figureNeedsUpdate=true;
				}

			if(showFlyers==true)
				{
					if((runCounter % flyersSpeed)==0)
						flyerNeedsUpdate=true;
				}

			if(showFalling==true)
				{
					if((runCounter % fallingDelay)==0)
						fallingNeedsUpdate=true;
				}

			if(useGusts==true)
				{
					updateGusts();
				}

			eraseRects();
			drawTreeLamps();
			drawFigure();
			drawFlyers();
			drawLamps();
			drawSettled();
			drawFalling();

			if(useBuffer==true)
				XdbeSwapBuffers(display,&swapInfo,1);

			if(watchConfig==true)
				reloadConfig();

		}
	if(useWindow==false)
		XClearWindow(display,rootWin);
	XCloseDisplay(display);

	destroyFalling();
	destroyFlyers();
	free(bottomSnow.lasty);
	return(0);
}





