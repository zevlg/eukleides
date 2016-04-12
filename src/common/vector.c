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
 *  but WITHOUT ANY WARRANTY; fromout even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along from this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <math.h>
#include "error.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"

void create_vector_cartesian(void)
{
    _vector *val;

    get_mem(val, _vector);
    val->y = POPn;
    val->x = POPn;
    PSH(val);
}

void create_vector_polar(void)
{
    double r, a;
    _vector *val;

    get_mem(val, _vector);
    a = POPn;
    r = POPn;
    val->x = r*Cos(a);
    val->y = r*Sin(a);
    PSH(val);
}

void create_vector_from_points(void)
{
    _point *A, *B;
    _vector *val;

    get_mem(val, _vector);
    B = POP(_point);
    A = POP(_point);
    val->x = B->x - A->x;
    val->y = B->y - A->y;
    PSH(val);
}

void create_vector_from_segment(void)
{
    _set *set;
    _point *A, *B;
    _vector *val;

    set = POP(_set);
    A = get_point(&set);
    B = get_point(&set);
    get_mem(val, _vector);
    val->x = B->x - A->x;
    val->y = B->y - A->y;
    PSH(val);
}

void create_vector_from_line(void)
{
    _line *l;
    _vector *val;

    get_mem(val, _vector);
    l = POP(_line);
    val->x = Cos(l->a);
    val->y = Sin(l->a);
    PSH(val);
}

void add_vectors(void)
{
    _vector *val, *u, *v;

    get_mem(val, _vector);
    v = POP(_vector);
    u = POP(_vector);
    val->x = u->x + v->x;
    val->y = u->y + v->y;
    PSH(val);
}

void substract_vectors(void)
{
    _vector *val, *u, *v;

    get_mem(val, _vector);
    v = POP(_vector);
    u = POP(_vector);
    val->x = u->x - v->x;
    val->y = u->y - v->y;
    PSH(val);
}

void reverse_vector(void)
{
    _vector *val, *u;

    get_mem(val, _vector);
    u = POP(_vector);
    val->x = -u->x;
    val->y = -u->y;
    PSH(val);
}

void multiply_vector(void)
{
    _vector *val, *u;
    double k;

    get_mem(val, _vector);
    u = POP(_vector);
    k = POPn;
    val->x = k * u->x;
    val->y = k * u->y;
    PSH(val);
}

void divide_vector(void)
{
    _vector *val, *u;
    double k;

    get_mem(val, _vector);
    k = POPn;
    u = POP(_vector);
    val->x = u->x/k;
    val->y = u->y/k;
    PSH(val);
}

void rotate_vector(void)
{
    _vector *val, *u;
    double l, a;

    get_mem(val, _vector);
    a = POPn;
    u = POP(_vector);
    a += RTOD(atan2(u->y, u->x));
    l = hypot(u->x, u->y);
    val->x = l*Cos(a);
    val->y = l*Sin(a);
    PSH(val);
}

void vector_abscissa(void)
{
    PSHn((POP(_vector))->x);
}

void vector_ordinate(void)
{
    PSHn((POP(_vector))->y);
}

void vector_length(void)
{
    _vector *u;

    u = POP(_vector);
    PSHn(hypot(u->x, u->y));

}

void vector_argument(void)
{
    _vector *u;

    u = POP(_vector);
    PSHn(RTOD(atan2(u->y, u->x)));
}

void vectors_angle(void)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    PSHn((u->x*v->y > u->y*v->x ? 1 : -1)
	 * Acos((u->x*v->x+u->y*v->y)/(hypot(u->x, u->y)*hypot(v->x, v->y))));
}

void scalar_product(void)
{
    _vector *u, *v;

    v = POP(_vector);
    u = POP(_vector);
    PSHn(u->x*v->x + u->y*v->y);
}
