/*
 *  Eukleides version 1.5.4
 *  Copyright (c) Christian Obrecht 2004-2010
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "error.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"

int strokes[26][2];

void store_mobile(_param p)
{
    double step, min_x = 0, min_y = 0, max_x = 0, max_y = 0, x, y;
    int mode, i;
    _point *O, *val;
    char *c;
    
    O = POP(_point);
    mode = pop_param().addr;
    if (mode >= 4) {
	max_y = POPn;
	max_x = POPn;
	min_y = POPn;
	min_x = POPn;
    }
    if (mode % 2 == 1) step = POPn;
    else step = .1;
    if (mode % 4 < 2) {
	c = ((_symbol *)p.ptr)->symbol;
	if (*c < 'A' || *c > 'Z' || c[1] != '\0')
	    runtime_error(_("invalid state"));
	i = (int)(*c - 'A');
    }
    else i = pop_param().addr;
    x = O->x + step*strokes[i][0]; 
    y = O->y + step*strokes[i][1]; 
    if (mode >= 4) {
	if (min_x > max_x || min_y > max_y)
	    runtime_error(_("invalid boundaries"));
	x = Clamp(x, min_x, max_x);
	y = Clamp(y, min_y, max_y);
    }	
    get_mem(val, _point);
    val->x = x;
    val->y = y;
    ((_symbol *)p.ptr)->content->value.point = val;
}

void store_interactive(_param p)
{
    double n, step = 0.1, min = 0, max = 0;
    int dir, i, mode;

    dir = pop_param().addr;
    n = POPn;
    mode = pop_param().addr;
    switch (mode) {
	case 3: step = POPn;
	case 2: max = POPn;
		min = POPn;
		if (min > max) runtime_error(_("invalid boundaries"));
		break;
	case 1: step = POPn;
    }
    i = pop_param().addr;
    n += step*strokes[i][dir];
    if (mode > 1) n = Clamp(n, min, max);
    ((_symbol *)p.ptr)->content->value.number = n;
}
