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

void orthogonal_projection(void)
{
    _point *A, *val;
    _line *l;
    double c, s, p;

    l = POP(_line);
    A = POP(_point);
    c = Cos(l->a);
    s = Sin(l->a);
    p = c*(A->x - l->x) + s*(A->y - l->y);
    get_mem(val, _point);
    val->x = l->x + p*c;
    val->y = l->y + p*s;
    PSH(val);
}

void push_intersection_point(double x1, double y1, double a1,
			     double x2, double y2, double a2)
{
    double c1, c2, s1, s2, d, b1, b2;
    _point *val;

    c1 = Cos(a1);
    s1 = Sin(a1);
    c2 = Cos(a2);
    s2 = Sin(a2);
    d = det2(c1, c2, s1, s2);
    if (ZERO(d)) runtime_error(_("parallel lines"));
    b1 = det2(x1, y1, c1, s1);
    b2 = det2(x2, y2, c2, s2);
    get_mem(val, _point);
    val->x = det2(c1, c2, b1, b2)/d;
    val->y = det2(s1, s2, b1, b2)/d;
    PSH(val);
}

void parallel_projection(void)
{
    _line *l1, *l2;
    _point *A;

    l2 = POP(_line);
    l1 = POP(_line);
    A = POP(_point);
    push_intersection_point(l1->x, l1->y, l1->a, A->x, A->y, l2->a);
}

void lines_intersection(void)
{
    _line *l1, *l2;

    l2 = POP(_line);
    l1 = POP(_line);
    push_intersection_point(l1->x, l1->y, l1->a, l2->x, l2->y, l2->a);
}

#define dist(s)		a*(s)->p->x+b*(s)->p->y+c

_set *line_segment_intersection(_set *s, double a, double b, double c, double d)
{
    _set *val, *t;
    double e, f, g;

    t = s->next;
    if (t == NULL) return NULL;
    e = dist(t);
    if (d*e <= EPSILON && fabs(e) > EPSILON) {
	get_elem(val);
	f = fabs(d) + fabs(e);
	g = f == 0 ? 0 : fabs(d)/f;
	val->p->x = s->p->x + g*(t->p->x-s->p->x);
	val->p->y = s->p->y + g*(t->p->y-s->p->y);
	val->next = line_segment_intersection(t, a, b, c, e);
	return val;
    }
    return line_segment_intersection(t, a, b, c, e);
}

void line_set_intersection(void)
{
    _line *l;
    _set *s;
    double a, b, c;

    s = POP(_set);
    l = POP(_line);
    if (s == NULL) {
	PSH(NULL);
	return;
    }
    a = Sin(l->a);
    b = -Cos(l->a);
    c = -a*l->x-b*l->y;
    PSH(line_segment_intersection(s, a, b, c, dist(s)));
}

int solve(double *x1, double *x2, double a, double b, double c)
{
    double d, r;

    if (ZERO(a)) {
	if (ZERO(b)) return 0;
	*x1 = *x2 = -c/b;
	return 1;
    }
    d = b*b - 4*a*c;
    if (d < 0) return 0;
    if (ZERO(d)) {
	*x1 = *x2 = -b/(2*a);
	return 1;
    }
    r = sqrt(d);
    *x1 = (-b - r)/(2*a);
    *x2 = (-b + r)/(2*a);
    return 2;
}

void line_circle_intersection(void)
{
    _line *l;
    _circle *C;
    _set *first = NULL, *second = NULL;
    double c, s, x, y, t1, t2;
    int n = 0;

    C = POP(_circle);
    l = POP(_line);
    x = l->x - C->x;
    y = l->y - C->y;
    c = Cos(l->a);
    s = Sin(l->a);
    n = solve(&t1, &t2, 1, 2*(x*c + y*s), x*x + y*y - C->r*C->r);
    if (n > 0) {
	get_elem(first);
	first->p->x = l->x + c*t1;
	first->p->y = l->y + s*t1;
	if (n > 1) {
	    get_elem(second);
	    second->p->x = l->x + c*t2;
	    second->p->y = l->y + s*t2;
	    second->next = NULL;
	}
	first->next = second;
    }
    PSH(first);
}

void line_conic_intersection(void)
{
    _line *l;
    _conic *C;
    _set *first = NULL, *second = NULL;
    double c, s, a, b, x, y, ca, sa, u, v, t1, t2;
    int n = 0;

    C = POP(_conic);
    l = POP(_line);
    c = Cos(C->d);
    s = Sin(C->d);
    a = C->a*C->a;
    b = C->b*C->b;
    x = c*(l->x - C->x) + s*(l->y - C->y);
    y = -s*(l->x - C->x) + c*(l->y - C->y);
    ca = Cos(l->a);
    sa = Sin(l->a);
    u = c*ca + s*sa;
    v = -s*ca + c*sa;
    switch (C->type) {
	case ELLIPSE:
	    n = solve(&t1, &t2, u*u/a + v*v/b,
		      2*(x*u/a + y*v/b), x*x/a + y*y/b - 1);
	    break;
	case HYPERBOLA:
	    n = solve(&t1, &t2, u*u/a - v*v/b,
		      2*(x*u/a - y*v/b), x*x/a - y*y/b - 1);
	    break;
	case PARABOLA:
	    n = solve(&t1, &t2, v*v/(2*C->a),
		      y*v/C->a - u, y*y/(2*C->a) - x - C->a/2);
	    break;
    }
    if (n > 0) {
	get_elem(first);
	first->p->x = l->x + ca*t1;
	first->p->y = l->y + sa*t1;
	if (n > 1) {
	    get_elem(second);
	    second->p->x = l->x + ca*t2;
	    second->p->y = l->y + sa*t2;
	    second->next = NULL;
	}
	first->next = second;
    }
    PSH(first);
}

#define min(a, b)	((a) < (b) ? (a) : (b))
#define max(a, b)	((a) > (b) ? (a) : (b))

void sets_intersection(void)
{
    _set *s, *t, *u, *v, *w, *a = NULL, *b;
    double d1, d2, c1, c2, s1, s2, x, y, d, t1, t2;

    u = POP(_set);
    s = POP(_set);
    if (u == NULL || s == NULL) {
	PSH(NULL);
	return;
    }
    t = s->next;
    while (t) {
	v = u;
	w = u->next;
	while (w) {
	    if (max(s->p->x, t->p->x) >= min(v->p->x, w->p->x)
	        && max(v->p->x, w->p->x) >= min(s->p->x, t->p->x)
	        && max(s->p->y, t->p->y) >= min(v->p->y, w->p->y)
	        && max(v->p->y, w->p->y) >= min(s->p->y, t->p->y))
	    {
		d1 = distance(s->p, t->p);
		c1 = (t->p->x - s->p->x)/d1;
		s1 = (t->p->y - s->p->y)/d1;
		d2 = distance(v->p, w->p);
		c2 = (w->p->x - v->p->x)/d2;
		s2 = (w->p->y - v->p->y)/d2;
		x = v->p->x - s->p->x;
		y = v->p->y - s->p->y;
		d = det2(c1, c2, s1, s2);
		if (ZERO(d)) {
		    if (ZERO(det2(x, y, c1, s1))) {
			b = a;
			get_mem(a, _set);
			a->p = s->p;
			a->next = b;
		    }
		} else {
		    t1 = det2(x, y, c2, s2)/d;
		    t2 = det2(x, y, c1, s1)/d;
		    if (t1 >= 0 && t1 <= d1 && t2 >= 0 && t2 <= d2) {
			b = a;
			get_elem(a);
			a->p->x = s->p->x + c1*t1;
			a->p->y = s->p->y + s1*t1;
			a->next = b;
		    }
		}
	    }
	    v = w;
	    w = w->next;
	}
	s = t;
	t = t->next;
    }
    PSH(a);
}

void circles_intersection(void)
{
    _circle *c1, *c2;
    _set *first, *second;
    double x, y, a, b, c, t, u, v;

    c2 = POP(_circle);
    c1 = POP(_circle);
    x = c2->x-c1->x;
    y = c2->y-c1->y;
    a = hypot(x, y);
    b = c2->r;
    c = c1->r;
    if (ZERO(a) || a > b+c || a < fabs(b-c)) {
	PSH(NULL);
	return;
    }
    x /= a;
    y /= a;
    if (a == b+c || a == fabs(b-c)) {
	get_elem(first);
	first->p->x = c1->x + c*x;
	first->p->y = c1->y + c*y;
	first->next = NULL;
	PSH(first);
	return;
    }
    get_elem(first);
    get_elem(second);
    t = 2 * atan(sqrt((b-a+c)*(b+a-c)/((a+c-b)*(a+b+c))));
    u = c*cos(t);
    v = c*sin(t);
    first->p->x = c1->x + u*x - v*y;
    first->p->y = c1->y + v*x + u*y;
    first->next = second;
    second->p->x = c1->x + u*x + v*y;
    second->p->y = c1->y - v*x + u*y;
    second->next = NULL;
    PSH(first);
}

#undef dist
#define dist(s)	hypot((s)->p->x - c->x, (s)->p->y - c->y)

_set *circle_segment_intersection(_set *s, _circle* c, double d)
{
    _set *v1 = NULL, *v2 = NULL, *t;
    double e, f, x, y, u, v, t1, t2;
    int n;

    t = s->next;
    if (t == NULL) return NULL;
    e = dist(t);
    if (d >= c->r || e >= c->r) {
	f = distance(s->p, t->p);
	x = s->p->x - c->x;
	y = s->p->y - c->y;
	u = (t->p->x - s->p->x)/f;
	v = (t->p->y - s->p->y)/f;
	n = solve(&t1, &t2, 1, 2*(x*u + y*v), x*x + y*y - c->r*c->r);
	if (n > 0) {
	    if (n > 1 && t2 >= 0 && t2 <= f) {
		get_elem(v2);
		v2->p->x = s->p->x + u*t2;
		v2->p->y = s->p->y + v*t2;
		v2->next = circle_segment_intersection(t, c, e);
	    }
	    if (t1 >= 0 && t1 <= f) {
		get_elem(v1);
		v1->p->x = s->p->x + u*t1;
		v1->p->y = s->p->y + v*t1;
		if (v2) v1->next = v2;
		else v1->next = circle_segment_intersection(t, c, e);
	    }
	    if (v1) return v1;
	    if (v2) return v2;
	}
    }
    return circle_segment_intersection(t, c, e);
}

void circle_set_intersection(void)
{
    _circle *c;
    _set *s;

    s = POP(_set);
    c = POP(_circle);
    if (s == NULL) {
	PSH(NULL);
	return;
    }
    PSH(circle_segment_intersection(s, c, dist(s)));
}
