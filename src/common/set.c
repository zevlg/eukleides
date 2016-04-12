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

void create_set(void)
{
    _set *set;
    _point *pnt;

    get_mem(set, _set);
    pnt = POP(_point);
    set->p = pnt;
    set->next = NULL;
    PSH(set);
}

void create_segment(void)
{
    _set *first, *second;
    _point *tmp;

    get_mem(second, _set);
    tmp = POP(_point);
    second->p = tmp;
    second->next = NULL;
    get_mem(first, _set);
    tmp = POP(_point);
    first->p = tmp;
    first->next = second;
    PSH(first);
}

void add_head_point(void)
{
    _set *head, *tail;

    tail = POP(_set);
    get_mem(head, _set);
    head->p = POP(_point);
    head->next = tail;
    PSH(head);
}

void add_tail_point(void)
{
    _set *head, *cur, *tail;

    get_mem(tail, _set);
    tail->p = POP(_point);
    tail->next = NULL;
    head = POP(_set);
    if (head == NULL) head = tail;
    else {
	for (cur = head; cur->next != NULL; cur = cur->next);
	cur->next = tail;
    }
    PSH(head);
}

void catenate_sets(void)
{
    _set *first, *second, *cur;

    second = POP(_set);
    first = POP(_set);
    if (first == NULL) first = second;
    else {
	for (cur = first; cur->next != NULL; cur = cur->next);
	cur->next = second;
    }
    PSH(first);
}

void push_point(void)
{
    _set *s;

    s = POP(_set);
    if (s == NULL) PSH(0);
    else {
	PSH(s->next);
	PSH(s->p);
	PSH(1);
    }
}
    
void extract_point(_param p)
{
    double ind;
    _set *s;
    _point *val;

    ind = POPn;
    if (p.ptr) s = ((_symbol *)p.ptr)->content->value.set;
    else s = POP(_set);	    
    if (ind < 0) runtime_error(_("index out of range"));
    while (ind-- >= 1 && s != NULL) s = s->next;
    if (s == NULL) runtime_error(_("index out of range"));
    get_mem(val, _point);
    val->x = s->p->x;
    val->y = s->p->y;
    PSH(val);
}

void extract_subset(_param p)
{
    double i, j, k;
    _set *cur, *val;

    j = POPn;
    i = POPn;
    cur = POP(_set);
    if (i < 0 || j < i) runtime_error(_("invalid indices"));
    k = i;
    while (k-- >= 1 && cur != NULL) cur = cur->next;
    if (cur == NULL) runtime_error(_("first index out of range"));
    val = cur;
    k = j - i;
    while (k-- >= 1 && cur != NULL) cur = cur->next;
    if (cur == NULL) runtime_error(_("second index out of range"));
    cur->next = NULL;
    PSH(val);
}

_set *set_vertex(double x, double y, _point* O, double S, double C, double n)
{
    _set *val;

    if (n < 1) return NULL;
    get_elem(val);
    val->p->x = O->x + x;
    val->p->y = O->y + y;
    val->next = set_vertex(C*x-S*y, S*x+C*y, O, S, C, n-1);
    return val;
}

void create_polygon(void)
{
    _point *O;
    double n, r, a;

    a = POPn;
    r = POPn;
    O = POP(_point);
    n = POPn;
    if (n < 2) runtime_error(_("invalid vertices number"));
    PSH(set_vertex(r*Cos(a), r*Sin(a), O, sin(2*M_PI/n), cos(2*M_PI/n), n));
}

void translate_set(void)
{
    _set *s, *first;
    _point *cur;
    _vector *u;

    u = POP(_vector);
    first = s = POP(_set);
    while (s != NULL) {
	get_mem(cur, _point);
	cur->x = s->p->x + u->x;
	cur->y = s->p->y + u->y;
	s->p = cur;
	s = s->next;
    }
    PSH(first);
}

void reflect_set(void)
{
    _set *s, *first;
    _line *l;
    _point *cur;
    double C, S, x, y, p;

    l = POP(_line);
    first = s = POP(_set);
    C = Cos(l->a);
    S = Sin(l->a);
    while (s != NULL) {
	get_mem(cur, _point);
	x = s->p->x - l->x;
	y = s->p->y - l->y;
	p = 2*(C*x + S*y);
	cur->x = l->x + p*C - x;
	cur->y = l->y + p*S - y;
	s->p = cur;
	s = s->next;
    }
    PSH(first);
}

void symetric_set(void)
{
    _set *s, *first;
    _point *O, *cur;

    O = POP(_point);
    first = s = POP(_set);
    while (s != NULL) {
	get_mem(cur, _point);
	cur->x = 2*O->x - s->p->x;
	cur->y = 2*O->y - s->p->y;
	s->p = cur;
	s = s->next;
    }
    PSH(first);
}

void rotate_set(void)
{
    _set *s, *first;
    _point *O, *cur;
    double a, C, S, x, y;

    a = POPn;
    O = POP(_point);
    first = s = POP(_set);
    C = Cos(a);
    S = Sin(a);
    while (s != NULL) {
	get_mem(cur, _point);
	x = s->p->x - O->x;
	y = s->p->y - O->y;
	cur->x = O->x + C*x - S*y;
	cur->y = O->y + S*x + C*y;
	s->p = cur;
	s = s->next;
    }
    PSH(first);
}

void homothetic_set(void)
{
    _set *s, *first;
    _point *O, *cur;
    double k;

    k = POPn;
    O = POP(_point);
    first = s = POP(_set);
    while (s != NULL) {
	get_mem(cur, _point);
	cur->x = O->x + k*(s->p->x - O->x);
	cur->y = O->y + k*(s->p->y - O->y);
	s->p = cur;
	s = s->next;
    }
    PSH(first);
}

void set_cardinal(void)
{
    double c = 0;
    _set *cur;

    cur = POP(_set);
    if (cur != NULL) {
	c = 1;
	while (cur->next != NULL) {
	    c++;
	    cur = cur->next;
	}
    }
    PSHn(c);
}

void path_length(_param p)
{
    double l = 0;
    _set *cur, *first;

    first = cur = POP(_set);
    if (cur != NULL) {
	while (cur->next != NULL) {
	    l += distance(cur->p, cur->next->p);
	    cur = cur->next;
	}
	if (p.addr) l += distance(cur->p, first->p);
    }
    PSHn(l);
}

double compute_area(_set *A, _set* B)
{
    if (B->next == NULL)
	return B->p->x*A->p->y - A->p->x*B->p->y;
    else
	return B->p->x*B->next->p->y - B->next->p->x*B->p->y
	       + compute_area(A, B->next);
}

void polygon_area(void)
{
    _set *A;

    A = POP(_set);
    if (A == NULL)
	PSHn(0);
    else
        PSHn(fabs(compute_area(A, A))*.5);
}

void compute_centroid(_set *A, _set *B, _point *C)
{
    double d;
    if (B->next == NULL) {
	d = B->p->x*A->p->y - A->p->x*B->p->y;
	C->x += (B->p->x + A->p->x)*d;
	C->y += (B->p->y + A->p->y)*d;
    } else {
	d = B->p->x*B->next->p->y - B->next->p->x*B->p->y;
	C->x += (B->p->x + B->next->p->x)*d;
	C->y += (B->p->y + B->next->p->y)*d;
	compute_centroid(A, B->next, C);
    }
}

void create_centroid(void)
{
    _set *A;
    _point *C;
    double a;

    A = POP(_set);
    if (A == NULL) runtime_error(_("empty set"));
    get_mem(C, _point);
    if (A->next == NULL) {
	C->x = A->p->x;
	C->y = A->p->y;
    } else {
	compute_centroid(A, A, C);
	a = 6.*fabs(compute_area(A, A))*.5;
	C->x /= a;
	C->y /= a;
    }
    PSH(C);
}

void segment_argument(void)
{
    _set *seg;
    _point *A, *B;

    seg = POP(_set);
    A = get_point(&seg);
    B = get_point(&seg);
    PSHn(argument(A, B));
}

void vertex_angle(void)
{
    _point *A, *B, *C;
    double a, b, c;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = distance(B, C);
    b = distance(C, A);
    c = distance(A, B);
    if (ZERO(a) || ZERO(c)) runtime_error(_("invalid angle"));
    PSHn(Acos((a*a + c*c - b*b)/(2*a*c)));
}

void triangle_height(void)
{
    _point *A, *B, *C;
    double a, b, c, k;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = distance(B, C);
    b = distance(C, A);
    c = distance(A, B);
    if (ZERO(a)) runtime_error(_("invalid triangle"));
    k = (a + b + c)/2;
    PSHn(2*sqrt(k*(k-a)*(k-b)*(k-c))/a);
}
