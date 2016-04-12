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

#ifdef __debug__

void add_instr(const char* inst, int arg, void (*f)(), _param p);

#define instruction(arg, fct, param) add_instr(#fct, arg, fct, param)

#else

void add_instr(void (*f)(), _param p);

#define instruction(arg, fct, param) add_instr(fct, param)

#endif

int get_addr(void);

void store_mark(void);

int pop_mark(void);

void back_patch(void);

void set_type(_symbol *symbol, int type);

void lift_type(_symbol *symbol, int type);

#define PSH_(arg)	instruction(2, push_param, (_param)(arg))
#define INC(ptr)	instruction(1, increment, (_param)(void *)ptr)
#define GTO(addr)	instruction(3, goto_addr, (_param)(addr))
#define GSB(addr)	instruction(3, gosub_addr, (_param)(addr))
#define JPZ(addr)	instruction(3, jump_if_zero, (_param)(addr))
#define RTN		instruction(0, go_back, (_param)NULL)
#define STP		instruction(0, stop, (_param)NULL)
#define XEQ_(fct)	instruction(0, fct, (_param)NULL)
#define XEQ(fct,ptr)    instruction(1, fct, (_param)(void *)ptr)
#define SET_(fct,lbl)   instruction(3, fct, (_param)(lbl))
