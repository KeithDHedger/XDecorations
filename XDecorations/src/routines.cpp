/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:44:33 BST 2018 keithdhedger@gmail.com

 * This file (routines.cpp) is part of XDecorations.

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
#include <sys/stat.h>

XErrorHandler	oldHandler=(XErrorHandler)0;

int randInt(int maxVal)
{
	return rand() % maxVal;
}

bool randomEvent(int max)
{
	if((random() % max)==0)
		return(true);

	return(false);
}

int	randomDirection(void)
{
	int	r;

	if(randomEvent(2)==true)
		r=1;
	else
		r=-1;
	return(r);
}

void setGravity(int *x,int *y,int w,int h)
{
	switch(*x)
		{
		case LEFT:
			*x=0;
			break;
		case CENTRE:
			*x=(displayWidth/2)-(w/2);
			break;
		case RIGHT:
			*x=displayWidth-w;
			break;
		default:
			break;
		}
	switch(*y)
		{
		case TOP:
			*y=0+offSetY;
			break;
		case CENTRE:
			*y=(displayHeight/2)-(h/2)+offSetY;
			break;
		case BOTTOM:
			*y=displayHeight-h+offSetY;
			break;
		default:
			break;
		}
}

int translateGravity(char* str)
{
	int retcode=0;
	if(isalpha(str[0]))
		{
			if(strcasecmp(str,"left")==0)
				retcode=LEFT;
			if(strcasecmp(str,"centre")==0)
				retcode=CENTRE;
			if(strcasecmp(str,"right")==0)
				retcode=RIGHT;
			if(strcasecmp(str,"top")==0)
				retcode=TOP;
			if(strcasecmp(str,"bottom")==0)
				retcode=BOTTOM;
		}
	else
		retcode=atol(str);
	return(retcode);
}

int justSkip(Display *display,XErrorEvent *theEvent)
{
    return 0 ;
}

void skipErrors(bool skip)
{
	XSync(display,true);
	XFlush(display);

	if(skip==true)
		oldHandler=XSetErrorHandler(justSkip);
	else
		XSetErrorHandler(oldHandler);

	XFlush(display);
	XSync(display,false);
}

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}
