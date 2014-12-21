/******************************************************
*
*     ©keithhedger Wed 17 Dec 11:17:18 GMT 2014
*     kdhedger68713@gmail.com
*
*     update.cpp
*
******************************************************/

#include "globals.h"
#include "routines.h"

void clearBottomSnow(void)
{
	if(maxBottomHeight==0)
		return;

	for(int j=0; j<displayWidth; j++)
		bottomSnow.lasty[j]=maxBottomHeight;

	XSetClipMask(display,bottomSnow.maskgc,0);
	XSetClipOrigin(display,bottomSnow.maskgc,0,0);

	XSetForeground(display,bottomSnow.maskgc,blackColor);
	XSetFillStyle(display,bottomSnow.maskgc,FillSolid);
	XFillRectangle(display,bottomSnow.mask,bottomSnow.maskgc,0,0,displayWidth,bottomSnow.maxHeight);

	XSetClipMask(display,gc,0);
	XSetClipOrigin(display,gc,0,0);

	XSetForeground(display,gc,blackColor);
	XSetFillStyle(display,gc,FillSolid);
	XFillRectangle(display,bottomSnow.pixmap,gc,0,0,displayWidth,bottomSnow.maxHeight);
	bottomSnow.keepSettling=true;
}

void clearWindowSnow(int winid,bool newrsrc)
{
	if(newrsrc==true)
		{
			if(windowSnow[winid].lasty!=NULL)
				free(windowSnow[winid].lasty);
			if(windowSnow[winid].pixmap!=0)
				XFreePixmap(display,windowSnow[winid].pixmap);
			if(windowSnow[winid].mask!=0)
				XFreePixmap(display,windowSnow[winid].mask);
			if(windowSnow[winid].maskgc!=0)
				XFreeGC(display,windowSnow[winid].maskgc);

			windowSnow[winid].pixmap=XCreatePixmap(display,drawOnThis,windowSnow[winid].width,windowSnow[winid].maxHeight,depth);
			windowSnow[winid].mask=XCreatePixmap(display,drawOnThis,windowSnow[winid].width,windowSnow[winid].maxHeight,1);
			windowSnow[winid].maskgc=XCreateGC(display,windowSnow[winid].mask,0,NULL);

			windowSnow[winid].lasty=(int*)malloc(sizeof(int)*windowSnow[winid].width);
		}

	for(int j=0; j<windowSnow[winid].width; j++)
		windowSnow[winid].lasty[j]=windowSnow[winid].maxHeight;

	if(windowSnow[winid].mask!=0)
		{
			XSetClipMask(display,windowSnow[winid].maskgc,0);
			XSetClipOrigin(display,windowSnow[winid].maskgc,0,0);

			//XSetForeground(display,windowSnow[winid].maskgc,blackColor);
			XSetForeground(display,windowSnow[winid].maskgc,whiteColor);
			XSetFillStyle(display,windowSnow[winid].maskgc,FillSolid);
			XFillRectangle(display,windowSnow[winid].mask,windowSnow[winid].maskgc,0,0,windowSnow[winid].width,windowSnow[winid].maxHeight);

			XSetClipMask(display,gc,0);
			XSetClipOrigin(display,gc,0,0);

			XSetForeground(display,gc,blackColor);
			//XSetForeground(display,gc,whiteColor);
			XSetFillStyle(display,gc,FillSolid);
			XFillRectangle(display,windowSnow[winid].pixmap,gc,0,0,windowSnow[winid].width,windowSnow[winid].maxHeight);
			windowSnow[winid].keepSettling=true;
		}
}

void clearAllWindowSnow(bool newrsrc)
{
	for(int j=0;j<MAXWINDOWS;j++)
		clearWindowSnow(j,false);
}

void doGusts(void)
{
	if(doingGusts==false)
		{
			gustOffSet=0;
			return;
		}

	if(gustPortion==1)
		{
			gustInc=gustInc*incr;
			if(realGustSpeed<gustSpeed)
				realGustSpeed=realGustSpeed+gustInc;
			else
				{
					gustPortion=2;
					realGustSpeed=gustSpeed;
				}
			gustOffSet=realGustSpeed;
		}

	if(gustPortion==2)
		{
			gustCountdown--;
			realGustSpeed=gustSpeed;
			gustOffSet=realGustSpeed;
			if(gustCountdown<=0)
				{
					gustPortion=3;
					gustCountdown=0;
				}
		}

	if(gustPortion==3)
		{
			gustInc=gustInc/incr;
			if(gustInc<1)
				gustInc=1;
			if(realGustSpeed>0)
				realGustSpeed=realGustSpeed-(int)gustInc;
			else
				{
					gustPortion=1;
					gustCountdown=0;
					realGustSpeed=0;
					doingGusts=false;
					gustOffSet=0;
				}
			gustOffSet=realGustSpeed;
			clearBottomSnow();
			clearAllWindowSnow(false);
		}
}


void updateGusts(void)
{
	if(doingGusts==false)
		{
			if((randomEvent(gustEvent)==true) && (gustPortion==1))
				{
					gustCountdown=gustDuration;
					gustDirection=randomDirection();
					realGustSpeed=1;
					gustPortion=1;
					gustInc=1.0;
					doingGusts=true;
				}
		}
	else
		doGusts();
}

void updateWindowSnow(movement *mov,int windownum)
{
	int		downx;
	Pixmap	*pmm;

	if(maxWindowHeight==0)
		return;

	if((windowSnow[windownum].showing==true) && (windowSnow[windownum].keepSettling==true))
		{
			downx=mov->x;
			if((downx>windowSnow[windownum].x) && (downx<(windowSnow[windownum].width+windowSnow[windownum].x)))
				{
					pmm=mov->object->mask[0];
					downx=mov->x-windowSnow[windownum].x;

					if(windowSnow[windownum].lasty[downx]-settleRate<=0)
						{
							if(clearOnMaxHeight==true)
								clearWindowSnow(windownum,false);
							else
								windowSnow[windownum].lasty[downx]=windowSnow[windownum].maxHeight;
						}

					windowSnow[windownum].lasty[downx]=windowSnow[windownum].lasty[downx]-settleRate;
					if(windowSnow[windownum].lasty[downx]<=0)
						windowSnow[windownum].keepSettling=false;

					XSetClipMask(display,gc,*(pmm));
					XSetClipOrigin(display,gc,downx,windowSnow[windownum].lasty[downx]);
					XCopyArea(display,*(mov->object->pixmap[0]),windowSnow[windownum].pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx,windowSnow[windownum].lasty[downx]);
				}
		}
}

bool checkOnWindow(movement *mov)
{
	if(maxWindowHeight==0)
		return(false);

	for(int j=0; j<MAXWINDOWS; j++)
		{
			if((windowSnow[j].wid>0) && (windowSnow[j].showing==true))
				{
					if((mov->x>windowSnow[j].x) && (mov->x<(windowSnow[j].width+windowSnow[j].x)))
						{
							if((mov->y<=windowSnow[j].y) && (mov->y>=(windowSnow[j].y-mov->stepY)))
								{
									updateWindowSnow(mov,j);
									return(true);
								}
						}
				}
		}
	return(false);
}

void updateBottomSnow(movement *mov)
{
	int		downx;
	Pixmap	*pmm;

	if(maxBottomHeight==0)
		return;

	if(bottomSnow.keepSettling==true)
		{
			downx=mov->x;
			if((downx>=0) && (downx<displayWidth) && (maxBottomHeight>0))
				{
					pmm=mov->object->mask[0];

					if(bottomSnow.lasty[downx]-settleRate<=0)
						{
							if(clearOnMaxHeight==true)
								clearBottomSnow();
							else
								bottomSnow.lasty[downx]=maxBottomHeight;
						}
					bottomSnow.lasty[downx]=bottomSnow.lasty[downx]-settleRate;
					if(bottomSnow.lasty[downx]<=0)
						bottomSnow.keepSettling=false;

					XSetClipMask(display,bottomSnow.maskgc,*(pmm));
					XSetClipOrigin(display,bottomSnow.maskgc,downx,bottomSnow.lasty[downx]);
					XCopyArea(display,*(mov->object->mask[0]),bottomSnow.mask,bottomSnow.maskgc,0,0,mov->object->w[0],mov->object->h[0],downx,bottomSnow.lasty[downx]);

					XSetClipMask(display,gc,*(pmm));
					XSetClipOrigin(display,gc,downx,bottomSnow.lasty[downx]);
					XCopyArea(display,*(mov->object->pixmap[0]),bottomSnow.pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx,bottomSnow.lasty[downx]);
				}
		}
}

void updateFalling(void)
{
	for(int j=0; j<numberOfFalling; j++)
		{
			if(moving[j].use==true)
				{
					swirlingDirection=randInt(MAXSWIRL);
					if(randomEvent(2)==true)
						swirlingDirection=-swirlingDirection;
					moving[j].stepX=moving[j].stepX+swirlingDirection;
					if(moving[j].stepX>maxXStep)
						moving[j].stepX=maxXStep;
					if(moving[j].stepX<-maxXStep)
						moving[j].stepX=-maxXStep;

					moving[j].y=moving[j].y+moving[j].stepY;
					moving[j].x=moving[j].x+moving[j].stepX+windSpeed+(gustOffSet*gustDirection);

					if(checkOnWindow(&moving[j])==false)
						{
							if(moving[j].y>displayHeight+moving[j].object->h[0])
								{
									moving[j].use=false;
									moving[j].y=0-moving[j].object->h[0];
									updateBottomSnow(&moving[j]);
								}
						}
					else
						{
							moving[j].use=false;
							moving[j].y=0-moving[j].object->h[0];
						}

					if(moving[j].x>displayWidth+moving[j].object->w[0])
						moving[j].x=0-moving[j].object->w[0];

					if(moving[j].x<0-moving[j].object->w[0])
						moving[j].x=displayWidth;
				}
			else
				{
					if(randomEvent(fallingSpread)==true)
						{
							moving[j].use=true;
							moving[j].stepY=randInt(fallSpeed-minFallSpeed+1)+minFallSpeed;
							moving[j].x=(rand() % displayWidth);
							moving[j].imageNum=randInt(moving[j].object->anims);
							moving[j].countDown=fallingAnimSpeed;
							moving[j].direction=randomEvent(2);
						}
				}
		}
}

void updateLamps(void)
{
	lampsOnOff=(lampsOnOff+1) & 1;
	lampsNeedsUpdate=true;

	if(lampCountdown<1)
		{
			if((lampAnim==LAMPCYCLE) && (lampsOnOff==1))
				{
					lastLampAnim++;

					if(lastLampAnim==LAMPCYCLE)
						lastLampAnim=LAMPFLASH;

					lampCountdown=lampCycleDelay;
				}
		}
	else
		lampCountdown--;
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
	int	j=0;

	for(j=0; j<numberOfFlyers; j++)
		{
			if(flyersMove[j].use==true)
				{
					flyersMove[j].x+=flyersStep+((gustOffSet*gustDirection)/4);

					if(flyersMove[j].x>displayWidth+flyersMove[j].object->w[0])
						{
							flyersMove[j].y=(rand() % flyersMaxY);
							flyersMove[j].x=0-flyersMove[j].object->w[0];
							flyersMove[j].use=false;
						}
				}
			else
				{
					if(randomEvent(flyerSpread)==true)
						flyersMove[j].use=true;
				}
		}
}

void eraseRects(void)
{
	int	j;

	if((figureNumber!=0) && (figureNeedsUpdate==true))
		{
			if(useBuffer==false)
				XClearArea(display,drawOnThis,figureX,figureY,figureW,figureH,False);
			updateFigure();
		}

	if((showTree==true) && (treeNeedsUpdate==true))
		{
			if(useBuffer==false)
				XClearArea(display,drawOnThis,treeX,treeY,treeWidth,treeHeight,False);
			updateTreeLamps();
		}

	if((showFlyers==true) && (flyerNeedsUpdate==true))
		{
			if(useBuffer==false)
				{
					for(j=0; j<flyerCount; j++)
						XClearArea(display,drawOnThis,flyersMove[j].x,flyersMove[j].y,flyersMove[j].maxW,flyersMove[j].maxH,False);
				}
			updateFlyers();
		}

	if((showFalling==true) && (fallingNeedsUpdate==true))
		{
			if(useBuffer==false)
				{
					for(int j=0; j<numberOfFalling; j++)
						XClearArea(display,drawOnThis,moving[j].x,moving[j].y,moving[j].object->w[0],moving[j].object->h[0],False);
				}
			updateFalling();
		}

	treeNeedsUpdate=false;
	figureNeedsUpdate=false;
	flyerNeedsUpdate=false;
	fallingNeedsUpdate=false;
}

bool checkForWindowChange(Window wid,XWindowAttributes *attr)
{
	Window	rootWindow=RootWindow(display,screen);
	int		screen_x,screen_y;
	bool	retval=false;
	bool	newwindow=true;
	int		newwinid=-1;
	Window	dummy;

	if(maxWindowHeight==0)
		return(false);

	for(int j=0; j<MAXWINDOWS; j++)
		{
			if(windowSnow[j].wid==wid)
				{
					XTranslateCoordinates(display,rootWindow,wid,attr->x,attr->y,&screen_x,&screen_y,&dummy);
					screen_x=abs(screen_x);
					screen_y=abs(screen_y);

					if((windowSnow[j].x!=screen_x) || (windowSnow[j].y!=screen_y) || (windowSnow[j].width!=attr->width))
						{
							windowSnow[j].x=screen_x;
							windowSnow[j].y=screen_y;
							if(windowSnow[j].width!=attr->width)
								{
									windowSnow[j].width=attr->width;
									clearWindowSnow(j,true);
									return(true);
								}
							return(false);
						}
				}

			if(windowSnow[j].wid==wid)
				{
					newwindow=false;
					if(attr->map_state!=IsViewable)
						windowSnow[j].showing=false;
					else
						windowSnow[j].showing=true;
					break;
				}
		}

	for(int j=0; j<MAXWINDOWS; j++)
		{
			if(windowSnow[j].wid==0)
				{
					newwinid=j;
					break;
				}
		}

	if (newwinid==-1)
		{
			printf("NO MORE WINDOWS\n");
			return(false);
		}

	if(newwindow==true)
		{
			windowSnow[newwinid].keepSettling=true;
			windowSnow[newwinid].maxHeight=maxWindowHeight;
			windowSnow[newwinid].lasty=NULL;
			windowSnow[newwinid].wid=wid;
			windowSnow[newwinid].width=attr->width;
			XTranslateCoordinates(display,rootWindow,wid,attr->x,attr->y,&screen_x,&screen_y,&dummy);
			windowSnow[newwinid].x=abs(screen_x);
			windowSnow[newwinid].y=abs(screen_y);
			windowSnow[newwinid].showing=true;
			clearWindowSnow(newwinid,true);
			retval=true;
		}

	return(retval);
}

void getOpenwindows(void)
{
	Window				rootWindow;
	Atom				actualType;
	int					format;
	unsigned long		numItems,bytesAfter;
	XWindowAttributes	attr;
	long				*array;
	Window				w;
	unsigned char *data;
	Atom *atoms;
	int status, real_format;
	Atom real_type;
	unsigned long items_read, items_left, i;

	Atom _NET_WM_WINDOW_TYPE=XInternAtom(display,"_NET_WM_WINDOW_TYPE",False);
	Atom _NET_WM_WINDOW_TYPE_NORMAL=XInternAtom(display,"_NET_WM_WINDOW_TYPE_NORMAL",False);

	XGrabServer(display);
	skipErrors(true);
	rootWindow=RootWindow(display,screen);
	Atom _NET_CLIENT_LIST=XInternAtom(display,"_NET_CLIENT_LIST" , true);
	status=XGetWindowProperty(display,rootWindow,_NET_CLIENT_LIST,0L,(~0L),false,AnyPropertyType,&actualType,&format,&numItems,&bytesAfter,&data);

	if((status==Success) && (numItems>0))
		{
			array=(long*)data;
			for(unsigned long g=0;g<numItems;g++)
				{
					for(int j=0;j<MAXWINDOWS;j++)
						windowSnow[j].showing=false;

					for(int j=0; j<MAXWINDOWS; j++)
						if(windowSnow[j].wid==array[g])
							windowSnow[j].showing=true;
				}

			for(unsigned long k=0; k<numItems; k++)
				{
					w=(Window)array[k];

					status=XGetWindowProperty(display,w,_NET_WM_WINDOW_TYPE,0L,1L,False,XA_ATOM,&real_type,&real_format,&items_read,&items_left,&data);
					atoms=(Atom *)data;
					for(i=0;i<items_read;i++)
						{
							if(atoms[i]==_NET_WM_WINDOW_TYPE_NORMAL)
								{
									skipErrors(true);
									XGetWindowAttributes(display,w,&attr);
									checkForWindowChange(w,&attr);
									skipErrors(false);
								}
						}
				}
			XFree(data);
		}
	skipErrors(true);
	XUngrabServer(display);
}

