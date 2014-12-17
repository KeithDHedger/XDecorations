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

void clearWindowSnow(int winid)
{
	for(int j=0; j<windowSnow[winid].width; j++)
		windowSnow[winid].lasty[j]=maxBottomHeight;

	//XSetClipMask(display,windowSnow[winid].maskgc,0);
	XSetClipMask(display,windowSnow[winid].maskgc,0);
	XSetClipOrigin(display,windowSnow[winid].maskgc,0,0);

	//XSetForeground(display,windowSnow[winid].maskgc,blackColor);
	XSetForeground(display,windowSnow[winid].maskgc,whiteColor);
	XSetFillStyle(display,windowSnow[winid].maskgc,FillSolid);
	XFillRectangle(display,windowSnow[winid].mask,bottomSnow.maskgc,0,0,windowSnow[winid].width,windowSnow[winid].maxHeight);

	XSetClipMask(display,gc,0);
	XSetClipOrigin(display,gc,0,0);

	XSetForeground(display,gc,blackColor);
	//XSetForeground(display,gc,whiteColor);
	XSetFillStyle(display,gc,FillSolid);
	XFillRectangle(display,windowSnow[winid].pixmap,gc,0,0,windowSnow[winid].width,windowSnow[winid].maxHeight);
	bottomSnow.keepSettling=true;
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

	if(windowSnow[windownum].keepSettling==true)
		{
			downx=mov->x;
			//downx=200;
			if((downx>windowSnow[windownum].x) && (downx<(windowSnow[windownum].width+windowSnow[windownum].x)))
				{
					pmm=mov->object->mask[0];

					if(windowSnow[windownum].lasty[downx]-settleRate<=0)
						{
							//if(clearOnMaxHeight==true)
							//	clearBottomSnow();
							//else
								windowSnow[windownum].lasty[downx]=windowSnow[windownum].maxHeight;
						}
					windowSnow[windownum].lasty[downx]=windowSnow[windownum].lasty[downx]-settleRate;
					if(windowSnow[windownum].lasty[downx]<=0)
						windowSnow[windownum].keepSettling=false;

					//XSetClipMask(display,windowSnow[windownum].maskgc,*(pmm));
					//XSetClipOrigin(display,windowSnow[windownum].maskgc,downx,windowSnow[windownum].lasty[downx]);
					//XCopyArea(display,*(mov->object->mask[0]),windowSnow[windownum].mask,windowSnow[windownum].maskgc,0,0,mov->object->w[0],mov->object->h[0],downx,windowSnow[windownum].lasty[downx]);

					XSetClipMask(display,gc,*(pmm));
					//XSetClipMask(display,gc,0);
					//XSetClipOrigin(display,gc,downx,windowSnow[windownum].lasty[downx]);
					XSetClipOrigin(display,gc,downx-windowSnow[windownum].x,windowSnow[windownum].lasty[downx]);
					//XCopyArea(display,*(mov->object->pixmap[0]),windowSnow[windownum].pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx,windowSnow[windownum].lasty[downx]);
					XCopyArea(display,*(mov->object->pixmap[0]),windowSnow[windownum].pixmap,gc,0,0,mov->object->w[0],mov->object->h[0],downx-windowSnow[windownum].x,windowSnow[windownum].lasty[downx]);
					printf("w=%i h=%i x=%i y=%i\n",windowSnow[windownum].width,0,downx-windowSnow[windownum].x,windowSnow[windownum].lasty[downx]);
				}
	}
}

bool checkOnWindow(movement *mov)
{
	for(int j=0;j<MAXWINDOWS;j++)
		{
			if((mov->x>windowSnow[j].x) && (mov->x<(windowSnow[j].width-windowSnow[j].x)))
				{
					if(mov->y>=windowSnow[j].y)
						{
							//printf("on window\n");
							updateWindowSnow(mov,j);
						}
				}
//			if(windowSnow[j].wid==wid)
//				{
//					XFetchName(display,wid,&name);
//					if(name!=NULL)
//						{
//							windowSnow[j].width=attr->width;
//							printf("xid=%i name=%s changedn",(int)(long)wid,name);
//							printf("width=%in",attr->width);
//							return(false);
//						}
//				}
//			if(windowSnow[j].wid==wid)
//				{
//					newwindow=false;
//					break;
//				}
		}

	return(false);
}

void updateBottomSnow(movement *mov)
{
	int		downx;
	Pixmap	*pmm;

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
	char	*name=NULL;
	Window	dummy;

	for(int j=0;j<MAXWINDOWS;j++)
		{
			if((windowSnow[j].wid==wid) && (windowSnow[j].width!=attr->width))
				{
					XFetchName(display,wid,&name);
					if(name!=NULL)
						{
							windowSnow[j].width=attr->width;
							printf("xid=%i name=%s changed\n",(int)(long)wid,name);
							printf("width=%i\n",attr->width);
							return(false);
						}
				}
			if(windowSnow[j].wid==wid)
				{
					newwindow=false;
					break;
				}
		}
	
	for(int j=0;j<MAXWINDOWS;j++)
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
			windowSnow[newwinid].pixmap=XCreatePixmap(display,drawOnThis,attr->width,maxBottomHeight,depth);
			windowSnow[newwinid].mask=XCreatePixmap(display,drawOnThis,attr->width,maxBottomHeight,1);
			windowSnow[newwinid].maskgc=XCreateGC(display,windowSnow[newwinid].mask,0,NULL);
			windowSnow[newwinid].keepSettling=true;
			windowSnow[newwinid].maxHeight=maxBottomHeight;
			windowSnow[newwinid].lasty=(int*)malloc(sizeof(int)*attr->width);
			windowSnow[newwinid].wid=wid;
			windowSnow[newwinid].width=attr->width;
			XTranslateCoordinates(display,wid,rootWindow,attr->x,attr->y,&screen_x,&screen_y,&dummy);
			windowSnow[newwinid].x=screen_x;
			windowSnow[newwinid].y=screen_y;
			clearWindowSnow(newwinid);
			retval=true;
			XFetchName(display,wid,&name);
			if(name!=NULL)
				{
					printf("xid=%i name=%s added\n",(int)(long)wid,name);
					printf("width=%i x=%i y=%i maxhite=%i\n",attr->width,windowSnow[newwinid].x,windowSnow[newwinid].y,windowSnow[newwinid].maxHeight);
				}
		}

	return(retval);	
}

void getOpenwindows(void)
{
	Window				rootWindow;
	Atom				windowlist;
	Atom				stateatom;
	Atom				type;
	Atom				actualType;
	int					format;
	unsigned long		numItems,bytesAfter;
	unsigned char		*data;
	XWindowAttributes	attr;
	char				*name=NULL;
	long				*array;
	int					status;
	int					form;
	unsigned long		remain,len;
	unsigned char		*list;
	Window				w;
	int					screen_x,screen_y;
	Window				dummy;
	bool				windowchanged=false;

	XGrabServer(display);

	rootWindow=RootWindow(display,screen);
	windowlist=XInternAtom(display, "_NET_CLIENT_LIST" , true);
	status=XGetWindowProperty(display,rootWindow,windowlist,0L,(~0L),false,AnyPropertyType,&actualType,&format,&numItems,&bytesAfter,&data);

	if ((status==Success) && (numItems>0))
		{
			name=NULL;
			array=(long*)data;
			for(long k=0;k<numItems;k++)
				{
					w=(Window)array[k];
					XGetWindowAttributes(display,w,&attr);

					if((attr.map_state==2))
						{
							stateatom=XInternAtom(display,"_NET_WM_STATE",False);
							type=0;
							status=XGetWindowProperty(display,w,stateatom,0,(~0L),false,AnyPropertyType,&type,&form,&len,&remain,&list);
							if (status == Success)
								{
									if(len==0)
										{
											windowchanged=checkForWindowChange(w,&attr);
											if(windowchanged==true)
												{
													//XTranslateCoordinates(display,w,rootWindow,attr.x,attr.y,&screen_x,&screen_y,&dummy);
												//	XFetchName(display,w,&name);
													//if(name!=NULL)
													//	printf("xid=%i name=%s\n",(int)(long)w,name);
													//printf("x=%i y=%i w=%i h=%i\n",(int)(long)screen_x,screen_y,attr.width,attr.height);
												}
										}
								}
						}
				}
			XFree(data);
		}
	XUngrabServer(display);
}
