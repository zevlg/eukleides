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

typedef union {
    void *ptr;
    double num;
    int addr;
} _param;

void init_stack_machine(void);

void push_param(_param p);

#define PSHn(num)	push_param((_param)(num))
#define PSH(addr)	push_param((_param)((void *)addr))

_param pop_param(void);

#define POPn		pop_param().num
#define POPs		(char *)pop_param().ptr
#define POP(type)	(type *)pop_param().ptr

typedef struct {
#ifdef __debug__
    const char* inst;
    int arg;
#endif
    void (*func)();
    _param param;
} _instr;

typedef struct {
    _instr *tbl;
    _instr *last;
    _instr *end;
    _instr *cur;
} _itable;

void resize_instr_table(void);

void goto_addr(_param p);

void gosub_addr(_param p);

void go_back(void);

void jump_if_zero(_param p);

void stop(void);

void set_cur_line(_param p);

void set_cur_file(_param p);

void exec_itable(void);

void reset_stack_machine(void);

void *allocate(int size);

#define get_mem(var,type)	var = (type *)allocate(sizeof(type))

#define get_elem(var) \
        var = (_set *)allocate(sizeof(_set)); \
        var->p = (_point *)allocate(sizeof(_point))
