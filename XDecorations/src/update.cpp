/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:45:35 BST 2018 keithdhedger@gmail.com

 * This file (update.cpp) is part of XDecorations.

 * XDecorations is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * XDecorations is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with XDecorations.  If not, see <http://www.gnu.org/licenses/>.
 */

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

			XSetForeground(display,windowSnow[winid].maskgc,blackColor);
			XSetFillStyle(display,windowSnow[winid].maskgc,FillSolid);
			XFillRectangle(display,windowSnow[winid].mask,windowSnow[winid].maskgc,0,0,windowSnow[winid].width,windowSnow[winid].maxHeight);

			XSetClipMask(display,gc,0);
			XSetClipOrigin(display,gc,0,0);

			XSetForeground(display,gc,blackColor);
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
	int		imgnumber=mov->imageNum;

	if(maxWindowHeight==0)
		return;

	if((windowSnow[windownum].showing==true) && (windowSnow[windownum].keepSettling==true))
		{
			downx=mov->x;
			if((downx>windowSnow[windownum].x) && (downx<(windowSnow[windownum].width+windowSnow[windownum].x-(mov->object->w[mov->imageNum]))))
				{
					pmm=mov->object->mask[imgnumber];
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

					XSetClipMask(display,windowSnow[windownum].maskgc,*(pmm));
					XSetClipOrigin(display,windowSnow[windownum].maskgc,downx,windowSnow[windownum].lasty[downx]);
					XCopyArea(display,*(mov->object->mask[imgnumber]),windowSnow[windownum].mask,windowSnow[windownum].maskgc,0,0,mov->object->w[0],mov->object->h[0],downx,windowSnow[windownum].lasty[downx]);

					XSetClipMask(display,gc,*(pmm));
					XSetClipOrigin(display,gc,downx,windowSnow[windownum].lasty[downx]);
					XCopyArea(display,*(mov->object->pixmap[imgnumber]),windowSnow[windownum].pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx,windowSnow[windownum].lasty[downx]);
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
					if((mov->x>windowSnow[j].x) && (mov->x<(windowSnow[j].width+windowSnow[j].x-mov->maxW)))
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
	int		imgnumber=mov->imageNum;

	if(maxBottomHeight==0)
		return;

	if(bottomSnow.keepSettling==true)
		{
			downx=mov->x;
			if((downx>=0) && (downx<displayWidth) && (maxBottomHeight>0))
				{
					pmm=mov->object->mask[imgnumber];

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
					XCopyArea(display,*(mov->object->mask[imgnumber]),bottomSnow.mask,bottomSnow.maskgc,0,0,mov->object->w[0],mov->object->h[0],downx,bottomSnow.lasty[downx]);

					XSetClipMask(display,gc,*(pmm));
					XSetClipOrigin(display,gc,downx,bottomSnow.lasty[downx]);
					XCopyArea(display,*(mov->object->pixmap[imgnumber]),bottomSnow.pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx,bottomSnow.lasty[downx]);
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

bool checkForWindowChange(Window wid,XWindowAttributes *attr)
{
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
					XTranslateCoordinates(display,rootWin,wid,attr->x,attr->y,&screen_x,&screen_y,&dummy);
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
			XTranslateCoordinates(display,rootWin,wid,attr->x,attr->y,&screen_x,&screen_y,&dummy);
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
	unsigned long		*array;
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

	gotWMExtents=false;
	if((status==Success) && (numItems>0))
		{
			array=(unsigned long*)data;
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
									attr.width=0;
									XGetWindowAttributes(display,w,&attr);
									if((attr.width>0) && (gotWMExtents==false))
										{
											//fprintf(stderr,"attr.wid=%i\n",attr.width);
											unsigned long itemcnt;
											unsigned long todo;
											Atom atomtype;
											int format;
											unsigned char *data=0;
											wmExtents=NULL;

											XGetWindowProperty(display,w,wmFrameExtents,0,4,false,AnyPropertyType,&atomtype,&format,&itemcnt,&todo,&data);
											wmExtents=(long*) data;
											if((wmExtents!=NULL) && (wmExtents[2]>0))
												{
													gotWMExtents=true;
													//fprintf (stderr,"Got frame extents: left %i right %i top %i bottom %i gotWMExtents=%i\n",wmExtents[0], wmExtents[1], wmExtents[2], wmExtents[3],gotWMExtents);
												}
										}
									if(gotWMExtents==true)
										{
											attr.x=attr.x-wmExtents[WMEXTLEFT];
											//attr.width=attr.width+wmExtents[WMEXTRIGHT]+wmExtents[WMEXTLEFT];
											//attr.y=attr.y+wmExtents[WMEXTTOP];
										}
											
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

