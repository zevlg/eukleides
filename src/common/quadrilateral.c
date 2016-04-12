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

double l, m, x, y, u, v;

_point *A, *B, *C, *D;

void assign(int p, int s)
{
    if (p < 3) {
	switch(p) {
	    case 0: get_mem(A, _point);
		    A->x = 0;
		    A->y = 0;
		    STO(A);
		    get_mem(B, _point);
		    B->x = l*x;
		    B->y = l*y;
		    STO(B);
		    break;
	    case 1: A = POP(_point);
		    get_mem(B, _point);
		    B->x = A->x + l*x;
		    B->y = A->y + l*y;
		    STO(B);
		    break;
	    case 2: A = POP(_point);
		    B = POP(_point);
		    l = distance(A, B);
		    if (ZERO(l)) runtime_error(_("invalid points"));
		    x = (B->x - A->x)/l;
		    y = (B->y - A->y)/l;
	}
	if (s == 1) m = l;
	get_mem(C, _point);
	C->x = B->x + m*(u*x - v*y);
	C->y = B->y + m*(v*x + u*y);
	STO(C);
    } else {
	A = POP(_point);
	B = POP(_point);
	C = POP(_point);
    }
    get_mem(D, _point);
    D->x = A->x + C->x - B->x;
    D->y = A->y + C->y - B->y;
    STO(D);
}

void define_parallelogram_SSA(_param p)
{
    double a, b;

    if (p.addr < 2) {
	b = POPn;
	a = POPn;
	m = POPn;
	l = POPn;
	x = Cos(b);
	y = Sin(b);
	u = Cos(a);
	v = Sin(a);
    } else if (p.addr == 2) {
	a = POPn;
	m = POPn;
	u = Cos(a);
	v = Sin(a);
    }
    assign(p.addr, 0);
}

void define_parallelogram_VV(_param p)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    if (p.addr == 0) {
	get_mem(A, _point);
	A->x = 0;
	A->y = 0;
	STO(A);
    } else A = POP(_point);
    get_mem(B, _point);
    B->x = A->x + u->x;
    B->y = A->y + u->y;
    STO(B);
    get_mem(C, _point);
    C->x = A->x + u->x + v->x;
    C->y = A->y + u->y + v->y;
    STO(C);
    get_mem(D, _point);
    D->x = A->x + v->x;
    D->y = A->y + v->y;
    STO(D);
}

void define_rectangle(_param p)
{
    double b;

    if (p.addr < 2) {
	b = POPn;
	m = POPn;
	l = POPn;
	x = Cos(b);
	y = Sin(b);
    } else m = POPn;
    u = 0;
    v = 1;
    assign(p.addr, 0);
}

void define_square(_param p)
{
    double b;

    if (p.addr < 2) {
	b = POPn;
	l = POPn;
	x = Cos(b);
	y = Sin(b);
    }
    u = 0;
    v = 1;
    assign(p.addr, 1);
}
