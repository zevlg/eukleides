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

void create_circle_with_radius(void)
{
    _point *center;
    _circle *val;

    get_mem(val, _circle);
    val->r = POPn;
    if (val->r <= 0) runtime_error("invalid radius");
    center = POP(_point);
    val->x = center->x;
    val->y = center->y;
    PSH(val);
}

void circle_center(void)
{
    _circle *cir;
    _point *val;

    cir = POP(_circle);
    get_mem(val, _point);
    val->x = cir->x;
    val->y = cir->y;
    PSH(val);
}

void create_circle_with_diameter(void)
{
    _set *set;
    _point *A, *B;
    _circle *val;

    set = POP(_set);
    A = get_point(&set);
    B = get_point(&set);
    get_mem(val, _circle);
    val->r = distance(A, B)/2;
    if (val->r <= 0) runtime_error("invalid radius");
    val->x = (A->x + B->x)/2;
    val->y = (A->y + B->y)/2;
    PSH(val);
}

void create_circumcircle(void)
{
    _point *A, *B, *C;
    double a, d, e, f, s1, s2, s3;
    _circle *val;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    get_mem(val, _circle);
    s1 = A->x*A->x + A->y*A->y;
    s2 = B->x*B->x + B->y*B->y;
    s3 = C->x*C->x + C->y*C->y;
    a = det3(A->x, B->x, C->x, A->y, B->y, C->y, 1, 1, 1);
    if (ZERO(a)) runtime_error("invalid points");
    d = det3(s1, s2, s3, A->y, B->y, C->y, 1, 1, 1);
    e = det3(s1, s2, s3, A->x, B->x, C->x, 1, 1, 1);
    f = det3(s1, s2, s3, A->x, B->x, C->x, A->y, B->y, C->y);
    val->r = sqrt((d*d+e*e)/(4*a*a)+f/a);
    val->x = d/(2*a);
    val->y = -e/(2*a);
    PSH(val);
}

void create_incircle(void)
{
    _point *A, *B, *C;
    double a, b, c, s, s1, s2, r, t, u, v;
    _circle *val;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    get_mem(val, _circle);
    a = distance(B, C);
    b = distance(C, A);
    c = distance(A, B);
    s = det2(B->x-A->x, C->x-A->x, B->y-A->y, C->y-A->y) > 0 ? 1 : -1;
    s1 = sqrt((a-b+c)*(a+b-c));
    s2 = sqrt((b+c-a)*(b+c+a));
    if (ZERO(s1) || ZERO(s2)) runtime_error("invalid points");
    r = .5*s1*s2/(a+b+c);
    t = s1/s2;
    u = (B->x-A->x)/c;
    v = (B->y-A->y)/c;
    val->r = r;
    val->x = A->x + r*(u/t-s*v);
    val->y = A->y + r*(v/t+s*u);
    PSH(val);
}

void translate_circle(void)
{
    _circle *C, *val;
    _vector *u;

    u = POP(_vector);
    C = POP(_circle);
    get_mem(val, _circle);
    val->x = C->x + u->x;
    val->y = C->y + u->y;
    val->r = C->r;
    PSH(val);
}

void reflect_circle(void)
{
    _circle *C, *val;
    _line *l;
    double c, s, x, y, p;

    l = POP(_line);
    C = POP(_circle);
    c = Cos(l->a);
    s = Sin(l->a);
    x = C->x - l->x;
    y = C->y - l->y;
    p = 2*(c*x + s*y);
    get_mem(val, _circle);
    val->x = l->x + p*c - x;
    val->y = l->y + p*s - y;
    val->r = C->r;
    PSH(val);
}

void symetric_circle(void)
{
    _circle *C, *val;
    _point *O;

    O = POP(_point);
    C = POP(_circle);
    get_mem(val, _circle);
    val->x = 2*O->x - C->x;
    val->y = 2*O->y - C->y;
    val->r = C->r;
    PSH(val);
}

void rotate_circle(void)
{
    _circle *C, *val;
    _point *O;
    double a, c, s, x, y;

    a = POPn;
    O = POP(_point);
    C = POP(_circle);
    c = Cos(a);
    s = Sin(a);
    x = C->x - O->x;
    y = C->y - O->y;
    get_mem(val, _circle);
    val->x = O->x + c*x - s*y;
    val->y = O->y + s*x + c*y;
    val->r = C->r;
    PSH(val);
}

void homothetic_circle(void)
{
    _circle *C, *val;
    _point *O;
    double k;

    k = POPn;
    O = POP(_point);
    C = POP(_circle);
    get_mem(val, _circle);
    val->x = O->x + k*(C->x - O->x);
    val->y = O->y + k*(C->y - O->y);
    val->r = fabs(k)*C->r;
    PSH(val);
}

void circle_radius(void)
{
    PSHn((POP(_circle))->r);
}

void circle_perimeter(void)
{
    PSHn(2*M_PI*(POP(_circle))->r);
}

void circle_area(void)
{
    _circle *cir;

    cir = POP(_circle);
    PSHn(M_PI*cir->r*cir->r);
}

void point_on_circle_argument(void)
{
    _point *A;
    _circle *cir;

    cir = POP(_circle);
    A = POP(_point);
    PSHn(RTOD(atan2(A->y-cir->y, A->x-cir->x)));
}
