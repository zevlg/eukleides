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
#include <string.h>
#include "error.h"
#include "symbol.h"
#include "core.h"
#include "parser.tab.h"

void recall_num(_param p)
{
    PSHn(((_symbol *)p.ptr)->content->value.number);
}

#define recall(var, type) \
        push_param((_param)(void *)(((_symbol *)var.ptr)->content->value.type))

void recall_pnt(_param p)
{
    recall(p, point);
}

void recall_vec(_param p)
{
    recall(p, vector);
}

void recall_lin(_param p)
{
    recall(p, line);
}

void recall_cir(_param p)
{
    recall(p, circle);
}

void recall_con(_param p)
{
    recall(p, conic);
}

void recall_str(_param p)
{
    recall(p, string);
}

_set *copy_set(_set *set)
{
    _set *copy;

    if (set == NULL) return NULL;
    get_mem(copy, _set);
    copy->p = set->p;
    copy->next = copy_set(set->next);
    return copy;
}

void recall_set(_param p)
{
    PSH(copy_set(((_symbol *)p.ptr)->content->value.set));
}

void store_num(_param p)
{
    ((_symbol *)p.ptr)->content->value.number = POPn;
}

void store_pnt(_param p)
{
    ((_symbol *)p.ptr)->content->value.point = POP(_point);
}

void store_vec(_param p)
{
    ((_symbol *)p.ptr)->content->value.vector = POP(_vector);
}

void store_set(_param p)
{
    ((_symbol *)p.ptr)->content->value.set = POP(_set);
}

void multiple_store(_param p)
{
    _set *set;
    _symbol *cur;

    set = POP(_set);
    cur = POP(_symbol);
    while (cur != NULL) {
	if (set == NULL) runtime_error(_("invalid set"));
	cur->content->value.point = set->p;
	set = set->next;
	cur = POP(_symbol);
    }
}

void store_at_index(_param p)
{
    _set *cur;
    _point *A;
    double i;

    cur = ((_symbol *)p.ptr)->content->value.set;
    A = POP(_point);
    i = POPn;
    while (i >= 1 && cur != NULL) {
	i--;
	cur = cur->next;
    }
    if (i < 0 || cur == NULL) runtime_error(_("index out of range"));
    cur->p = A;
}

void store_at_end(_param p)
{
    _set *cur, *end;
    _point *A;

    cur = ((_symbol *)p.ptr)->content->value.set;
    A = POP(_point);
    get_mem(end, _set);
    if (cur == NULL) ((_symbol *)p.ptr)->content->value.set = end;
    else {
	while (cur->next != NULL) cur = cur->next;
	cur->next = end;
    }
    end->p = A;
    end->next = NULL;
}

void store_lin(_param p)
{
    ((_symbol *)p.ptr)->content->value.line = POP(_line);
}

void store_cir(_param p)
{
    ((_symbol *)p.ptr)->content->value.circle = POP(_circle);
}

void store_con(_param p)
{
    ((_symbol *)p.ptr)->content->value.conic = POP(_conic);
}

void store_str(_param p)
{
    ((_symbol *)p.ptr)->content->value.string = POPs;
}

void increment(_param p)
{
    ((_symbol *)p.ptr)->content->value.number += POPn;
    //push_param((_param)((_symbol *)p.ptr)->content->value.number);
}

void lift_num(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.number = POPn;
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_pnt(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.point = POP(_point);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_vec(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.vector = POP(_vector);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_set(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.set = POP(_set);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_lin(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.line = POP(_line);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_cir(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.circle = POP(_circle);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_con(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.conic = POP(_conic);
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_str(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->value.string = POPs;
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void lift_sym(_param p)
{
    _content *content;

    get_mem(content, _content);
    content->type = SYMBOL;
    content->next = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content;
}

void drop(_param p)
{
    _content *content;

    content = ((_symbol *)p.ptr)->content;
    ((_symbol *)p.ptr)->content = content->next;
}
