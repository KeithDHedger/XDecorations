/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:44:41 BST 2018 keithdhedger@gmail.com

 * This file (routines.h) is part of XDecorations.

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

#ifndef _ROUTINES_
#define _ROUTINES_

int randInt(int maxVal);
bool randomEvent(int max);
int	randomDirection(void);
void setGravity(int *x,int *y,int w,int h);
int translateGravity(char* str);
void skipErrors(bool skip);
int fileExists(const char *name);

#endif
