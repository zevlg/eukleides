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
#include "error.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"

#define STO(addr) (POP(_symbol))->content->value.point = (addr)

_point *A, *B, *C;

double a, x, y;

void assign_A_B(int p)
{
    switch(p) {
	case 0: get_mem(A, _point);
		A->x = 0;
		A->y = 0;
		STO(A);
		get_mem(B, _point);
		B->x = a*x;
		B->y = a*y;
		STO(B);
		break;
	case 1: A = POP(_point);
		get_mem(B, _point);
		B->x = A->x + a*x;
		B->y = A->y + a*y;
		STO(B);
		break;
	case 2: A = POP(_point);
		B = POP(_point);
		a = distance(A, B);
		if (ZERO(a)) runtime_error(_("invalid points"));
		x = (B->x - A->x)/a;
		y = (B->y - A->y)/a;
    }
}

void assign_C(double u, double v)
{
    get_mem(C, _point);
    C->x = A->x + u*x - v*y;
    C->y = A->y + v*x + u*y;
    STO(C);
}

void define_optimal_scalene(_param p)
{
    double t;

    if (p.addr < 2) {
	t = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    }
    assign_A_B(p.addr);
    assign_C(a*.375, a*.61237244);
}

void define_triangle_SSS(_param p)
{
    double t, b, c, s1, s2, s, u, v;

    if (p.addr < 2) {
	t = POPn;
	c = POPn;
	b = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else {
	c = POPn;
	b = POPn;
    }
    assign_A_B(p.addr);
    s1 = (b-a+c)*(b+a-c);
    s2 = (a+c-b)*(a+b+c);
    if (ZERO(s2)) {
	u = -c;
	v = 0;
    } else {
	s = s1/s2;
	if (s < 0) runtime_error(_("invalid lengths"));
	t = 2 * atan(sqrt(s));
	u = c*cos(t);
	v = c*sin(t);
    }
    assign_C(u, v);
}

void define_triangle_SAA(_param p)
{
    double t, c, u, v, cu, su, cv, sv, d;

    if (p.addr < 2) {
	t = POPn;
	v = POPn;
	u = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else {
	v = POPn;
	u = POPn;
    }
    cu = Cos(u);
    su = Sin(u);
    cv = Cos(v);
    sv = Sin(v);
    d = cu*sv + su*cv;
    if (ZERO(d)) runtime_error(_("invalid angles"));
    assign_A_B(p.addr);
    c = a*sv/d;
    assign_C(c*cu, c*su);
}

void define_triangle_SAS(_param p)
{
    double t, c, u;

    if (p.addr < 2) {
	t = POPn;
	c = POPn;
	u = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else {
	c = POPn;
	u = POPn;
    }
    assign_A_B(p.addr);
    assign_C(c*Cos(u), c*Sin(u));
}

void define_triangle_SSA(_param p)
{
    double t, b, c, v, cv, sv, s;

    if (p.addr < 2) {
	t = POPn;
	v = POPn;
	c = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else {
	v = POPn;
	c = POPn;
    }
    assign_A_B(p.addr);
    cv = Cos(v);
    sv = Sin(v);
    s = c*c - a*a*sv*sv;
    if (s < 0) runtime_error(_("invalid parameters"));
    b = a*cv + sqrt(s);
    assign_C(a-b*cv, b*sv);
}

void define_right_SS(_param p)
{
    double t, b;

    if (p.addr < 2) {
	t = POPn;
	b = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else b = POPn;
    assign_A_B(p.addr);
    assign_C(a, b);
}

void define_right_SA(_param p)
{
    double t, u;

    if (p.addr < 2) {
	t = POPn;
	u = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else u = POPn;
    if (u>=90 || u<=-90) runtime_error(_("invalid angle"));
    assign_A_B(p.addr);
    assign_C(a, a*Tan(u));
}

void define_isoceles_SS(_param p)
{
    double t, b, s;

    if (p.addr < 2) {
	t = POPn;
	b = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else b = POPn;
    assign_A_B(p.addr);
    a /= 2;
    s = b*b - a*a;
    if (s < 0) runtime_error(_("invalid lengths"));
    assign_C(a, sqrt(s));
}

void define_isoceles_SA(_param p)
{
    double t, u;

    if (p.addr < 2) {
	t = POPn;
	u = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    } else u = POPn;
    if (u>=90 || u<=-90) runtime_error(_("invalid angle"));
    assign_A_B(p.addr);
    a /= 2;
    assign_C(a, a*Tan(u));
}

void define_equilateral(_param p)
{
    double t;

    if (p.addr < 2) {
	t = POPn;
	a = POPn;
	x = Cos(t);
	y = Sin(t);
    }
    assign_A_B(p.addr);
    a /= 2;
    assign_C(a, a*sqrt(3));
}
