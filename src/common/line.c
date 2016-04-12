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

void create_line_with_argument(void)
{
    _point *O;
    _line *val;

    get_mem(val, _line);
    val->a = POPn;
    O = POP(_point);
    val->x = O->x;
    val->y = O->y;
    PSH(val);
}

void create_line_with_points(void)
{
    _point *A, *B;
    _line *val;

    B = POP(_point);
    A = POP(_point);
    if (EQL(A->x, B->x) && EQL(A->y, B->y)) runtime_error(_("undefined line"));
    get_mem(val, _line);
    val->x = A->x;
    val->y = A->y;
    val->a = argument(A, B);
    PSH(val);
}

void create_line_with_vector(void)
{
    _point *O;
    _vector *u;
    _line *val;

    get_mem(val, _line);
    u = POP(_vector);
    O = POP(_point);
    val->x = O->x;
    val->y = O->y;
    val->a = RTOD(atan2(u->y, u->x));
    PSH(val);
}

void create_line_with_segment(void)
{
    _set *seg;
    _point *A, *B;
    _line *val;

    seg = POP(_set);
    A = get_point(&seg);
    B = get_point(&seg);
    if (EQL(A->x, B->x) && EQL(A->y, B->y)) runtime_error(_("undefined line"));
    get_mem(val, _line);
    val->x = A->x;
    val->y = A->y;
    val->a = argument(A, B);
    PSH(val);
}

void create_tangent_to_circle(void)
{
    double a;
    _circle *c;
    _line *val;

    a = POPn;
    c = POP(_circle);
    get_mem(val, _line);
    val->x = c->x + c->r*Cos(a);
    val->y = c->y + c->r*Sin(a);
    val->a = a + (a<=90 ? 90 : -270);
    PSH(val);
}

void create_tangent_to_conic(void)
{
    double c, s, t, a = 0;
    _conic *C;
    _line *val;

    t = POPn;
    C = POP(_conic);
    c = Cos(C->d);
    s = Sin(C->d);
    get_mem(val, _line);
    switch (C->type) {
	case ELLIPSE:
	    parametric_ellipse(&(val->x), &(val->y), t,
			       C->x, C->y, C->a, C->b, c, s);
	    a = atan2(C->b*Cos(t), -C->a*Sin(t));
	    break;
	case HYPERBOLA:
	    if (t <= -180 || t > 180) runtime_error("invalid argument");
	    if (t == 0 || t == 180) {
		val->x = C->x;
		val->y = C->y;
		a = atan2(C->b, C->a*(t == 0 ? 1 : -1));
	    } else {
		parametric_hyperbola(&(val->x), &(val->y), t,
				     C->x, C->y, C->a, C->b, c, s);
		a = atan2(-C->b, -C->a*Cos(t));
	    }
	    break;
	case PARABOLA:
	    if (t <= -180 || t >= 180) runtime_error("invalid argument");
	    parametric_parabola(&(val->x), &(val->y), t,
				C->x, C->y, C->a, c, s);
	    a = atan2(-1 - Cos(t), Sin(t));
	    break;
    }
    val->a = principal(RTOD(a) + C->d);
    PSH(val);
}

void create_parallel_to_line(void)
{
    _point *O;
    _line *l, *val;

    O = POP(_point);
    l = POP(_line);
    get_mem(val, _line);
    val->x = O->x;
    val->y = O->y;
    val->a = l->a;
    PSH(val);
}

void create_parallel_to_segment(void)
{
    _set *seg;
    _point *O, *A, *B;
    _line *val;

    O = POP(_point);
    seg = POP(_set);
    A = get_point(&seg);
    B = get_point(&seg);
    if (EQL(A->x, B->x) && EQL(A->y, B->y)) runtime_error(_("invalid segment"));
    get_mem(val, _line);
    val->x = O->x;
    val->y = O->y;
    val->a = argument(A, B);
    PSH(val);
}

void create_perpendicular_to_line(void)
{
    _point *O;
    _line *l, *val;

    O = POP(_point);
    l = POP(_line);
    get_mem(val, _line);
    val->x = O->x;
    val->y = O->y;
    val->a = l->a + (l->a<=90 ? 90 : -270);
    PSH(val);
}

void create_perpendicular_to_segment(void)
{
    _set *seg;
    _point *O, *A, *B;
    double a;
    _line *val;

    O = POP(_point);
    seg = POP(_set);
    A = get_point(&seg);
    B = get_point(&seg);
    if (EQL(A->x, B->x) && EQL(A->y, B->y)) runtime_error(_("invalid segment"));
    a = argument(A, B);
    get_mem(val, _line);
    val->x = O->x;
    val->y = O->y;
    val->a = a + (a<=90 ? 90 : -270);
    PSH(val);
}

void create_perpendicular_bisector(void)
{
    _set *set;
    _point *A, *B;
    double a;
    _line *val;

    set = POP(_set);
    A = get_point(&set);
    B = get_point(&set);
    if (EQL(A->x, B->x) && EQL(A->y, B->y)) runtime_error(_("invalid set"));
    get_mem(val, _line);
    a = argument(A, B);
    val->x = (A->x + B->x)/2;
    val->y = (A->y + B->y)/2;
    val->a = a + (a<=90 ? 90 : -270);
    PSH(val);
}

void create_angle_bisector(void)
{
    _point *A, *B, *C;
    _line *val;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    if ((EQL(A->x, B->x) && EQL(A->y, B->y))
	|| (EQL(B->x, C->x) && EQL(B->y, C->y)))
	runtime_error(_("invalid angle"));
    get_mem(val, _line);
    val->x = B->x;
    val->y = B->y;
    val->a = (argument(B, A) + argument(B, C))/2;
    PSH(val);
}

void create_lines_bisector(void)
{
    double c1, c2, s1, s2, d, b1, b2, a;
    _line *l1, *l2, *val = NULL;

    l1 = POP(_line);
    l2 = POP(_line);
    c1 = Cos(l1->a);
    s1 = Sin(l1->a);
    c2 = Cos(l2->a);
    s2 = Sin(l2->a);
    d = det2(c1, c2, s1, s2);
    if (ZERO(d))
	if (ZERO(det2(l2->x-l1->x, l2->y-l1->y, c1, s1))) val = l1;
	else runtime_error(_("parallel lines"));
    else {
	get_mem(val, _line);
	b1 = det2(l1->x, l1->y, c1, s1);
	b2 = det2(l2->x, l2->y, c2, s2);
	val->x = det2(c1, c2, b1, b2)/d;
	val->y = det2(s1, s2, b1, b2)/d;
	a = (l1->a + l2->a)/2;
	if (fabs(l1->a - l2->a)>90) a += (a<=90 ? 90 : -270);
	val->a = a;
    }
    PSH(val);
}

void create_altitude(void)
{
    _point *A, *B, *C;
    double a;
    _line *val;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = argument(B, C);
    get_mem(val, _line);
    val->x = A->x;
    val->y = A->y;
    val->a = a + (a<=90 ? 90 : -270);
    PSH(val);
}

void create_median(void)
{
    _point *A, *B, *C;
    double x, y;
    _line *val;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    x = (B->x + C->x)/2;
    y = (B->y + C->y)/2;
    if (EQL(A->x, x) && EQL(A->y, y)) runtime_error(_("invalid triangle"));
    get_mem(val, _line);
    val->x = A->x;
    val->y = A->y;
    val->a = RTOD(atan2(y-A->y, x-A->x));
    PSH(val);
}

void translate_line(void)
{
    _line *l, *val;
    _vector *u;

    u = POP(_vector);
    l = POP(_line);
    get_mem(val, _line);
    val->x = l->x + u->x;
    val->y = l->y + u->y;
    val->a = l->a;
    PSH(val);
}

void reflect_line(void)
{
    _line *l, *d, *val;
    double c, s, x, y, p;

    d = POP(_line);
    l = POP(_line);
    c = Cos(l->a);
    s = Sin(l->a);
    x = l->x - d->x;
    y = l->y - d->y;
    p = 2*(c*x + s*y);
    get_mem(val, _line);
    val->x = d->x + p*c - x;
    val->y = d->y + p*s - y;
    val->a = principal(2*d->a - l->a);
    PSH(val);
}

void symetric_line(void)
{
    _point *O;
    _line *l, *val;

    O = POP(_point);
    l = POP(_line);
    get_mem(val, _line);
    val->x = 2*O->x - l->x;
    val->y = 2*O->y - l->y;
    val->a = l->a + (l->a > 0 ? -180 : 180);
    PSH(val);
}

void rotate_line(void)
{
    _point *O;
    _line *l, *val;
    double a, c, s, x, y;

    a = POPn;
    O = POP(_point);
    l = POP(_line);
    c = Cos(a);
    s = Sin(a);
    x = l->x - O->x;
    y = l->y - O->y;
    get_mem(val, _line);
    val->x = O->x + c*x - s*y;
    val->y = O->y + s*x + c*y;
    val->a = principal(l->a + a);
    PSH(val);
}

void homothetic_line(void)
{
    _point *O;
    _line *l, *val;
    double k;

    k = POPn;
    O = POP(_point);
    l = POP(_line);
    if (k == 0) runtime_error(_("invalid ratio"));
    get_mem(val, _line);
    val->x = O->x + k*(l->x - O->x);
    val->y = O->y + k*(l->y - O->y);
    val->a = l->a + (k < 0 ? (l->a > 0 ? -180 : 180) : 0);
    PSH(val);
}

void point_line_distance(void)
{
    _line *l;
    _point *A;
    double c, s;

    l = POP(_line);
    A = POP(_point);
    c = Cos(l->a);
    s = Sin(l->a);
    PSHn(fabs(s*(A->x-l->x) - c*(A->y-l->y)));
}

void line_argument(void)
{
    PSHn((POP(_line))->a);
}
