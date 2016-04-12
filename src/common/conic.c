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
#include "parser.tab.h"

void create_ellipse(void)
{
    _point *v;
    _conic *val;

    get_mem(val, _conic);
    val->type = ELLIPSE;
    val->d = POPn;
    val->b = POPn;
    val->a = POPn;
    if (val->a <= 0 || val->b <= 0 || val->a < val->b)
	runtime_error("invalid parameters");
    v = POP(_point);
    val->x = v->x;
    val->y = v->y;
    PSH(val);
}

void create_hyperbola(void)
{
    _point *v;
    _conic *val;

    get_mem(val, _conic);
    val->type = HYPERBOLA;
    val->d = POPn;
    val->b = POPn;
    val->a = POPn;
    if (val->a <= 0 || val->b <= 0) runtime_error("invalid parameters");
    v = POP(_point);
    val->x = v->x;
    val->y = v->y;
    PSH(val);
}

void create_parabola(void)
{
    _point *v;
    _conic *val;

    get_mem(val, _conic);
    val->type = PARABOLA;
    val->d = POPn;
    val->b = 0;
    val->a = POPn;
    if (val->a <= 0) runtime_error("invalid parameters");
    v = POP(_point);
    val->x = v->x + val->a*Cos(val->d)/2;
    val->y = v->y + val->a*Sin(val->d)/2;
    PSH(val);
}

void conic_center(void)
{
    _conic *c;
    _point *val;

    c = POP(_conic);
    if (c->type == PARABOLA) runtime_error("undefined center");
    get_mem(val, _point);
    val->x = c->x;
    val->y = c->y;
    PSH(val);
}

void conic_foci(void)
{
    _conic *C;
    _set *first, *second;
    double c, s, f;

    C = POP(_conic);
    if (C->type == PARABOLA) {
	get_elem(first);
	first->p->x = C->x;
	first->p->y = C->y;
	first->next = NULL;
    } else {
	c = Cos(C->d);
	s = Sin(C->d);
	f = sqrt(C->a*C->a + C->b*C->b*(C->type == ELLIPSE ? -1 : 1));
	get_elem(first);
	get_elem(second);
	first->p->x = C->x + c*f;
	first->p->y = C->y + s*f;
	first->next = second;
	second->p->x = C->x - c*f;
	second->p->y = C->y - s*f;
	second->next = NULL;
    }
    PSH(first);
}

void create_conic_with_directrix(void)
{
    _point *A;
    _line *l;
    _conic *val;
    double e, c, s, d, f, h;

    e = POPn;
    if (e <= 0) runtime_error("invalid eccentricity");
    l = POP(_line);
    A = POP(_point);
    c = Cos(l->a);
    s = Sin(l->a);
    d = s*(A->x-l->x) - c*(A->y-l->y);
    if (ZERO(d)) runtime_error("invalid parameters");
    get_mem(val, _conic);
    if (e == 1) {
	val->x = A->x;
	val->y = A->y;
	val->a = fabs(d);
	val->b = 0;
	val->type = PARABOLA;
    } else {
	h = 1/e - e;
	f = fabs(d)*e/h;
	val->x = A->x + s*f;
	val->y = A->y - c*f;
	val->a = fabs(d/h);
	if (e < 1) {
	    val->b = val->a*sqrt(1 - e*e);
	    val->type = ELLIPSE;
	} else {
	    val->b = val->a*sqrt(e*e - 1);
	    val->type = HYPERBOLA;
	}
    }
    val->d = principal(l->a + (d < 0 ? 90 : -90));
    PSH(val);
}

void create_conic_with_foci(void)
{
    _point *A, *B;
    _conic *val;
    double a, f;

    a = POPn;
    if (a <= 0) runtime_error("invalid major or real axis");
    B = POP(_point);
    A = POP(_point);
    f = distance(A, B);
    if (ZERO(f) || EQL(f, a)) runtime_error("invalid parameters");
    get_mem(val, _conic);
    val->x = (A->x + B->x)/2;
    val->y = (A->y + B->y)/2;
    val->a = a;
    val->d = argument(A, B);
    if (f < a) {
	val->b = sqrt(a*a - f*f);
	val->type = ELLIPSE;
    } else {
	val->b = sqrt(f*f - a*a);
	val->type = HYPERBOLA;
    }
    PSH(val);
}

void translate_conic(void)
{
    _conic *C, *val;
    _vector *u;

    u = POP(_vector);
    C = POP(_conic);
    get_mem(val, _conic);
    val->x = C->x + u->x;
    val->y = C->y + u->y;
    val->a = C->a;
    val->b = C->b;
    val->d = C->d;
    val->type = C->type;
    PSH(val);
}

void reflect_conic(void)
{
    _conic *C, *val;
    _line *l;
    double c, s, x, y, p;

    l = POP(_line);
    C = POP(_conic);
    c = Cos(l->a);
    s = Sin(l->a);
    x = C->x - l->x;
    y = C->y - l->y;
    p = 2*(c*x + s*y);
    get_mem(val, _conic);
    val->x = l->x + p*c - x;
    val->y = l->y + p*s - y;
    val->a = C->a;
    val->b = C->b;
    val->d = principal(2*l->a - C->d);
    val->type = C->type;
    PSH(val);
}

void symetric_conic(void)
{
    _conic *C, *val;
    _point *O;

    O = POP(_point);
    C = POP(_conic);
    get_mem(val, _conic);
    val->x = 2*O->x - C->x;
    val->y = 2*O->y - C->y;
    val->a = C->a;
    val->b = C->b;
    val->d = C->d + (C->d > 0 ? -180 : 180);
    val->type = C->type;
    PSH(val);
}

void rotate_conic(void)
{
    _conic *C, *val;
    _point *O;
    double a, c, s, x, y;

    a = POPn;
    O = POP(_point);
    C = POP(_conic);
    c = Cos(a);
    s = Sin(a);
    x = C->x - O->x;
    y = C->y - O->y;
    get_mem(val, _conic);
    val->x = O->x + c*x - s*y;
    val->y = O->y + s*x + c*y;
    val->a = C->a;
    val->b = C->b;
    val->d = principal(C->d + a);
    val->type = C->type;
    PSH(val);
}

void homothetic_conic(void)
{
    _conic *C, *val;
    _point *O;
    double k;

    k = POPn;
    O = POP(_point);
    C = POP(_conic);
    get_mem(val, _conic);
    val->x = O->x + k*(C->x - O->x);
    val->y = O->y + k*(C->y - O->y);
    val->a = fabs(k)*C->a;
    val->b = fabs(k)*C->b;
    val->d = C->d;
    val->type = C->type;
    PSH(val);
}

void major_axis(void)
{
    PSHn((POP(_conic))->a);
}

void minor_axis(void)
{
    PSHn((POP(_conic))->b);
}

void eccentricity(void)
{
    _conic *C;

    C = POP(_conic);
    switch (C->type) {
	case ELLIPSE:
	    PSHn(sqrt(1 - C->b*C->b/(C->a*C->a)));
	    break;
	case HYPERBOLA:
	    PSHn(sqrt(1 + C->b*C->b/(C->a*C->a)));
	    break;
	case PARABOLA:
	    PSHn(1.);
	    break;
    }
}

void conic_argument(void)
{
    PSHn((POP(_conic))->d);
}

void point_on_conic_argument(void)
{
    _conic *C;
    _point *A;
    double c, s, x, y, u, v;

    C = POP(_conic);
    A = POP(_point);
    c = Cos(C->d);
    s = Sin(C->d);
    x = A->x - C->x;
    y = A->y - C->y;
    u = c*x + s*y;
    v = -s*x + c*y;
    switch (C->type) {
	case ELLIPSE:
	    PSHn(RTOD(atan2(v*C->a, u*C->b)));
	    break;
	case HYPERBOLA:
	    PSHn(RTOD(atan2(C->b, v)) - (u < 0 ? 180 : 0));
	    break;
	case PARABOLA:
	    PSHn(RTOD(atan2(-v, (C->a - v*v/C->a)/2)));
	    break;
    }
}

void parametric_ellipse(double *x, double *y, double t,
			double x0, double y0, double a, double b,
			double c, double s)
{
    double u, v;

    u = a*Cos(t);
    v = b*Sin(t);
    *x = x0 + c*u - s*v;
    *y = y0 + s*u + c*v;
}

void parametric_hyperbola(double *x, double *y, double t,
			  double x0, double y0, double a, double b,
			  double c, double s)
{
    double u, v;

    u = a/Sin(t);
    v = b/Tan(t);
    *x = x0 + c*u - s*v;
    *y = y0 + s*u + c*v;
}


void parametric_parabola(double *x, double *y, double t,
			 double x0, double y0, double p,
			 double c, double s)
{
    double q, r, u, v;

    q = Cos(t);
    r = -p/(1 + q);
    u = r*q;
    v = r*Sin(t);
    *x = x0 + c*u - s*v;
    *y = y0 + s*u + c*v;
}
