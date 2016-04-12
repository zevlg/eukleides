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
#include <stdio.h>
#include "error.h"
#include "core.h"
#include "symbol.h"

	/* Parameter stack */

typedef struct {
    _param *stk;
    _param *last;
    _param *end;
} _pstack;

_pstack pstack;

#define PSTACK_INIT_SIZE 1024

void init_param_stack(void)
{
    pstack.stk = (_param *)malloc(PSTACK_INIT_SIZE * sizeof(_param));
    check_mem(pstack.stk);
    pstack.last = pstack.stk;
    pstack.end = pstack.stk + PSTACK_INIT_SIZE;
}

void resize_param_stack(void)
{
    int size = pstack.end - pstack.stk;

    pstack.stk = (_param *)realloc(pstack.stk, 2 * size * sizeof(_param));
    check_mem(pstack.stk);
    pstack.last = pstack.stk + size;
    pstack.end = pstack.stk + 2 * size;
}

void push_param(_param p)
{
    *(pstack.last++) = p;
    if (pstack.last == pstack.end) resize_param_stack();
}

_param pop_param(void)
{
    if (pstack.last > pstack.stk) return *(--pstack.last);
    runtime_error(_("empty parameter stack"));
    return (_param)0;
}

void reset_param_stack(void)
{
    pstack.last = pstack.stk;
}

	/* Return address stack */

typedef struct {
    _instr **stk;
    _instr **last;
    _instr **end;
} _rstack;

_rstack rstack;

#define RSTACK_INIT_SIZE 1024

void init_return_stack(void)
{
   rstack.stk = (_instr **)malloc(RSTACK_INIT_SIZE * sizeof(_instr *));
   check_mem(rstack.stk);
   rstack.last = rstack.stk;
   rstack.end = rstack.stk + RSTACK_INIT_SIZE;
}

void resize_return_stack(void)
{
    int size = rstack.end - rstack.stk;

    rstack.stk = (_instr **)realloc(rstack.stk, 2 * size * sizeof(_instr *));
    check_mem(rstack.stk);
    rstack.last = rstack.stk + size;
    rstack.end = rstack.stk + 2 * size;
}

void push_addr(_instr *a)
{
    *(rstack.last++) = a;
    if (rstack.last == rstack.end) resize_return_stack();
}

_instr *pop_addr(void)
{
    if (rstack.last > rstack.stk) return *(--rstack.last);
    runtime_error(_("empty return stack"));
    return (_instr *)NULL;
}

void reset_return_stack(void)
{
    rstack.last = rstack.stk;
}

	/* Instruction table */

_itable itable;

#define ITABLE_INIT_SIZE 16384

void init_instr_table(void)
{
   itable.tbl = (_instr *)malloc(ITABLE_INIT_SIZE * sizeof(_instr));
   check_mem(itable.tbl);
   itable.last = itable.tbl;
   itable.end = itable.tbl + ITABLE_INIT_SIZE;
}

void resize_instr_table(void)
{
    int size = itable.end - itable.tbl;

    itable.tbl = (_instr *)realloc(itable.tbl, 2 * size * sizeof(_instr));
    check_mem(itable.tbl);
    itable.last = itable.tbl + size;
    itable.end = itable.tbl + 2 * size;
}

void goto_addr(_param p)
{
    itable.cur = itable.tbl + p.addr - 1;
}

void gosub_addr(_param p)
{
    push_addr(itable.cur);
    itable.cur = itable.tbl + p.addr - 1;
}

void go_back(void)
{
    itable.cur = pop_addr();
}

void jump_if_zero(_param p)
{
    if (pop_param().addr == 0) itable.cur = itable.tbl + p.addr - 1;
}

void stop(void)
{
    itable.cur = itable.last;
}

int cur_line;

void set_cur_line(_param p)
{
    cur_line = p.addr;
}

char* cur_file;

void set_cur_file(_param p)
{
    cur_file = (char *)p.ptr;
}

#ifdef __debug__
void echo_itable(void)
{
    static int i = 0;
    _instr* ins;

    if (i) return;
    for (ins = itable.tbl; ins < itable.last; ins++, i++)
	switch(ins->arg) {
	    case 0: fprintf(stderr, "%04d %-12s\n", i, ins->inst);
		    break;
	    case 1: fprintf(stderr, "%04d %-12s %p\n", i, ins->inst,
			    ins->param.ptr);
		    break;
	    case 2: fprintf(stderr, "%04d %-12s %g\n", i, ins->inst,
			    ins->param.num);
		    break;
	    case 3: fprintf(stderr, "%04d %-12s %04d\n", i, ins->inst,
			    ins->param.addr);
	}
}
#endif

void exec_itable(void)
{
#ifdef __debug__
    echo_itable();
#endif
    for (itable.cur = itable.tbl; itable.cur < itable.last; itable.cur++)
	(itable.cur->func)(itable.cur->param);
}

void reset_instr_table(void)
{
    itable.last = itable.tbl;
}

	/* Memory pages */

typedef struct {
    void **start;
    void **cur;
    void *first;
    void *last;
} _mpages;

_mpages mem;

#define PAGE_SIZE	(16384 - sizeof(void *))

void init_memory(void)
{
    mem.first = malloc(PAGE_SIZE);
    check_ptr(mem.first);
    mem.start = mem.cur = (void **)mem.first;
    *mem.cur = NULL;
    mem.last = mem.first + sizeof(void *);
}

void *allocate(int size)
{
    void *addr;

    if (mem.last-mem.first+size > PAGE_SIZE) {
	if (size > PAGE_SIZE) runtime_error(_("memory allocation error"));
	if (*mem.cur == NULL) {
	    mem.first = malloc(PAGE_SIZE);
	    check_ptr(mem.first);
	    *mem.cur = mem.first;
	    mem.cur = (void **)mem.first;
	    *mem.cur = NULL;
	} else mem.first = *mem.cur;
	mem.last = mem.first + sizeof(void *);
    }
    addr = mem.last;
    mem.last += size;
    return addr;
}

void reset_memory(void)
{
    mem.cur = mem.start;
    mem.first = (void *)mem.start;
    mem.last = mem.first + sizeof(void *);
}

	/* Stack machine */

void init_stack_machine(void)
{
    init_param_stack();
    init_return_stack();
    init_instr_table();
    init_memory();
}

void reset_stack_machine(void)
{
    reset_param_stack();
    reset_return_stack();
    reset_instr_table();
    reset_memory();
}
