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
#include "core.h"
#include "symbol.h"

extern _itable itable;

#ifdef __debug__
void add_instr(const char* inst, int arg, void (*f)(void), _param p)
{
    itable.last->inst = inst;
    itable.last->arg = arg;
#else
void add_instr(void (*f)(void), _param p)
{
#endif
    itable.last->func = f;
    itable.last->param = p;
    itable.last++;
    if (itable.last == itable.end) resize_instr_table();
}

int get_addr(void)
{
    return itable.last - itable.tbl;
}

void store_mark()
{
    push_param((_param)get_addr());
}

int pop_mark(void)
{
    return pop_param().addr;
}

void back_patch(void)
{
    _instr *instr;
    instr = itable.tbl + pop_mark() - 1;
    instr->param.addr += get_addr();
}

void set_type(_symbol *symbol, int type)
{
    symbol->content->type = type;
}

void lift_type(_symbol *symbol, int type)
{
    _content *content;

    get_mem(content, _content);
    content->type = type;
    content->next = symbol->content;
    symbol->content = content;
}
