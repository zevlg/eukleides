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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "error.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"
#include "parser.tab.h"

void not(void)
{
    PSHn(!pop_param().addr);
}

void and(void)
{
    int a, b;

    a = pop_param().addr;
    b = pop_param().addr;
    PSHn(a && b);
}

void or(void)
{
    int a, b;

    a = pop_param().addr;
    b = pop_param().addr;
    PSHn(a || b);
}

void ternary(void)
{
    double x, y;
    int z;
    x = POPn;
    y = POPn;
    z = pop_param().addr;
    PSHn(z ? y : x);
}

void num_eq(void)
{
    PSHn(EQL(POPn, POPn));
}

void num_neq(void)
{
    PSHn(NEQL(POPn, POPn));
}

void num_lt(void)
{
    PSHn(POPn > POPn);
}

void num_leq(void)
{
    PSHn(POPn >= POPn);
}

void num_gt(void)
{
    PSHn(POPn < POPn);
}

void num_geq(void)
{
    PSHn(POPn <= POPn);
}

void pnt_eq(void)
{
    _point *A, *B;

    B = POP(_point);
    A = POP(_point);
    PSHn(EQL(A->x, B->x) && EQL(A->y, B->y));
}

void pnt_neq(void)
{
    _point *A, *B;

    B = POP(_point);
    A = POP(_point);
    PSHn(NEQL(A->x, B->x) || NEQL(A->y, B->y));
}

void collinear_points(void)
{
    _point *A, *B, *C;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    PSHn(ZERO(det2(B->x-A->x, B->y-A->y, C->x-A->x, C->y-A->y)));
}

void vec_eq(void)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    PSHn(EQL(u->x, v->x) && EQL(u->y, v->y));
}

void vec_neq(void)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    PSHn(NEQL(u->x, v->x) || NEQL(u->y, v->y));
}

void collinear_vectors(void)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    PSHn(ZERO(det2(u->x, u->y, v->x, v->y)));
}

int elem_eq(_set *s1, _set *s2)
{
    if (s1 == NULL && s2 == NULL) return 1;
    if (s1 == NULL || s2 == NULL) return 0;
    if (NEQL(s1->p->x, s2->p->x) || NEQL(s1->p->y, s2->p->y)) return 0;
    return elem_eq(s1->next, s2->next);
}

void set_eq(void)
{
    _set *s1, *s2;

    s2 = POP(_set);
    s1 = POP(_set);
    PSHn(elem_eq(s1, s2));
}

void set_neq(void)
{
    _set *s1, *s2;

    s2 = POP(_set);
    s1 = POP(_set);
    PSHn(!elem_eq(s1, s2));
}

void is_empty(void)
{
    PSHn(POP(_set) == NULL);
}

int elem_on_path(_point *A, double x, double y, _set *s)
{
    double u, v;

    if (s->next == NULL) return 0;
    s = s->next;
    u = s->p->x-A->x;
    v = s->p->y-A->y;
    if (ZERO(det2(x, y, u, v)) && u*x+v*y <= 0) return 1;
    return elem_on_path(A, u, v, s);
}

void on_path(void)
{
    _set *s;
    _point *A;

    s = POP(_set);
    A = POP(_point);
    if (s == NULL) PSHn(0);
    else if (s->next == NULL) PSHn(EQL(A->x, s->p->x) && EQL(A->y, s->p->y));
    else PSHn(elem_on_path(A, s->p->x-A->x, s->p->y-A->y, s));
}

int elem_in_set(_point *A, _set *s)
{
    if (s == NULL) return 0;
    if (EQL(A->x, s->p->x) && EQL(A->y, s->p->y)) return 1;
    return elem_in_set(A, s->next);
}

void in_set(void)
{
    _set *s;
    _point *A;

    s = POP(_set);
    A = POP(_point);
    PSHn(elem_in_set(A, s));
}

void lin_eq(void)
{
    _line *l1, *l2;
    double u, v;

    l2 = POP(_line);
    l1 = POP(_line);
    u = l2->x - l1->x;
    v = l2->y - l1->y;
    PSHn(ZERO(det2(u, v, Cos(l1->a), Sin(l1->a)))
	 && ZERO(det2(u, v, Cos(l2->a), Sin(l2->a))));
}

void lin_neq(void)
{
    _line *l1, *l2;
    double u, v;

    l2 = POP(_line);
    l1 = POP(_line);
    u = l2->x - l1->x;
    v = l2->y - l1->y;
    PSHn(NEQL(det2(u, v, Cos(l1->a), Sin(l1->a)), 0)
	 || NEQL(det2(u, v, Cos(l2->a), Sin(l2->a)), 0));
}

void on_line(void)
{
    _line *l;
    _point *A;

    l = POP(_line);
    A = POP(_point);
    PSHn(ZERO(det2(l->x-A->x, l->y-A->y, Cos(l->a), Sin(l->a))));
}

double delta(double a, double b, double m)
{
    return fmod(fabs(a - b), m);
}

void parallel_lines(void)
{
    _line *l1, *l2;

    l2 = POP(_line);
    l1 = POP(_line);
    PSHn(ZERO(delta(l1->a, l2->a, 180)));
}

void perpendicular_lines(void)
{
    _line *l1, *l2;

    l2 = POP(_line);
    l1 = POP(_line);
    PSHn(EQL(delta(l1->a, l2->a, 180), 90));
}

void cir_eq(void)
{
    _circle *c1, *c2;

    c2 = POP(_circle);
    c1 = POP(_circle);
    PSHn(EQL(c1->x, c2->x) && EQL(c1->y, c2->y) && EQL(c1->r, c2->r));
}

void cir_neq(void)
{
    _circle *c1, *c2;

    c2 = POP(_circle);
    c1 = POP(_circle);
    PSHn(NEQL(c1->x, c2->x) || NEQL(c1->y, c2->y) || NEQL(c1->r, c2->r));
}

void on_circle(void)
{
    _circle *c;
    _point *A;

    c = POP(_circle);
    A = POP(_point);
    PSHn(EQL(hypot(c->x-A->x, c->y-A->y), c->r));
}

void con_eq(void)
{
    _conic *c1, *c2;

    c2 = POP(_conic);
    c1 = POP(_conic);
    PSHn(c1->type == c2->type
	 && EQL(c1->x, c2->x) && EQL(c1->y, c2->y)
	 && EQL(c1->a, c2->a) && EQL(c1->b, c2->b)
	 && ZERO(delta(c1->d, c2->d, c1->type == PARABOLA ? 360 : 180)));
}

void con_neq(void)
{
    _conic *c1, *c2;

    c2 = POP(_conic);
    c1 = POP(_conic);
    PSHn(c1->type != c2->type
	 || NEQL(c1->x, c2->x) || NEQL(c1->y, c2->y)
	 || NEQL(c1->a, c2->a) || NEQL(c1->b, c2->b)
	 || NEQL(delta(c1->d, c2->d, c1->type == PARABOLA ? 360 : 180), 0));
}

void on_conic(void)
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
	    PSHn(EQL(u*u/(C->a*C->a) + v*v/(C->b*C->b), 1));
	    break;
	case HYPERBOLA:
	    PSHn(EQL(u*u/(C->a*C->a) - v*v/(C->b*C->b), 1));
	    break;
	case PARABOLA:
	    PSHn(EQL(C->a*(2*u + C->a), v*v));
	    break;
    }
}

void is_ellipse(void)
{
    PSHn((POP(_conic))->type == ELLIPSE);
}

void is_hyperbola(void)
{
    PSHn((POP(_conic))->type == HYPERBOLA);
}

void is_parabola(void)
{
    PSHn((POP(_conic))->type == PARABOLA);
}

void is_isosceles(void)
{
    _point *A, *B, *C;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    PSHn(EQL(distance(C, A), distance(C, B)));
}

void is_equilateral(void)
{
    _point *A, *B, *C;
    double c;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    c = distance(A, B);
    PSHn(EQL(c, distance(B, C)) && EQL(c, distance(C, A)));
}

void is_right(void)
{
    _point *A, *B, *C;
    double a, b, c;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = distance(B, C);
    b = distance(C, A);
    c = distance(A, B);
    PSHn(EQL(b*b, a*a + c*c));
}

void is_parallelogram(void)
{
    _point *A, *B, *C, *D;

    D = POP(_point);
    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    PSHn(EQL(A->x+C->x, B->x+D->x) && EQL(A->y+C->y, B->y+D->y));
}

void is_rectangle(void)
{
    _point *A, *B, *C, *D;
    double a, b;

    D = POP(_point);
    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = distance(A, C);
    b = distance(B, D);
    PSHn(EQL(A->x+C->x, B->x+D->x) && EQL(A->y+C->y, B->y+D->y) && EQL(a, b));
}

void is_square(void)
{
    _point *A, *B, *C, *D;
    double a, b, c, d;

    D = POP(_point);
    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = distance(A, C);
    b = distance(B, D);
    c = distance(A, B);
    d = distance(A, D);
    PSHn(EQL(A->x+C->x, B->x+D->x) && EQL(A->y+C->y, B->y+D->y)
	 && EQL(a, b) && EQL(c, d));
}

void eps_output(void)
{
#ifdef __eukleides__
    PSHn(1);
#else
    PSHn(0);
#endif
}

void pstricks_output(void)
{
#ifdef __euktopst__
    PSHn(1);
#else
    PSHn(0);
#endif
}

void display_output(void)
{
#ifdef __geukleides__
    PSHn(1);
#else
    PSHn(0);
#endif
}

void for_test(void)
{
    double c, s, e;

    c = POPn;
    s = POPn;
    e = POPn;

    PSHn(s);
    if ((c > e && s > 0) || (c < e && s < 0))
	PSHn(0);
    else
	PSHn(1);
}
