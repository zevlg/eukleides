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

#include <math.h>
#include "error.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"

void add(void)
{
    PSHn(POPn + POPn);
}

void substract(void)
{
    double arg;
    arg = POPn;
    PSHn(POPn - arg);
}

void multiply(void)
{
    PSHn(POPn * POPn);
}

void divide(void)
{
    double arg;
    arg = POPn;
    if (arg == 0) runtime_error(_("division by zero"));
    PSHn(POPn / arg);
}

void modulo(void)
{
    double x, y;
    y = POPn;
    x = POPn;
    if (y == 0) runtime_error(_("division by zero"));
    PSHn(x - floor(x/y)*y);
}

void change_sign(void)
{
    PSHn(-POPn);
}

void square_root(void)
{
    double arg;
    arg = POPn;
    if (arg < 0) runtime_error(_("invalid argument in sqrt"));
    PSHn(sqrt(arg));
}

void power(void)
{
    double x, y;
    x = POPn;
    y = POPn;
    if (y<0 && x != ceil(x)) runtime_error(_("invalid exponent"));
    PSHn(pow(y,x));
}

void exponential(void)
{
    PSHn(exp(POPn));
}

void logarithm(void)
{
    double arg;
    arg = POPn;
    if (arg <= 0) runtime_error(_("invalid argument in ln"));
    PSHn(log(POPn));
}

void rtod(void)
{
    PSHn(RTOD(POPn));
}

void dtor(void)
{
    PSHn(DTOR(POPn));
}

void sine(void)
{
    PSHn(Sin(POPn));
}

void cosine(void)
{
    PSHn(Cos(POPn));
}

void tangent(void)
{
    double arg;
    arg = POPn;
    if (Cos(arg) == 0) runtime_error(_("invalid argument in tan"));
    PSHn(Tan(arg));
}

void arcsine(void)
{
    double arg;
    arg = POPn;
    if (arg<-1 || arg>1) runtime_error(_("invalid argument in asin"));
    PSHn(Asin(arg));
}

void arccosine(void)
{
    double arg;
    arg = POPn;
    if (arg<-1 || arg>1) runtime_error(_("invalid argument in acos"));
    PSHn(Acos(arg));
}

void arctangent(void)
{
    PSHn(Atan(POPn));
}

void absolute_value(void)
{
    PSHn(fabs(POPn));
}

void sign(void)
{
    double arg;
    arg = POPn;
    PSHn(arg == 0 ? 0. : arg > 0 ? 1. : -1.);
}

void ceil_value(void)
{
    PSHn(ceil(POPn));
}

void floor_value(void)
{
    PSHn(floor(POPn));
}

void round_value(void)
{
    PSHn(rint(POPn));
}

void minimum(void)
{
    double x, y;
    x = POPn;
    y = POPn;
    PSHn(x < y ? x : y);
}

void maximum(void)
{
    double x, y;
    x = POPn;
    y = POPn;
    PSHn(x > y ? x : y);
}


void clamp(void)
{
    double x, y, z;
    z = POPn;
    y = POPn;
    x = POPn;
    PSHn(Clamp(x, y, z));
}
