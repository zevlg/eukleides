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
#include "conic.h"
#include "utils.h"
#include "parser.tab.h"

void create_point_cartesian(void)
{
    _point *val;

    get_mem(val, _point);
    val->y = POPn;
    val->x = POPn;
    PSH(val);
}

void create_point_polar(void)
{
    double r, a;
    _point *val;

    get_mem(val, _point);
    a = POPn;
    r = POPn;
    val->x = r*Cos(a);
    val->y = r*Sin(a);
    PSH(val);
}

void create_point_on_segment(void)
{
    double x;
    _set *set;
    _point *A, *B;
    _point *val;

    x = POPn;
    set = POP(_set);
    A = get_point(&set);
    B = get_point(&set);
    get_mem(val, _point);
    val->x = A->x + x*(B->x - A->x);
    val->y = A->y + x*(B->y - A->y);
    PSH(val);
}

void create_point_on_line(void)
{
    double x;
    _line *l;
    _point *val;

    x = POPn;
    l = POP(_line);
    get_mem(val, _point);
    val->x = l->x + x*Cos(l->a);
    val->y = l->y + x*Sin(l->a);
    PSH(val);
}

void create_point_with_abscissa(void)
{
    double x, c;
    _line *l;
    _point *val;

    x = POPn;
    l = POP(_line);
    c = Cos(l->a);
    if (ZERO(c)) runtime_error(_("invalid line"));
    get_mem(val, _point);
    val->x = x;
    val->y = l->y + (x-l->x)*Sin(l->a)/c;
    PSH(val);
}

void create_point_with_ordinate(void)
{
    double y, s;
    _line *l;
    _point *val;

    y = POPn;
    l = POP(_line);
    s = Sin(l->a);
    if (ZERO(s)) runtime_error(_("invalid line"));
    get_mem(val, _point);
    val->x = l->x + (y-l->y)*Cos(l->a)/s;
    val->y = y;
    PSH(val);
}

void create_point_on_circle(void)
{
    double a;
    _circle *c;
    _point *val;

    a = POPn;
    c = POP(_circle);
    get_mem(val, _point);
    val->x = c->x + c->r*Cos(a);
    val->y = c->y + c->r*Sin(a);
    PSH(val);
}

void create_point_on_conic(void)
{
    _conic *C;
    _point *val;
    double c, s, t;

    t = POPn;
    C = POP(_conic);
    c = Cos(C->d);
    s = Sin(C->d);
    get_mem(val, _point);
    switch (C->type) {
	case ELLIPSE:
	    parametric_ellipse(&(val->x), &(val->y), t,
			       C->x, C->y, C->a, C->b, c, s);
	    break;
	case HYPERBOLA:
	    if (t <= -180 || t >= 180 || t == 0)
		runtime_error("invalid argument");
	    parametric_hyperbola(&(val->x), &(val->y), t,
				 C->x, C->y, C->a, C->b, c, s);
	    break;
    	case PARABOLA:
	    if (t <= -180 || t >= 180) runtime_error("invalid argument");
	    parametric_parabola(&(val->x), &(val->y), t,
				C->x, C->y, C->a, c, s);
	    break;
    }
    PSH(val);
}

void create_midpoint(void)
{
    _set *set;
    _point *A, *B;
    _point *val;

    set = POP(_set);
    A = get_point(&set);
    B = get_point(&set);
    get_mem(val, _point);
    val->x = (A->x + B->x)/2;
    val->y = (A->y + B->y)/2;
    PSH(val);
}

void create_isobarycenter(void)
{
    int n = 0;
    _set *set;
    _point *val;

    set = POP(_set);
    if (set == NULL) runtime_error(_("empty set"));
    get_mem(val, _point);
    val->x = 0;
    val->y = 0;
    do {
	val->x += set->p->x;
	val->y += set->p->y;
	n++;
	set = set->next;
    } while (set != NULL);
    val->x /= n;
    val->y /= n;
    PSH(val);
}

void create_barycenter(void)
{
    double c, s = 0;
    _point *cur, *val;

    get_mem(val, _point);
    val->x = 0;
    val->y = 0;
    c = POPn;
    cur = POP(_point);
    do {
	val->x += c*cur->x;
	val->y += c*cur->y;
	s += c;
	c = POPn;
	cur = POP(_point);
    } while (cur != NULL);
    val->x /= s;
    val->y /= s;
    PSH(val);
}

#define cosine(a, b, c)	(b*b + c*c - a*a)/(2*b*c)

#define tangent(x)	sqrt(1-x*x)/x

void create_orthocenter(void)
{
    _point *A, *B, *C, *val;
    double a, b, c, ca, cb, cc, d;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    get_mem(val, _point);
    a = distance(B, C);
    b = distance(C, A);
    c = distance(A, B);
    if (ZERO(a) || ZERO(b) || ZERO(c)) runtime_error(_("invalid triangle"));
    ca = cosine(a, b, c);
    cb = cosine(b, c, a);
    cc = cosine(c, a, b);
    if (ca == 0) {
	ca = 1; cb = 0; cc = 0;
    } else if (cb == 0) {
	ca = 0; cb = 1; cc = 0;
    } else if (cc == 0) {
	ca = 0; cb = 0; cc = 1;
    } else {
	ca = tangent(ca); cb = tangent(cb); cc = tangent(cc);
    }
    d = ca + cb + cc;
    val->x = (ca*A->x + cb*B->x + cc*C->x)/d;
    val->y = (ca*A->y + cb*B->y + cc*C->y)/d;
    PSH(val);
}

void translate_point(void)
{
    _point *A, *val;
    _vector *u;

    u = POP(_vector);
    A = POP(_point);
    get_mem(val, _point);
    val->x = A->x + u->x;
    val->y = A->y + u->y;
    PSH(val);
}

void reflect_point(void)
{
    _point *A, *val;
    _line *l;
    double c, s, x, y, p;

    l = POP(_line);
    A = POP(_point);
    c = Cos(l->a);
    s = Sin(l->a);
    x = A->x - l->x;
    y = A->y - l->y;
    p = 2*(c*x + s*y);
    get_mem(val, _point);
    val->x = l->x + p*c - x;
    val->y = l->y + p*s - y;
    PSH(val);
}

void symetric_point(void)
{
    _point *A, *O, *val;

    O = POP(_point);
    A = POP(_point);
    get_mem(val, _point);
    val->x = 2*O->x - A->x;
    val->y = 2*O->y - A->y;
    PSH(val);
}

void rotate_point(void)
{
    _point *A, *O, *val;
    double a, c, s, x, y;

    a = POPn;
    O = POP(_point);
    A = POP(_point);
    c = Cos(a);
    s = Sin(a);
    x = A->x - O->x;
    y = A->y - O->y;
    get_mem(val, _point);
    val->x = O->x + c*x - s*y;
    val->y = O->y + s*x + c*y;
    PSH(val);
}

void homothetic_point(void)
{
    _point *A, *O, *val;
    double k;

    k = POPn;
    O = POP(_point);
    A = POP(_point);
    get_mem(val, _point);
    val->x = O->x + k*(A->x - O->x);
    val->y = O->y + k*(A->y - O->y);
    PSH(val);
}

void point_abscissa(void)
{
    PSHn((POP(_point))->x);
}

void point_ordinate(void)
{
    PSHn((POP(_point))->y);
}

void points_distance(void)
{
    _point *A, *B;

    A = POP(_point);
    B = POP(_point);
    PSHn(distance(A, B));
}
