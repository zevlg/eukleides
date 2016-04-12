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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symbol.h"
#include "error.h"
#include "utils.h"

double Sin(double x)
{
    return sin(DTOR(x));
}

double Cos(double x)
{
    return cos(DTOR(x));
}

double Tan(double x)
{
    return tan(DTOR(x));
}

double Asin(double x)
{
    return RTOD(asin(x));
}

double Acos(double x)
{
    return RTOD(acos(x));
}

double Atan(double x)
{
    return RTOD(atan(x));
}

double det2(double a, double b,
	    double c, double d)
{
    return a*d - b*c;
}

double det3(double a, double b, double c,
	    double d, double e, double f,
	    double g, double h, double i)
{
    return a*det2(e, f, h, i) - d*det2(b, c, h, i) + g*det2(b, c, e, f);
}

double Clamp(double x, double y, double z)
{
    return x > z ? z : x < y ? y : x;
}

double distance(_point *A, _point *B)
{
    return hypot(B->x-A->x, B->y-A->y);
}

double argument(_point *A, _point *B)
{
    return RTOD(atan2(B->y-A->y, B->x-A->x));
}

double principal(double x)
{
    return x - 360*ceil(x/360-.5);
}

_point *get_point(_set **set)
{
    _point *val;
    if (*set == NULL) runtime_error(_("invalid set"));
    val = (*set)->p;
    (*set) = (*set)->next;
    return val;
}
