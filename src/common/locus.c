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
#include "error.h"
#include "symbol.h"
#include "core.h"

int locus_addr = 0, num;

double start, step;

_symbol *variable;

void init_locus(_param p)
{
    double end;

    num = (int)POPn;
    if (num <= 0) runtime_error(_("illegal step number"));
    end = POPn;
    start = POPn;
    variable = (_symbol *)p.ptr;
    variable->content->value.number = end;
    step = (end-start)/num;
    PSH(NULL);
    PSHn(1);
}

void put_pnt(void)
{
    _set *head, *tail;
    _point *cur;

    cur = POP(_point);
    tail = POP(_set);
    get_mem(head, _set);
    head->p = cur;
    head->next = tail;
    PSH(head);
    num--;
    variable->content->value.number = start + num*step;
    PSHn(num >= 0);
}
