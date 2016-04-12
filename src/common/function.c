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
#include "parser.tab.h"
#include "compiler.h"
#include "variable.h"

_function *func;

int func_type;

#define MAX_ARGS 32
#define MAX_LOCALS 32

void create_function(_symbol *symbol, int type)
{
    if (func != NULL) yyerror(_("invalid function definition"));
    set_type(symbol, type);
    func_type = type;
    func = &symbol->content->value.func;
    func->addr = get_addr() + 1;
    func->arg_tbl = (_argument *)allocate(MAX_ARGS*sizeof(_argument *));
    check_mem(func->arg_tbl);
    func->last_arg = func->arg_tbl;
    func->loc_tbl = (_symbol **)allocate(MAX_LOCALS*sizeof(_symbol *));
    check_mem(func->loc_tbl);
    func->last_loc = func->loc_tbl;
}

void add_arg(_symbol* symbol, int type)
{
    func->last_arg->arg = symbol;
    func->last_arg->type = type;
    func->last_arg++;
    if (func->last_arg - func->arg_tbl == MAX_ARGS)
	yyerror(_("too many arguments"));
    lift_type(symbol, type);
}

void end_args(void)
{
    func->last_arg->type = END;
}

void add_local(_symbol* symbol)
{
    if (func == NULL) yyerror(_("invalid local declaration"));
    *(func->last_loc++) = symbol;
    if (func->last_loc - func->loc_tbl == MAX_LOCALS)
	yyerror(_("too many arguments"));
    lift_type(symbol, SYMBOL);
    XEQ(lift_sym, symbol);
}

void check_return(int type)
{
    if (type != func_type) yyerror(_("invalid return statement"));
}

void restore_table(_function func)
{
    _argument *cur;
    _symbol **sym;

    for (cur = func.arg_tbl ; cur < func.last_arg; cur++)
	drop((_param)(void *)cur->arg);
    for (sym = func.loc_tbl ; sym < func.last_loc; sym++)
	drop((_param)(void *)*sym);
}

void return_error(void)
{
    runtime_error(_("function ending without return statement"));
}

void end_function(void)
{
    restore_table(*func);
    func = NULL;
    if (func_type == COMMAND) RTN;
    else XEQ_(return_error);
    func_type = 0;
}

void restore_variables(void)
{
    restore_table(((_symbol *)pop_param().ptr)->content->value.func);
}

#define MAX_CALLS 32

_argument *call[MAX_CALLS];

int ind = -1;

void init_call(_symbol *symbol)
{
    if (++ind == MAX_CALLS) yyerror(_("too many overlapping calls"));
    call[ind] = symbol->content->value.func.arg_tbl;
}

void lift_param(int type)
{
    if (call[ind]->type == END) yyerror(_("too many parameters"));
    if (call[ind]->type != type) yyerror(_("wrong parameter type"));
    switch(type) {
	case _NUMBER: XEQ(lift_num, call[ind]->arg);
		      break;
	case _POINT:  XEQ(lift_pnt, call[ind]->arg);
		      break;
	case _VECTOR: XEQ(lift_vec, call[ind]->arg);
		      break;
	case _SET:    XEQ(lift_set, call[ind]->arg);
		      break;
	case _LINE:   XEQ(lift_lin, call[ind]->arg);
		      break;
	case _CIRCLE: XEQ(lift_cir, call[ind]->arg);
		      break;
	case _CONIC:  XEQ(lift_con, call[ind]->arg);
		      break;
	case _STRING: XEQ(lift_str, call[ind]->arg);
		      break;
    }
    call[ind]++;
}

void end_call(void)
{
    if (call[ind]->type != END) yyerror(_("too few parameters"));
    ind--;
}
