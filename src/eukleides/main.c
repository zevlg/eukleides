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
#include "args.h"
#include "module.h"
#include "keywords.h"
#include "symbol.h"
#include "strings.h"
#include "core.h"
#include "compiler.h"
#include "interactive.h"
#include "parser.tab.h"

int yyparse(void);

int main(int argc, char *argv[])
{
    process_args(argc, argv);
    load_keywords();
    init_symbol_list();
    init_string();
    init_module_stack();
    init_stack_machine();
    yyparse();
    if (anim_amount)
	for (anim_cur = 0; anim_cur < anim_amount; anim_cur++) {
	    exec_itable();
	    strokes[anim_state][anim_dir] += anim_inc;
	}
    else exec_itable();
    return EXIT_SUCCESS;
}
